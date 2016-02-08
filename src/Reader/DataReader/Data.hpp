#ifndef KRONOS_READER_DATA_HPP
#define KRONOS_READER_DATA_HPP

#include <Reader/DataReader/vtkInformationDataTypeKey.hpp>

#include <vtkInformationDataObjectMetaDataKey.h>

class Data {
public:
	/**
	 * A VTK information key for storing data type information in an object that will be passed
	 * through the pipeline.
	 */
	static vtkInformationDataTypeKey* VTK_DATA_TYPE();
	
	/**
	 * Enum that denotes a data type. Do not assign values to these. Adding a new data type here
	 * requires changes to the `JsonValidator` class.
	 */
	enum Type {
		CITIES, FLIGHTS, TWEETS, PRECIPITATION, TEMPERATURE, WIND, CLOUD_COVERAGE
	};

	/**
	 * Check whether a data type contains time information.
	 * @param dataType Data type to be checked
	 * @return True if the data type contains time information, false otherwise
	 */
	static bool isTemporal(const Data::Type dataType);
};

#endif
