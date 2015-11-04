#ifndef FRUSTUMTRIANGLES
#define FRUSTUMTRIANGLES

#ifndef FRUSTUMPLANES
#define FRUSTUMPLANES

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>

class FrustumTriangles {

public:
    FrustumTriangles ()
    {
        for (int i = 0; i != 4; i++) {

        }
    }

    void updateTriangle (double points[3][3], int index) {

    }

    vtkSmartPointer<vtkActor> *getActors() {
        return myActors;
    }

private:

    vtkSmartPointer<vtkPoints> myPoints [4];
    vtkSmartPointer<vtkTriangle> myTriangles [4];
    vtkSmartPointer<vtkCellArray> myCellArrays [4];
    vtkSmartPointer<vtkPolyData> myPolyData [4];
    vtkSmartPointer<vtkPolyDataMapper> myMapper [4];
    vtkSmartPointer<vtkActor> myActors [4];
};

#endif // FRUSTUMPLANES



#endif // FRUSTUMTRIANGLES_HPP

