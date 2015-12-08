#ifndef vtkGlobeFilter_h
#define vtkGlobeFilter_h

#include "vtkPolyDataAlgorithm.h"

#define VTK_VECTOR_MODE_PASS_VECTORS      0
#define VTK_VECTOR_MODE_COMPUTE_GRADIENT  1
#define VTK_VECTOR_MODE_COMPUTE_VORTICITY 2

#define VTK_TENSOR_MODE_PASS_TENSORS     0
#define VTK_TENSOR_MODE_COMPUTE_GRADIENT 1
#define VTK_TENSOR_MODE_COMPUTE_STRAIN   2


class vtkGlobeFilter : public vtkPolyDataAlgorithm {
public:
	vtkTypeMacro(vtkGlobeFilter, vtkPolyDataAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);
	void reloadStuff(void);
	// Description:
	// Construct to compute the gradient of the scalars and vectors.
	static vtkGlobeFilter *New();
	// Description:
	// Control how the filter works to generate vector cell data. You
	// can choose to pass the input cell vectors, compute the gradient
	// of the input scalars, or extract the vorticity of the computed
	// vector gradient tensor. By default (VectorModeToComputeGradient),
	// the filter will take the gradient of the input scalar data.
	vtkSetMacro(VectorMode, int);
	vtkGetMacro(VectorMode, int);
	void SetVectorModeToPassVectors() {
		this->SetVectorMode(VTK_VECTOR_MODE_PASS_VECTORS);
	};
	void SetVectorModeToComputeGradient() {
		this->SetVectorMode(VTK_VECTOR_MODE_COMPUTE_GRADIENT);
	};
	void SetVectorModeToComputeVorticity()                                 {
this->SetVectorMode(VTK_VECTOR_MODE_COMPUTE_VORTICITY);};
  const char *GetVectorModeAsString();

  // Description:
  // Control how the filter works to generate tensor cell data. You can
  // choose to pass the input cell tensors, compute the gradient of
  // the input vectors, or compute the strain tensor of the vector gradient
  // tensor. By default (TensorModeToComputeGradient), the filter will
  // take the gradient of the vector data to construct a tensor.
  vtkSetMacro(TensorMode,int);
  vtkGetMacro(TensorMode,int);
  void SetTensorModeToPassTensors()
    {this->SetTensorMode(VTK_TENSOR_MODE_PASS_TENSORS);};
  void SetTensorModeToComputeGradient()
    {this->SetTensorMode(VTK_TENSOR_MODE_COMPUTE_GRADIENT);};
  void SetTensorModeToComputeStrain()
    {this->SetTensorMode(VTK_TENSOR_MODE_COMPUTE_STRAIN);};
  const char *GetTensorModeAsString();

protected:
  vtkGlobeFilter();
  ~vtkGlobeFilter() {}
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int VectorMode;
  int TensorMode;
private:
  vtkGlobeFilter(const vtkGlobeFilter&);  // Not implemented.
  void operator=(const vtkGlobeFilter&);  // Not implemented.
};

#endif
