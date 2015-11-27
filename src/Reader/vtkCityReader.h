#ifndef __vtkCityReader_h
#define __vtkCityReader_h

#include "vtkIOInfovisModule.h" // For export macro
#include "vtkTableAlgorithm.h"
#include "vtkUnicodeString.h" // Needed for vtkUnicodeString
#include "vtkStdString.h" // Needed for vtkStdString
#include "vtkPolyDataAlgorithm.h"
class VTKIOINFOVIS_EXPORT vtkCityReader : public vtkPolyDataAlgorithm
{
public:
  static vtkCityReader* New();
  vtkTypeMacro(vtkCityReader, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
    void SetFileName(std::string name);
    virtual int ProcessRequest	(	vtkInformation * 	request,
                             vtkInformationVector ** 	inInfo,
                             vtkInformationVector * 	outInfo
                             );
//BTX
protected:
  vtkCityReader();
  ~vtkCityReader();

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*);


private:
  vtkCityReader(const vtkCityReader&); // Not implemented
  void operator=(const vtkCityReader&);   // Not implemented
//ETX
};

#endif
