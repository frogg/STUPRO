#include <Filter/DataDensityFilter.h>

#include <vtkDataObject.h>
#include <vtkKMeansStatistics.h>
#include <vtkDoubleArray.h>
#include <vtkTable.h>
#include <vtkCellArray.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkObjectFactory.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkPointData.h>
#include <vtkAbstractArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCleanUnstructuredGrid.h>

#include <sstream>
#include <algorithm>

#include <qmap.h>

DataDensityFilter::DataDensityFilter() { }
DataDensityFilter::~DataDensityFilter() { }

vtkStandardNewMacro(DataDensityFilter)
int DataDensityFilter::FillOutputPortInformation(int port, vtkInformation* info) {
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
	return 1;
}

int DataDensityFilter::FillInputPortInformation(int port, vtkInformation* info) {
	info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkUnstructuredGrid");
	return 1;
}

int DataDensityFilter::RequestData(vtkInformation* info,
                                   vtkInformationVector** inputVector,
                                   vtkInformationVector* outputVector) {
	// Get output information from the request vectors
	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	// Get the actual objects from the obtained information
	vtkPointSet* input = vtkPointSet::SafeDownCast(inInfo->Get(vtkPolyData::DATA_OBJECT()));
	
	vtkSmartPointer<vtkCleanUnstructuredGrid> cleanFilter = vtkCleanUnstructuredGrid::New();
	cleanFilter->SetInputData(input);
	cleanFilter->Update();
	
	vtkSmartPointer<vtkUnstructuredGrid> cleanedInput = vtkSmartPointer<vtkUnstructuredGrid>::New();
	cleanedInput->ShallowCopy(vtkUnstructuredGrid::SafeDownCast(cleanFilter->GetOutput()));
	
	vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkPolyData::DATA_OBJECT()));

	PointMap reducedDataPoints;
	this->centroidIndices.clear();

	// Depending on the checkbox either use the kMeans or the simple algorithm
	if (kMeansEnabled) {
		reducedDataPoints = this->reducePointsKMeans(cleanedInput);
	} else {
		reducedDataPoints = this->reducePointsSimple(cleanedInput);
	}

	output->ShallowCopy(this->generateOutputData(reducedDataPoints, cleanedInput));

	return 1;
}

void DataDensityFilter::setDataPercentage(double percentage) {
	this->dataPercentage = percentage;
	this->Modified();
}

void DataDensityFilter::setKMeansEnabled(bool enableKMeans) {
	this->kMeansEnabled = enableKMeans;
	this->Modified();
}

void DataDensityFilter::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "Twitter Filter, Kronos Project" << endl;
}

DataDensityFilter::PointMap DataDensityFilter::reducePointsKMeans(vtkPointSet* input) {
	// Get the points into the format needed for KMeans
	vtkSmartPointer<vtkTable> inputData = vtkSmartPointer<vtkTable>::New();

	// Create the table needed for the KMeans statistics filter
	for (int c = 0; c < 3; ++c) {
		std::stringstream colName;
		colName << "coord " << c;
		vtkSmartPointer<vtkDoubleArray> doubleArray =
		    vtkSmartPointer<vtkDoubleArray>::New();
		doubleArray->SetNumberOfComponents(1);
		doubleArray->SetName(colName.str().c_str());
		doubleArray->SetNumberOfTuples(input->GetNumberOfPoints());

		for (int r = 0; r < input->GetNumberOfPoints(); ++r) {
			double p[3];
			input->GetPoint(r, p);
			doubleArray->SetValue(r, p[c]);
		}
		inputData->AddColumn(doubleArray);
	}

	// Actually do the KMeans to calculate the centroids for the clusters
	vtkSmartPointer<vtkKMeansStatistics> kMeansStatistics =
	    vtkSmartPointer<vtkKMeansStatistics>::New();

	kMeansStatistics->SetInputData(vtkStatisticsAlgorithm::INPUT_DATA, inputData);
	kMeansStatistics->SetColumnStatus(inputData->GetColumnName(0), 1);
	kMeansStatistics->SetColumnStatus(inputData->GetColumnName(1), 1);
	kMeansStatistics->SetColumnStatus(inputData->GetColumnName(2), 1);
	kMeansStatistics->RequestSelectedColumns();

	// Set the default number of clusters to the number of points that should be kept
	kMeansStatistics->SetDefaultNumberOfClusters(std::max(int((1 - this->dataPercentage) *
	        input->GetNumberOfPoints()), 1));

	// Make the filter only do one iteration for a better performance since we do not need a precise kMeans approximation
	kMeansStatistics->SetMaxNumIterations(1);
	kMeansStatistics->SetAssessOption(true);
	kMeansStatistics->Update();

	// Extract the calculated cluster centers from the filter
	vtkSmartPointer<vtkPoints> centroids =
	    vtkSmartPointer<vtkPoints>::New();

	vtkMultiBlockDataSet* outputMetaDS = vtkMultiBlockDataSet::SafeDownCast(
	        kMeansStatistics->GetOutputDataObject(vtkStatisticsAlgorithm::OUTPUT_MODEL));
	vtkSmartPointer<vtkTable> outputMeta = vtkTable::SafeDownCast(outputMetaDS->GetBlock(0));

	vtkDoubleArray* coord0 = vtkDoubleArray::SafeDownCast(outputMeta->GetColumnByName("coord 0"));
	vtkDoubleArray* coord1 = vtkDoubleArray::SafeDownCast(outputMeta->GetColumnByName("coord 1"));
	vtkDoubleArray* coord2 = vtkDoubleArray::SafeDownCast(outputMeta->GetColumnByName("coord 2"));

	PointMap outputMap;

	// Put the centroids into the map
	for (int i = 0; i < coord0->GetNumberOfTuples(); ++i) {
		PointCoordinates centroidCoordinates(coord0->GetValue(i), coord1->GetValue(i), coord2->GetValue(i));
		QList<int> subordinatePoints;

		// Extract the point IDs in the centroid's bucket
		for (int r = 0; r < kMeansStatistics->GetOutput()->GetNumberOfRows(); ++r) {
			vtkVariant v = kMeansStatistics->GetOutput()->GetValue(r,
			               kMeansStatistics->GetOutput()->GetNumberOfColumns() - 1);
			if (v.ToInt() == i) {
				subordinatePoints.append(r);
			}
		}
		outputMap.insert(std::make_pair(centroidCoordinates, subordinatePoints));
	}

	return outputMap;
}

DataDensityFilter::PointMap DataDensityFilter::reducePointsSimple(vtkPointSet* input) {
	int numberOfPoints = input->GetNumberOfPoints();
	PointMap outputMap;

	// Get the width, height and depth of the data in order to set the absoprtion distance
	double bounds[6];
	input->GetBounds(bounds);

	double minX = bounds[0];
	double maxX = bounds[1];
	double minY = bounds[2];
	double maxY = bounds[3];
	double minZ = bounds[4];
	double maxZ = bounds[5];
	double width = maxX - minX;
	double height = maxY - minY;
	double depth = maxZ - minZ;

	double absorptionDistance = this->dataPercentage * (width + height + depth);

	// This data structure maps a point index to a boolean flag denoting whether it has been absorped by a central point or has become a central point
	std::map<int, bool> pointAbsorptionStatus;

	// In the beginning all points are unabsorbed
	for (int i = 0; i < numberOfPoints; ++i) {
		pointAbsorptionStatus[i] = false;
	}
	
	QMap<PointCoordinates, int> indices;

	// Generate central points and assign close points to them
	for (int i = 0; i < numberOfPoints; ++i) {
		if (pointAbsorptionStatus[i] == false) {
			double centralPointCoordinatesArray[3];
			input->GetPoint(i, centralPointCoordinatesArray);

			PointCoordinates centralPointCoordinates(centralPointCoordinatesArray[0],
			        centralPointCoordinatesArray[1], centralPointCoordinatesArray[2]);
			QList<int> subordinatePointIndices;
			pointAbsorptionStatus[i] = true;
			
			indices[centralPointCoordinates] = i;

			for (int j = 0; j < numberOfPoints; ++j) {
				if (pointAbsorptionStatus[j] == false) {
					double subordinatePointCoordinatesArray[3];
					input->GetPoint(j, subordinatePointCoordinatesArray);
					PointCoordinates subordinatePointCoordinates(subordinatePointCoordinatesArray[0],
					        subordinatePointCoordinatesArray[1], subordinatePointCoordinatesArray[2]);

					if (centralPointCoordinates.getDistanceTo(subordinatePointCoordinates) < absorptionDistance) {
						subordinatePointIndices.append(j);
						pointAbsorptionStatus[j] = true;
					}
				}
			}
			outputMap[centralPointCoordinates] = subordinatePointIndices;
		}
	}
	
	typedef PointMap::iterator it_type;
	for(it_type iterator = outputMap.begin(); iterator != outputMap.end(); iterator++) {
		this->centroidIndices.append(indices[iterator->first]);
	}

	return outputMap;
}

vtkSmartPointer<vtkPolyData> DataDensityFilter::generateOutputData(
    PointMap centralPoints, vtkPointSet* input) {
	vtkSmartPointer<vtkPolyData> output = vtkSmartPointer<vtkPolyData>::New();

	// Create the content of the output poly data object
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
	vertices->Allocate(vertices->EstimateSize(1, centralPoints.size()));
	vertices->InsertNextCell(centralPoints.size());

	// Create all arrays from the input data
	QList<vtkSmartPointer<vtkAbstractArray>> inputArrays;
	QList<vtkSmartPointer<vtkAbstractArray>> outputArrays;

	for (int h = 0; h < input->GetPointData()->GetNumberOfArrays(); ++h) {
		vtkSmartPointer<vtkAbstractArray> inputArray = input->GetPointData()->GetAbstractArray(h);

		if (!inputArray) {
			vtkErrorMacro( << "An input array could not be read.");
			return 0;
		}

		vtkSmartPointer<vtkAbstractArray> outputArray = vtkAbstractArray::CreateArray(
		            inputArray->GetDataType());

		outputArray->SetNumberOfComponents(inputArray->GetNumberOfComponents());
		outputArray->SetNumberOfTuples(centralPoints.size());
		outputArray->SetName(inputArray->GetName());

		inputArrays.append(inputArray);
		outputArrays.append(outputArray);
	}

	// Iterate through all the central points and calculate the arithmetic mean of the subordinate point's data
	PointMap::const_iterator i = centralPoints.cbegin();
	int centralPointIndex = 0;
	while (i != centralPoints.cend()) {
		vertices->InsertCellPoint(points->InsertNextPoint(i->first.getX(), i->first.getY(),
		                          i->first.getZ()));

		for (int j = 0; j < inputArrays.size(); ++j) {
			if (inputArrays[j]->IsNumeric()) {
				// For numeric input arrays, calculate the average for all values of subordinate points associated with the central point
				vtkSmartPointer<vtkDataArray> inputArray = vtkDataArray::SafeDownCast(inputArrays[j]);
				vtkSmartPointer<vtkDataArray> outputArray = vtkDataArray::SafeDownCast(outputArrays[j]);

				if (!inputArray || !outputArray) {
					vtkErrorMacro( << "There was an error while calculating the average.");
					return 0;
				}

				std::vector<double> cumulativeAverage(inputArrays[j]->GetNumberOfComponents());
				for (int l = 0; l < inputArrays[j]->GetNumberOfComponents(); ++l) {
					cumulativeAverage[l] = inputArray->GetTuple(centralPointIndex)[l];
				}

				QListIterator<int> k(i->second);
				while (k.hasNext()) {
					int pointIndex = k.next();
					for (int l = 0; l < inputArrays[j]->GetNumberOfComponents(); l++) {
						cumulativeAverage[l] = (cumulativeAverage[l] + inputArray->GetTuple(pointIndex)[l]) / 2;
					}
				}

				outputArray->InsertTuple(centralPointIndex, cumulativeAverage.data());
			} else {
				// For non-numeric input arrays, simply keep the value of the central point
				if (this->centroidIndices.size() > 0) {
					outputArrays[j]->SetTuple(centralPointIndex, this->centroidIndices[centralPointIndex], inputArrays[j]);
				} else {
					outputArrays[j]->SetTuple(centralPointIndex, centralPointIndex, inputArrays[j]);
				}
			}
		}

		centralPointIndex++;
		++i;
	}
	output->SetPoints(points);
	output->SetVerts(vertices);

	// Add the output arrays to the data set
	QList<vtkSmartPointer<vtkAbstractArray>>::iterator j;
	for (j = outputArrays.begin(); j != outputArrays.end(); ++j) {
		output->GetPointData()->AddArray(*j);
	}
	return output;
}
