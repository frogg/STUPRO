#ifndef KRONOS_TEMPORAL_AGGREGATION_FILTER_HPP
#define KRONOS_TEMPORAL_AGGREGATION_FILTER_HPP

#include <Reader/DataReader/Data.hpp>

#include <vtkFiltersHybridModule.h>
#include <vtkMultiTimeStepAlgorithm.h>
#include <vtkDataSet.h>

#include <qstring.h>

class VTKFILTERSHYBRID_EXPORT TemporalAggregationFilter : public vtkMultiTimeStepAlgorithm {
public:
    static TemporalAggregationFilter *New();

    vtkTypeMacro(TemporalAggregationFilter, vtkMultiTimeStepAlgorithm);
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
    
    TemporalAggregationFilter(const TemporalAggregationFilter&); // Not implemented.
    void operator=(const TemporalAggregationFilter&); // Not implemented.
};

#endif