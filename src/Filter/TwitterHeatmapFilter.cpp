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
    
    int numberOfXComponents = 10;
    int numberOfYComponents = 10;
    
    
    
    
    
    
    double minX;
    double maxX;
    double minY;
    double maxY;
    double width;
    double height;
    
    // get the input data
    vtkPolyData* dataInput = vtkPolyData::GetData(inputVector[0],0);
    
    vtkPoints* intputDataPoints = dataInput->GetPoints();
    
    int numberOfDataInputPoints = intputDataPoints->GetNumberOfPoints();
    
    for(int i=0; i<numberOfDataInputPoints; i++) {
        double point[3];
        intputDataPoints->GetPoint(i,point);
        std::cout << "Coodrinaten" << point[0] << "," << point[1] << "," << point[2] << std::endl;
        
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
    
    double stepWidthX = ((maxX-minX) / numberOfXComponents);
    double stepWidthY = ((maxY-minY) / numberOfYComponents);
    
    int density[numberOfXComponents * numberOfYComponents];
    
    
    for(int x = 0; x < numberOfXComponents; x++) {
        for(int y = 0; y < numberOfYComponents; y++) {
            
            
            
            
            int pointID = y*numberOfXComponents + x;
            
            for(int i=0; i<numberOfDataInputPoints; i++) {
                double point[3];
                intputDataPoints->GetPoint(i,point);
                
                double relativeX = (minX + point[0]) / width;
                double relativeY = (minY + point[1]) / height;
                
                if(relativeX < x + stepWidthX && relativeX > x - stepWidthX) {
                    //within x range of this point
                    
                    if(relativeY < y + stepWidthY && relativeY > y - stepWidthY) {
                        //within x & y range of this point!
                        
                        density[pointID] = density[pointID] + 1;
                    }
                    
                }
            }
        }
    }
    
    
    
    
    
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkSmartPointer<vtkPolyData> output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
    
    
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    
    vtkCellArray* verts = vtkCellArray::New();
    
    // Add a cell containing all points
    verts->Allocate(verts->EstimateSize(1, numberOfXComponents*numberOfYComponents));
    verts->InsertNextCell(numberOfXComponents*numberOfYComponents);

    
    //we could also use the upper for loops to insert these values, but we use a second one to make it more clear. we trust the compiler to optimize this code.
    for(double x = minX; x < maxX; x+=stepWidthX) {
        for(double y = minY; y < maxY; y+=stepWidthY) {
            
            
            
            verts->InsertCellPoint(points->InsertNextPoint(x,y,0));

            
        }
    }
    
    
    output->SetPoints(points);
    output->SetVerts(verts);
    
    vtkSmartPointer<vtkTypeInt16Array> coverageValues = vtkSmartPointer<vtkTypeInt16Array>::New();
    coverageValues->SetNumberOfComponents(1);
    coverageValues->SetNumberOfTuples(numberOfXComponents * numberOfYComponents);
    coverageValues->SetName("density");
    
    
    
    
    output->GetPointData()->AddArray(coverageValues);
    
    
    
    
    
    
    
    //dataInput->GetPoints()->InsertNextPoint(1.0, 1.0, 1.0);
    
    
    
    
    
    return 1;
}

int TwitterHeatmapFilter::FillInputPortInformation(int port, vtkInformation* info) {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
    return 1;
}

