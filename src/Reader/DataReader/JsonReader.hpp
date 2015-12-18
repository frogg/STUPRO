#ifndef KRONOS_ABSTRACT_JSON_READER_HPP
#define KRONOS_ABSTRACT_JSON_READER_HPP

#include <Utils/Misc/Macros.hpp>
#include <QString>

#include <vtkSmartPointer.h>
#include <rapidjson/document.h>
#include <vtkPolyData.h>
#include <Reader/DataReader/PointDataSet.hpp>

class JsonReader {
    
    KRONOS_FRIEND_TEST(TestJsonReader, ReadCityData);
    
public:
    /**
     * Create a new AbstractJsonReader from a map of data from a JSON file
     * @param data A potentially nested map of data from the JSON file
     */
    JsonReader(rapidjson::Value& jsonDocument, int dataType, bool temporal);
    
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
     * A set of all points and their stored information read by this reader
     */
    PointDataSet pointDataSet;

    /**
     * Iterate through a JSON representation of data points and save them to the PointDataSet this
     * reader holds.
     * @param jsonValue ...
     * @param depth ...
     */
    void indexDataPoints(rapidjson::Value& jsonValue, int depth);
};

#endif
