#ifndef __vtkCityReader_h
#define __vtkCityReader_h

#include "vtkTableAlgorithm.h"
#include "vtkPolyDataAlgorithm.h"

class vtkCityReader : public vtkPolyDataAlgorithm {
public:
	static vtkCityReader* New();
	vtkTypeMacro(vtkCityReader, vtkPolyDataAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent) override;
	void SetFileName(std::string name);
	virtual int ProcessRequest(vtkInformation * 	request,
							   vtkInformationVector ** 	inInfo,
							   vtkInformationVector * 	outInfo
							   ) override;

protected:
	vtkCityReader();
	~vtkCityReader();

	int RequestData(
		vtkInformation*,
		vtkInformationVector**,
		vtkInformationVector*) override;

private:
	vtkCityReader(const vtkCityReader&); // Not implemented
	void operator=(const vtkCityReader&);   // Not implemented

};

#endif
