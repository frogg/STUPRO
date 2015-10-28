#ifndef STUPRO_STUPROINTERACTOR_HPP
#define STUPRO_STUPROINTERACTOR_HPP

#include <vtkInteractorStyleTerrain.h>

class StuproInteractor : public vtkInteractorStyleTerrain
{
public:
	static StuproInteractor *New();
	
	vtkTypeMacro(StuproInteractor,vtkInteractorStyleTerrain);
	
	void OnTimer() override {}
};

#endif
