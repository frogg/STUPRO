#ifndef vtkGlobeFilter_h
#define vtkGlobeFilter_h

#include "vtkDataSetAlgorithm.h"

class vtkFlowVisualisationFilter : public vtkDataSetAlgorithm {
public:
	vtkTypeMacro(vtkFlowVisualisationFilter, vtkDataSetAlgorithm);

	static vtkFlowVisualisationFilter *New();

	int ProcessRequest(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector) override;
	
	enum VisualisationMethod {
		StreamLines, PathLines, LIC
	};

	void switchVisualisationMethod(int method);
	
	void PrintSelf(ostream& os, vtkIndent indent) override;
	
protected:
	vtkFlowVisualisationFilter()
	{
		this->currentMethod = StreamLines;
	};

	~vtkFlowVisualisationFilter() {};
	
	int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
	vtkFlowVisualisationFilter(const vtkFlowVisualisationFilter&);  // Not implemented.
	void operator=(const vtkFlowVisualisationFilter&);  // Not implemented.

	VisualisationMethod currentMethod;
};

#endif
