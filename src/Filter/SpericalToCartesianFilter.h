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
#include <Utils/Misc/Macros.hpp>

class vtkInformation;
class vtkInformationVector;


class SpericalToCartesianFilter: public vtkDataSetAlgorithm
{
    
    KRONOS_FRIEND_TEST(TestSphericalToCartesianFilter, ValidCalls);
    KRONOS_FRIEND_TEST(TestSphericalToCartesianFilter, InvalidCalls);
    
public:
	vtkTypeMacro(SpericalToCartesianFilter, vtkDataSetAlgorithm)

	static SpericalToCartesianFilter *New();

	int ProcessRequest(vtkInformation *request, vtkInformationVector **inputVector,
	        vtkInformationVector *outputVector) override;

	void PrintSelf(ostream& os, vtkIndent indent) override;


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
    double* transformToCartesian(double* point, double heightOffset = 100);


};

#endif /* SpericalToCartesianFilter_hpp */
