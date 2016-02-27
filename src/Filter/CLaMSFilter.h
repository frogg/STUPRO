#ifndef KRONOS_ABSTRACT_SELECTION_FILTER_HPP
#define KRONOS_ABSTRACT_SELECTION_FILTER_HPP

#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkDataObjectAlgorithm.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkPointData.h>
#include <vtkUnsignedIntArray.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>

#include <qstring.h>
#include <qlist.h>

#include <Reader/DataReader/Data.hpp>
#include <Globe/Coordinate.hpp>


/**
 * This class extracts the attributes of the CLaMS Data, and displays them with Sliders
 * to be able to change the CLaMS_Nabro and CLaMSPuyehue's appearance.
 */
class CLaMSFilter : public vtkDataObjectAlgorithm {
public:

	vtkTypeMacro(CLaMSFilter, vtkDataObjectAlgorithm)
	static CLaMSFilter* New();

	void PrintSelf(ostream& os, vtkIndent indent) override;

	int RequestData(vtkInformation* info,
	                vtkInformationVector** inputVector,
	                vtkInformationVector* outputVector) override;
	int RequestInformation(vtkInformation* request,
	                       vtkInformationVector** inputVector,
	                       vtkInformationVector* outputVector) override;

	int FillOutputPortInformation(int port, vtkInformation* info) override;
	int FillInputPortInformation(int port, vtkInformation* info) override;
	void SetInputConnection(vtkAlgorithmOutput* input) override;

	//set the lower and upper Limit for altitude
	/**
	 * Sets the upper Limit of the Altitude Index
	 * @param upperLimit the upper Limit of the Altitude Index
	 */
	void setUpperAltitude(float upperLimit);

	/**
	 * Sets the lower Limit of the Altitude Index
	 * @param lowerLimit the lower Limit of the Altitude Index
	 */
	void setLowerAltitude(float lowerLimit);


	/**
	 * Sets the lower Limit of the potential Temperature
	 * @param lowerLimit the lower Limit of the potential Temperature
	 */
	void setLowerPotTemperature(float lowerLimit);

	/**
	 * Sets the upper Limit of the potential Temperature
	 * @param upperLimit the upper Limit of the potential Temperature
	 */
	void setUpperPotTemperature(float upperLimit);

	/**
	 * Sets the lower Limit of the potential Vorticity
	 * @param lowerLimit the lower Limit of the potential Vorticity
	 */
	void setLowerVorticity(float lowerLimit);

	/**
	 * Sets the upper Limit of the potential Vorticity
	 * @param upperLimit the upper Limit of the potential Vorticity
	 */
	void setUpperVorticity(float upperLimit);

	/**
	 * Sets the lower Limit of the Pressure Index
	 * @param lowerLimit the lower Limit of the Pressure Index
	 */
	void setLowerPressure(float lowerLimit);

	/**
	 * Sets the upper Limit of the Pressure Index
	 * @param upperLimit the lower Limit of the Pressure Index
	 */
	void setUpperPressure(float upperLimit);

	/**
	 * Sets the lower Limit of the Temperature
	 * @param lowerLimit the lower Limit of the Temperature
	 */
	void setLowerTemperature(float lowerLimit);
	/**
	 * Sets the upper Limit of the Temperature
	 * @param upperLimit the lower Limit of the Temperature
	 */
	void setUpperTemperature(float upperLimit);

	//set the lower and upper Limit for the date
	/**
	 * Sets the lower Limit for the Time
	 * @param lowerLimit the lower Limit of the Time
	 */
	void setLowerDate(double lowerLimit);
	/**
	 * Sets the upper Limit for the Time
	 * @param upperLimit the upper Limit of the Time
	 */
	void setUpperDate(double upperLimit);




protected:
	/**
	 * Display an error message and remember that this filter does not hold valid data.
	 * @param message The error message to be shown to the user
	 */
	void fail(QString message);

private:
	CLaMSFilter();
	~CLaMSFilter();

	CLaMSFilter(const CLaMSFilter&);  // Not implemented.
	void operator=(const CLaMSFilter&);  // Not implemented.

	/**
	 * Boolean flag denoting whether there was an error.
	 */
	bool error;


	/**
	 * the upper Limit for the Altitude
	 */
	float UpperLimitAltitude;
	/**
	 * the lower Limit for the Altitude
	 */
	float LowerLimitAltitude;


	/**
	 * the lower Limit for the Potential Temperature
	 */
	float lowerPotTemperature;
	/**
	 * the upper Limit of the Potential Temperature
	 */
	float upperPotTemperature;


	/**
	 * the lower Limit for the potential Vorticity
	 */
	float lowerVorticity;
	/**
	 * the upper Limit for the potential Vorticity
	 */
	float upperVorticity;


	/**
	 * The lower Limit for the Pressure
	 */
	float lowerPressure;
	/**
	 * The upper Limit for the Pressure
	 */
	float upperPressure;

	/**
	 * the lower Limit of the Temperature
	 */
	float lowerTemperature;
	/**
	 * the upper Limit of the Temperature
	 */
	float upperTemperature;

	/**
	 * The lower Limit for the Time
	 */
	double lowerDate;
	/**
	 * The upper Limit for the Time
	 */
	double upperDate;

	/**
	 * Decide whether a data point should be kept in the selection.
	 * @param pointIndex The index of the point to be checked
	 * @param coordinate The coordinate of the point
	 * @param pointData All scalar point data
	 * @return True if the point should be kept, false otherwise
	 */
	bool evaluatePoint(int pointIndex, Coordinate coordinate, vtkPointData* pointData);

};

#endif
