//
//  SpericalToCartesianFilter.hpp
//  kronos
//
//  Created by Larissa Laich on 04.12.15.
//
//

#ifndef SpericalToCartesianFilter_hpp
#define SpericalToCartesianFilter_hpp

#include "vtkPointSetAlgorithm.h"
#include <stdio.h>

class vtkPointSet;
class vtkDataSet;

class SpericalToCartesianFilter : public vtkPointSetAlgorithm {
public:
    vtkTypeMacro(SpericalToCartesianFilter, vtkPointSetAlgorithm)
    static SpericalToCartesianFilter *New();
    int ProcessRequest(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector) override;
    int RequestDataObject(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector) override;
    void PrintSelf(ostream& os, vtkIndent indent) override;
    int FillInputPortInformation(int port, vtkInformation *info) override;
protected:
    SpericalToCartesianFilter(){
        
    };
    ~SpericalToCartesianFilter(){};
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    SpericalToCartesianFilter(const SpericalToCartesianFilter&);  // Not implemented.
    void operator=(const SpericalToCartesianFilter&);  // Not implemented.
    double* transformToCartesian(double* point,double heightOffset = 100);
    vtkPointSet *createOutputData(vtkDataSet* const input, vtkInformationVector *outputVector);
};


#endif /* SpericalToCartesianFilter_hpp */
