

#include "DataDensityFilter.h"
#include <Utils/Misc/KronosLogger.hpp>

#include "vtkUnstructuredGrid.h"
#include "vtkCell.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkObjectFactory.h"

#include "vtkDataObject.h"
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSet.h>
#include <vtkTable.h>
#include <vtkDoubleArray.h>
#include <vtkKMeansStatistics.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkCellArray.h>

#include <vtkCellData.h>
#include "vtkCell.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkObjectFactory.h"

#include "vtkIntArray.h"
#include "vtkCollection.h"
#include "vtkMergePoints.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

#include <sstream>

DataDensityFilter::DataDensityFilter() {}

void DataDensityFilter::setToleranceValue(int newTolerance) {
	toleranceValue = newTolerance;
	this->Modified();
}


void DataDensityFilter::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "Twitter Filter, Kronos Project" << endl;
}

DataDensityFilter::~DataDensityFilter() {}

vtkStandardNewMacro(DataDensityFilter)

int DataDensityFilter::RequestData(vtkInformation* info,
                                   vtkInformationVector** inputVector,
                                   vtkInformationVector* outputVector) {



	// Get output information from the request vectors
	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	//-------------------------------------------------------
	// Get the actual objects from the obtained information
	vtkPolyData* input = vtkPolyData::SafeDownCast(
	                        inInfo->Get(vtkPolyData::DATA_OBJECT()));
	vtkPolyData* output = vtkPolyData::SafeDownCast(
	                                  outInfo->Get(vtkPolyData::DATA_OBJECT()));
	output->GetCellData()->PassData(input->GetCellData());
	output->GetPointData()->PassData(input->GetPointData());

	// Get the points into the format needed for KMeans
	vtkSmartPointer<vtkTable> inputData =
	    vtkSmartPointer<vtkTable>::New();

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
	//kMeansStatistics->SetColumnStatus( "Testing", 1 );
	kMeansStatistics->RequestSelectedColumns();
	kMeansStatistics->SetAssessOption(true);
	kMeansStatistics->SetDefaultNumberOfClusters(toleranceValue);
	kMeansStatistics->Update();

	// Display the results
	// kMeansStatistics->GetOutput()->Dump();


	/* The clusters
	vtkSmartPointer<vtkIntArray> clusterArray =
	    vtkSmartPointer<vtkIntArray>::New();
	clusterArray->SetNumberOfComponents(1);
	clusterArray->SetName("ClusterId");

	for (int r = 0; r < kMeansStatistics->GetOutput()->GetNumberOfRows(); r++) {
		vtkVariant v = kMeansStatistics->GetOutput()->GetValue(r,
		               kMeansStatistics->GetOutput()->GetNumberOfColumns() - 1);
		std::cout << "Point " << r << " is in cluster " << v.ToInt() << std::endl;
		clusterArray->InsertNextValue(v.ToInt());
	}
	*/


	vtkSmartPointer<vtkPoints> centroids =
	    vtkSmartPointer<vtkPoints>::New();

	// Output the cluster centers / centroids

	vtkMultiBlockDataSet* outputMetaDS = vtkMultiBlockDataSet::SafeDownCast(
	        kMeansStatistics->GetOutputDataObject(vtkStatisticsAlgorithm::OUTPUT_MODEL));
	vtkSmartPointer<vtkTable> outputMeta = vtkTable::SafeDownCast(outputMetaDS->GetBlock(0));
	//vtkSmartPointer<vtkTable> outputMeta = vtkTable::SafeDownCast( outputMetaDS->GetBlock( 1 ) );
	vtkDoubleArray* coord0 = vtkDoubleArray::SafeDownCast(outputMeta->GetColumnByName("coord 0"));
	vtkDoubleArray* coord1 = vtkDoubleArray::SafeDownCast(outputMeta->GetColumnByName("coord 1"));
	vtkDoubleArray* coord2 = vtkDoubleArray::SafeDownCast(outputMeta->GetColumnByName("coord 2"));
	
	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
	vertices->Allocate(vertices->EstimateSize(1, coord0->GetNumberOfTuples()));
	vertices->InsertNextCell(coord0->GetNumberOfTuples());

	for (unsigned int i = 0; i < coord0->GetNumberOfTuples(); ++i) {
		vertices->InsertCellPoint(centroids->InsertNextPoint(coord0->GetValue(i), coord1->GetValue(i), coord2->GetValue(i)));
		/*std::cout << coord0->GetValue(i) << " " << coord1->GetValue(i) << " " << coord2->GetValue(
		              i) << std::endl;*/
	}
	
	output->SetPoints(centroids);
	output->SetVerts(vertices);

	/*vtkIdType num = centroids->GetNumberOfPoints();

	output->Allocate(num);
	output->SetPoints(centroids);





	output->Squeeze();*/

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
