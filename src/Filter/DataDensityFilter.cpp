#include <Filter/DataDensityFilter.h>

#include <vtkDataObject.h>
#include <vtkSmartPointer.h>
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
#include <Utils/Misc/PointCoordinates.hpp>

DataDensityFilter::DataDensityFilter() { }

void DataDensityFilter::setDataPercentage(double percentage) {
	this->dataPercentage = percentage;
	this->Modified();
}

void DataDensityFilter::setDensityDistance(double distance) {
	this->densityDistance = distance;
	this->Modified();
}

void DataDensityFilter::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "Twitter Filter, Kronos Project" << endl;
}

void DataDensityFilter::KMeansDensityFilter(vtkPolyData* input, vtkPolyData* output) {

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

	output->SetPoints(centroids);
	output->SetVerts(vertices);
}

void DataDensityFilter::NormalFilter(vtkPolyData* input, vtkPolyData* output) {

	std::vector<int> absorbArray(input->GetNumberOfPoints(), -1);
	vtkSmartPointer<vtkPoints> resultPoints =
	    vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
	vertices->Allocate(vertices->EstimateSize(1, resultPoints->GetNumberOfPoints()));
	vertices->InsertNextCell(resultPoints->GetNumberOfPoints());

	for (int r = 0; r < input->GetNumberOfPoints(); ++r) {
		if (absorbArray[r] == -1) {
			double p[3];
			input->GetPoint(r, p);
			PointCoordinates firstCoords(p[1], p[2], p[3]);

			for (int j = 0; j < input->GetNumberOfPoints(); ++j) {
				if (absorbArray[j] == -1) {
					double o[3];
					input->GetPoint(j, o);
					PointCoordinates secondCoords(o[1], o[2], o[3]);
					if (firstCoords.getDistanceTo(secondCoords) <= densityDistance) {
						absorbArray[j] = 0;
					}
				}
			}
			vertices->InsertCellPoint(resultPoints->InsertNextPoint(p));
		}
	}
	output->SetPoints(resultPoints);
	output->SetVerts(vertices);
}

DataDensityFilter::~DataDensityFilter() {}

vtkStandardNewMacro(DataDensityFilter)

int DataDensityFilter::RequestData(vtkInformation* info,
                                   vtkInformationVector** inputVector,
                                   vtkInformationVector* outputVector) {
	// Get output information from the request vectors
	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	// Get the actual objects from the obtained information
	vtkPolyData* input = vtkPolyData::SafeDownCast(inInfo->Get(vtkPolyData::DATA_OBJECT()));
	vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkPolyData::DATA_OBJECT()));

	NormalFilter(input, output);

	return 1;
}

int DataDensityFilter::FillOutputPortInformation(int port, vtkInformation* info) {
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
	return 1;
}

int DataDensityFilter::FillInputPortInformation(int port, vtkInformation* info) {
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
	return 1;
}
