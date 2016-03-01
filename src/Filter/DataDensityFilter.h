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
#include <vtkCleanUnstructuredGrid.h>
#include <vtkPointLocator.h>
#include <vtkTransformFilter.h>
#include <vtkSmartPointer.h>

class DataDensityFilter : public vtkCleanUnstructuredGrid {

public:
	vtkTypeMacro(DataDensityFilter, vtkCleanUnstructuredGrid)
	static DataDensityFilter* New();
	void PrintSelf(ostream& os, vtkIndent indent) override;

	int toleranceValue = 0;
	void setToleranceValue(int newTolerance);

protected:
	vtkSmartPointer<vtkPointLocator> Locator;

	int RequestData(vtkInformation* info,
	                vtkInformationVector** inputVector,
	                vtkInformationVector* outputVector) override;


	virtual int FillInputPortInformation(int port, vtkInformation* info) override;


private:
	DataDensityFilter();
	~DataDensityFilter();


	DataDensityFilter(const DataDensityFilter&);  // Not implemented.
	void operator=(const DataDensityFilter&);  // Not implemented.

};



#endif /* DataDensityFilter_hpp */
