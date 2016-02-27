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
    static AIRSFilter *New();

	void PrintSelf(ostream& os, vtkIndent indent) override;

	int RequestData(vtkInformation* info,
	                vtkInformationVector** inputVector,
	                vtkInformationVector* outputVector) override;
	int RequestInformation(vtkInformation* request,
	                       vtkInformationVector** inputVector,
	                       vtkInformationVector* outputVector) override;

	int FillOutputPortInformation(int port, vtkInformation* info) override;
	int FillInputPortInformation(int port, vtkInformation* info) override;
    void SetInputConnection(vtkAlgorithmOutput *input) override;

    //set the upper and Lower Limit for the given Time
    void setUpper(double upperLimit);
    void setLower(double lowerLimit);

    //set the upper and Lower Limit for the given Ash Attribute
    void setUpperAsh(float upperLimit);
    void setLowerAsh(float lowerLimit);

    //set the upper and Lower Limit for the given SO Attribute
    void setUpperSO(float upperLimit);
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

    double UpperLimit;
    double LowerLimit;

    //upper and lower Limit for ash
    float UpperLimitAsh;
    float LowerLimitAsh;

    //upper and lower Limit  for SO
    float UpperLimitSO;
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
