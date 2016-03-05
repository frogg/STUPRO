#include <Filter/HeatmapDensityFilter.h>

#include <Reader/DataReader/Data.hpp>
#include <Utils/Misc/PointCoordinates.hpp>

#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>

#include <qmap.h>
#include <cmath>

HeatmapDensityFilter::HeatmapDensityFilter() {
	this->SetNumberOfInputPorts(1);
	this->SetNumberOfOutputPorts(1);
}

HeatmapDensityFilter::~HeatmapDensityFilter() { }

void HeatmapDensityFilter::setHorizontalHeatmapResolution(int horizontalHeatmapResolution) {
	this->horizontalResolution = horizontalHeatmapResolution;
	this->Modified();
}

void HeatmapDensityFilter::setVerticalHeatmapResolution(int verticalHeatmapResolution) {
	this->verticalResolution = verticalHeatmapResolution;
	this->Modified();
}

void HeatmapDensityFilter::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "Heatmap Density Filter, Kronos Project" << endl;
}

vtkStandardNewMacro(HeatmapDensityFilter);

void HeatmapDensityFilter::fail(QString message) {
	vtkErrorMacro( << QString("%1. This filter may not work, please proceed with caution.").arg(
	                   message).toStdString());
}

int HeatmapDensityFilter::RequestData(vtkInformation* info,
                                      vtkInformationVector** inputVector,
                                      vtkInformationVector* outputVector) {
	// Get the input data
	vtkPolyData* dataInput = vtkPolyData::GetData(inputVector[0], 0);

	// Compute the bounds of the input data
	double bounds[6];
	dataInput->GetBounds(bounds);

	double minX = bounds[0];
	double maxX = bounds[1];
	double minY = bounds[2];
	double maxY = bounds[3];
	double width = maxX - minX;
	double height = maxY - minY;

	QMap<PointCoordinates, int> densities;
	double horizontalStep = width / double(this->horizontalResolution);
	double verticalStep = height / double(this->verticalResolution);

	// Fill the map of densities with zeroes
	for (int x = 0; x <= this->horizontalResolution; x++) {
		for (int y = 0; y <= this->verticalResolution; y++) {
			densities[PointCoordinates(minX + (x * horizontalStep), minY + (y * verticalStep), 0)] = 0;
		}
	}

	// Iterate through all points and add them to the nearest grid point
	for (int i = 0; i < dataInput->GetNumberOfPoints(); i++) {
		double point[3];
		dataInput->GetPoint(i, point);

		densities[PointCoordinates(minX + (round((point[0] - minX) / horizontalStep) * horizontalStep),
		                           minY + (round((point[1] - minY) / verticalStep) * verticalStep), 0)] += 1;
	}

	// Output the densities as a poly data object
	vtkInformation* outInfo = outputVector->GetInformationObject(0);
	vtkSmartPointer<vtkPolyData> output = vtkPolyData::SafeDownCast(outInfo->Get(
	        vtkDataObject::DATA_OBJECT()));
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkCellArray* verts = vtkCellArray::New();
	verts->Allocate(verts->EstimateSize(1, densities.size()));
	verts->InsertNextCell(densities.size());

	// Add the coordinates of all points and the densities as a new array
	vtkSmartPointer<vtkIntArray> densityValues = vtkSmartPointer<vtkIntArray>::New();
	densityValues->SetNumberOfComponents(1);
	densityValues->SetNumberOfValues(densities.size());
	densityValues->SetName("density");

	int valueIndex = 0;
	QMap<PointCoordinates, int>::const_iterator i = densities.constBegin();
	while (i != densities.constEnd()) {
		verts->InsertCellPoint(points->InsertNextPoint(i.key().getX(), i.key().getY(), i.key().getZ()));
		densityValues->InsertValue(valueIndex, i.value());
		valueIndex++;
		++i;
	}

	output->SetPoints(points);
	output->SetVerts(verts);
	output->GetPointData()->AddArray(densityValues);

	return 1;
}

int HeatmapDensityFilter::RequestInformation(vtkInformation* request,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector) {
	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	outInfo->Set(Data::VTK_DATA_STATE(), Data::DENSITY_MAPPED);

	return 1;
}

int HeatmapDensityFilter::FillOutputPortInformation(int port, vtkInformation* info) {
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
	return 1;
}

int HeatmapDensityFilter::FillInputPortInformation(int port, vtkInformation* info) {
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
	return 1;
}
