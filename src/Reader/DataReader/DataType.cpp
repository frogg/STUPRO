#include <Reader/DataReader/DataType.hpp>

const int DataType::CITIES;
const int DataType::FLIGHTS;
const int DataType::TWEETS;
const int DataType::PRECIPITATION;
const int DataType::TEMPERATURE;
const int DataType::WIND;
const int DataType::CLOUDCOVER;

bool DataType::isTemporal(const int dataType) {
    return (
        dataType == DataType::TWEETS || dataType == DataType::PRECIPITATION ||
        dataType == DataType::TEMPERATURE || dataType == DataType::WIND ||
        dataType == DataType::CLOUDCOVER
    );
}