#ifndef KRONOSGENERATEGEODESICS_H
#define KRONOSGENERATEGEODESICS_H

#include<vtkPolyDataAlgorithm.h>


class KronosGenerateGeodesics : public vtkPolyDataAlgorithm
{
public:
    vtkTypeMacro(KronosGenerateGeodesics, vtkPolyDataAlgorithm)
    static KronosGenerateGeodesics *New();
    void PrintSelf(ostream &os, vtkIndent indent) override;

private:
    KronosGenerateGeodesics();
};

#endif // KRONOSGENERATEGEODESICS_H
