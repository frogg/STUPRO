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




class SciVisFilter  : public vtkThreshold
{
public:
    static SciVisFilter *New();
    vtkTypeMacro(SciVisFilter,vtkThreshold);
    void PrintSelf(ostream& os, vtkIndent indent) override;


    void chooseDateMIPAS(double initTime);
		void chooseDateAIRS(double initTime);
		void chooseDateCLaMS(double iniTime);


    int	RequestData (vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
    //int FillInputPortInformation (int port, vtkInformation *info) override;
    //int FillOutputPortInformation(int port, vtkInformation* info) override;

protected:
    SciVisFilter();
    ~SciVisFilter();



private:

		double MIPASTime;
		double AIRSTime;
		double CLaMSTime;

    SciVisFilter(const SciVisFilter&); //Not implemented
    void operator=(const SciVisFilter&); //Not implemented

};

#endif
