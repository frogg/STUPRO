#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/FlightDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

FlightDataPoint::FlightDataPoint(Coordinate coordinate, int priority, Coordinate destination)
<<<<<<< HEAD
	: NonTemporalDataPoint(DataType::FLIGHTS, coordinate, priority),
	  destination(destination) { }
=======
	: NonTemporalDataPoint(Data::FLIGHTS, coordinate, priority), destination(destination) { }
>>>>>>> develop

Coordinate FlightDataPoint::getDestination() const {
	return this->destination;
}