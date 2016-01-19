#include "InterpolateImageData.h"

#include <vtkObjectFactory.h>
#include <vtkInformation.h>
#include <vtkDataObject.h>
#include <vtkSmartPointer.h>
#include <vtkProbeFilter.h>

InterpolateImageData::InterpolateImageData() {
}

InterpolateImageData::~InterpolateImageData() {
}

vtkStandardNewMacro(InterpolateImageData)

int InterpolateImageData::RequestData(vtkInformation *info,
        vtkInformationVector **inputVector,
        vtkInformationVector *outputVector) {
    vtkSmartPointer<vtkProbeFilter> probe = vtkSmartPointer<vtkProbeFilter>::New();
}

void InterpolateImageData::PrintSelf(ostream &os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);
    os << indent << "Interpolate any data into an uniform grid, Kronos Project" << endl;
}

int InterpolateImageData::FillOutputPortInformation(int, vtkInformation *info) {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
    return 1;
}

int InterpolateImageData::FillInputPortInformation(int, vtkInformation *info) {
    info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkRectilinearGrid");
    return 1;
}
