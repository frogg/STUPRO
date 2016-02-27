#ifndef KRONOS_ABSTRACT_SELECTION_FILTER_HPP
#define KRONOS_ABSTRACT_SELECTION_FILTER_HPP

#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkDataObjectAlgorithm.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>

#include <qstring.h>
#include <qlist.h>

#include <Reader/DataReader/Data.hpp>
#include <Globe/Coordinate.hpp>

/**
 * This class extracts the attributes of the AIRS Data, and displays them with Sliders
 * to be able to change the AIRS_am and AIRS_pm appearance.
 */
class AIRSFilter : public vtkDataObjectAlgorithm {
public:

	vtkTypeMacro(AIRSFilter, vtkDataObjectAlgorithm)
	static AIRSFilter* New();

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


	/**
	 * Sets the upper Limit for the given Time
	 * @param upperLimit the upper Limit of the Time
	 */
	void setUpper(double upperLimit);

	/**
	 * Sets the lower Limit for the given Time
	 * @param lowerLimit the lower Limit of the Time
	 */
	void setLower(double lowerLimit);

	/**
	 * Sets the upper Limit for the given Ash Index
	 * @param upperLimit the upper Limit of the Ash Index
	 */
	void setUpperAsh(float upperLimit);

	/**
	 * Sets the lower Limit for the given Ash Index
	 * @param lowerLimit the lower Limit of the Ash Index
	 */
	void setLowerAsh(float lowerLimit);

	/**
	 * Sets the upper Limit for the given SO Index
	 * @param upperLimit the upper Limit of the SO Index
	 */
	void setUpperSO(float upperLimit);

	/**
	 * Sets the lower Limit for the given SO Index
	 * @param lowerLimit the lower Limit of the SO Index
	 */
	void setLowerSO(float lowerLimit);


protected:
	/**
	 * Display an error message and remember that this filter does not hold valid data.
	 * @param message The error message to be shown to the user
	 */
	void fail(QString message);

private:
	AIRSFilter();
	~AIRSFilter();

	AIRSFilter(const AIRSFilter&);  // Not implemented.
	void operator=(const AIRSFilter&);  // Not implemented.

	/**
	 * Boolean flag denoting whether there was an error.
	 */
	bool error;

	/**
	 * the upper Limit for the Time
	 */
	double UpperLimit;
	/**
	 * the lower Limit for the Time
	 */
	double LowerLimit;

	/**
	 * the upper Limit for the ash Index
	 */
	float UpperLimitAsh;
	/**
	 * the lower Limit for the ash Index
	 */
	float LowerLimitAsh;

	/**
	 * the upper Limit for the SO Index
	 */
	float UpperLimitSO;
	/**
	 * the lower Limit for the SO Index
	 */
	float LowerLimitSO;

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
