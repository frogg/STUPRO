#include <Reader/DataReader/MetaInformationKeys/vtkInformationDataTransformationKey.hpp>

#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>

vtkInformationDataTransformationKey::vtkInformationDataTransformationKey(const char* name,
		const char* location) : vtkInformationIntegerKey(name, location) { }

vtkInformationDataTransformationKey::~vtkInformationDataTransformationKey() { }

void vtkInformationDataTransformationKey::CopyDefaultInformation(
	vtkInformation* request,
	vtkInformation* fromInfo,
	vtkInformation* toInfo) {
	this->ShallowCopy(fromInfo, toInfo);
}

void vtkInformationDataTransformationKey::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
}
