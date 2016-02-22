//
//  TwitterHeatmapFilter.cpp
//  kronos
//
//  Created by Frederik Riedel on 12.02.16.
//
//


#include "TwitterHeatmapFilter.h"

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
#include <vtkTypeInt16Array.h>
#include <QList>
#include <vtkCellArray.h>


TwitterHeatmapFilter::TwitterHeatmapFilter() {
    //take this transformation, true -> it transforms per default
    
    Locator = vtkSmartPointer<vtkPointLocator>::New();
    Locator->SetTolerance(toleranceValue);
    
    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(1);
    
}

void TwitterHeatmapFilter::setToleranceValue(int newTolerance) {
    toleranceValue = newTolerance;
    Locator->SetTolerance(toleranceValue);
    this->Modified();
}


void TwitterHeatmapFilter::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);
    os << indent << "Twitter Filter, Kronos Project" << endl;
}

TwitterHeatmapFilter::~TwitterHeatmapFilter() {}

vtkStandardNewMacro(TwitterHeatmapFilter)

int TwitterHeatmapFilter::RequestData(vtkInformation* info,
                                      vtkInformationVector** inputVector,
                                      vtkInformationVector* outputVector) {
    
    int numberOfXComponents = 50;
    int numberOfYComponents = 50;
    
    
    
    
    
    
    double minX = 360;
    double maxX = -360.0;
    double minY = 360;
    double maxY = -360.0;
    double width = 0;
    double height = 0;
    
    // get the input data
    vtkPolyData* dataInput = vtkPolyData::GetData(inputVector[0],0);
    
    vtkPoints* intputDataPoints = dataInput->GetPoints();
    
    int numberOfDataInputPoints = intputDataPoints->GetNumberOfPoints();
    
    for(int i=0; i<numberOfDataInputPoints; i++) {
        double point[3];
        intputDataPoints->GetPoint(i,point);
        
        
        if(point[0] < minX) {
            minX = point[0];
        }
        
        if(point[0] > maxX) {
            maxX = point[0];
        }
        
        if(point[1] < minY) {
            minY = point[1];
        }
        
        if(point[1] > maxY) {
            maxY = point[1];
        }
        
    }
    
    
    width = maxX-minX;
    height = maxY-minY;
    
    double stepWidthX = (width / numberOfXComponents);
    double stepWidthY = (height / numberOfYComponents);
    
    int density[numberOfXComponents * numberOfYComponents];
    
    

    
    
    for(int x = 0; x < numberOfXComponents; x++) {
        for(int y = 0; y < numberOfYComponents; y++) {
            
            int pointID = y*numberOfXComponents + x;
            density[pointID] = 0.0;
        }
    }
    
    for(int i=0; i<numberOfDataInputPoints; i++) {
        
        double point[3];
        intputDataPoints->GetPoint(i,point);
        
        double relativeX = ((point[0] - minX) / width);
        double relativeY = ((point[1] - minY) / height);
        
        
        
        int x = round(relativeX * numberOfXComponents);
        int y = round(relativeY * numberOfYComponents);
        
        std::cout << x << ", " << y << std::endl;
        
        int pointID = x*numberOfXComponents + y;
        density[pointID] = density[pointID]+1;
        
        
    }
    
    
    
    
    
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkSmartPointer<vtkPolyData> output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
    
    
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    
    vtkCellArray* verts = vtkCellArray::New();
    
    // Add a cell containing all points. I don't know why we need this, but it doesn't work without.
    verts->Allocate(verts->EstimateSize(1, numberOfXComponents*numberOfYComponents));
    verts->InsertNextCell(numberOfXComponents*numberOfYComponents);
    
    
    //we could also use the upper "for loops" to insert these values, but we use a second one to make it more clear to read this code. we trust the compiler to optimize this code properly.
    for(int x=0; x<numberOfXComponents; x++) {
        for(int y = 0; y<numberOfYComponents; y++) {
            double relativeX = minX + x * stepWidthX;
            double relativeY = minY + y * stepWidthY;
            verts->InsertCellPoint(points->InsertNextPoint(relativeX,relativeY,0));
        }
    }
    
    
    
    //    output->GetPoints();
    output->SetPoints(points);
    output->SetVerts(verts);
    
    vtkSmartPointer<vtkTypeInt16Array> coverageValues = vtkSmartPointer<vtkTypeInt16Array>::New();
    coverageValues->SetNumberOfComponents(1);
    coverageValues->SetNumberOfValues(sizeof(density)/sizeof(*density));
    coverageValues->SetName("density");
    
    for(int i=0; i< sizeof(density)/sizeof(*density); i++) {
        coverageValues->InsertValue(i, density[i]);
    }
    
    
    
    output->GetPointData()->AddArray(coverageValues);
    
    
    return 1;
}

int TwitterHeatmapFilter::FillInputPortInformation(int port, vtkInformation* info) {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
    return 1;
}

