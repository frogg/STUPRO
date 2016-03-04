#ifndef KRONOS_WIND_VELOCITY_VECTOR_CALCULATION_FILTER_HPP
#define KRONOS_WIND_VELOCITY_VECTOR_CALCULATION_FILTER_HPP

#include <Reader/DataReader/Data.hpp>

#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkDataObjectAlgorithm.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkPointData.h>

#include <qstring.h>
#include <qlist.h>

/**
 * Filter that can calculate velocity vectors from given wind data for use with flow visualisation.
 */
class WindVelocityVectorCalculationFilter : public vtkDataObjectAlgorithm {
public:
	WindVelocityVectorCalculationFilter();
	~WindVelocityVectorCalculationFilter();

	void PrintSelf(ostream& os, vtkIndent indent) override;

	int RequestData(vtkInformation* info,
	                vtkInformationVector** inputVector,
	                vtkInformationVector* outputVector) override;
	int RequestInformation(vtkInformation* request,
	                       vtkInformationVector** inputVector,
	                       vtkInformationVector* outputVector) override;
	int FillOutputPortInformation(int port, vtkInformation* info) override;
	int FillInputPortInformation(int port, vtkInformation* info) override;

	/**
	 * Display an error message and remember that this filter does not hold valid data.
	 * @param message The error message to be shown to the user
	 */
	void fail(QString message);

private:
	WindVelocityVectorCalculationFilter(const WindVelocityVectorCalculationFilter&);  // Not implemented.
	void operator=(const WindVelocityVectorCalculationFilter&);  // Not implemented.

	/**
	 * Boolean flag denoting whether there was an error.
	 */
	bool error;
	
	/**
	 * The state of this filter's input data.
	 */
	Data::State dataState;
};

#endif
