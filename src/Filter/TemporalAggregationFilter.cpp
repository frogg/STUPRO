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

const QList<Data::Type> TemporalAggregationFilter::SUPPORTED_DATA_TYPES = QList<Data::Type>() << Data::PRECIPITATION << Data::TEMPERATURE << Data::WIND << Data::CLOUD_COVERAGE;

TemporalAggregationFilter::TemporalAggregationFilter() : currentTimeStep(0) {
    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(1);
}

TemporalAggregationFilter::~TemporalAggregationFilter() { }

void TemporalAggregationFilter::fail(QString message) {
	vtkErrorMacro( << message.toStdString());
	this->error = true;
}

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
    if (this->error) {
      return 0;
    }

    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    
    if (inInfo->Has(Data::VTK_DATA_TYPE())) {
		this->dataType = static_cast<Data::Type>(inInfo->Get(Data::VTK_DATA_TYPE()));
	} else {
		this->fail("This filter only works with data read by the Kronos reader.");
		return 0;
	}
    
    // Overkill code for creating a comma-separated string with the names of all supported data types
    QString supportedTypes = "";
    int amountOfSupportedDataTypes = TemporalAggregationFilter::SUPPORTED_DATA_TYPES.size();
    if (amountOfSupportedDataTypes == 1) {
        supportedTypes.append(Data::getDataTypeName(TemporalAggregationFilter::SUPPORTED_DATA_TYPES.value(0)));
    } else if (amountOfSupportedDataTypes > 1) {
        for (int i = 0; i < amountOfSupportedDataTypes - 2; i++) {
            supportedTypes.append(Data::getDataTypeName(TemporalAggregationFilter::SUPPORTED_DATA_TYPES.value(i)));
            if (i < amountOfSupportedDataTypes - 3) {
                supportedTypes.append(", ");
            }
        }
        supportedTypes.append(" and ").append(Data::getDataTypeName(TemporalAggregationFilter::SUPPORTED_DATA_TYPES.value(amountOfSupportedDataTypes - 1)));
    }
    
    if (!TemporalAggregationFilter::SUPPORTED_DATA_TYPES.contains(this->dataType)) {
        this->fail(QString("This filter only supports %1 data, but the input contains %2 data.").arg(supportedTypes, Data::getDataTypeName(this->dataType)));
        return 0;
    }
    
    // This filter's output is an aggregation of values over time and therefore has no time information
    outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_RANGE());

    return 1;
}

int TemporalAggregationFilter::RequestData(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector) {
    if (this->error) {
      return 0;
    }
    
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // TODO: Step through the whole array and aggregate the data

    return 1;
}

int TemporalAggregationFilter::RequestUpdateExtent (
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector) {
    if (this->error) {
        return 0;
    }
    
    vtkInformation *inputInformation = inputVector[0]->GetInformationObject(0);

    // Make the pipeline executive iterate the upstream pipeline time steps by setting the update time step appropiately
    double *inputTimeSteps = inputInformation->Get(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    if (inputTimeSteps) {
        inputInformation->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), inputTimeSteps[this->currentTimeStep]);
    }

    return 1;
}