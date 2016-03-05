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

#include <sstream>
#include <algorithm>

//TESTING
#include <vtkPointData.h>
#include <vtkAbstractArray.h>
#include <vtkFloatArray.h>
#include <vtkSetGet.h>
#include <vtkInformation.h>
#include <vtkInformationDoubleKey.h>

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
	vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkPolyData::DATA_OBJECT()));

	QMap<PointCoordinates, QList<int>> reducedDataPoints;
	
	if (kMeansEnabled) {
		reducedDataPoints = this->reducePointsKMeans(input);
	} else {
		reducedDataPoints = this->reducePointsSimple(input);
	}
	
	output->ShallowCopy(this->generateOutputData(reducedDataPoints, input));

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

QMap<PointCoordinates, QList<int>> DataDensityFilter::reducePointsKMeans(vtkPointSet* input) {
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
	kMeansStatistics->SetDefaultNumberOfClusters(std::max(int(this->dataPercentage *
	        input->GetNumberOfPoints()), 1));

	// Make the filter only do one iteration for a better performance since we do not need a precise kMeans approximation
	kMeansStatistics->SetMaxNumIterations(1);

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
	
	QMap<PointCoordinates, QList<int>> outputMap;

	// Put the centroids into the map
	for (int i = 0; i < coord0->GetNumberOfTuples(); ++i) {
		PointCoordinates centroidCoordinates(coord0->GetValue(i), coord1->GetValue(i), coord2->GetValue(i));
		QList<int> subordinatePoints;
		
		// Extract the point IDs in the centroid's bucket
		for (int r = 0; r < kMeansStatistics->GetOutput()->GetNumberOfRows(); r++) {
			vtkVariant v = kMeansStatistics->GetOutput()->GetValue(r, kMeansStatistics->GetOutput()->GetNumberOfColumns() - 1);
			
			if (v.ToInt() == i) {
				subordinatePoints.append(r);
			}
		}
		
		outputMap.insert(centroidCoordinates, subordinatePoints);
	}
	
	return outputMap;
}

QMap<PointCoordinates, QList<int>> DataDensityFilter::reducePointsSimple(vtkPointSet* input) {
	int numberOfPoints = input->GetNumberOfPoints();
	QMap<PointCoordinates, QList<int>> outputMap;
	
	// Calculate the distance where points should be absorbed by central points
	double absorptionDistance = 0.0;
	
	// Overkill code for calculating the convex hull sphere of the data points
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
	
	PointCoordinates dataSetCenter(minX + (width / 2.0), minY + (height / 2.0), minZ + (depth / 2.0));

	for (int i = 0; i < numberOfPoints; i++) {
		double pointCoordinatesArray[3];
		input->GetPoint(i, pointCoordinatesArray);
		PointCoordinates pointCoordinates(pointCoordinatesArray[0], pointCoordinatesArray[1], pointCoordinatesArray[2]);
		
		absorptionDistance = std::max(absorptionDistance, dataSetCenter.getDistanceTo(pointCoordinates));
	}
	
	absorptionDistance *= this->dataPercentage;
	
	// This data structure maps a point index to a boolean flag denoting whether it has been absorped by a central point or has become a central point
	QMap<int, bool> pointAbsorptionStatus;
	
	// In the beginning all points are unabsorbed
	for (int i = 0; i < numberOfPoints; i++) {
		pointAbsorptionStatus[i] = false;
	}
	
	// Generate central points and assign close points to them
	for (int i = 0; i < numberOfPoints; i++) {
		if (pointAbsorptionStatus[i] == false) {
			double centralPointCoordinatesArray[3];
			input->GetPoint(i, centralPointCoordinatesArray);
			PointCoordinates centralPointCoordinates(centralPointCoordinatesArray[0], centralPointCoordinatesArray[1], centralPointCoordinatesArray[2]);
			QList<int> subordinatePointIndices;
			
			pointAbsorptionStatus[i] = true;
			
			for (int j = 0; j < numberOfPoints; j++) {
				if (pointAbsorptionStatus[j] == false) {
					double subordinatePointCoordinatesArray[3];
					input->GetPoint(j, subordinatePointCoordinatesArray);
					PointCoordinates subordinatePointCoordinates(subordinatePointCoordinatesArray[0], subordinatePointCoordinatesArray[1], subordinatePointCoordinatesArray[2]);
					
					if (centralPointCoordinates.getDistanceTo(subordinatePointCoordinates) <= absorptionDistance) {
						subordinatePointIndices.append(j);
						pointAbsorptionStatus[j] = true;
					}
				}
			}
			
			outputMap[centralPointCoordinates] = subordinatePointIndices;
		}
	}
	
	return outputMap;
}

vtkSmartPointer<vtkPolyData> DataDensityFilter::generateOutputData(QMap<PointCoordinates, QList<int>> centralPoints, vtkPointSet* input) {
	vtkSmartPointer<vtkPolyData> output = vtkSmartPointer<vtkPolyData>::New();
	
	// Create the content of the output poly data object
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
	vertices->Allocate(vertices->EstimateSize(1, centralPoints.size()));
	vertices->InsertNextCell(centralPoints.size());
	
	// Create all arrays from the input data
	QList<vtkSmartPointer<vtkDataArray>> inputArrays;
	QList<vtkSmartPointer<vtkDataArray>> outputArrays;

	for (int i = 0; i < input->GetPointData()->GetNumberOfArrays(); i++) {
		vtkSmartPointer<vtkDataArray> inputArray = input->GetPointData()->GetArray(i);

		if (!inputArray) {
			vtkErrorMacro( << "An input array could not be read.");
			return 0;
		}

		if (inputArray->IsNumeric()) {
			vtkSmartPointer<vtkDataArray> outputArray = vtkDataArray::CreateDataArray(inputArray->GetDataType());

			outputArray->SetNumberOfComponents(inputArray->GetNumberOfComponents());
			outputArray->SetNumberOfTuples(centralPoints.size());
			outputArray->SetName(inputArray->GetName());
			
			inputArrays.append(inputArray);
			outputArrays.append(outputArray);
		}
	}
	
	QMap<PointCoordinates, QList<int>>::const_iterator i = centralPoints.constBegin();
	int centralPointIndex = 0;
	while (i != centralPoints.constEnd()) {
	    vertices->InsertCellPoint(points->InsertNextPoint(i.key().getX(), i.key().getY(), i.key().getZ()));
		
		for (int j = 0; j < inputArrays.size(); j++) {
			std::vector<double> cumulativeAverage(inputArrays[j]->GetNumberOfComponents());
			for (int l = 0; l < inputArrays[j]->GetNumberOfComponents(); l++) {
				cumulativeAverage[l] = inputArrays[j]->GetTuple(centralPointIndex)[l] / float(2);
			}
			
			QListIterator<int> k(i.value());
			int subordinatePointCount = 1;
			while (k.hasNext()) {
				int pointIndex = k.next();
				
				for (int l = 0; l < inputArrays[j]->GetNumberOfComponents(); l++) {
					cumulativeAverage[l] = (subordinatePointCount * cumulativeAverage[l] + inputArrays[j]->GetTuple(pointIndex)[l]) / float(subordinatePointCount + 1);
				}
				
				subordinatePointCount++;
			}
			
			outputArrays[j]->InsertTuple(centralPointIndex, cumulativeAverage.data());
		}
		
		centralPointIndex++;
	    ++i;
	}
	
	output->SetPoints(points);
	output->SetVerts(vertices);
	
	// Add the output arrays to the data set
	QList<vtkSmartPointer<vtkDataArray>>::iterator j;
	for (j = outputArrays.begin(); j != outputArrays.end(); ++j) {
		output->GetPointData()->AddArray(*j);
	}
	
	return output;
}
