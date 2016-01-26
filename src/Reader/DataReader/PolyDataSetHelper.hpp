#ifndef KRONOS_POLYDATASETHELPER_HPP
#define KRONOS_POLYDATASETHELPER_HPP

#include <QList>
#include <Reader/DataReader/DataPoints/DataPoint.hpp>
#include <Reader/DataReader/PointDataSet.hpp>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

/**
* Offers static utility methods for creating vtkPolyData from data points.
*/
class PolyDataSetHelper {

public:
    /**
     * Convert a list of non-temporal data points into vtkPolyData.
     * @param dataPoints A list of non-temporal data points to be converted
     * @param zoomLevel The current zoom level
     * @param dataType The type of data points in the list
     * @return A vtkPolyData object containing all relevant data points
     */
	static vtkSmartPointer<vtkPolyData> getPolyDataFromDataPoints(
        PointDataSet dataPoints,
        int zoomLevel,
        int dataType
    );
    
    /**
     * Convert a list of temporal data points into vtkPolyData.
     * @param dataPoints A list of temporal data points to be converted
     * @param zoomLevel The current zoom level
     * @param dataType The type of data points in the list
     * @param timeResolution Amount of seconds a single time step contains
     * @param timeStep The current time step, which is the amount of time spans from the start time
     * with the length of one such time span specified by the time resolution
     * @param startTime Timestamp of the earliest data point in the list
     * @return A vtkPolyData object containing all relevant data points
     */
    static vtkSmartPointer<vtkPolyData> getPolyDataFromDataPoints(
        PointDataSet dataPoints,
        int zoomLevel,
        int dataType,
        int timeResolution,
        int timeStep,
        int startTime
    );

private:
    /**
     * Internal method that can convert a list of either temporal or non-temporal data points into
     * vtkPolyData.
     * @param dataPoints A list of data points to be converted
     * @param zoomLevel The current zoom level
     * @param dataType The type of data points in the list
     * @param timeResolution Amount of seconds a single time step contains
     * @param timeStep The current time step
     * @param startTime Timestamp of the earliest data point in the list
     * @return A vtkPolyData object containing all relevant data points
     */
    static vtkSmartPointer<vtkPolyData> getPolyDataFromDataPoints(
        PointDataSet dataPoints,
        int zoomLevel,
        int dataType,
        int timeResolution,
        int timeStep,
        int startTime,
        bool temporal
    );
    
    /**
     * Extract relevant data points from a list of non-temporal data points.
     * @param points A list of non-temporal data points
     * @param zoomLevel The current zoom level
     * @return A list of all data points relevant to the given zoom level
     */
    static QList<DataPoint*> extractRelevantDataPoints(
        PointDataSet points,
        int zoomLevel
    );
    
    /**
     * Extract relevant data points from a list of temporal data points.
     * @param points A list of temporal data points
     * @param zoomLevel The current zoom level
     * @param timeResolution Amount of seconds a single time step contains
     * @param timeStep The current time step
     * @param startTime Timestamp of the earliest data point in the list
     * @return A list of all data points relevant to the given zoom level and time information
     */
    static QList<DataPoint*> extractRelevantDataPoints(
        PointDataSet points,
        int zoomLevel,
        int timeResolution,
        int timeStep,
        int startTime
    );
    
    /**
     * Create a vtkPolyData object from a set of points.
     * @param points A list of data points to be put into the data set
     * @param dataType The type of data points in the list
     * @return A vtkPolyData object containing all points from the list
     */
    static vtkSmartPointer<vtkPolyData> createPolyDataSet(
        QList<DataPoint*> relevantDataPoints,
        int dataType
    );
    
    /**
     * Hidden since this class only offers static utility methods.
     */
    PolyDataSetHelper();
    ~PolyDataSetHelper();

};

#endif