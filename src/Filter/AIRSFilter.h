#ifndef __AIRSFilter_h
#define __AIRSFilter_h

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
 * This class extracts the attributes of the AIRS Data, and displays them with Sliders
 */
class AIRSFilter  : public AbstractSelectionFilter {
public:
	static AIRSFilter* New();
	vtkTypeMacro(AIRSFilter, vtkDataObjectAlgorithm);

	void SetInputConnection(vtkAlgorithmOutput* input);


	/**
	 * This Method sets the min and max values of the Time to be represented in silders
	 * @param lowerLimit the lower range
	 * @param upperLimit the upper range
	 */
	void setTimeThreshold(double lowerLimit, double upperLimit);

	/**
	* This Method sets the min and max values of the SO2 Index to be represented in silders
	* @param lowerLimit the lower range
	* @param upperLimit the upper range
	*/
	void setSOThreshold(float lowerLimit, float upperLimit);

	/**
	* This Method sets the min and max values of the Ash Index to be represented in silders
	* @param lowerLimit the lower range
	* @param upperLimit the upper range
	*/
	void setAshThreshold(float lowerLimit, float upperLimit);
	int RequestInformation(vtkInformation* request,
	                       vtkInformationVector** inputVector,
	                       vtkInformationVector* outputVector) override;


	/**
	 * Callback for the input array selection. This has to exist for the filter to be correctly assembled but can be ignored since the scalar is locked to the temperature values and the UI is hidden.
	 */
	void ignore(int id, int port, int connection, int fieldAssociation, const char* name) { }

private:

	AIRSFilter();
	~AIRSFilter();
	AIRSFilter(const AIRSFilter&); //Not implemented
	void operator=(const AIRSFilter&); //Not implemented
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
	 * the upper Limit for the SO2 Index
	 */
	float upperSOLimit;
	/**
	 * the lower Limit for the SO2 Index
	 */
	float lowerSOLimit;

	/**
	 * The upper Limit for the Ash Index
	 */
	float upperAshLimit;
	/**
	 * the lower Limit for the Ash Index
	 */
	float lowerAshLimit;

};

#endif
