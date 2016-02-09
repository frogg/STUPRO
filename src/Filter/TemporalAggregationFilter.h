#ifndef KRONOS_TEMPORAL_AGGREGATION_FILTER_HPP
#define KRONOS_TEMPORAL_AGGREGATION_FILTER_HPP

#include <vtkFiltersHybridModule.h>
#include <vtkMultiTimeStepAlgorithm.h>
#include <vtkDataSet.h>

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
    TemporalAggregationFilter(const TemporalAggregationFilter&); // Not implemented.
    void operator=(const TemporalAggregationFilter&); // Not implemented.
};

#endif