#include "vtkPVStuproView.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkPVStuproView);
//----------------------------------------------------------------------------
vtkPVStuproView::vtkPVStuproView()
{
}

//----------------------------------------------------------------------------
vtkPVStuproView::~vtkPVStuproView()
{
}

//----------------------------------------------------------------------------
void vtkPVStuproView::Initialize(unsigned int id)
{
	this->Superclass::Initialize(id);
	SetGradientBackground(true);
	SetBackground(255, 0, 0);
	SetBackground2(0, 0, 255);
}

//----------------------------------------------------------------------------
void vtkPVStuproView::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
