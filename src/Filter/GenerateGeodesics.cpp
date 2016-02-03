#include "GenerateGeodesics.h"

#include <vtkObjectFactory.h>
#include <vtkCellArray.h>
#include <vtkInformation.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>

#include <QVector>

#include "Utils/Math/Vector3.hpp"
#include "Utils/Math/SphericalCoordinateFunctions.h"

vtkStandardNewMacro(GenerateGeodesics)

void GenerateGeodesics::PrintSelf(std::ostream& os, vtkIndent indent) {
    Superclass::PrintSelf(os, indent);
    os << indent << "Generate Geodesics, Kronos Project" << endl;
}

int GenerateGeodesics::RequestData(vtkInformation* info, vtkInformationVector** inputVector,
                                   vtkInformationVector* outputVector) {
    vtkPolyData* output = vtkPolyData::GetData(outputVector);
    vtkPoints* points = vtkPoints::New();
    vtkCellArray* lines = vtkCellArray::New();

    int numberOfFlights = 1;
    int numberOfPointsPerFlight[numberOfFlights];

    // for each flight
    for (int flight = 0; flight < numberOfFlights; flight++) {
        // insert moar points into points
        Vector3d start(40.712778, -74.005833, 1); // New York City
        Vector3d end(52.518611, 13.408333, 1); // Berlin
        numberOfPointsPerFlight[flight] = calculateFlightPoints(start, end, points);
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



