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
#include <QList>


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
    
    
    
    

    
    int minX;
    int maxX;
    int minY;
    int maxY;
    
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
    
    double density[numberOfXComponents * numberOfYComponents];
    
    double stepWidthX = (maxX-minX) / numberOfXComponents;
    double stepWidthY = (maxY-minY) / numberOfYComponents;
    
    

    vtkPolyData* output = vtkPolyData::GetData(outputVector,0);
    
    
    
    vtkSmartPointer<vtkTypeFloat32Array> coverageValues = vtkSmartPointer<vtkTypeFloat32Array>::New();
    coverageValues->SetNumberOfComponents(1);
    coverageValues->SetNumberOfTuples(numberOfXComponents * numberOfYComponents);
    coverageValues->SetName("density");
    


    
    output->GetPointData()->AddArray(coverageValues);
    
    
    
    
    
    
    
    
    dataInput->GetPoints()->InsertNextPoint(1.0, 1.0, 1.0);
    
    
    output->ShallowCopy(dataInput);
    
    
    return 1;
}

int TwitterHeatmapFilter::FillInputPortInformation(int port, vtkInformation* info) {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
    return 1;
}

