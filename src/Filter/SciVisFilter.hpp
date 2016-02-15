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
		void PrintSelf();
		void SetInputConnection();



	protected:

	private:

};

#endif
