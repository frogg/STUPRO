#include <Filter/TemporalAggregationFilter.h>

#include "vtkCellData.h"
#include "vtkCompositeDataIterator.h"
#include "vtkDataSet.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPointSet.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkMultiBlockDataSet.h"

#include <algorithm>
#include <vector>

vtkStandardNewMacro(TemporalAggregationFilter);

TemporalAggregationFilter::TemporalAggregationFilter() {
    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(1);
}

TemporalAggregationFilter::~TemporalAggregationFilter() { }

void TemporalAggregationFilter::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);

    os << indent << "Temporal data point aggregation, Kronos Project" << endl;
}

int TemporalAggregationFilter::FillInputPortInformation(int port, vtkInformation* info) {
    if (port == 0) {
		info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
		info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
	}
    
    return 1;
}

int TemporalAggregationFilter::FillOutputPortInformation(int port, vtkInformation* info) {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPointSet");
    return 1;
}

int TemporalAggregationFilter::RequestInformation (
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector) {
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

    // TODO: Set this filter's data type for deciding later on what kind of aggregation should be performed
    // TODO: Check the data type for compatibility
    
    // This filter's output is an aggregation of values over time and therefore has no time information
    outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_RANGE());

    return 1;
}

int TemporalAggregationFilter::RequestData(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector) {
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // TODO: Step through the whole array and aggregate the data

    return 1;
}

int TemporalAggregationFilter::RequestUpdateExtent (
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector) {
    // TODO: Make the VTK executive step through all time steps in order
    
    return 1;
}