#ifndef KRONOS_VTK_INFORMATION_DATA_TYPE_KEY_HPP
#define KRONOS_VTK_INFORMATION_DATA_TYPE_KEY_HPP

#include <vtkCommonExecutionModelModule.h>
#include <vtkInformationIntegerKey.h>
#include <vtkCommonInformationKeyManager.h>

/**
 * A key containing the type of data a Kronos reader has read so this information can be
 * propagated down the pipeline.
 * This class needs to have this name to work with VTK's information key macro.
 */
class VTKCOMMONEXECUTIONMODEL_EXPORT vtkInformationDataTypeKey : public vtkInformationIntegerKey {
public:
	vtkTypeMacro(vtkInformationDataTypeKey, vtkInformationIntegerKey);
	void PrintSelf(ostream& os, vtkIndent indent);

	vtkInformationDataTypeKey(const char* name, const char* location);
	~vtkInformationDataTypeKey();

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
	vtkInformationDataTypeKey(const vtkInformationDataTypeKey&); // Not implemented.
	void operator=(const vtkInformationDataTypeKey&); // Not implemented.
};

#endif