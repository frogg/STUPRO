//
//  TwitterHeatmapFilter.cpp
//  kronos
//
//  Created by Frederik Riedel on 12.02.16.
//
//


#include "TwitterHeatmapFilter.h"

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


TwitterHeatmapFilter::TwitterHeatmapFilter() {
	//take this transformation, true -> it transforms per default

	Locator = vtkSmartPointer<vtkPointLocator>::New();
	Locator->SetTolerance(toleranceValue);


}

void TwitterHeatmapFilter::setToleranceValue(int newTolerance) {
	toleranceValue = newTolerance;
	Locator->SetTolerance(toleranceValue);
	this->Modified();
}


void TwitterHeatmapFilter::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "Twitter Filter, Kronos Project" << endl;
}

TwitterHeatmapFilter::~TwitterHeatmapFilter() {}

vtkStandardNewMacro(TwitterHeatmapFilter)

int TwitterHeatmapFilter::RequestData(vtkInformation* info,
                                      vtkInformationVector** inputVector,
                                      vtkInformationVector* outputVector) {



	// Get output information from the request vectors
	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation* outInfo = outputVector->GetInformationObject(0);


	//-------------------------------------------------------
	// Get the actual objects from the obtained information

	//vtkDataObject* input = vtkDataObject::GetData(inputVector[0]);
	vtkDataSet* input = vtkDataSet::SafeDownCast(
	                        inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkUnstructuredGrid* output = vtkUnstructuredGrid::SafeDownCast(
	                                  outInfo->Get(vtkDataObject::DATA_OBJECT()));
	//Locator->SetTolerance(<#double _arg#>)

	//vtkDataObject* output = vtkDataObject::GetData(outputVector);

	//Locator->GetPoints();
	//// Check the input for compatibility
	//if (!input->IsA("vtkPolyData")) {
	//	//
	//	return 0;
	//}
	//return 1;
	//-------------------------------------------------------


	//if (input->GetNumberOfCells() == 0) {
	//	// set up a ugrid with same data arrays as input, but
	//	// no points, cells or data.
	//	output->Allocate(1);
	//	//output->GetPointData()->CopyAllocate(input->GetPointData(), VTK_CELL_SIZE);
	//	output->GetPointData()->CopyAllocate(input->GetPointData());
	//	output->GetCellData()->CopyAllocate(input->GetCellData(), 1);
	//	vtkPoints* pts = vtkPoints::New();
	//	output->SetPoints(pts);
	//	pts->Delete();
	//	return 1;
	//}

	//output->GetPointData()->CopyAllocate(input->GetPointData());
	output->GetCellData()->PassData(input->GetCellData());

	// First, create a new points array that eliminate duplicate points.
	// Also create a mapping from the old point id to the new.
	if (input->GetNumberOfCells() == 0) {
		// set up a ugrid with same data arrays as input, but
		// no points, cells or data.
		output->Allocate(1);
		output->GetPointData()->CopyAllocate(input->GetPointData());
		output->GetCellData()->CopyAllocate(input->GetCellData(), 1);
		vtkPoints* pts = vtkPoints::New();
		output->SetPoints(pts);
		pts->Delete();
		return 1;
	}

	output->GetPointData()->CopyAllocate(input->GetPointData());
	output->GetCellData()->PassData(input->GetCellData());

	vtkPoints* newPts = vtkPoints::New();
	vtkIdType num = input->GetNumberOfPoints();
	vtkIdType id;
	vtkIdType newId;
	vtkIdType* ptMap = new vtkIdType[num];
	double pt[3];

	this->Locator->InitPointInsertion(newPts, input->GetBounds(), num);

	vtkIdType progressStep = num / 100;
	if (progressStep == 0) {
		progressStep = 1;
	}
	for (id = 0; id < num; ++id) {
		if (id % progressStep == 0) {
			this->UpdateProgress(0.8 * ((float)id / num));
		}
		input->GetPoint(id, pt);
		if (this->Locator->InsertUniquePoint(pt, newId)) {
			output->GetPointData()->CopyData(input->GetPointData(), id, newId);
		}
		ptMap[id] = newId;
	}
	output->SetPoints(newPts);
	newPts->Delete();

	// Now copy the cells.
	vtkIdList* cellPoints = vtkIdList::New();
	num = input->GetNumberOfCells();
	output->Allocate(num);
	for (id = 0; id < num; ++id) {
		if (id % progressStep == 0) {
			this->UpdateProgress(0.8 + 0.2 * ((float)id / num));
		}
		//// special handling for polyhedron cells
		if (vtkUnstructuredGrid::SafeDownCast(input) &&
		        input->GetCellType(id) == VTK_POLYHEDRON) {
			vtkUnstructuredGrid::SafeDownCast(input)->GetFaceStream(id, cellPoints);
			vtkUnstructuredGrid::ConvertFaceStreamPointIds(cellPoints, ptMap);
		} else {
			input->GetCellPoints(id, cellPoints);
			for (int i = 0; i < cellPoints->GetNumberOfIds(); i++) {
				int cellPtId = cellPoints->GetId(i);
				newId = ptMap[cellPtId];
				cellPoints->SetId(i, newId);
			}
		}
		output->InsertNextCell(input->GetCellType(id), cellPoints);
	}

	delete[] ptMap;
	cellPoints->Delete();
	output->Squeeze();

	this->Locator->InitPointInsertion(newPts, input->GetBounds(), num);

	cout << "numberOfPoints" << Locator->GetNumberOfPointsPerBucket() << endl;


	return 1;
}

int TwitterHeatmapFilter::FillInputPortInformation(int port, vtkInformation* info) {
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
	return 1;
}

