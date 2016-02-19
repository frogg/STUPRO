#include <Reader/DataReader/vtkInformationTimeResolutionKey.hpp>

#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>

vtkInformationTimeResolutionKey::vtkInformationTimeResolutionKey(const char* name,
        const char* location) : vtkInformationIntegerKey(name, location) { }

vtkInformationTimeResolutionKey::~vtkInformationTimeResolutionKey() { }

void vtkInformationTimeResolutionKey::CopyDefaultInformation(
    vtkInformation* request,
    vtkInformation* fromInfo,
    vtkInformation* toInfo) {
	this->ShallowCopy(fromInfo, toInfo);
}

void vtkInformationTimeResolutionKey::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
}