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
	this->radius = value * Configuration::getInstance().getDouble("world.radius");
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
		if (treeDepth > 20) {
			vtkWarningMacro( << "  ==>> Possibly caught in infinite calculation. Advancing to next point")
			index++;
			treeDepth--;
		}
	}

	insertAndConnectPoints(dataSet, points);
}

GPS GenerateGeodesics::moveToOtherSide(GPS point) {
	if (point.x < 0) {
		point.x += 360;
	} else {
		point.x -= 360;
	}

	return point;
}

void GenerateGeodesics::insertAndConnectPoints(vtkPolyData* dataSet,
        QVector<GPS >& points) {
	int currentPointIndex = dataSet->GetPoints()->GetNumberOfPoints();
	dataSet->GetLines()->InsertNextCell(points.size());

	QVectorIterator<GPS> it(points);
	while (it.hasNext()) {
		dataSet->GetPoints()->InsertNextPoint(it.next().array());
		dataSet->GetLines()->InsertCellPoint(currentPointIndex);
		currentPointIndex++;
	}
}

Cartesian GenerateGeodesics::getCircleCenterPoint(const Cartesian& point1,
        const Cartesian& point2, double radius) {
	Cartesian center = (point1 + point2) / 2;
	Cartesian p1p2 = point2 - point1;

	Cartesian orthToP1P2 = p1p2.cross(point1.cross(point2));
	double absOfCenterCircle = sqrt(radius * radius - p1p2.length() * p1p2.length() / 4);
	Cartesian circleCenter = center + orthToP1P2.normTyped() * absOfCenterCircle;

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
	middle /= 2;

	GPS retVal = cartesianToSpherical(middle);

	return scaleTo(retVal, (point1.z + point2.z) / 2);
}
