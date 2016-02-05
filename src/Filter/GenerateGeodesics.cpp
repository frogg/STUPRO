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
            || input->GetPointData()->GetArray(DESTINATION_ARRAY_NAME)->GetNumberOfTuples() != numberOfFlights) {
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

        vtkWarningMacro(<< "Next Point:"
                        << QString("  (%1|%2|%3)").arg(start.x).arg(start.y).arg(start.z).toStdString()
                        << QString("  (%1|%2|%3)").arg(destination.x).arg(destination.y).arg(destination.z).toStdString());

        numberOfPointsPerFlight[flight] = calculateFlightPoints(start, destination, points);
	}
	output->SetPoints(points);

	// draw lines between the points
	for (int flight = 0, index = 0; flight < numberOfFlights; flight++, index++) {
		int flightStartIdx = index;

		lines->InsertNextCell(numberOfPointsPerFlight[flight]);
		// for each point of a flight
		for (index; index < flightStartIdx + numberOfPointsPerFlight[flight]; index++) {
			lines->InsertCellPoint(index);
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

GenerateGeodesics::GenerateGeodesics() {
}

int GenerateGeodesics::calculateFlightPoints(const Vector3d& start, const Vector3d& end,
        vtkPoints* const dataSet) {
	QVector<Vector3d> points;
	points.append(start);
	points.append(end);

	// calculate points between if necessary
	int index = 1;
	while (index < points.size()) {
		if ((points.at(index) - points.at(index - 1)).length() > maxLenOfLineSegment) {
			points.insert(index, calculateCenter(points.at(index), points.at(index - 1)));
		} else {
			index++;
		}
	}

	for (QVectorIterator<Vector3d> it (points); it.hasNext();) {
		dataSet->InsertNextPoint(it.next().array());
	}

	return points.size();
}



