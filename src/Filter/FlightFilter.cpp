#include <Filter/FlightFilter.h>

#include <Reader/DataReader/Data.hpp>


#include <vtkObjectFactory.h>
#include <vtkPointSet.h>
#include <vtkTypeFloat32Array.h>
#include <vtkStringArray.h>



FlightFilter::FlightFilter() {
	this->modeAirline = CONTAINING;
	this->minFlightLength = 0;
	this->maxFlightLength = 20000.0;
}

FlightFilter::~FlightFilter() { }



vtkStandardNewMacro(FlightFilter);

QList<Data::Type> FlightFilter::getCompatibleDataTypes() {
	return (QList<Data::Type>() << Data::FLIGHTS);
}


bool FlightFilter::evaluatePoint(int pointIndex, Coordinate coordinate,
                                 vtkPointData* pointData) {
	return this->visibleAirline(pointIndex, pointData)
	       && this->visibleAirportCodeOrigin(pointIndex, pointData)
	       && this->visibleAirportCodeDestination(pointIndex, pointData)
	       && this->visibleLength(pointIndex, pointData);
}

bool FlightFilter::visibleAirline(int pointIndex, vtkPointData* pointData) {
	if (this->visibleAirlines.count() == 0) {
		return true;
	} else {
		vtkSmartPointer<vtkStringArray> airlines = vtkStringArray::SafeDownCast(
		            pointData->GetAbstractArray("airlines"));
		QString airline = QString::fromStdString(airlines->GetValue(pointIndex));
		airline.remove(' ');

		//check if the given airline is contained in a airline in visibleAirlines
		for (int i = 0; i < visibleAirlines.count(); i++) {
			if (this->modeAirline == CONTAINING) {
				//containing Mode
				if (airline.contains(visibleAirlines.at(i), Qt::CaseInsensitive)) {
					return true;
				}
			} else if (this->modeAirline == MATCHING) {
				//exact match
				if (QString::compare(airline, visibleAirlines.at(i), Qt::CaseInsensitive) == 0) {
					return true;
				}
			}
		}
		return false;
	}
}

bool FlightFilter::visibleAirportCodeOrigin(int pointIndex, vtkPointData* pointData) {
	vtkSmartPointer<vtkStringArray> airportCodesOrigin = vtkStringArray::SafeDownCast(
	            pointData->GetAbstractArray("originAirportCodes"));
	QString airportCodeOrigin = QString::fromStdString(airportCodesOrigin->GetValue(pointIndex));

	if (this->visibleAirportCodesOrigin.count() == 0) {
		return true;
	} else {
		for (int i = 0; i < visibleAirportCodesOrigin.count(); i++) {
			//accept only exact match
			if (QString::compare(airportCodeOrigin, visibleAirportCodesOrigin.at(i),
			                     Qt::CaseInsensitive) == 0) {
				return true;
			}
		}
		return false;
	}
}

bool FlightFilter::visibleAirportCodeDestination(int pointIndex, vtkPointData* pointData) {
	vtkSmartPointer<vtkStringArray> airportCodesDestination = vtkStringArray::SafeDownCast(
	            pointData->GetAbstractArray("destinationAirportCodes"));
	QString airportCodeDestination = QString::fromStdString(airportCodesDestination->GetValue(
	                                     pointIndex));
	if (this->visibleAirportCodesDestination.count() == 0) {
		return true;
	} else {
		for (int i = 0; i < visibleAirportCodesDestination.count(); i++) {
			//accept only exact match
			if (QString::compare(airportCodeDestination, visibleAirportCodesDestination.at(i),
			                     Qt::CaseInsensitive) == 0) {
				return true;
			}
		}
		return false;
	}
}

bool FlightFilter::visibleLength(int pointIndex, vtkPointData* pointData) {
	vtkSmartPointer<vtkTypeFloat32Array> flightLengths = vtkTypeFloat32Array::SafeDownCast(
	            pointData->GetAbstractArray("flightLengths"));
	double flightLength = flightLengths->GetTuple1(pointIndex);
	return this->minFlightLength <= flightLength && flightLength <= this->maxFlightLength;
}

void FlightFilter::setAirportCodeOrigin(const char* airportCodeOrigin) {
	QString airportCodesOrigin = QString::fromStdString(airportCodeOrigin);
	airportCodesOrigin.remove(' ');

	if (QString::compare(airportCodesOrigin, "", Qt::CaseInsensitive) == 0) {
		this->visibleAirportCodesOrigin.clear();
	} else {
		this->visibleAirportCodesOrigin = airportCodesOrigin.split( "," );
	}
	this->Modified();
}


void FlightFilter::setAirportCodeDestination(const char* airportCodeDestinatioin) {
	QString airportCodesDestination = QString::fromStdString(airportCodeDestinatioin);
	airportCodesDestination.remove(' ');
	if (QString::compare(airportCodesDestination, "", Qt::CaseInsensitive) == 0) {
		this->visibleAirportCodesDestination.clear();
	} else {
		this->visibleAirportCodesDestination = airportCodesDestination.split( "," );
	}
	this->Modified();
}

void FlightFilter::setMinFlightLength(double minLength) {
	this->minFlightLength = minLength;
	this->Modified();
}

void FlightFilter::setMaxFlightLength(double maxLength) {
	this->maxFlightLength = maxLength;
	this->Modified();
}


void FlightFilter::setAirlineMatchingMode(int matchingMode) {
	this->modeAirline = static_cast<Mode>(matchingMode);
	this->Modified();
}

void FlightFilter::setAirline(const char* airline) {
	QString airlines = QString::fromStdString(airline);
	airlines.remove(' ');

	if (QString::compare(airlines, "", Qt::CaseInsensitive) == 0) {
		this->visibleAirlines.clear();
	} else {
		this->visibleAirlines = airlines.split( "," );
	}
	this->Modified();
}

void FlightFilter::SetInputConnection(vtkAlgorithmOutput* input) {
	this->Superclass::SetInputConnection(input);
}


