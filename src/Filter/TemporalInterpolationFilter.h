#ifndef KRONOS_TEMPORAL_INTERPOLATION_FILTER_HPP
#define KRONOS_TEMPORAL_INTERPOLATION_FILTER_HPP

#include <Reader/DataReader/Data.hpp>
#include <Utils/Misc/PointCoordinates.hpp>

#include <vtkFiltersGeneralModule.h>
#include <vtkPassInputTypeAlgorithm.h>
#include <vtkDataSet.h>
#include <Filter/TemporalInterpolationFilter/InterpolationValue.hpp>

#include <qstring.h>
#include <qlist.h>
#include <qmap.h>

class TemporalInterpolationFilter : public vtkPassInputTypeAlgorithm {
public:
    static TemporalInterpolationFilter *New();

    vtkTypeMacro(TemporalInterpolationFilter, vtkPassInputTypeAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

protected:
    TemporalInterpolationFilter();
    ~TemporalInterpolationFilter();

    virtual int FillInputPortInformation(int port, vtkInformation* info);
    virtual int FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info);

    virtual int RequestData(vtkInformation *request,
                          vtkInformationVector **inputVector,
                          vtkInformationVector *outputVector);

    virtual int RequestUpdateExtent(vtkInformation *request,
                                  vtkInformationVector **inputVector,
                                  vtkInformationVector *outputVector);
    virtual int RequestInformation(vtkInformation *request,
                                 vtkInformationVector **inputVector,
                                 vtkInformationVector *outputVector);

private:
    /**
     * Display an error message and remember that this filter does not hold valid data.
     * @param message The error message to be shown to the user
     */
    void fail(QString message);
    
    void updateQMap(int timestep, vtkPolyData *inputData);
    
    /**
     * Create a temporal data point from the data available in a `vtkPolyData` object.
     * @param pointIndex The index of the data point in the input data
     * @param inputData The poly data object containing the point data
     * @return A temporal data point of the right type with all available data
     */
    InterpolationValue createDataPoint(int pointIndex, vtkPolyData *inputData);
    
    QMap<int, QMap<PointCoordinates, InterpolationValue>> timestampMap;

    void addDataInFirstTimeStep();
    void addDataInLastTimeStep();
    void printData();
    void interpolateData();
    void interpolateDataPoint(InterpolationValue lower, InterpolationValue higher, int distanceToFirstInterpolationTimestep, int index, PointCoordinates coordinate, int distance);

    QList<PointCoordinates> allPointCooridinates;
    
    /**
     * Check whether this filter has successfully finished preprocessing data.
     * @return True if this filter has preprocessed data available, false otherwise
     */
    bool hasPreprocessed();
    
    /**
     * Boolean flag denoting whether the filter has successfully finished preprocessing.
     */
    bool preprocessed;
    
    /**
     * Boolean flag denoting whether there was an error.
     */
    bool error;
    
    /**
     * This integer will store the current time step while this filter iterates through all of them
     * to aggregate data.
     */
    int currentTimeStep;
    
    /**
     * Stores the type of data this filter receives.
     */
    Data::Type dataType;
    
    /**
     * This list contains all data types that are supported by this filter.
     */
    const static QList<Data::Type> SUPPORTED_DATA_TYPES;
    
    TemporalInterpolationFilter(const TemporalInterpolationFilter&); // Not implemented.
    void operator=(const TemporalInterpolationFilter&); // Not implemented.
};

#endif