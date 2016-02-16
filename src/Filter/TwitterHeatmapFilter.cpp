//
//  TwitterHeatmapFilter.cpp
//  kronos
//
//  Created by Frederik Riedel on 12.02.16.
//
//


#include "TwitterHeatmapFilter.h"
#include "vtkObjectFactory.h"
#include "vtkDataObject.h"
#include <vtkInformationVector.h>


TwitterHeatmapFilter::TwitterHeatmapFilter() {
	//take this transformation, true -> it transforms per default

	Locator = vtkSmartPointer<vtkPointLocator>::New();

}

void TwitterHeatmapFilter::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "Twitter Filter, Kronos Project" << endl;
}

TwitterHeatmapFilter::~TwitterHeatmapFilter() {
}

vtkStandardNewMacro(TwitterHeatmapFilter)

int TwitterHeatmapFilter::RequestData(vtkInformation* info,
                                      vtkInformationVector** inputVector,
                                      vtkInformationVector* outputVector) {


	// Get output information from the request vectors
	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	// Get the actual objects from the obtained information
	vtkDataObject* input = vtkDataObject::GetData(inputVector[0]);

	//Locator->SetTolerance(<#double _arg#>)


	vtkDataObject* output = vtkDataObject::GetData(outputVector);

	Locator->GetPoints();

	// Check the input for compatibility
	if (!input->IsA("vtkPolyData")) {
		//
		return 0;
	}
	return 1;
}

int TwitterHeatmapFilter::FillInputPortInformation(int port, vtkInformation* info) {

	return 1;
}

