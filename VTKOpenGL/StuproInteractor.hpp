#ifndef STUPRO_STUPROINTERACTOR_HPP
#define STUPRO_STUPROINTERACTOR_HPP

#include <vtkInteractorStyleTerrain.h>
#include <cstring>

class StuproInteractor: public vtkInteractorStyleTerrain
{
public:
	static StuproInteractor *New();

	vtkTypeMacro(StuproInteractor,vtkInteractorStyleTerrain);

	void OnTimer() override {}
};

#endif
