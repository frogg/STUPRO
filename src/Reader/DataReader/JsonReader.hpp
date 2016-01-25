#ifndef KRONOS_ABSTRACT_JSON_READER_HPP
#define KRONOS_ABSTRACT_JSON_READER_HPP

#include <Utils/Misc/Macros.hpp>
#include <QString>
#include <QMap>

#include <vtkSmartPointer.h>
#include <rapidjson/document.h>
#include <vtkPolyData.h>
#include <Reader/DataReader/PointDataSet.hpp>

class JsonReader {
    
    KRONOS_FRIEND_TEST(TestJsonReader, ReadCityData);
    KRONOS_FRIEND_TEST(TestJsonReader, ReadTwitterData);
    
public:
    /**
     * Create a new AbstractJsonReader from a map of data from a JSON file
     * @param data A potentially nested map of data from the JSON file
     */
    JsonReader(rapidjson::Value& jsonDocument, int dataType, bool temporal, int timeResolution);
    
    virtual ~JsonReader() { }

    /**
     * Get the type of data read by this reader
     * @return The type of data read by this reader
     */
    int getDataType() const;

    /**
     * Check whether the data read by this reader is of temporal nature
     * @return True if each data point has time information, false otherwise
     */
    bool hasTemporalData() const;

    /**
     * Get all data stored in the file this reader uses, pruned by a specified zoom level.
     * @param zoomLevel The zoom level of the data set. Every data point with a higher zoom level
     * will be discarded while creating the vtkPolyData.
     * @return All data relevant to the specified zoom level, formatted as vtkPolyData with each
     * point's data stored in the data point's scalar values.
     */
    vtkSmartPointer<vtkPolyData> getVtkDataSet(int zoomLevel);
    
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
     * Clear all cached vtkPolyData
     */
    void clearCache();

private:
    /**
     * Boolean flag denoting whether the data read by this reader contains time information
     */
    bool temporal;

    /**
     * Integer denoting the type of data read by this reader, using constants defined in the
     * DataType.hpp file
     */
    int dataType;
    
    /**
     * Amount of seconds in each time step, only used if this object reads temporal data
     */
    int timeResolution;

    /**
     * A set of all points and their stored information read by this reader
     */
    PointDataSet pointDataSet;
    
    /**
     * A map with cached vtkPolyData for specified zoom levels
     */
    QMap<int, vtkSmartPointer<vtkPolyData>> cache;
    
    /**
     * Boolean flag denoting whether a cache should be used
     */
    bool cachingEnabled;

    /**
     * Iterate through a JSON representation of data points and save them to the PointDataSet this
     * reader holds.
     * @param jsonValue JSON object to be used
     * @param depth Current recursion depth
     */
    void indexDataPoints(rapidjson::Value& jsonValue, int depth);
};

#endif
