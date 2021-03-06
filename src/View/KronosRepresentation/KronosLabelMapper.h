/*=========================================================================

  This class is based on the vtkLabelPlacementMapper class.
  with a fix that labels are drawn in 2D mode too.

=========================================================================*/

#ifndef __KronosLabelMapper_h
#define __KronosLabelMapper_h

#include "vtkRenderingLabelModule.h" // For export macro
#include "vtkMapper2D.h"

class vtkCoordinate;
class vtkLabelRenderStrategy;
class KronosSelectPoints;

class KronosLabelMapper : public vtkMapper2D {
public:
	static KronosLabelMapper* New();
	vtkTypeMacro(KronosLabelMapper, vtkMapper2D);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Draw non-overlapping labels to the screen.
	void RenderOverlay(vtkViewport* viewport, vtkActor2D* actor);

	// Description:
	// Set the label rendering strategy.
	virtual void SetRenderStrategy(vtkLabelRenderStrategy* s);
	vtkGetObjectMacro(RenderStrategy, vtkLabelRenderStrategy);

	// Description:
	// The maximum fraction of the screen that the labels may cover.
	// Label placement stops when this fraction is reached.
	vtkSetClampMacro(MaximumLabelFraction, double, 0., 1.);
	vtkGetMacro(MaximumLabelFraction, double);

	// Description:
	// The type of iterator used when traversing the labels.
	// May be vtkLabelHierarchy::FRUSTUM or vtkLabelHierarchy::FULL_SORT
	vtkSetMacro(IteratorType, int);
	vtkGetMacro(IteratorType, int);

	// Description:
	// Set whether, or not, to use unicode strings.
	vtkSetMacro(UseUnicodeStrings, bool);
	vtkGetMacro(UseUnicodeStrings, bool);
	vtkBooleanMacro(UseUnicodeStrings, bool);

	// Description:
	// Use label anchor point coordinates as normal vectors and eliminate those
	// pointing away from the camera. Valid only when points are on a sphere
	// centered at the origin (such as a 3D geographic view). Off by default.
	vtkGetMacro(PositionsAsNormals, bool);
	vtkSetMacro(PositionsAsNormals, bool);
	vtkBooleanMacro(PositionsAsNormals, bool);

	// Description:
	// Enable drawing spokes (lines) to anchor point coordinates that were perturbed
	// for being coincident with other anchor point coordinates.
	vtkGetMacro(GeneratePerturbedLabelSpokes, bool);
	vtkSetMacro(GeneratePerturbedLabelSpokes, bool);
	vtkBooleanMacro(GeneratePerturbedLabelSpokes, bool);

	// Description:
	// Use the depth buffer to test each label to see if it should not be displayed if
	// it would be occluded by other objects in the scene. Off by default.
	vtkGetMacro(UseDepthBuffer, bool);
	vtkSetMacro(UseDepthBuffer, bool);
	vtkBooleanMacro(UseDepthBuffer, bool);

	// Description:
	// Tells the placer to place every label regardless of overlap.
	// Off by default.
	vtkSetMacro(PlaceAllLabels, bool);
	vtkGetMacro(PlaceAllLabels, bool);
	vtkBooleanMacro(PlaceAllLabels, bool);

	// Description:
	// Whether to render traversed bounds. Off by default.
	vtkSetMacro(OutputTraversedBounds, bool);
	vtkGetMacro(OutputTraversedBounds, bool);
	vtkBooleanMacro(OutputTraversedBounds, bool);

	//BTX
	enum LabelShape {
		NONE,
		RECT,
		ROUNDED_RECT,
		NUMBER_OF_LABEL_SHAPES
	};
	//ETX

	// Description:
	// The shape of the label background, should be one of the
	// values in the LabelShape enumeration.
	vtkSetClampMacro(Shape, int, 0, NUMBER_OF_LABEL_SHAPES - 1);
	vtkGetMacro(Shape, int);
	virtual void SetShapeToNone() {
		this->SetShape(NONE);
	}
	virtual void SetShapeToRect() {
		this->SetShape(RECT);
	}
	virtual void SetShapeToRoundedRect() {
		this->SetShape(ROUNDED_RECT);
	}

	//BTX
	enum LabelStyle {
		FILLED,
		OUTLINE,
		NUMBER_OF_LABEL_STYLES
	};
	//ETX

	// Description:
	// The style of the label background shape, should be one of the
	// values in the LabelStyle enumeration.
	vtkSetClampMacro(Style, int, 0, NUMBER_OF_LABEL_STYLES - 1);
	vtkGetMacro(Style, int);
	virtual void SetStyleToFilled() {
		this->SetStyle(FILLED);
	}
	virtual void SetStyleToOutline() {
		this->SetStyle(OUTLINE);
	}

	// Description:
	// The size of the margin on the label background shape.
	// Default is 5.
	vtkSetMacro(Margin, double);
	vtkGetMacro(Margin, double);

	// Description:
	// The color of the background shape.
	vtkSetVector3Macro(BackgroundColor, double);
	vtkGetVector3Macro(BackgroundColor, double);

	// Description:
	// The opacity of the background shape.
	vtkSetClampMacro(BackgroundOpacity, double, 0.0, 1.0);
	vtkGetMacro(BackgroundOpacity, double);

	// Description:
	// Get the transform for the anchor points.
	vtkGetObjectMacro(AnchorTransform, vtkCoordinate);

protected:
	KronosLabelMapper();
	~KronosLabelMapper();

	virtual void SetAnchorTransform( vtkCoordinate* );

	virtual int FillInputPortInformation( int port, vtkInformation* info );

	//BTX
	class Internal;
	Internal* Buckets;
	//ETX

	vtkLabelRenderStrategy* RenderStrategy;
	vtkCoordinate* AnchorTransform;
	KronosSelectPoints* VisiblePoints;
	double MaximumLabelFraction;
	bool PositionsAsNormals;
	bool GeneratePerturbedLabelSpokes;
	bool UseDepthBuffer;
	bool UseUnicodeStrings;
	bool PlaceAllLabels;
	bool OutputTraversedBounds;

	int LastRendererSize[2];
	double LastCameraPosition[3];
	double LastCameraFocalPoint[3];
	double LastCameraViewUp[3];
	double LastCameraParallelScale;
	int IteratorType;

	int Style;
	int Shape;
	double Margin;
	double BackgroundOpacity;
	double BackgroundColor[3];

private:
	KronosLabelMapper(const KronosLabelMapper&);  // Not implemented.
	void operator=(const KronosLabelMapper&);  // Not implemented.
};

#endif
