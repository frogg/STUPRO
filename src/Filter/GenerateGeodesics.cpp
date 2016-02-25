#include "GenerateGeodesics.h"

#include <vtkObjectFactory.h>
#include <vtkCellArray.h>
#include <vtkInformation.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>

#include <QVector>
#include <QString>

#include "Utils/Math/Vector3.hpp"
#include "Utils/Math/SphericalCoordinateFunctions.h"

typedef Vector3d GPS;
typedef Vector3d Cartesian;

vtkStandardNewMacro(GenerateGeodesics)

#define DESTINATION_ARRAY_NAME "destinations"

void GenerateGeodesics::PrintSelf(std::ostream& os, vtkIndent indent) {
	Superclass::PrintSelf(os, indent);
	os << indent << "Generate Geodesics, Kronos Project" << endl;
}

int GenerateGeodesics::RequestData(vtkInformation* info, vtkInformationVector** inputVector,
                                   vtkInformationVector* outputVector) {
	vtkPolyData* input = vtkPolyData::GetData(inputVector[0]);
	vtkPolyData* output = vtkPolyData::GetData(outputVector);

	if (!input) {
		vtkErrorMacro( << "Input Error. Aborting...");
		return 0;
	}

	int numberOfFlights = input->GetNumberOfPoints();

	if (!input->GetPointData()->GetArray(DESTINATION_ARRAY_NAME)
	        || input->GetPointData()->GetArray(DESTINATION_ARRAY_NAME)->GetNumberOfTuples() !=
	        numberOfFlights) {
		vtkErrorMacro( << "The input data has an unexpected format. Details:" << endl
		               << "    Number of input arrays: " << input->GetPointData()->GetNumberOfArrays() << endl
		               << "    Array size: " << numberOfFlights << " and "
		               << input->GetPointData()->GetArray(DESTINATION_ARRAY_NAME)->GetNumberOfTuples());
		return 0;
	}

	vtkPoints* points = vtkPoints::New();
	output->SetPoints(points);
	vtkCellArray* lines = vtkCellArray::New();
	output->SetLines(lines);

	vtkDataArray* startPoints = input->GetPoints()->GetData();
	vtkDataArray* destinationPoints = input->GetPointData()->GetArray(DESTINATION_ARRAY_NAME);

	// for each flight
	for (int flight = 0; flight < numberOfFlights; flight++) {
		// insert moar points into points
		GPS start(startPoints->GetTuple(flight)[0],
		          startPoints->GetTuple(flight)[1],
		          0);
		GPS destination(destinationPoints->GetTuple(flight)[0],
		                destinationPoints->GetTuple(flight)[1],
		                0);

		insertNextFlight(start, destination, output);
	}

	return 1;
}

int GenerateGeodesics::FillOutputPortInformation(int, vtkInformation* info) {
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
	return 1;
}

int GenerateGeodesics::FillInputPortInformation(int, vtkInformation* info) {
	info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
	return 1;
}

void GenerateGeodesics::setArcSize(double value) {
	// this value is statistically determined
	double v1 = 0.95 - 0.95 * pow(M_E, -4.25 * value);
	double v2 = cbrt(0.85 * value);
	this->radius = (v1 + v2) / 2;

	this->Modified();
}

void GenerateGeodesics::setLoD(double value) {
	// maxLenOfLineSegment should always be \in (0, radius] and grow exponentially
	this->maxLenOfLineSegment = Configuration::getInstance().getDouble("globe.radius") * (pow(2,
								1 - value) - 0.99);
	this->Modified();
}

void GenerateGeodesics::setLimitDepth(bool value) {
	this->limitCalcDepth = value;
}

GenerateGeodesics::GenerateGeodesics() {
}

void GenerateGeodesics::insertNextFlight(const GPS& start, const GPS& end,
        vtkPolyData* const dataSet) {
	QVector<GPS> points;
	points.append(start);
	points.append(end);

	Cartesian center = getCircleCenterPoint(sphericalToCartesian
	                                        (start), sphericalToCartesian(end), radius);

	// calculate points between if necessary
	int index = 1;
	int treeDepth = 0;
	while (index < points.size()) {
		if (distance(points.at(index), points.at(index - 1)) > maxLenOfLineSegment) {
			points.insert(index, getPointInbetween(points.at(index), points.at(index - 1), center));
			treeDepth++;
		} else if (abs(points.at(index - 1).x - points.at(index).x) > 180) {
			// NOTE: This is not exactly correct, but simpler to implement
			GPS p = points.at(index - 1);
			GPS v = moveToOtherSide(points.at(index)) - p;
			v /= v.x;
			if (p.x < 0) {
				v *= -180 - p.x;
			} else {
				v *= 180 - p.x;
			}
			GPS center = p + v;
			points.insert(index, center);

			QVector<Vector3d> nextPoints;
			while (points.size() > index + 1) {
				nextPoints.append(points.last());
				points.remove(points.size() - 1);
			}

			insertAndConnectPoints(dataSet, points);

			points = nextPoints;
			points.insert(0, moveToOtherSide(center));

			index = 1;
		} else {
			index++;
			treeDepth--;
		}
		if (this->limitCalcDepth && treeDepth > 10) {
			vtkWarningMacro( << "  ==>> Possibly caught in infinite calculation. Advancing to next point."
							 << endl
							 << "Remove the limit of the calculation depth if you need more detail. This may"
							 << " result in an infinity loop though, so be careful.")
			index++;
			treeDepth--;
		}
	}

	insertAndConnectPoints(dataSet, points);
}

void GenerateGeodesics::insertAndConnectPoints(vtkPolyData* dataSet,
        QVector<GPS >& points) {
	int currentPointIndex = dataSet->GetPoints()->GetNumberOfPoints();
	dataSet->GetLines()->InsertNextCell(points.size());

	QVectorIterator<GPS> it(points);
	while (it.hasNext()) {
		GPS point = it.next();
		if (point.z < 0) {
			// fixes a problem with the arc
			point.z = -point.z;
		}
		dataSet->GetPoints()->InsertNextPoint(point.array());
		dataSet->GetLines()->InsertCellPoint(currentPointIndex);
		currentPointIndex++;
	}
}

Cartesian GenerateGeodesics::getCircleCenterPoint(const Cartesian& point1,
        const Cartesian& point2, double radius) {
	GPS middle = getPointInbetween(cartesianToSpherical(point1),
								   cartesianToSpherical(point2),
								   Vector3d(0, 0, 0));

	Cartesian circleCenter = sphericalToCartesian(middle) * radius;

	return circleCenter;
}

GPS GenerateGeodesics::getPointInbetween(const GPS& point1, const GPS& point2,
        const Cartesian& center) {
	Cartesian p1 = sphericalToCartesian(point1);
	Cartesian p2 = sphericalToCartesian(point2);

	// normalize points (that is: make the center the origin)
	p1 -= center;
	p2 -= center;

	// get point in between
	Cartesian middle = p1 + p2;
	// apply correct scale
	middle = middle.normTyped() * (p1.lengthTyped() + p2.lengthTyped()) / 2;
	// re-scale the point
	middle += center;

	GPS retVal = cartesianToSpherical(middle);

	return retVal;
}

GPS GenerateGeodesics::moveToOtherSide(GPS point) {
	if (point.x < 0) {
		point.x += 360;
	} else {
		point.x -= 360;
	}

	return point;
}
