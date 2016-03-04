#ifndef KRONOS_READER_DATA_HPP
#define KRONOS_READER_DATA_HPP

#include <Reader/DataReader/MetaInformationKeys/vtkInformationDataTypeKey.hpp>
#include <Reader/DataReader/MetaInformationKeys/vtkInformationTimeResolutionKey.hpp>
#include <Reader/DataReader/MetaInformationKeys/vtkInformationDataStateKey.hpp>
#include <Reader/DataReader/MetaInformationKeys/vtkInformationDataTransformationKey.hpp>

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
	 * A VTK information key for storing the state of the data in an object that will be passed
	 * through the pipeline.
	 */
	static vtkInformationDataStateKey* VTK_DATA_STATE();

	/**
	 * A VTK information key for storing the transformation of the data in an object that will be passed
	 * through the pipeline.
	 */
	static vtkInformationDataTransformationKey* VTK_DATA_TRANSFORMATION();

	/**
	 * Enum that denotes a data type. Do not assign values to these. Adding a new data type here
	 * requires changes to the `JsonValidator` class.
	 */
	enum Type {
		CITIES, FLIGHTS, TWEETS, PRECIPITATION, TEMPERATURE, WIND, CLOUD_COVERAGE
	};

	/**
	 * Enum that denotes the different states a data set can have.
	 */
	enum State {
		RAW, AGGREGATED, DENSITY_MAPPED
	};

	/**
	 * Enum that denotes the different application states of the spherical to cartesian transform filter
	 */
	enum Transformation {
		UNTRANSFORMED, CONDENSED, TRANSFORMED
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

	/**
	 * Get a human-readable name of a data state.
	 * @param dataState The data state whose name should be retrieved
	 * @return The data state's name
	 */
	static QString getDataStateName(const Data::State dataState);

	/**
	 * Get a human-readable name of a data state.
	 * @param dataState The data state whose name should be retrieved
	 * @return The data state's name
	 */
	static QString getDataTransformationName(const Data::Transformation dataTransformation);

private:
	/**
	 * Maps data types to human-readable names of themselves.
	 */
	static const QMap<Data::Type, QString> DATA_TYPE_NAMES;

	/**
	 * Maps data states to human-readable names of themselves.
	 */
	static const QMap<Data::State, QString> DATA_STATE_NAMES;

	/**
	 * Maps data transformation to human-readable names of themselves.
	 */
	static const QMap<Data::Transformation, QString> DATA_TRANSFORMATION_NAMES;
};

#endif
