#ifndef KRONOS_ABSTRACT_SELECTION_FILTER_HPP
#define KRONOS_ABSTRACT_SELECTION_FILTER_HPP

#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkDataObjectAlgorithm.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkPointData.h>
#include <vtkIntArray.h>
#include <vtkFloatArray.h>
#include <vtkUnsignedIntArray.h>
#include <vtkUnsignedCharArray.h>

#include <qstring.h>
#include <qlist.h>

#include <Reader/DataReader/Data.hpp>
#include <Globe/Coordinate.hpp>


/**
 * This class extracts the attributes of the MIPAS Data, and displays them with Sliders
 * to be able to change it's appearance.
 */
class MIPASFilter : public vtkDataObjectAlgorithm {
public:

	vtkTypeMacro(MIPASFilter, vtkDataObjectAlgorithm)
	static MIPASFilter* New();

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
	 * Sets the upper Limit for Time
	 * @param upperLimit the upper Limit of the Time
	 */
	void setUpper(double upperLimit);

	/**
	 * Sets th lower Limit for Time
	 * @param lowerLimit the lower Limit of the Time
	 */
	void setLower(double lowerLimit);

	/**
	 * Sets the upper Limit of the Orbit
	 * @param upperLimit the upper Limit of the Orbit
	 */
	void setUpperOrbit(int upperLimit);

	/**
	 * Sets th upper Limit for Orbit
	 * @param lowerLimit the lower Limit of the Orbit
	 */
	void setLowerOrbit(int lowerLimit);

	/**
	 * Sets the upper Limit of the Altitude
	 * @param upperLimit the upper Limit of the Altitude
	 */
	void setUpperAltitude(float upperLimit);

	/**
	 * Sets the lower Limit of the Altitude
	 * @param lowerLimit the upper Limit of the Altitude
	 */
	void setLowerAltitude(float lowerLimit);




protected:
	/**
	 * Display an error message and remember that this filter does not hold valid data.
	 * @param message The error message to be shown to the user
	 */
	void fail(QString message);

private:
	MIPASFilter();
	~MIPASFilter();

	MIPASFilter(const MIPASFilter&);  // Not implemented.
	void operator=(const MIPASFilter&);  // Not implemented.

	/**
	 * Boolean flag denoting whether there was an error.
	 */
	bool error;

	/**
	 * upper limit for the time
	 */
	double UpperLimit;
	/**
	 * lower limit for the time
	 */
	double LowerLimit;

	/**
	 * upper limit for orbit
	 */
	int UpperLimitOrbit;
	/**
	 * lower limit for orbit
	 */
	int LowerLimitOrbit;

	/**
	 * upper limit for altitude
	 */
	float UpperLimitAltitude;
	/**
	 * lower limit for altitude
	 */
	float LowerLimitAltitude;




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
