#include "InterpolateImageData.h"

#include <vtkObjectFactory.h>
#include <vtkInformation.h>
#include <vtkDataObject.h>
#include <vtkSmartPointer.h>
#include <vtkProbeFilter.h>
#include <vtkImageData.h>
#include <vtkImageTranslateExtent.h>

InterpolateImageData::InterpolateImageData() {
}

InterpolateImageData::~InterpolateImageData() {
}

vtkSmartPointer<vtkImageData> InterpolateImageData::createInterpolationStructure(vtkDataSet* data) {
	vtkSmartPointer<vtkImageData>  retVal = vtkSmartPointer<vtkImageData>::New();

	double dataBounds[6];
	data->GetBounds(dataBounds);
	int resolution[] = {10, 10, 10};
	double spacing[3];
	// calculate the necessary spacing to meet the required bounds
	for (int i = 0; i < 3; i++) {
		if (resolution[i] == 0) {
			spacing[i] = dataBounds[2 * i + 1] - dataBounds[2 * i];
		} else {
			spacing[i] = (dataBounds[2 * i + 1] - dataBounds[2 * i]) / ((double) resolution[i]);
		}
	}

	retVal->SetExtent(0, resolution[0], 0, resolution[1], 0, resolution[2]);
	retVal->SetOrigin(dataBounds[0], dataBounds[2], dataBounds[4]);
	retVal->SetSpacing(spacing);

	return retVal;
}

vtkStandardNewMacro(InterpolateImageData)

int InterpolateImageData::RequestData(vtkInformation* info,
                                      vtkInformationVector** inputVector,
                                      vtkInformationVector* outputVector) {
	vtkSmartPointer<vtkProbeFilter> probe = vtkSmartPointer<vtkProbeFilter>::New();
	vtkImageData* output = vtkImageData::GetData(outputVector);
	vtkDataSet* input = vtkDataSet::GetData(inputVector[0]);
	probe->SetSourceData(input);
	probe->SetInputData(this->createInterpolationStructure(input));
	probe->SpatialMatchOn();
	probe->PassCellArraysOn();
	probe->PassFieldArraysOn();
	probe->PassPointArraysOn();
	probe->ComputeToleranceOn();
	probe->Update();

	output->CopyStructure(probe->GetOutput());

	return 1;
}

void InterpolateImageData::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "Interpolate any data into an uniform grid, Kronos Project" << endl;
}

int InterpolateImageData::FillOutputPortInformation(int, vtkInformation* info) {
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
	return 1;
}

int InterpolateImageData::FillInputPortInformation(int, vtkInformation* info) {
	info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkRectilinearGrid");
	return 1;
}
