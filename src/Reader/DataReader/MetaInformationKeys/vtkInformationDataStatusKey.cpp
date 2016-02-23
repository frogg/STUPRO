#include <Reader/DataReader/MetaInformationKeys/vtkInformationDataStatusKey.hpp>

#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>

vtkInformationDataStatusKey::vtkInformationDataStatusKey(const char* name,
        const char* location) : vtkInformationIntegerKey(name, location) { }

vtkInformationDataStatusKey::~vtkInformationDataStatusKey() { }

void vtkInformationDataStatusKey::CopyDefaultInformation(
    vtkInformation* request,
    vtkInformation* fromInfo,
    vtkInformation* toInfo) {
	this->ShallowCopy(fromInfo, toInfo);
}

void vtkInformationDataStatusKey::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
}