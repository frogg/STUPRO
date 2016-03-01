//
//  DataDensityFilter.hpp
//  kronos
//
//  Created by Frederik Riedel on 12.02.16.
//
//

#ifndef DataDensityFilter_hpp
#define DataDensityFilter_hpp

#include <stdio.h>
#include <Utils/Misc/Macros.hpp>
#include <vtkPolyData.h>
#include <vtkDataObjectAlgorithm.h>
#include <vtkPointLocator.h>
#include <vtkTransformFilter.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

class DataDensityFilter : public vtkDataObjectAlgorithm {

public:
	vtkTypeMacro(DataDensityFilter, vtkDataObjectAlgorithm)
	static DataDensityFilter* New();
	void PrintSelf(ostream& os, vtkIndent indent) override;

	int toleranceValue = 0;
	void setToleranceValue(int newTolerance);

protected:

	int RequestData(vtkInformation* info,
	                vtkInformationVector** inputVector,
	                vtkInformationVector* outputVector) override;


	int FillInputPortInformation(int port, vtkInformation* info) override;
	int FillOutputPortInformation(int port, vtkInformation* info) override;


private:
	DataDensityFilter();
	~DataDensityFilter();


	DataDensityFilter(const DataDensityFilter&);  // Not implemented.
	void operator=(const DataDensityFilter&);  // Not implemented.

};



#endif /* DataDensityFilter_hpp */
