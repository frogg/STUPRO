#ifndef KRONOS_TEMPORAL_AGGREGATION_FILTER_HPP
#define KRONOS_TEMPORAL_AGGREGATION_FILTER_HPP

#include <Reader/DataReader/Data.hpp>
#include <Utils/Misc/PointCoordinates.hpp>

#include <vtkFiltersGeneralModule.h>
#include <vtkPassInputTypeAlgorithm.h>
#include <vtkDataSet.h>

#include <qstring.h>
#include <qlist.h>
#include <qmap.h>

class VTKFILTERSGENERAL_EXPORT TemporalAggregationFilter : public vtkPassInputTypeAlgorithm {
public:
    static TemporalAggregationFilter *New();

    vtkTypeMacro(TemporalAggregationFilter, vtkPassInputTypeAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

protected:
    TemporalAggregationFilter();
    ~TemporalAggregationFilter();

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
    
    /**
     * A map storing the running averages of all points.
     */
    const QMap<PointCoordinates, double> averages;
    
    TemporalAggregationFilter(const TemporalAggregationFilter&); // Not implemented.
    void operator=(const TemporalAggregationFilter&); // Not implemented.
};

#endif