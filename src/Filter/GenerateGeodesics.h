#ifndef KRONOSGENERATEGEODESICS_H
#define KRONOSGENERATEGEODESICS_H

#include<vtkPolyDataAlgorithm.h>

template<typename T> class QVector;

#ifndef _MSC_VER
	template<typename T> class Spherical;
	template<typename T> class Cartesian;
#else
	template<typename T> class Vector3;
	template<typename T> using Spherical = Vector3<T>;
	template<typename T> using Cartesian = Vector3<T>;
#endif
typedef Spherical<double> GPS;
typedef Cartesian<double> World;

class GenerateGeodesics : public vtkPolyDataAlgorithm {
public:
	vtkTypeMacro(GenerateGeodesics, vtkPolyDataAlgorithm)
	static GenerateGeodesics* New();
	void PrintSelf(ostream& os, vtkIndent indent) override;

	int RequestData(vtkInformation* info,
	                vtkInformationVector** inputVector,
	                vtkInformationVector* outputVector) override;

	int RequestInformation(vtkInformation* request, vtkInformationVector** inputVector,
	                       vtkInformationVector* outputVector) override;

	int FillOutputPortInformation(int, vtkInformation* info) override;

	int FillInputPortInformation(int, vtkInformation* info) override;

	/**
	 * @brief setArcSize set the arc size.
	 * The arc size is a measurement of how much the flights will be displayed above the earth surface.
	 * In detail the geodesic arc is drawn as sth like a circle sector. The arc size modifies the radius
	 * of this circle
	 * @param value the arc size
	 */
	void setArcSize(double value);

	/**
	 * @brief setLoD set the level of detail.
	 * The Geodesics are approximated by multiple straight lines. The level of detail is related to the
	 * maximum length of a single straight line.
	 * @param value the level of detail to set.
	 */
	void setLoD(double value);

	/**
	 * @brief setLimitDepth determines if the calculation depth will be limited.
	 * @param value true if you want to limit the calculation.
	 */
	void setLimitDepth(bool value);

private:
	GenerateGeodesics();
	GenerateGeodesics(const GenerateGeodesics&); // not implemented
	void operator =(const GenerateGeodesics&); // not implemented

	// defaults will be set by ParaView as well
	double maxLenOfLineSegment = 0.0;
	double radius = 0.0;
	bool limitCalcDepth = true;
	bool firstRequestInformation = true;

	/**
	 * @brief insertNextFlight calculate the neccessary points between start and end airport, insert them into the given data set
	 * @param start the start airport
	 * @param end the destination airport
	 * @param dataSet the data set to insert the points
	 */
	void insertNextFlight(const GPS& start, const GPS& end,
	                      vtkPolyData* const dataSet);

	/**
	 * @brief insertAndConnectPoints insert and connect the points.
	 * This is neccessary twice if the flight goes over the date limit.
	 * @param dataSet the data set to insert into
	 * @param points the points to insert and connect
	 */
	void insertAndConnectPoints(vtkPolyData* dataSet, QVector<GPS>& points);

	/**
	 * @brief getCircleCenterPoint get the center point to draw the geodesic circle
	 * @param point1 the start point
	 * @param point2 the end point
	 * @param radius the radius to use
	 * @return the center point of a circle through point1 and point2 with radius
	 */
	World getCircleCenterPoint(const World& point1, const World& point2,
	                           double radius);

	/**
	 * @brief getPointInbetween calculates point between point1 and point2.
	 * This is neccessary to increase the detail of the geodesic.
	 * @param point1 the first point
	 * @param point2 the second point
	 * @param center the circle center
	 * @return a point inbetween
	 */
	GPS getPointInbetween(const GPS& point1, const GPS& point2,
	                      const World& center);

	/**
	 * @brief movePointToOppositeSide translates the point to the opposite side of the map(that is +- 180 lat)
	 * @param point the point to move
	 */
	GPS moveToOtherSide(GPS point);
};

#endif // KRONOSGENERATEGEODESICS_H
