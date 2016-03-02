#ifndef KRONOS_ABSTRACT_JSON_READER_HPP
#define KRONOS_ABSTRACT_JSON_READER_HPP

#include <Utils/Misc/Macros.hpp>
#include <QString>
#include <QMap>

#include <vtkSmartPointer.h>
#include <rapidjson/document.h>
#include <vtkPolyData.h>
#include <Reader/DataReader/PointDataSet.hpp>
#include <Reader/DataReader/Data.hpp>

class JsonReader {

	KRONOS_FRIEND_TEST(TestJsonReader, ReadCityData);
	KRONOS_FRIEND_TEST(TestJsonReader, ReadTwitterData);

public:
	/**
	 * Create a new JsonReader.
	 * @param jsonDocument The data in JSON format
	 * @param dataType The type of data this reader should expect
	 * @param path The absolute file path of the file read to be used with error messages
	 * @param temporal True if the data is time-sensitive, false otherwise
	 * @param timeResolution The amount of seconds in a time step
	 */
	JsonReader(rapidjson::Value& jsonDocument, Data::Type dataType, QString path, bool temporal,
	           int timeResolution);
	virtual ~JsonReader();

	/**
	 * Get the type of data read by this reader
	 * @return The type of data read by this reader
	 */
	Data::Type getDataType() const;

	/**
	 * Check whether the data read by this reader is of temporal nature
	 * @return True if each data point has time information, false otherwise
	 */
	bool hasTemporalData() const;

	/**
	 * Get the amount of time steps this reader exports.
	 * @return The amount of time steps this reader exports
	 */
	int getAmountOfTimeSteps() const;

	/**
	 * Get the amount of seconds in a time step. This will return 0 for non-temporal data.
	 * @return The amount of seconds in a time step
	 */
	int getTimeResolution() const;

	/**
	 * Get all data stored in the file this reader uses, pruned by a specified zoom level.
	 * @param zoomLevel The zoom level of the data set. Every data point with a higher zoom level
	 * will be discarded while creating the vtkPolyData.
	 * @return All data relevant to the specified zoom level, formatted as vtkPolyData with each
	 * point's data stored in the data point's scalar values.
	 */
	vtkSmartPointer<vtkPolyData> getVtkDataSet(int zoomLevel);

	/**
	 * Get all data stored in the file this reader uses, pruned by a specified zoom level and time
	 * range.
	 * @param zoomLevel The zoom level of the data set. Every data point with a higher zoom level
	 * will be discarded while creating the vtkPolyData.
	 * @param time The current time step to be retrieved as used by ParaView
	 * @return All data relevant to the specified zoom level, formatted as vtkPolyData with each
	 * point's data stored in the data point's scalar values.
	 */
	vtkSmartPointer<vtkPolyData> getVtkDataSet(int zoomLevel, int time);

	/**
	 * Specify whether vtkPolyData for a specific zoom level should be cached after it has been
	 * created using the method `getVtkDataSet(int zoomLevel)` and be retrieved from the cache
	 * in subsequent method calls.
	 * @param cachingEnabled True if data should be cached, false otherwise
	 */
	void setCachingEnabled(bool cachingEnabled);

	/**
	 * Check whether this reader caches vtkPolyData.
	 * @return True if data is being cached, false otherwise
	 */
	bool isCachingEnabled() const;

	/**
	 * Cache vtkPolyData for all possible zoom levels. This will take some time for once but enable
	 * quick retrieval for future method calls.
	 */
	void cacheAllData();

	/**
	 * Clear all cached vtkPolyData.
	 */
	void clearCache();

	/**
	 * Abort a caching process that may be in progress.
	 */
	void abortCaching();

private:
	/**
	 * The absolute path of the file reader, used for error messages
	 */
	QString filePath;

	/**
	 * Boolean flag denoting whether the data read by this reader contains time information
	 */
	bool temporal;

	/**
	 * Enum denoting the type of data read by this reader, as defined in the `Data.hpp` file
	 */
	Data::Type dataType;

	/**
	 * Amount of seconds in each time step, only used if this reader reads temporal data
	 */
	int timeResolution;

	/**
	 * Timestamp of the earliest data point, only used if this reader reads temporal data
	 */
	int startTime;

	/**
	 * Timestamp of the latest data point, only used if this reader reads temporal data
	 */
	int endTime;

	/**
	 * A set of all points and their stored information read by this reader
	 */
	PointDataSet pointDataSet;

	/**
	 * A map with cached non-temporal vtkPolyData for specified zoom levels
	 */
	QMap<int, vtkSmartPointer<vtkPolyData>> nonTemporalCache;

	/**
	 * A map with cached temporal vtkPolyData for specified zoom levels and time steps for each
	 * zoom level
	 */
	QMap<int, QMap<int, vtkSmartPointer<vtkPolyData>>> temporalCache;

	/**
	 * Boolean flag denoting whether a cache should be used
	 */
	bool cachingEnabled;

	/**
	 * Boolean flag denoting whether a caching process in progress should be aborted.
	 * To be used when caching is done in a different thread.
	 */
	bool cachingAbortRequested;

	/**
	 * Get all data stored in the file this reader uses, pruned by a specified zoom level and time
	 * range.
	 * @param zoomLevel The zoom level of the data set. Every data point with a higher zoom level
	 * will be discarded while creating the vtkPolyData.
	 * @param time The current time step to be retrieved as used by ParaView
	 * @param temporal True if the data to be retrieved is a time slice, false otherwise
	 * @return All data relevant to the specified zoom level, formatted as vtkPolyData with each
	 * point's data stored in the data point's scalar values.
	 */
	vtkSmartPointer<vtkPolyData> getVtkDataSet(int zoomLevel, int time, bool temporal);

	/**
	 * Iterate through a JSON representation of data points and save them to the PointDataSet this
	 * reader holds.
	 * @param jsonValue JSON object to be used
	 * @param depth Current recursion depth
	 */
	void indexDataPoints(rapidjson::Value& jsonValue, int depth);
};

#endif
