#include <Reader/DataReader/vtkInformationDataTypeKey.hpp>

#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>

vtkInformationDataTypeKey::vtkInformationDataTypeKey(const char *name, const char *location) : vtkInformationIntegerKey(name, location) { }

vtkInformationDataTypeKey::~vtkInformationDataTypeKey() { }

void vtkInformationDataTypeKey::CopyDefaultInformation(
  vtkInformation* request,
  vtkInformation* fromInfo,
  vtkInformation* toInfo) {
    this->ShallowCopy(fromInfo, toInfo);
}

void vtkInformationDataTypeKey::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);
}