#ifndef KRONOS_READER_DATA_HPP
#define KRONOS_READER_DATA_HPP

#include <Reader/DataReader/MetaInformationKeys/vtkInformationDataTypeKey.hpp>
#include <Reader/DataReader/MetaInformationKeys/vtkInformationTimeResolutionKey.hpp>
#include <Reader/DataReader/MetaInformationKeys/vtkInformationDataStatusKey.hpp>

#include <vtkInformationDataObjectMetaDataKey.h>

#include <QMap>
#include <QString>

class Data {
public:
	/**
	 * A VTK information key for storing data type information in an object that will be passed
	 * through the pipeline.
	 */
	static vtkInformationDataTypeKey* VTK_DATA_TYPE();

	/**
	 * A VTK information key for storing the time resolution of temporal data in an object that will be passed
	 * through the pipeline.
	 */
	static vtkInformationTimeResolutionKey* VTK_TIME_RESOLUTION();
	
	/**
	 * A VTK information key for storing the status of the data in an object that will be passed
	 * through the pipeline.
	 */
	static vtkInformationDataStatusKey* VTK_DATA_STATUS();

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

	/**
	 * Get a human-readable name of a data type.
	 * @param dataType The data type whose name should be retrieved
	 * @return The data type's name
	 */
	static QString getDataTypeName(const Data::Type dataType);

private:
	/**
	 * Maps data types to human-readable names of themselves.
	 */
	static const QMap<Data::Type, QString> DATA_TYPE_NAMES;
};

#endif
