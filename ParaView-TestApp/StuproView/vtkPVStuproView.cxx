#include "vtkPVStuproView.h"
#include "vtkObjectFactory.h"

#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"

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

	// Create a sphere
	vtkSmartPointer<vtkSphereSource> sphereSource =
		vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(5.0);

	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());

	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	GetRenderer()->AddActor(actor);
}

//----------------------------------------------------------------------------
void vtkPVStuproView::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
