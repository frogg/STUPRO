/**
 */

#include "SpericalToCartesianFilter.h"
#include <vtkIndent.h>
#include <vtkInformation.h>
#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkPointSet.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <cmath>
#include <vtkPVInformationKeys.h>
#include <vtkInformationVector.h>

// Still the same everytime
vtkStandardNewMacro(SpericalToCartesianFilter)

int SpericalToCartesianFilter::RequestData(vtkInformation *vtkNotUsed(request),
        vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{

	vtkDataSet *input = vtkDataSet::GetData(inputVector[0]);
	vtkPointSet *output = vtkPointSet::GetData(outputVector);

	if (!input || !output)
	{
		vtkErrorMacro(<< "You must supply valid input and output ports")
	}

	output->CopyStructure(input);
	output->CopyAttributes(input);

	vtkPoints *points = output->GetPoints();
	//get the point of the output and transform them to cartesian coordinate system
	for (int i = 0; i < points->GetNumberOfPoints(); i++)
	{
		points->SetPoint(i, transformToCartesian(points->GetPoint(i)));
	}

	return 1;
}
int SpericalToCartesianFilter::RequestUpdateExtent(vtkInformation *, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
      vtkDebugMacro("Request-Update");
     // get the info object
     vtkInformation *outInfo = outputVector->GetInformationObject(0);
     outInfo->Set(vtkPVInformationKeys::WHOLE_BOUNDING_BOX(),
     -100,
     100,
     -100,
     100,
     -1000,
     1000);
     
     /*
     int vtkStreamingDemandDrivenPipeline::SetWholeBoundingBox	( 	int 	port,
     double 	bb[6]
     )
     
     Set/Get the whole bounding box of an output port data object. The whole whole bounding box is meta data for data sets. It gets set by the algorithm during the update information pass.
     */
    return 1;
}

double* SpericalToCartesianFilter::transformToCartesian(double* point, double heightOffset)
{
	//get longitude, latitude and radius out of points, heightOffset is a default value set in the header file
	double lon = point[0] * M_PI / 180;
	double lat = point[1] * M_PI / 180;
	double radius = heightOffset + point[2];

	//formula to transform the sphere coordinates to cartesian coordinate system
	point[0] = radius * cos(lat) * cos(lon);
	point[1] = radius * cos(lat) * sin(lon);
	point[2] = radius * sin(lat);

	return point;
}

int SpericalToCartesianFilter::ProcessRequest(vtkInformation *request,
        vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{

	if (request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
	{
		return this->RequestData(request, inputVector, outputVector);
	}

	if (request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT()))
	{
		return this->RequestUpdateExtent(request, inputVector, outputVector);
	}

	// create the output
	if (request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT()))
	{
		return this->RequestDataObject(request, inputVector, outputVector);
	}

	// execute information
	if (request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION()))
	{
		return this->RequestInformation(request, inputVector, outputVector);
	}

	return Superclass::ProcessRequest(request, inputVector, outputVector);
}

void SpericalToCartesianFilter::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);

	os << indent << "Hello, this is our filter." << endl;
}
