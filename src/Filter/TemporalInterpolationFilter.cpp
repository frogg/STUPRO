#include <Filter/TemporalInterpolationFilter.h>

#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <Utils/Misc/MakeUnique.hpp>

#include <Filter/TemporalInterpolationFilter/PrecipitationInterpolationValue.hpp>
#include <Filter/TemporalInterpolationFilter/TemperatureInterpolationValue.hpp>
#include <Filter/TemporalInterpolationFilter/WindInterpolationValue.hpp>
#include <Filter/TemporalInterpolationFilter/CloudCoverageInterpolationValue.hpp>
#include <Filter/TemporalInterpolationFilter/TwitterInterpolationValue.hpp>

vtkStandardNewMacro(TemporalInterpolationFilter);

const QList<Data::Type> TemporalInterpolationFilter::SUPPORTED_DATA_TYPES = QList<Data::Type>() <<
        Data::PRECIPITATION << Data::TEMPERATURE << Data::WIND << Data::CLOUD_COVERAGE << Data::TWEETS;

TemporalInterpolationFilter::TemporalInterpolationFilter() : preprocessed(false), error(false), currentTimeStep(0) {
	this->SetNumberOfInputPorts(1);
	this->SetNumberOfOutputPorts(1);
}

TemporalInterpolationFilter::~TemporalInterpolationFilter() {
    for(int i=0; i<this->timestampMap.count();i++){
        qDeleteAll(timestampMap[i]);
    }
    
}

void TemporalInterpolationFilter::fail(QString message) {
	vtkErrorMacro( << message.toStdString());
	this->error = true;
}

void TemporalInterpolationFilter::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);

	os << indent << "Temporal data point interpolation, Kronos Project" << endl;
}

int TemporalInterpolationFilter::FillInputPortInformation(int port, vtkInformation* info) {
	if (port == 0) {
		info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
		info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
	}

	return 1;
}

int TemporalInterpolationFilter::FillOutputPortInformation(int port, vtkInformation* info) {
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
	return 1;
}

int TemporalInterpolationFilter::RequestInformation (
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) {
	if (this->error) {
		return 0;
	}

	vtkInformation* outInfo = outputVector->GetInformationObject(0);
	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);

	if (inInfo->Has(Data::VTK_DATA_TYPE())) {
		this->dataType = static_cast<Data::Type>(inInfo->Get(Data::VTK_DATA_TYPE()));
	} else {
		this->fail("This filter only works with data read by the Kronos reader.");
		return 0;
	}

	// Overkill code for creating a comma-separated string with the names of all supported data types
	QString supportedTypes = "";
	int amountOfSupportedDataTypes = TemporalInterpolationFilter::SUPPORTED_DATA_TYPES.size();
	if (amountOfSupportedDataTypes == 1) {
		supportedTypes.append(Data::getDataTypeName(TemporalInterpolationFilter::SUPPORTED_DATA_TYPES.value(
		                          0)));
	} else if (amountOfSupportedDataTypes > 1) {
		for (int i = 0; i < amountOfSupportedDataTypes - 2; i++) {
			supportedTypes.append(Data::getDataTypeName(TemporalInterpolationFilter::SUPPORTED_DATA_TYPES.value(
			                          i)));
			if (i < amountOfSupportedDataTypes - 3) {
				supportedTypes.append(", ");
			}
		}
		supportedTypes.append(" and ").append(Data::getDataTypeName(
		        TemporalInterpolationFilter::SUPPORTED_DATA_TYPES.value(amountOfSupportedDataTypes - 1)));
	}

	if (!TemporalInterpolationFilter::SUPPORTED_DATA_TYPES.contains(this->dataType)) {
		this->fail(QString("This filter only supports %1 data, but the input contains %2 data.").arg(
		               supportedTypes, Data::getDataTypeName(this->dataType)));
		return 0;
	}
    
    // TODO: Check the data's state for compatibility

	return 1;
}

int TemporalInterpolationFilter::RequestData(
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) {
	if (this->error) {
		return 0;
	}
    
    if (this->hasPreprocessed()) {
        // TODO: Output the interpolated data set from the preprocessed data using the time in the request
        request->Set(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING(), 0);
        return 0;
    }

	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	vtkPolyData* input = vtkPolyData::GetData(inInfo);
	vtkPolyData* output = vtkPolyData::GetData(outInfo);

	if (this->currentTimeStep == 0) {
		this->SetProgressText("Interpolating data...");
		this->SetProgress(0.0);
	}

    // std::cout << "Current timestep: " << this->currentTimeStep << std::endl;
    this->updateQMap(this->currentTimeStep, input);
    
	if (this->currentTimeStep < inInfo->Length(vtkStreamingDemandDrivenPipeline::TIME_STEPS())) {
		// There are still time steps left, continue on
		this->currentTimeStep++;

		// Update the progress
		double progress = (double) (this->currentTimeStep / (1.0 * inInfo->Length(
		                                vtkStreamingDemandDrivenPipeline::TIME_STEPS())));
		this->UpdateProgress(progress);
	} else {
		// The data points have been iterated
		request->Remove(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING());
		this->currentTimeStep = 0;
		this->SetProgressText("");
		this->SetProgress(1.0);
        this->addDataInFirstTimeStep();
        this->addDataInLastTimeStep();
        this->interpolateData();
        std::cout << "Total number of points: " << this->allPointCooridinates.count();
        
        this->printData();
        this->preprocessed = true;
        
	}
    
    request->Set(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING(), 1);
	return 1;
}

void TemporalInterpolationFilter::interpolateData(){
    
    //interate over all coodinates
    for(PointCoordinates coord : this->allPointCooridinates){
        int firstTimestep = 0;
        //int numberOfGaps = 0;
        bool foundGap = false;
        //test for a specific coordinate all timesteps
    for(int i=1; i<this->timestampMap.count();i++){
        //nicht mehr lineare Laufzeit, weil contain nicht in O(1) geht
        //check if coordinate exists at this timestep
        if(timestampMap[i].contains(coord)){
            if(foundGap){
                //alle Zwischenpunkte interpolieren und in Map einfügen
                //Zeitschritte
                for(int x = firstTimestep+1; x<i; x++){
                    int distanceToFirstTimestep = x-firstTimestep;
                    this->interpolateDataPoint(timestampMap[firstTimestep][coord], timestampMap[i][coord], distanceToFirstTimestep, x, coord, i-firstTimestep);
                }
                firstTimestep = i;
                foundGap = false;
            }else{
                firstTimestep++;
            }
        }else{
            if(!foundGap){
                foundGap = true;
            }
        }
    }
    }
}

void TemporalInterpolationFilter::interpolateDataPoint(InterpolationValue* lower, InterpolationValue* higher, int distanceToFirstInterpolationTimestep, int index, PointCoordinates coordinate, int distance){
    //noch interpolieren erstmal  higher und lower genommen
    this->timestampMap[index].insert(coordinate,lower);
    //std::cout << "interpolateDataPoint" << std::endl;
    //TODO interpolieren
    //this->timestampMap[index].insert(coordinate,this->timestampMap[i][coordinate]);
    /*switch (this->dataType) {
        case Data::TEMPERATURE:{
            vtkSmartPointer<vtkDataArray> abstractTemperatureArray = inputData->GetPointData()->GetArray("temperatures");
            vtkSmartPointer<vtkTypeFloat32Array> temperatureArray = vtkTypeFloat32Array::SafeDownCast(abstractTemperatureArray);
            double currentTemperature = temperatureArray->GetValue(pointIndex);
            return 	TemperatureDataPoint(Coordinate(1.0,1.0),5,334,currentTemperature);
            break;
        }
        case Data::TWEETS:
            break;
        case Data::PRECIPITATION:
            break;
        case Data::WIND:
            break;
        case Data::CLOUD_COVERAGE:
            break;
        default:
            std::cout << "not corrrect!!!";
            //warning
            break;
    }
    */

}

void TemporalInterpolationFilter::updateQMap(int timestep, vtkPolyData *inputData){
    //QList<int> knownPoints;
    //auto content = makeUnique<QMap<PointCoordinates, TemporalDataPoint>>() ;
    QMap<PointCoordinates, InterpolationValue*> content;
    for (int i = 0; i < inputData->GetNumberOfPoints(); i++) {
        double coordinates[3];
        inputData->GetPoint(i, coordinates);
        PointCoordinates currentCoordinates(coordinates[0], coordinates[1], coordinates[2]);
        content.insert(currentCoordinates, this->createDataPoint(i, inputData));
        
        if(!this->allPointCooridinates.contains(currentCoordinates)){
            this->allPointCooridinates.append(currentCoordinates);
        }
        //this->dataAggregator
        //knownPoints.append(i);
      //  std::cout << dynamic_cast<TemperatureDataPoint>(content[currentCoordinates]) ;
    }
    //std::cout << "contentSize: " << content.count();
    this->timestampMap.insert(timestep,content);
}

InterpolationValue* TemporalInterpolationFilter::createDataPoint(int pointIndex, vtkPolyData *inputData) {
    vtkSmartPointer<vtkIntArray> priorityArray = vtkIntArray::SafeDownCast(inputData->GetPointData()->GetArray("priorities"));
    int priority = priorityArray->GetValue(pointIndex);
    
    vtkSmartPointer<vtkIntArray> timestampArray = vtkIntArray::SafeDownCast(inputData->GetPointData()->GetArray("timestamps"));
    int timestamp = timestampArray->GetValue(pointIndex);
    
    switch (this->dataType) {
        case Data::TEMPERATURE:{
            vtkSmartPointer<vtkFloatArray> temperatureArray = vtkFloatArray::SafeDownCast(inputData->GetPointData()->GetArray("temperatures"));
            return new TemperatureInterpolationValue(priority, timestamp, temperatureArray->GetValue(pointIndex));
            break;
        }
        case Data::TWEETS: {
            vtkSmartPointer<vtkFloatArray> densityArray = vtkFloatArray::SafeDownCast(inputData->GetPointData()->GetArray("density"));
            return new TwitterInterpolationValue(priority, timestamp, densityArray->GetValue(pointIndex));
            break;
        }
        case Data::PRECIPITATION: {
            vtkSmartPointer<vtkFloatArray> precipitationRateArray = vtkFloatArray::SafeDownCast(inputData->GetPointData()->GetArray("precipitationRates"));
            vtkSmartPointer<vtkIntArray> precipitationTypeArray = vtkIntArray::SafeDownCast(inputData->GetPointData()->GetArray("precipitationTypes"));
            return new PrecipitationInterpolationValue(priority, timestamp, precipitationRateArray->GetValue(pointIndex), static_cast<PrecipitationDataPoint::PrecipitationType>(precipitationTypeArray->GetValue(pointIndex)));
            break;
        }
        case Data::WIND: {
            vtkSmartPointer<vtkFloatArray> bearingArray = vtkFloatArray::SafeDownCast(inputData->GetPointData()->GetArray("directions"));
            vtkSmartPointer<vtkFloatArray> velocityArray = vtkFloatArray::SafeDownCast(inputData->GetPointData()->GetArray("speeds"));
            return new WindInterpolationValue(priority, timestamp, bearingArray->GetValue(pointIndex), velocityArray->GetValue(pointIndex));
            break;
        }
        case Data::CLOUD_COVERAGE: {
            vtkSmartPointer<vtkFloatArray> cloudCoverageArray = vtkFloatArray::SafeDownCast(inputData->GetPointData()->GetArray("cloudCovers"));
            return new CloudCoverageInterpolationValue(priority, timestamp, cloudCoverageArray->GetValue(pointIndex));
            break;
        }
        default: {
            this->fail("The data type of this filter seems to be invalid.");
            return new InterpolationValue();
            break;
        }
    }
}

void TemporalInterpolationFilter::addDataInFirstTimeStep(){
  //  std::cout << "Anzahl an erstem Zeitschritt: " <<this->timestampMap[0].count();
    //copy-on-write
    QList<PointCoordinates> missingCoordinates = this->allPointCooridinates;
    for(auto knownCoordinate : this->timestampMap[0].keys()){
        missingCoordinates.removeAll(knownCoordinate);
    }
    
    //as not sure about interator order
    for(int i=1; i<this->timestampMap.count(); i++){
            QList<PointCoordinates> temp = missingCoordinates;
            for(auto coordinate: temp){
                //if timestep i has missingPoint, append it on timestep 0
                //ÜBERPRÜFEN MIT C++ 
                if(this->timestampMap[i].contains(coordinate)){
                    this->timestampMap[0].insert(coordinate,this->timestampMap[i][coordinate]);
                    missingCoordinates.removeAll(coordinate);
                }
            }
        if(missingCoordinates.count() == 0){
        break;
        }
    }
    
   // std::cout << "Anzahl an erstem Zeitschritt: danach" <<this->timestampMap[0].count();
    
}

bool TemporalInterpolationFilter::hasPreprocessed() {
    return this->preprocessed;
}

void TemporalInterpolationFilter::printData(){
    for(int i=0; i<this->timestampMap.count(); i++){
        std::cout << "Number of Points in in Timestep: " << i << ": " << this->timestampMap[i].count() <<std::endl;
    }
    std::cout << "Gesamtanzahl" <<this->timestampMap.count() <<std::endl;
}

void TemporalInterpolationFilter::addDataInLastTimeStep(){
    QList<PointCoordinates> missingCoordinates = this->allPointCooridinates;
    
    //last timestep
    int lastTimestep = this->timestampMap.count()-1;

    for(auto knownCoordinate : this->timestampMap[lastTimestep].keys()){
        missingCoordinates.removeAll(knownCoordinate);
    }
    
    //as not sure about interator order
    for(int i=lastTimestep-1; i>=0; i--){
        QList<PointCoordinates> temp = missingCoordinates;
        for(auto coordinate: temp){
            //if timestep i has missingPoint, append it on timestep n
            //ÜBERPRÜFEN MIT C++
            if(this->timestampMap[i].contains(coordinate)){
                this->timestampMap[lastTimestep].insert(coordinate,this->timestampMap[i][coordinate]);
                missingCoordinates.removeAll(coordinate);
            }
        }
        if(missingCoordinates.count() == 0){
            break;
        }
    }
    std::cout << "Anzahl an letzen Zeitschritt: danach" <<this->timestampMap[lastTimestep].count();
}


int TemporalInterpolationFilter::RequestUpdateExtent (
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) {
	if (this->error) {
		return 0;
	}

	vtkInformation* inputInformation = inputVector[0]->GetInformationObject(0);

	// Make the pipeline executive iterate the upstream pipeline time steps by setting the update time step appropiately
	double* inputTimeSteps = inputInformation->Get(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
	if (inputTimeSteps) {
		inputInformation->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), this->currentTimeStep);
	}

	return 1;
}