#include <Reader/DataReader/MetaInformationKeys/vtkInformationDataStateKey.hpp>

#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>

vtkInformationDataStateKey::vtkInformationDataStateKey(const char* name,
        const char* location) : vtkInformationIntegerKey(name, location) { }

vtkInformationDataStateKey::~vtkInformationDataStateKey() { }

void vtkInformationDataStateKey::CopyDefaultInformation(
    vtkInformation* request,
    vtkInformation* fromInfo,
    vtkInformation* toInfo) {
	this->ShallowCopy(fromInfo, toInfo);
}

void vtkInformationDataStateKey::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
}