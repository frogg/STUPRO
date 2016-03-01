#ifndef __CLaMSFilter_h
#define __CLaMSFilter_h

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
#include <vtkFloatArray.h>

/**
 * This class extracts the attributes of the CLaMS Data, and displays them with Sliders
 */
class CLaMSFilter  : public AbstractSelectionFilter {
public:
	static CLaMSFilter* New();
	vtkTypeMacro(CLaMSFilter, vtkDataObjectAlgorithm);

	void SetInputConnection(vtkAlgorithmOutput* input);


	/**
	 * This Method sets the min and max values of the Time to be represented in silders
	 * @param lowerLimit the lower range
	 * @param upperLimit the upper range
	 */
	void setTimeThreshold(double lowerLimit, double upperLimit);

	/**
	* This Method sets the min and max values of the Altitude Index to be represented in silders
	* @param lowerLimit the lower range
	* @param upperLimit the upper range
	*/
	void setAltitudeThreshold(float lowerLimit, float upperLimit);

	/**
	* This Method sets the min and max values of the Temperature Index to be represented in silders
	* @param lowerLimit the lower range
	* @param upperLimit the upper range
	*/
	void setTemperatureThreshold(float lowerLimit, float upperLimit);

	/**
	* This Method sets the min and max values of the Pressure Index to be represented in silders
	* @param lowerLimit the lower range
	* @param upperLimit the upper range
	*/
	void setPressureThreshold(double lowerLimit, double upperLimit);

	/**
	* This Method sets the min and max values of the Vorticity Index to be represented in silders
	* @param lowerLimit the lower range
	* @param upperLimit the upper range
	*/
	void setVorticityThreshold(double lowerLimit, double upperLimit);

	/**
	* This Method sets the min and max values of the Pot_Temperature Index to be represented in silders
	* @param lowerLimit the lower range
	* @param upperLimit the upper range
	*/
	void setPotTemperatureThreshold(double lowerLimit, double upperLimit);
	int RequestInformation(vtkInformation* request,
	                       vtkInformationVector** inputVector,
	                       vtkInformationVector* outputVector) override;


	/**
	 * Callback for the input array selection. This has to exist for the filter to be correctly assembled but can be ignored since the scalar is locked to the temperature values and the UI is hidden.
	 */
	void ignore(int id, int port, int connection, int fieldAssociation, const char* name) { }

private:

	CLaMSFilter();
	~CLaMSFilter();
	CLaMSFilter(const CLaMSFilter&); //Not implemented
	void operator=(const CLaMSFilter&); //Not implemented
	QList<Data::Type> getCompatibleDataTypes() override;
	bool evaluatePoint(int pointIndex, Coordinate coordinate, vtkPointData* pointData) override;

	/**
	 * the upper Limit for the Time
	 */
	double upperTimeLimit;

	/**
	 * the lower Limit for the Time
	 */
	double lowerTimeLimit;
	/**
	 * the upper Limit for the Altitude Index
	 */
	float upperAltitudeLimit;
	/**
	 * the lower Limit for the Altitude Index
	 */
	float lowerAltitudeLimit;

	/**
	 * The upper Limit for the Temperature Index
	 */
	float upperTemperatureLimit;
	/**
	 * the lower Limit for the Temperature Index
	 */
	float lowerTemperatureLimit;

	/**
	 * the upper Limit for the Pressure
	 */
	double upperPressureLimit;

	/**
	 * the lower Limit for the Pressure
	 */
	double lowerPressureLimit;


	/**
	 * the upper Limit for the vorticity index
	 */
	double upperVorticityLimit;

	/**
	 * the lower Limit for the vorticity index
	 */
	double lowerVorticityLimit;

	/**
	 * the upper Limit for the Pot_temperature index
	 */
	double upperPotTemperatureLimit;

	/**
	 * the lower Limit for the Pot_temperature index
	 */
	double lowerPotTemperatureLimit;

};

#endif
