#ifndef INTERPOLATEUNIFORMGRID_H
#define INTERPOLATEUNIFORMGRID_H

#include <vtkImageAlgorithm.h>

class InterpolateImageData : public vtkImageAlgorithm {
public:
    vtkTypeMacro(InterpolateImageData, vtkAlgorithm)
    static InterpolateImageData *New();
    void PrintSelf(ostream &os, vtkIndent indent) override;

    int RequestData(vtkInformation *info,
                    vtkInformationVector **inputVector,
                    vtkInformationVector *outputVector);

    /**
     * Documentation see vtkAlgorithm
     */
    int FillOutputPortInformation(int, vtkInformation *info) override;

    /**
     * Documentation see vtkAlgorithm
     */
    int FillInputPortInformation(int, vtkInformation *info) override;

private:
    InterpolateImageData();
    ~InterpolateImageData();

    InterpolateImageData(const InterpolateImageData &); // Not implemented.
    void operator=(const InterpolateImageData &); // Not implemented.
};

#endif // INTERPOLATEUNIFORMGRID_H
