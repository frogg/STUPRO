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
    static MIPASFilter *New();

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

    //Set upper and lower Limit Time
    void setUpper(double upperLimit);
    void setLower(double lowerLimit);

    //Set upper and Lower Limit Orbit
    void setUpperOrbit(int upperLimit);
    void setLowerOrbit(int lowerLimit);

    //Set upper and lower limit altitude
    void setUpperAltitude(float upperLimit);
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

    double UpperLimit;
    double LowerLimit;

    //lower and upper limit for orbit
    int UpperLimitOrbit;
    int LowerLimitOrbit;

    //lower and upper limit for altitude
    float UpperLimitAltitude;
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
