//
//  SpericalToCartesianFilter.hpp
//  kronos
//
//  Created by Larissa Laich on 04.12.15.
//
//

#ifndef SpericalToCartesianFilter_hpp
#define SpericalToCartesianFilter_hpp

#include <vtkDataSetAlgorithm.h>
#include <vtkSetGet.h>
#include <iostream>
//#include <gtest/gtest.h>

class vtkInformation;
class vtkInformationVector;

class SpericalToCartesianFilter: public vtkDataSetAlgorithm
{
public:
	vtkTypeMacro(SpericalToCartesianFilter, vtkDataSetAlgorithm)

	static SpericalToCartesianFilter *New();

	int ProcessRequest(vtkInformation *request, vtkInformationVector **inputVector,
	        vtkInformationVector *outputVector) override;

	void PrintSelf(ostream& os, vtkIndent indent) override;

    double* transformToCartesian(double* point, double heightOffset = 100);

protected:
	SpericalToCartesianFilter()
	{

	}

	~SpericalToCartesianFilter()
	{
	}

	int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
	SpericalToCartesianFilter(const SpericalToCartesianFilter&);  // Not implemented.
	void operator=(const SpericalToCartesianFilter&);  // Not implemented.
    int RequestUpdateExtent(vtkInformation *, vtkInformationVector **inputVector, vtkInformationVector *outputVector);
   // FRIEND_TEST(TestSpericalToCartesianFilter, ValidCalls);
   // FRIEND_TEST(TestSpericalToCartesianFilter, InvalidCalls);
};

#endif /* SpericalToCartesianFilter_hpp */
