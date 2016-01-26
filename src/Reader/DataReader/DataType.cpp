#include <Reader/DataReader/DataType.hpp>

bool DataType::isTemporal(const int dataType) {
    return (
        dataType == DataType::TWEETS || dataType == DataType::PRECIPITATION ||
        dataType == DataType::TEMPERATURE || dataType == DataType::WIND ||
        dataType == DataType::CLOUDCOVER
    );
}