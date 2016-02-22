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
		Vector3d start(startPoints->GetTuple(flight)[0],
		               startPoints->GetTuple(flight)[1],
		               0);
		Vector3d destination(destinationPoints->GetTuple(flight)[0],
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
	this->arcSize = value;
}

GenerateGeodesics::GenerateGeodesics() {
}


void GenerateGeodesics::insertNextFlight(const Vector3d& start, const Vector3d& end,
        vtkPolyData* const dataSet) {
	QVector<Vector3d> points;
	points.append(start);
	points.append(end);

	// calculate points between if necessary
	int index = 1;
	int treeDepth = 0;
	while (index < points.size()) {
		if (distance(points.at(index), points.at(index - 1)) > maxLenOfLineSegment) {
			points.insert(index, calculateCenter(points.at(index), points.at(index - 1)));
			treeDepth++;
		} else if (abs(points.at(index - 1).x - points.at(index).x) > 180) {
			Vector3d p = points.at(index - 1);
			Vector3d v = moveToOtherSide(points.at(index)) - p;
			Vector3d center = p + (((p.x < 0 ? -180 : 180) - p.x ) / v.x) * v;
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

Vector3<double> GenerateGeodesics::moveToOtherSide(Vector3<double> point) {
	if (point.x < 0) {
		point.x += 360;
	} else {
		point.x -= 360;
	}

	return point;
}

void GenerateGeodesics::insertAndConnectPoints(vtkPolyData* dataSet,
        QVector<Vector3<double> >& points) {
	int currentPointIndex = dataSet->GetPoints()->GetNumberOfPoints();
	dataSet->GetLines()->InsertNextCell(points.size());

	QVectorIterator<Vector3d> it(points);
	while (it.hasNext()) {
		dataSet->GetPoints()->InsertNextPoint(it.next().array());
		dataSet->GetLines()->InsertCellPoint(currentPointIndex);
		currentPointIndex++;
	}
}
