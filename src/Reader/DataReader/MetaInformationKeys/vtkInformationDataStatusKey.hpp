#ifndef KRONOS_VTK_INFORMATION_DATA_STATUS_KEY_HPP
#define KRONOS_VTK_INFORMATION_DATA_STATUS_KEY_HPP

#include <vtkCommonExecutionModelModule.h>
#include <vtkInformationIntegerKey.h>
#include <vtkCommonInformationKeyManager.h>

/**
 * A key containing the status of data a Kronos reader has read so this information can be
 * propagated down the pipeline.
 * This class needs to have this name to work with VTK's information key macro.
 */
class vtkInformationDataStatusKey : public vtkInformationIntegerKey {
public:
	vtkTypeMacro(vtkInformationDataStatusKey, vtkInformationIntegerKey);
	void PrintSelf(ostream& os, vtkIndent indent);

	vtkInformationDataStatusKey(const char* name, const char* location);
	~vtkInformationDataStatusKey();

	/**
	 * This is the crucial method for this class. It is the only method that is not directly inherited
	 * from the superclass `vtkInformationIntegerKey`. It simply copies the data stored under this key
	 * from one `vtkInformation` object to another one, therefore allowing its data to be propagated
	 * downstream.
	 */
	virtual void CopyDefaultInformation(vtkInformation* request,
	                                    vtkInformation* fromInfo,
	                                    vtkInformation* toInfo);

private:
	vtkInformationDataStatusKey(const vtkInformationDataStatusKey&); // Not implemented.
	void operator=(const vtkInformationDataStatusKey&); // Not implemented.
};

#endif