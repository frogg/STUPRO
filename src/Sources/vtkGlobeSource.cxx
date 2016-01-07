#include "vtkGlobeSource.h"

#include "vtkObjectFactory.h"

#include <math.h>

#include <Utils/Config/Configuration.hpp>

vtkStandardNewMacro(vtkGlobeSource);


//----------------------------------------------------------------------------
// Construct sphere with radius=0.5 and default resolution 8 in both Phi
// and Theta directions. Theta ranges from (0,360) and phi (0,180) degrees.
vtkGlobeSource::vtkGlobeSource(int res) : vtkSphereSource(res) {
	// Calls the superconstructor.
    float globeRadius = 1.0f;
    if(Configuration::getInstance().hasKey("globe.radius")){
        globeRadius = Configuration::getInstance().getDouble("globe.radius");
    }
    this->SetRadius(globeRadius);
    this->SetThetaResolution(50);
    this->SetPhiResolution(50);
}
