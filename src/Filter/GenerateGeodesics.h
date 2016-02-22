#ifndef KRONOSGENERATEGEODESICS_H
#define KRONOSGENERATEGEODESICS_H

#include<vtkPolyDataAlgorithm.h>

template<typename T> class Vector3;

class GenerateGeodesics : public vtkPolyDataAlgorithm {
public:
	vtkTypeMacro(GenerateGeodesics, vtkPolyDataAlgorithm)
	static GenerateGeodesics* New();
	void PrintSelf(ostream& os, vtkIndent indent) override;

	int RequestData(vtkInformation* info,
					vtkInformationVector** inputVector,
					vtkInformationVector* outputVector) override;

	int FillOutputPortInformation(int, vtkInformation* info) override;

	int FillInputPortInformation(int, vtkInformation* info) override;

    /**
     * @brief setArcSize set the arc size.
     * The arc size is a measurement of how much the flights will be displayed above the earth surface.
     * In detail the geodesic arc is drawn as a circle sector from a point m, which is aequidistant to
     * both the start and target airport. The arc size is the difference of the (real) earth radius and
     * the radius of the geodesics.
     * @param value the arc size
     */
    void setArcSize(double value);

private:
	GenerateGeodesics();
	GenerateGeodesics(const GenerateGeodesics&); // not implemented
	void operator =(const GenerateGeodesics&); // not implemented

	double maxLenOfLineSegment = 0.9;
    double arcSize = 0.0;

	/**
     * @brief insertNextFlight calculate the neccessary points between start and end airport, insert them into the given data set
	 * @param start the start airport
     * @param end the destination airport
     * @param dataSet the data set to insert the points
	 */
    void insertNextFlight(const Vector3<double>& start, const Vector3<double>& end,
                              vtkPolyData* const dataSet);

    /**
     * @brief movePointToOppositeSide translates the point to the opposite side (that is +- 180 lat)
     * @param point the point to move
     */
    Vector3<double> movePointToOppositeSide(Vector3<double> point);
};

#endif // KRONOSGENERATEGEODESICS_H
