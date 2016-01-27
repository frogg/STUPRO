#include <Reader/DataReader/Data.hpp>

bool Data::isTemporal(const Data::Type dataType) {
    return (
        dataType == TWEETS || dataType == PRECIPITATION || dataType == TEMPERATURE ||
        dataType == WIND || dataType == CLOUD_COVERAGE
    );
}