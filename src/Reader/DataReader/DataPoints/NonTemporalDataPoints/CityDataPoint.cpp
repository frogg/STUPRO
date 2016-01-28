#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/CityDataPoint.hpp>
#include <Reader/DataReader/DataType.hpp>

CityDataPoint::CityDataPoint(Coordinate coordinate, int priority, QString name)
	: NonTemporalDataPoint(DataType::CITIES, coordinate, priority), name(name) { }

QString CityDataPoint::getName() const {
	return this->name;
}