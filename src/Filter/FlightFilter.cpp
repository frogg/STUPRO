#include <Filter/FlightFilter.h>

#include <Reader/DataReader/Data.hpp>

#include <vtkObjectFactory.h>
#include <vtkPointSet.h>
#include <vtkFloatArray.h>
#include <vtkStringArray.h>

FlightFilter::FlightFilter() {
	this->airlineMatchingMode = FlightFilter::CONTAINING;
}

FlightFilter::~FlightFilter() { }

vtkStandardNewMacro(FlightFilter);

QList<Data::Type> FlightFilter::getCompatibleDataTypes() {
	return (QList<Data::Type>() << Data::FLIGHTS);
}

bool FlightFilter::evaluatePoint(int pointIndex, Coordinate coordinate,
                                 vtkPointData* pointData) {
	return this->isVisibleBasedOnAirline(pointIndex, pointData)
	       && this->isVisibleBasedOnOriginAirportCode(pointIndex, pointData)
	       && this->isVisibleBasedOnDestinationAirportCode(pointIndex, pointData)
	       && this->isVisibleBasedOnFlightLength(pointIndex, pointData);
}

bool FlightFilter::isVisibleBasedOnAirline(int pointIndex, vtkPointData* pointData) {
	if (this->visibleAirlines.count() == 0) {
		return true;
	} else {
		vtkSmartPointer<vtkStringArray> airlines = vtkStringArray::SafeDownCast(
		            pointData->GetAbstractArray("airlines"));

		if (!airlines) {
			return false;
		}

		QString airline = QString::fromStdString(airlines->GetValue(pointIndex));
		airline.remove(' ');

		//check if the given airline is contained in a airline in visibleAirlines
		for (int i = 0; i < visibleAirlines.count(); i++) {
			if (this->airlineMatchingMode == CONTAINING) {
				//containing Mode
				if (airline.contains(visibleAirlines.at(i), Qt::CaseInsensitive)) {
					return true;
				}
			} else if (this->airlineMatchingMode == MATCHING) {
				//exact match
				if (QString::compare(airline, visibleAirlines.at(i), Qt::CaseInsensitive) == 0) {
					return true;
				}
			}
		}
		return false;
	}
}

bool FlightFilter::isVisibleBasedOnOriginAirportCode(int pointIndex, vtkPointData* pointData) {
	vtkSmartPointer<vtkStringArray> originAirportCodes = vtkStringArray::SafeDownCast(
	            pointData->GetAbstractArray("originAirportCodes"));

	if (!originAirportCodes) {
		return false;
	}

	QString originAirportCode = QString::fromStdString(originAirportCodes->GetValue(pointIndex));

	if (this->visibleOriginAirportCodes.count() == 0) {
		return true;
	} else {
		for (int i = 0; i < visibleOriginAirportCodes.count(); i++) {
			//accept only exact match
			if (QString::compare(originAirportCode, visibleOriginAirportCodes.at(i),
			                     Qt::CaseInsensitive) == 0) {
				return true;
			}
		}
		return false;
	}
}

bool FlightFilter::isVisibleBasedOnDestinationAirportCode(int pointIndex, vtkPointData* pointData) {
	vtkSmartPointer<vtkStringArray> destinationAirportCodes = vtkStringArray::SafeDownCast(
	            pointData->GetAbstractArray("destinationAirportCodes"));

	if (!destinationAirportCodes) {
		return false;
	}

	QString destinationAirportCode = QString::fromStdString(destinationAirportCodes->GetValue(
	                                     pointIndex));
	if (this->visibleDestinationAirportCodes.count() == 0) {
		return true;
	} else {
		for (int i = 0; i < visibleDestinationAirportCodes.count(); i++) {
			//accept only exact match
			if (QString::compare(destinationAirportCode, visibleDestinationAirportCodes.at(i),
			                     Qt::CaseInsensitive) == 0) {
				return true;
			}
		}
		return false;
	}
}

bool FlightFilter::isVisibleBasedOnFlightLength(int pointIndex, vtkPointData* pointData) {
	vtkSmartPointer<vtkFloatArray> flightLengths = vtkFloatArray::SafeDownCast(
	            pointData->GetAbstractArray("flightLengths"));

	if (!flightLengths) {
		return false;
	}

	double flightLength = flightLengths->GetTuple1(pointIndex);
	return this->minFlightLength <= flightLength && flightLength <= this->maxFlightLength;
}

void FlightFilter::updateStringList(QString inputString, QStringList& list) {
	inputString.remove(' ');
	if (QString::compare(inputString, "") == 0) {
		list.clear();
	} else {
		list = inputString.split( "," );
	}
}


void FlightFilter::setOriginAirportCode(const char* originAirportCode) {
	QString airportCodesOrigin = QString::fromStdString(originAirportCode);
	this->updateStringList(airportCodesOrigin, this->visibleOriginAirportCodes);
	this->Modified();
}


void FlightFilter::setDestinationAirportCode(const char* destinationAirportCode) {
	QString airportCodesDestination = QString::fromStdString(destinationAirportCode);
	this->updateStringList(airportCodesDestination, this->visibleDestinationAirportCodes);
	this->Modified();
}

void FlightFilter::setFlightLengthThreshold(double lowerLimit, double upperLimit) {
	this->minFlightLength = lowerLimit;
	this->maxFlightLength = upperLimit;
	this->Modified();
}

void FlightFilter::setAirlineMatchingMode(int matchingMode) {
	this->airlineMatchingMode = static_cast<Mode>(matchingMode);
	this->Modified();
}

void FlightFilter::setAirline(const char* airline) {
	QString airlines = QString::fromStdString(airline);
	this->updateStringList(airlines, this->visibleAirlines);
	this->Modified();
}

void FlightFilter::SetInputConnection(vtkAlgorithmOutput* input) {
	this->Superclass::SetInputConnection(input);
}


