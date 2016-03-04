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

	// Output the centroids to a new data set
	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
	vertices->Allocate(vertices->EstimateSize(1, coord0->GetNumberOfTuples()));
	vertices->InsertNextCell(coord0->GetNumberOfTuples());

	for (int i = 0; i < coord0->GetNumberOfTuples(); ++i) {
		vertices->InsertCellPoint(centroids->InsertNextPoint(coord0->GetValue(i), coord1->GetValue(i),
		                          coord2->GetValue(i)));
	}

	/*output->SetPoints(centroids);
	output->SetVerts(vertices);*/
	
	// TODO: Sort centroids and the points in their respective buckets into the map
	
	return QMap<PointCoordinates, QList<int>>();
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
	
	PointCoordinates dataSetCenter(minX + (width / 2.0), minY / (height / 2.0), minZ / (depth / 2.0));

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
	
	
	
	/*std::vector<int> absorbArray(input->GetNumberOfPoints(), -1);
	vtkSmartPointer<vtkPoints> resultPoints =
	    vtkSmartPointer<vtkPoints>::New();

	vtkSmartPointer<vtkFloatArray> temperatureArray = vtkFloatArray::SafeDownCast(
	            input->GetPointData()->GetArray("temperatures"));

	vtkSmartPointer<vtkFloatArray> tempValues = vtkSmartPointer<vtkFloatArray>::New();
	tempValues->SetNumberOfComponents(1);
	tempValues->SetName("temperatures");


	Get all attribute arrays
	for (int count = 0; count < input->GetPointData()->GetNumberOfArrays(); count++) {
		vtkErrorMacro( << "PointData stuff : " << input->GetPointData()->GetArrayName(count));
	}
	

	for (int r = 0; r < input->GetNumberOfPoints(); r++) {
		if (absorbArray[r] == -1) {
			double p[3];
			input->GetPoint(r, p);
			float temp = temperatureArray->GetValue(r);
			PointCoordinates firstCoords(p[0], p[1], p[2]);
			for (int j = 0; j < input->GetNumberOfPoints(); j++) {
				if (absorbArray[j] == -1) {
					double o[3];
					input->GetPoint(j, o);
					PointCoordinates secondCoords(o[0], o[1], o[2]);
					if (firstCoords.getDistanceTo(secondCoords) < 10) { // TODO
						temp = (temp + temperatureArray->GetValue(j)) / 2;
						absorbArray[j] = 0;
					}
				}
			}
			tempValues->InsertNextValue(temp);
			resultPoints->InsertNextPoint(p);
		}
	}
	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
	vertices->Allocate(vertices->EstimateSize(1, resultPoints->GetNumberOfPoints()));
	vertices->InsertNextCell(resultPoints->GetNumberOfPoints());

	for (int i = 0; i < resultPoints->GetNumberOfPoints(); i++) {
		vertices->InsertCellPoint(i);
	}

	output->SetPoints(resultPoints);
	output->SetVerts(vertices);
	output->GetPointData()->AddArray(tempValues);
	
	return QMap<PointCoordinates, QList<int>>();*/
}

vtkSmartPointer<vtkPolyData> DataDensityFilter::generateOutputData(QMap<PointCoordinates, QList<int>> centralPoints, vtkPointSet* input) {
	
}
