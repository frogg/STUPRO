#ifndef KRONOS_ABSTRACT_SELECTION_FILTER_HPP
#define KRONOS_ABSTRACT_SELECTION_FILTER_HPP

#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkPointSetAlgorithm.h>

#include <qstring.h>
#include <qlist.h>

#include <Reader/DataReader/Data.hpp>

/**
 * Abstract super class for filters that work on a Kronos point data set and extract some of those points.
 */
class AbstractSelectionFilter : public vtkPointSetAlgorithm {
public:
    vtkTypeMacro(AbstractSelectionFilter, vtkPointSetAlgorithm)
	static AbstractSelectionFilter *New();
	void PrintSelf(ostream &os, vtkIndent indent) override;

	int RequestData(vtkInformation *info,
					vtkInformationVector **inputVector,
					vtkInformationVector *outputVector) override;
    int RequestInformation(vtkInformation *request,
                    vtkInformationVector **inputVector,
                    vtkInformationVector *outputVector) override;
	int FillOutputPortInformation(int port, vtkInformation *info) override;
	int FillInputPortInformation(int port, vtkInformation *info) override;
    
    /**
     * Get a list of all data types this concrete filter is compatible with.
     * @return A list of all compatible data types
     */
    virtual QList<Data::Type> getCompatibleDataTypes() = 0;
    
protected:
    /**
     * Display an error message and remember that this filter does not hold valid data.
     * @param message The error message to be shown to the user
     */
    void fail(QString message);

private:
	AbstractSelectionFilter();
	~AbstractSelectionFilter();

	AbstractSelectionFilter(const AbstractSelectionFilter &); // Not implemented.
    void operator=(const AbstractSelectionFilter &); // Not implemented.

    /**
     * Boolean flag denoting whether there was an error.
     */
    bool error;
};

#endif
