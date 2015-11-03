#ifndef FRUSTUMPLANES
#define FRUSTUMPLANES

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>

class FrustumPlanes {

public:
    FrustumPlanes ()
    {
        for (int i = 0; i < 4; i++) {
            myPlanes[i] = vtkSmartPointer<vtkPlaneSource>::New();

            myMapper[i] = vtkSmartPointer<vtkPolyDataMapper>::New();
            myMapper[i]->SetInputConnection(myPlanes[i]->GetOutputPort());

            myActors[i] = vtkSmartPointer<vtkActor>::New();
        }
    }

    void updatePlanes (double planes [24])
    {
        for (int i = 0; i < 4; i++) {
            double spurpoints[9];

            calcSpurpoints(planes + 4*i, spurpoints);

            // set plane
            myPlanes[i]->SetOrigin(spurpoints[0], spurpoints[1], spurpoints[2]);
            myPlanes[i]->SetPoint1(spurpoints[3], spurpoints[4], spurpoints[5]);
            myPlanes[i]->SetPoint2(spurpoints[6], spurpoints[7], spurpoints[8]);

            // set actor
            myActors[i]->SetMapper(myMapper[i]);
        }
    }

    void updatePlanes (double planePoints [4][3][3]) {
        for (int i = 0; i < 4; i++) {
            myPlanes[i]->SetOrigin(planePoints[i][0][0], planePoints[i][0][1], planePoints[i][0][2]);
            myPlanes[i]->SetPoint1(planePoints[i][1][0], planePoints[i][1][1], planePoints[i][1][2]);
            myPlanes[i]->SetPoint2(planePoints[i][2][0], planePoints[i][2][1], planePoints[i][2][2]);

            // set actor
            myActors[i]->SetMapper(myMapper[i]);
        }
    }

    void updatePlane (double planePoints[3][3], int i) {
        myPlanes[i]->SetOrigin(planePoints[0][0], planePoints[0][1], planePoints[0][2]);
        myPlanes[i]->SetPoint1(planePoints[1][0], planePoints[1][1], planePoints[1][2]);
        myPlanes[i]->SetPoint2(planePoints[2][0], planePoints[2][1], planePoints[2][2]);

        // set actor
        myActors[i]->SetMapper(myMapper[i]);
    }

    vtkSmartPointer<vtkActor> *getActors() {
        return myActors;
    }

private:

    void calcSpurpoints (double plane[4], double spurpoints[9]) {
        std::cout << "Plane: " << plane[0] << "x + " << plane[1] << "y + " << plane[2] << "y + " << plane[3] << " = 0" << std::endl;
        for (int i = 0; i != 3; i++) {
            int axisIndex = 3 * i + i;
            int axispIndex = 3 * i + ((i + 1) % 3);
            int axisppIndex = 3 * i + ((i + 2) % 3);
            if (plane[i] != 0) {
                spurpoints[axisIndex] = -plane[3] / plane[i];
                spurpoints[axispIndex] = 0.;
                spurpoints[axisppIndex] = 0.;
            } else {
                spurpoints[axisIndex] = 1.;
                if (plane[(i + 1) % 3] != 0) {
                    spurpoints[axispIndex] = -plane[3] / plane[(i + 1) % 3];
                    spurpoints[axisppIndex] = 0.;
                } else {
                    spurpoints[axispIndex] = 1.;
                    spurpoints[axisppIndex] = -plane[3] / plane[(i + 2) % 3];
                }
            }
            std::cout << " Point " << i << " = ( " << spurpoints[3 * i] << " / " << spurpoints[3 * i + 1] << " / " << spurpoints[3 * i + 2] << " )" << std::endl;
        }
    }

    vtkSmartPointer<vtkActor> myActors [4];
    vtkSmartPointer<vtkPlaneSource> myPlanes [4];
    vtkSmartPointer<vtkPolyDataMapper> myMapper [4];
};

#endif // FRUSTUMPLANES

