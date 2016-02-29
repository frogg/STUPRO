#include <Filter/HeatmapDensityFilter.h>

#include <Reader/DataReader/Data.hpp>

#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkExecutive.h>
#include <vtkInformationExecutivePortVectorKey.h>

HeatmapDensityFilter::HeatmapDensityFilter() {
    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(1);
}

HeatmapDensityFilter::~HeatmapDensityFilter() { }

void HeatmapDensityFilter::setHeatmapResolution(double heatmapResolution) {
    this->heatmapResolution = heatmapResolution;
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
    // get the input data
    vtkPolyData* dataInput = vtkPolyData::GetData(inputVector[0], 0);
    
    double bounds[6];
    dataInput->GetBounds(bounds);
    
    double minX = bounds[0];
    double maxX = bounds[1];
    double minY = bounds[2];
    double maxY = bounds[3];
    double width = maxX - minX;
    double height = maxY - minY;
    
    int numberOfXComponents = (int) (50.0 * this->heatmapResolution + 0.5);
    int numberOfYComponents = (int) ((height * numberOfXComponents) / width + 0.5);
    
    std::cout << "numberOfXComponents: " << numberOfXComponents << std::endl;
    std::cout << "numberOfYComponents: " << numberOfYComponents << std::endl;
    
    double stepWidthX = (width / (numberOfXComponents-1));
    double stepWidthY = (height / (numberOfYComponents-1));
    
    vtkPoints* intputDataPoints = dataInput->GetPoints();
    
    int numberOfDataInputPoints = intputDataPoints->GetNumberOfPoints();
    
    std::vector<int> density(numberOfXComponents * numberOfYComponents);
    
    
    for (int x = 0; x < numberOfXComponents; x++) {
        for (int y = 0; y < numberOfYComponents; y++) {
            
            int pointID = y * numberOfXComponents + x;
            density[pointID] = 0.0;
        }
    }
    
    for (int i = 0; i < numberOfDataInputPoints; i++) {
        
        double point[3];
        intputDataPoints->GetPoint(i, point);
        
        double relativeX = ((point[0] - minX) / width);
        double relativeY = ((point[1] - minY) / height);
        
        
        int x = (int)(relativeX * (numberOfXComponents-1) + 0.5);
        int y = (int)(relativeY * (numberOfYComponents-1) + 0.5);
        
        
        int pointID = y * numberOfXComponents + x;
        
        std::cout << "relativeX: " << relativeX << ", x: " << x << std::endl;
        std::cout << "relativeY: " << relativeY << ", y: " << y << std::endl;
        
        //diese Abfrage sollte eigentlich nicht existieren müssen, aber es gibt hier manchmal falsche Werte!
        if (pointID >= 0 && pointID < density.size()) {
            density[pointID] = density[pointID] + 1;
        } else {
            std::cout << "fail" << std::endl;
        }
    }
    
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    vtkSmartPointer<vtkPolyData> output = vtkPolyData::SafeDownCast(outInfo->Get(
                                                                                 vtkDataObject::DATA_OBJECT()));
    
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    
    vtkCellArray* verts = vtkCellArray::New();
    
    // Add a cell containing all points. I don't know why we need this, but it doesn't work without.
    verts->Allocate(verts->EstimateSize(1, numberOfXComponents * numberOfYComponents));
    verts->InsertNextCell(numberOfXComponents * numberOfYComponents);
    
    
    //we could also use the upper "for loops" to insert these values, but we use a second one to make it more clear to read this code. we trust the compiler to optimize this code properly.
    for (int x = 0; x < numberOfXComponents; x++) {
        for (int y = 0; y < numberOfYComponents; y++) {
            double relativeX = minX + x * stepWidthX;
            double relativeY = minY + y * stepWidthY;
            verts->InsertCellPoint(points->InsertNextPoint(relativeX, relativeY, 0));
        }
    }
    
    
    output->SetPoints(points);
    output->SetVerts(verts);
    
    vtkSmartPointer<vtkIntArray> coverageValues = vtkSmartPointer<vtkIntArray>::New();
    coverageValues->SetNumberOfComponents(1);
    coverageValues->SetNumberOfValues(density.size());
    coverageValues->SetName("density");
    
    for (int i = 0; i < density.size(); i++) {
        coverageValues->InsertValue(i, density[i]);
    }
    
    
    output->GetPointData()->AddArray(coverageValues);
    
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
