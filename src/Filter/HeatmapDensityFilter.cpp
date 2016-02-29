#include <Filter/HeatmapDensityFilter.h>

#include <Reader/DataReader/Data.hpp>
#include <Utils/Misc/PointCoordinates.hpp>

#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkExecutive.h>
#include <vtkInformationExecutivePortVectorKey.h>

#include <qmap.h>
#include <cmath>

HeatmapDensityFilter::HeatmapDensityFilter() {
    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(1);
}

HeatmapDensityFilter::~HeatmapDensityFilter() { }

void HeatmapDensityFilter::setHeatmapResolution(double heatmapResolution) {
    this->Modified();
}

void HeatmapDensityFilter::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);
    os << indent << "Heatmap Density Filter, Kronos Project" << endl;
}

vtkStandardNewMacro(HeatmapDensityFilter)

void HeatmapDensityFilter::fail(QString message) {
	vtkErrorMacro( << message.toStdString());
	this->error = true;
}

int HeatmapDensityFilter::RequestData(vtkInformation* info,
                                      vtkInformationVector** inputVector,
                                      vtkInformationVector* outputVector) {
    // Get the input data
    vtkPolyData* dataInput = vtkPolyData::GetData(inputVector[0], 0);
    
    // Compute the bounds of the input data
    double bounds[4];
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
    for (double x = minX; x <= maxX; x += horizontalStep) {
        for (double y = minY; y <= maxY; y += verticalStep) {
            densities.insert(PointCoordinates(x, y, 0), 0);
        }
    }
    
    // Iterate through all points and add them to the nearest grid point
    for (int i = 0; i < dataInput->GetNumberOfPoints(); i++) {
        double point[3];
        dataInput->GetPoint(i, point);
        
        PointCoordinates closestGridPoint(minX, minY, 0);
        double distance = closestGridPoint.getDistanceTo(PointCoordinates(point[0], point[1], point[2]));
        
        for (double x = minX; x <= maxX; x += horizontalStep) {
            for (double y = minY; y <= maxY; y += verticalStep) {
                PointCoordinates currentGridPoint(x, y, 0);
                double currentDistance = currentGridPoint.getDistanceTo(PointCoordinates(point[0], point[1], point[2]));
                
                if (currentDistance < distance) {
                    closestGridPoint = currentGridPoint;
                    distance = currentDistance;
                }
            }
        }
        
        densities[closestGridPoint] += 1;
    }
    
    // Output the densities as a poly data object
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    vtkSmartPointer<vtkPolyData> output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
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
	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	// Check the meta information containing the data's type
	if (inInfo->Has(Data::VTK_DATA_TYPE())) {
		Data::Type dataType = static_cast<Data::Type>(inInfo->Get(Data::VTK_DATA_TYPE()));
		if (dataType != Data::TWEETS) {
			this->fail(
			    QString("This filter only works with Twitter data, but the input contains %1 data.").arg(
			            Data::getDataTypeName(dataType)));
			return 0;
		}
	} else {
		this->fail("This filter only works with data read by the Kronos reader.");
		return 0;
	}

	// Check the meta information containing the data's state
	if (vtkExecutive::CONSUMERS()->Length(outInfo) == 0) {
		if (inInfo->Has(Data::VTK_DATA_STATE())) {
			Data::State dataState = static_cast<Data::State>(inInfo->Get(Data::VTK_DATA_STATE()));
			if (dataState != Data::RAW) {
				this->fail(
				    QString("This filter only works with raw input data, but the input data has the state %1.").arg(
				        Data::getDataStateName(dataState)));
				return 0;
			}
		} else {
			this->fail("The input data has no data state information attached.");
			return 0;
		}
	}

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
