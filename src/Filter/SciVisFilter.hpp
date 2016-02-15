#ifndef __SciVisFilter_hpp
#define __SciVisFilter_hpp

#include "vtkPoints.h"
#include "vtkDataSetAlgorithm.h"

#include <iostream>
#include <string>

class SciVisFilter  : public vtkDataSetAlgorithm
{
	public:
		static SciVisFilter *New();
		vtkTypeMacro(SciVisFilter,vtkDataSetAlgorithm);
		void PrintSelf(ostream& os, vtkIndent indent);


		void chooseDate();


	protected:
		SciVisFilter();
		~SciVisFilter();

	private:
		SciVisFilter(const SciVisFilter&); //Not implemented
		void operator=(const SciVisFilter&); //Not implemented

};

#endif
