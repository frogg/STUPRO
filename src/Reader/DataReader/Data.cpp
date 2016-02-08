#include <Reader/DataReader/Data.hpp>

#include <vtkInformationIntegerKey.h>

vtkInformationKeyMacro(Data, VTK_DATA_TYPE, DataType);

bool Data::isTemporal(const Data::Type dataType) {
	return (
	           dataType == TWEETS || dataType == PRECIPITATION || dataType == TEMPERATURE ||
	           dataType == WIND || dataType == CLOUD_COVERAGE
	       );
}