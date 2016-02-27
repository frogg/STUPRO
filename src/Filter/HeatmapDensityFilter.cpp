//
//  TwitterHeatmapFilter.cpp
//  kronos
//
//  Created by Frederik Riedel on 12.02.16.
//
//


#include "HeatmapDensityFilter.h"

#include "vtkUnstructuredGrid.h"
#include "vtkCell.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkObjectFactory.h"

#include "vtkDataObject.h"
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSet.h>

#include <vtkCellData.h>
#include "vtkCell.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkObjectFactory.h"

#include "vtkIntArray.h"
#include "vtkCollection.h"
#include "vtkMergePoints.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

#include <vtkPolyData.h>
#include <vtkTypeFloat32Array.h>
#include <vtkIntArray.h>
#include <QList>
#include <vtkCellArray.h>
#include <vtkTypeInt16Array.h>


HeatmapDensityFilter::HeatmapDensityFilter() {
    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(1);
    
}

/**
 *called from XML to set the heatmap's resolution
 */
void HeatmapDensityFilter::setHeatmapResolution(double heatmapResolution) {
    this->heatmapResolution = heatmapResolution;
    this->Modified();
}

/**
 * print self methode
 */
void HeatmapDensityFilter::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);
    os << indent << "Heatmap Density Filter, Kronos Project" << endl;
}

HeatmapDensityFilter::~HeatmapDensityFilter() {}

vtkStandardNewMacro(HeatmapDensityFilter)



/**
 * this method is called when new a higher instance requests new data
 */
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

int HeatmapDensityFilter::FillOutputPortInformation(int port, vtkInformation* info) {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
    
    return 1;
}

int HeatmapDensityFilter::FillInputPortInformation(int port, vtkInformation* info) {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
    return 1;
}

