#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/CityDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

CityDataPoint::CityDataPoint(Coordinate coordinate, int priority, QString name)
<<<<<<< HEAD
	: NonTemporalDataPoint(DataType::CITIES, coordinate, priority), name(name) { }
=======
	: NonTemporalDataPoint(Data::CITIES, coordinate, priority), name(name) { }
>>>>>>> develop

QString CityDataPoint::getName() const {
	return this->name;
}