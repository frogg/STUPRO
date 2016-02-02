#ifndef KRONOSGENERATEGEODESICS_H
#define KRONOSGENERATEGEODESICS_H

#include<vtkPolyDataAlgorithm.h>

template<typename T> class Vector3;

class GenerateGeodesics : public vtkPolyDataAlgorithm {
public:
    vtkTypeMacro(GenerateGeodesics, vtkPolyDataAlgorithm)
    static GenerateGeodesics* New();
    void PrintSelf(ostream& os, vtkIndent indent) override;

    int RequestData(vtkInformation* info,
                    vtkInformationVector** inputVector,
                    vtkInformationVector* outputVector) override;

    int FillOutputPortInformation(int, vtkInformation* info) override;

    int FillInputPortInformation(int, vtkInformation* info) override;

private:
    GenerateGeodesics();
    GenerateGeodesics(const GenerateGeodesics&); // not implemented
    void operator =(const GenerateGeodesics&); // not implemented

    /**
     * @brief calculateFlightPoints calculate the neccessary points between start and end airport and insert them into a data set
     * @param start the start airport
     * @param end the destination airport
     * @param maxLineLenght the max length for a straight line (defines how smooth the geodesic seems)
     * @param dataSet the data set to insert the points
     * @return the number of points inserted
     */
    int calculateFlightPoints(const Vector3<double>& start, const Vector3<double>& end,
                              const float maxLineLenght, vtkPoints* const dataSet);
};

#endif // KRONOSGENERATEGEODESICS_H
