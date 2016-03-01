#ifndef __MIPASFilter_h
#define __MIPASFilter_h

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
 * This class extracts the attributes of the MIPAS Data, and displays them with Sliders
 * to be able to change the AIRS_am and AIRS_pm appearance.
 */
class MIPASFilter  : public AbstractSelectionFilter {
public:
	static MIPASFilter* New();
	vtkTypeMacro(MIPASFilter, vtkDataObjectAlgorithm);

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
	* This Method sets the min and max values of the Orbit Index to be represented in silders
	* @param lowerLimit the lower range
	* @param upperLimit the upper range
	*/
	void setOrbitThreshold(float lowerLimit, float upperLimit);

	
	int RequestInformation(vtkInformation* request,
	                       vtkInformationVector** inputVector,
	                       vtkInformationVector* outputVector) override;


	/**
	 * Callback for the input array selection. This has to exist for the filter to be correctly assembled but can be ignored since the scalar is locked to the temperature values and the UI is hidden.
	 */
	void ignore(int id, int port, int connection, int fieldAssociation, const char* name) { }

private:

	MIPASFilter();
	~MIPASFilter();
	MIPASFilter(const MIPASFilter&); //Not implemented
	void operator=(const MIPASFilter&); //Not implemented
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
	 * The upper Limit for the Orbit Index
	 */
	float upperOrbitLimit;
	/**
	 * the lower Limit for the Orbit Index
	 */
	float lowerOrbitLimit;

};

#endif
