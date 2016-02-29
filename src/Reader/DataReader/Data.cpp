#include <Reader/DataReader/Data.hpp>

#include <vtkInformationIntegerKey.h>

vtkInformationKeyMacro(Data, VTK_DATA_TYPE, DataType);
vtkInformationKeyMacro(Data, VTK_DATA_STATE, DataState);
vtkInformationKeyMacro(Data, VTK_TIME_RESOLUTION, TimeResolution);

static QMap<Data::Type, QString> dataTypeNameMap() {
	QMap<Data::Type, QString> map;
	map.insert(Data::CITIES, "City");
	map.insert(Data::FLIGHTS, "Flight");
	map.insert(Data::TWEETS, "Tweet");
	map.insert(Data::PRECIPITATION, "Precipitation");
	map.insert(Data::TEMPERATURE, "Temperature");
	map.insert(Data::WIND, "Wind");
	map.insert(Data::CLOUD_COVERAGE, "Cloud Coverage");
	return map;
}
const QMap<Data::Type, QString> Data::DATA_TYPE_NAMES = dataTypeNameMap();

static QMap<Data::State, QString> dataStateNameMap() {
	QMap<Data::State, QString> map;
	map.insert(Data::RAW, "Raw");
	map.insert(Data::AGGREGATED, "Aggregated");
	map.insert(Data::DENSITY_MAPPED, "Density-mapped");
	return map;
}
const QMap<Data::State, QString> Data::DATA_STATE_NAMES = dataStateNameMap();

bool Data::isTemporal(const Data::Type dataType) {
	return (
	           dataType == TWEETS || dataType == PRECIPITATION || dataType == TEMPERATURE ||
	           dataType == WIND || dataType == CLOUD_COVERAGE
	       );
}

QString Data::getDataTypeName(const Data::Type dataType) {
	return Data::DATA_TYPE_NAMES.value(dataType);
}

QString Data::getDataStateName(const Data::State dataState) {
	return Data::DATA_STATE_NAMES.value(dataState);
}