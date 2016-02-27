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
 * Abstract super class for filters that work on a Kronos point data set and extract some of those points.
 */
class CLaMSFilter : public vtkDataObjectAlgorithm {
public:

    vtkTypeMacro(CLaMSFilter, vtkDataObjectAlgorithm)
    static CLaMSFilter *New();

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

    //lower and upper Limit for altitude
    void setUpperAltitude(float upperLimit);
    void setLowerAltitude(float lowerLimit);

    //lower and upper Limit for PotTemperature
    void setLowerPotTemperature(float lowerLimit);
    void setUpperPotTemperature(float upperLimit);

    //lower and upper Limit for the vorticity
    void setLowerVorticity(float lowerLimit);
    void setUpperVorticity(float upperLimit);

    //lower and upper limit for pressure
    void setLowerPressure(float lowerLimit);
    void setUpperPressure(float upperLimit);

    //lower and upper Limit for Temperature
    void setLowerTemperature(float lowerLimit);
    void setUpperTemperature(float upperLimit);

    //lower and upper Limit for the date
    void setLowerDate(double lowerLimit);
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

  // Lower and Upper Limit for the altitution
    float UpperLimitAltitude;
    float LowerLimitAltitude;

    //lower and upper limit for the PotTemperature
    float lowerPotTemperature;
    float upperPotTemperature;

    //lower and upper Limit for the vorticity
    float lowerVorticity;
    float upperVorticity;

    //lower and upper limit for the pressure
    float lowerPressure;
    float upperPressure;

    //lower and upper Limit fot the Temperature
    float lowerTemperature;
    float upperTemperature;

    //lower and upper Limit for the date
    double lowerDate;
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
