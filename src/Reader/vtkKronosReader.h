#ifndef __vtkKronosReader_h
#define __vtkKronosReader_h

#include "vtkTableAlgorithm.h"
#include "vtkPolyDataAlgorithm.h"
#include "Utils/Math/Vector3.hpp"
#include "QString"
#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Reader/DataReader/JsonReader.hpp>
class vtkKronosReader : public vtkPolyDataAlgorithm {
public:
    vtkTypeMacro(vtkKronosReader, vtkPolyDataAlgorithm);
    
	void SetFileName(std::string name);
    void SetCameraPos(double x,double y,double z);
    
    static vtkKronosReader* New();
    
protected:
	vtkKronosReader();
	~vtkKronosReader();
	int RequestData(
		vtkInformation*,
		vtkInformationVector**,
		vtkInformationVector*) override;
private:
	vtkKronosReader(const vtkKronosReader&); // Not implemented
	void operator=(const vtkKronosReader&);   // Not implemented
    void calcLOD();

    Vector3d cameraPos;
    double globeRadius;
    double distanceToFocalPoint;
    int zoomLevel;
    QString fileName;
    std::unique_ptr<JsonReader> jsonReader;

};

#endif
