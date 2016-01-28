#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/CityDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

CityDataPoint::CityDataPoint(Coordinate coordinate, int priority, QString name)
	: NonTemporalDataPoint(Data::CITIES, coordinate, priority), name(name) { }

QString CityDataPoint::getName() const {
	return this->name;
}