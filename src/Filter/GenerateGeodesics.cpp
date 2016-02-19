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
	vtkCellArray* lines = vtkCellArray::New();

	int numberOfPointsPerFlight[numberOfFlights];

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

		numberOfPointsPerFlight[flight] = calculateFlightPoints(start, destination, points);
	}
	output->SetPoints(points);

	// draw lines between the points
	int currentPointIndex = 0;
	for (int flight = 0; flight < numberOfFlights; flight++) {
		int flightStartIdx = currentPointIndex;

		lines->InsertNextCell(numberOfPointsPerFlight[flight]);
		// for each point of a flight
		while (currentPointIndex < flightStartIdx + numberOfPointsPerFlight[flight]) {
			lines->InsertCellPoint(currentPointIndex);
			currentPointIndex++;
		}
	}

	output->SetLines(lines);

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

int GenerateGeodesics::calculateFlightPoints(const Vector3d& start, const Vector3d& end,
        vtkPoints* const dataSet) {
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

	QVectorIterator<Vector3d> it (points);
	while ( it.hasNext()) {
		dataSet->InsertNextPoint(it.next().array());
	}

	return points.size();
}



