#include "GenerateGeodesics.h"

#include <vtkObjectFactory.h>
#include <vtkCellArray.h>
#include <vtkInformation.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkInformationVector.h>

#include <QVector>
#include <QString>

#include "Utils/Math/Vector3.hpp"
#include "Utils/Math/GeographicFunctions.hpp"
#include "Utils/Misc/KronosLogger.hpp"
#include "Reader/DataReader/Data.hpp"

typedef Vector3d GPS;
typedef Vector3d Cartesian;

vtkStandardNewMacro(GenerateGeodesics)

#define DESTINATION_ARRAY_NAME "destinations"
#define PRIORITY_ARRAY_NAME "priorities"
#define AIRLINE_ARRAY_NAME "airlines"
#define START_CODE_ARRAY_NAME "originAirportCodes"
#define DEST_CODE_ARRAY_NAME "destinationAirportCodes"
#define LENGTH_ARRAY_NAME "flightLengths"

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

	// generate the geodesics
	vtkPoints* points = vtkPoints::New();
	output->SetPoints(points);
	vtkCellArray* lines = vtkCellArray::New();
	output->SetLines(lines);

	/*
	 * copy arrays as far as necessary
	 */
	vtkDataArray* startPoints = input->GetPoints()->GetData();
	vtkDataArray* destinationPoints = input->GetPointData()->GetArray(DESTINATION_ARRAY_NAME);
	vtkIntArray* inPrio = vtkIntArray::SafeDownCast(input->GetPointData()->GetArray(
	                          PRIORITY_ARRAY_NAME));
	vtkStringArray* inAirline = vtkStringArray::SafeDownCast(input->GetPointData()->GetAbstractArray(
	                                AIRLINE_ARRAY_NAME));
	vtkStringArray* inStartCode = vtkStringArray::SafeDownCast(input->GetPointData()->GetAbstractArray(
	                                  START_CODE_ARRAY_NAME));
	vtkStringArray* inDestCode = vtkStringArray::SafeDownCast(input->GetPointData()->GetAbstractArray(
	                                 DEST_CODE_ARRAY_NAME));
	vtkFloatArray* inFlightLength = vtkFloatArray::SafeDownCast(input->GetPointData()->GetArray(
	                                    LENGTH_ARRAY_NAME));

	vtkSmartPointer<vtkIntArray> priorities  = vtkSmartPointer<vtkIntArray>::New();
	priorities->SetNumberOfComponents(1);
	priorities->SetName(PRIORITY_ARRAY_NAME);
	output->GetCellData()->AddArray(priorities);
	vtkSmartPointer<vtkStringArray> airlines  = vtkSmartPointer<vtkStringArray>::New();
	airlines->SetName(AIRLINE_ARRAY_NAME);
	output->GetCellData()->AddArray(airlines);
	vtkSmartPointer<vtkStringArray> startCode  = vtkSmartPointer<vtkStringArray>::New();
	startCode->SetName(START_CODE_ARRAY_NAME);
	output->GetCellData()->AddArray(startCode);
	vtkSmartPointer<vtkStringArray> destCode  = vtkSmartPointer<vtkStringArray>::New();
	destCode->SetName(DESTINATION_ARRAY_NAME);
	output->GetCellData()->AddArray(destCode);
	vtkSmartPointer<vtkFloatArray> flightLengths  = vtkSmartPointer<vtkFloatArray>::New();
	flightLengths->SetNumberOfComponents(1);
	flightLengths->SetName(LENGTH_ARRAY_NAME);
	output->GetCellData()->AddArray(flightLengths);

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

		while (priorities->GetNumberOfTuples() < output->GetLines()->GetNumberOfCells()) {
			priorities->InsertNextValue(inPrio->GetValue(flight));
			airlines->InsertNextValue(inAirline->GetValue(flight));
			startCode->InsertNextValue(inStartCode->GetValue(flight));
			destCode->InsertNextValue(inDestCode->GetValue(flight));
			flightLengths->InsertNextValue(inFlightLength->GetValue(flight));
		}
	}

	return 1;
}

int GenerateGeodesics::RequestInformation(vtkInformation* request,
        vtkInformationVector** inputVector, vtkInformationVector* outputVector) {
	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
	if (inInfo->Has(Data::VTK_DATA_TRANSFORMATION()) && firstRequestInformation) {
		Data::Transformation dataTransformation = static_cast<Data::Transformation>(inInfo->Get(
		            Data::VTK_DATA_TRANSFORMATION()));
		if (dataTransformation == Data::TRANSFORMED) {
			vtkErrorMacro( << "The geodesics generation filter does not properly work on transformed data.");
		}
	}
	if (!inInfo->Has(Data::VTK_DATA_TYPE()) || inInfo->Get(Data::VTK_DATA_TYPE()) != Data::FLIGHTS) {
		vtkErrorMacro( <<
		               "The geodesics generation filter does only work with Kronos Flight Data. Please provied appropriate data.");
		return 0;
	}

	firstRequestInformation = false;
	return Superclass::RequestInformation(request, inputVector, outputVector);
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
	double v1 = 0.95 - 0.95 * pow(KRONOS_E, -4.25 * value);
	double v2 = cbrt(0.85 * value);
	this->radius = (v1 + v2) / 2;

	this->Modified();
}

void GenerateGeodesics::setLoD(double value) {
	// maxLenOfLineSegment should always be \in (0, radius] and grow exponentially
	this->maxLenOfLineSegment = Configuration::getInstance().getDouble("globe.radius")
	                            * pow(2, - 10 * value);
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

	World center = getCircleCenterPoint(sphericalToCartesian
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

			QVector<GPS> nextPoints;
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
			vtkWarningMacro( << "  ==>> Possibly caught in infinite calculation. Advancing to next point.\n"
			                 "Remove the limit of the calculation depth if you need more detail. This may"
			                 " result in an infinity loop though, so be careful.");
			index++;
			treeDepth--;
		}
	}

	insertAndConnectPoints(dataSet, points);
}

void GenerateGeodesics::insertAndConnectPoints(vtkPolyData* dataSet,
        QVector<GPS>& points) {
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

World GenerateGeodesics::getCircleCenterPoint(const World& point1,
        const World& point2, double radius) {
	GPS middle = getPointInbetween(cartesianToSpherical(point1),
	                               cartesianToSpherical(point2),
	                               World(0, 0, 0));

	World circleCenter = sphericalToCartesian(middle) * radius;

	return circleCenter;
}

GPS GenerateGeodesics::getPointInbetween(const GPS& point1, const GPS& point2,
        const World& center) {
	World p1 = sphericalToCartesian(point1);
	World p2 = sphericalToCartesian(point2);

	// normalize points (that is: make the center the origin)
	p1 -= center;
	p2 -= center;

	// get point in between
	World middle = p1 + p2;
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
