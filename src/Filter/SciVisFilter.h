#ifndef __SciVisFilter_h
#define __SciVisFilter_h

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




class SciVisFilter  : public AbstractSelectionFilter
{
public:
    static SciVisFilter *New();
    vtkTypeMacro(SciVisFilter,vtkDataObjectAlgorithm);

    void SetInputConnection(vtkAlgorithmOutput *input);

		void setUpperMIPAS(double upperLimit);
		void setLowerMIPAS(double lowerLimit);

    /*
		void setUpperAIRS(double upperLimit);
		void setLowerAIRS(double lowerLimit);

		void setUpperCLaMS(double upperLimit);
		void setLowerCLaMS(double lowerLimit);
 */

private:

    SciVisFilter();
    ~SciVisFilter();
    SciVisFilter(const SciVisFilter&); //Not implemented
    void operator=(const SciVisFilter&); //Not implemented

};

#endif
