#include "GenerateGeodesics.h"

#include<vtkObjectFactory.h>
#include<vtkCellArray.h>
#include<vtkInformation.h>
#include<vtkFloatArray.h>
#include<vtkPointData.h>

#include <Utils/Math/Vector3.hpp>

#define RESOLUTION 2 // number of line segments

vtkStandardNewMacro(GenerateGeodesics)

void GenerateGeodesics::PrintSelf(std::ostream& os, vtkIndent indent) {
    Superclass::PrintSelf(os, indent);
    os << indent << "Generate Geodesics, Kronos Project" << endl;
}

int GenerateGeodesics::RequestData(vtkInformation* info, vtkInformationVector** inputVector,
                                   vtkInformationVector* outputVector) {
    vtkPolyData* output = vtkPolyData::GetData(outputVector);

    vtkPoints* points = vtkPoints::New();

    // for each flight
    for (int i = 0; i < 1; i++) {
        // insert moar points into points
    }

    output->SetPoints(points);

    vtkCellArray* lines = vtkCellArray::New();

    // draw lines between the points
    for (int flight = 0, index = 0; flight < 1; flight++, index++) {
        int flightStartIdx = index;

        lines->InsertNextCell(RESOLUTION + 1);
        // for each point of a flight
        for (index; index <= flightStartIdx + RESOLUTION; index++) {
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
int GenerateGeodesics::addFlightPoints(Vector3<double>& start, Vector3<double>& end,
                                       float maxLineLenght) {
    // TODO
}



