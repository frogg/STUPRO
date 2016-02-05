#include <Reader/DataReader/DataPoints/DataPoint.hpp>

<<<<<<< HEAD
DataPoint::DataPoint(int dataType, Coordinate coordinate, int priority)
	: dataType(dataType), coordinate(coordinate), priority(priority) { }

int DataPoint::getDataType() const {
=======
DataPoint::DataPoint(Data::Type dataType, Coordinate coordinate, int priority)
	: dataType(dataType), coordinate(coordinate), priority(priority) { }

Data::Type DataPoint::getDataType() const {
>>>>>>> develop
	return this->dataType;
}

int DataPoint::getPriority() const {
	return this->priority;
}

Coordinate DataPoint::getCoordinate() const {
	return this->coordinate;
}