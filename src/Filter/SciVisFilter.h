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



class SciVisFilter  : public AbstractSelectionFilter
{
public:
    static SciVisFilter *New();
    vtkTypeMacro(SciVisFilter,vtkThreshold);
    void PrintSelf(ostream& os, vtkIndent indent) override;
		int RequestData(vtkInformation* info,
		                vtkInformationVector** inputVector,
		                vtkInformationVector* outputVector) override;
		int RequestInformation(vtkInformation* request,
		                       vtkInformationVector** inputVector,
		                       vtkInformationVector* outputVector) override;
		int FillOutputPortInformation(int port, vtkInformation* info) override;
		int FillInputPortInformation(int port, vtkInformation* info) override;


		void setUpperMIPAS(double upperLimit);
		void setLowerMIPAS(double lowerLimit);

		void setUpperAIRS(double upperLimit);
		void setLowerAIRS(double lowerLimit);

		void setUpperCLaMS(double upperLimit);
		void setLowerCLaMS(double lowerLimit);
protected:
    SciVisFilter();
    ~SciVisFilter();



private:


    SciVisFilter(const SciVisFilter&); //Not implemented
    void operator=(const SciVisFilter&); //Not implemented

};

#endif
