#ifndef __AIRSFilter_h
#define __AIRSFilter_h

#include "vtkPoints.h"
#include "vtkDataSetAlgorithm.h"
#include <vtkThreshold.h>
#include <vtkInformation.h>
#include "vtkInformationVector.h"
#include <vtkIndent.h>
#include <vtkDataSet.h>
#include <vtkUnstructuredGridAlgorithm.h>
#include <vtkPolyData.h>
#include <vtkDataObjectAlgorithm.h>
#include <AbstractSelectionFilter.hpp>


class AIRSFilter  : public AbstractSelectionFilter
{
public:
    static AIRSFilter *New();
    vtkTypeMacro(AIRSFilter,vtkDataObjectAlgorithm);

    void SetInputConnection(vtkAlgorithmOutput *input);

    
    //void setUpperMIPAS(double upperLimit);
    void setTimeThreshold(double lowerLimit, double upperLimit);
    int RequestInformation(vtkInformation* request,
                           vtkInformationVector** inputVector,
                           vtkInformationVector* outputVector) override;

    /*
		void setUpperAIRS(double upperLimit);
		void setLowerAIRS(double lowerLimit);

		void setUpperCLaMS(double upperLimit);
		void setLowerCLaMS(double lowerLimit);
 */
    /**
     * Callback for the input array selection. This has to exist for the filter to be correctly assembled but can be ignored since the scalar is locked to the temperature values and the UI is hidden.
     */
    void ignore(int id, int port, int connection, int fieldAssociation, const char* name) { }

private:

    AIRSFilter();
    ~AIRSFilter();
    AIRSFilter(const AIRSFilter&); //Not implemented
    void operator=(const AIRSFilter&); //Not implemented
    QList<Data::Type> getCompatibleDataTypes() override;
    bool evaluatePoint(int pointIndex, Coordinate coordinate, vtkPointData* pointData) override;
    double upperTimeLimit;
    double lowerTimeLimit;
    
};

#endif
