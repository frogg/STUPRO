#ifndef KRONOS_PRECIPITATION_TYPE_FILTER_HPP
#define KRONOS_PRECIPITATION_TYPE_FILTER_HPP

#include <vtkExtractSelection.h>
#include <vtkPoints.h>

/**
 * This filter can extract data from precipitation point sets read by a Kronos reader depending on the precipitation type of each point.
 */
class PrecipitationTypeFilter : public vtkExtractSelection {
public:
    vtkTypeMacro(PrecipitationTypeFilter, vtkExtractSelection)
	static PrecipitationTypeFilter *New();
	void PrintSelf(ostream &os, vtkIndent indent) override;

	int RequestData(vtkInformation *info,
					vtkInformationVector **inputVector,
					vtkInformationVector *outputVector) override;
	int FillOutputPortInformation(int, vtkInformation *info) override;
	int FillInputPortInformation(int, vtkInformation *info) override;
	
	void enableRain(int enabled);
	void enableSnow(int enabled);
	void enableSleet(int enabled);
	void enableHail(int enabled);

private:
	PrecipitationTypeFilter();
	~PrecipitationTypeFilter();

	PrecipitationTypeFilter(const PrecipitationTypeFilter &); // Not implemented.
    void operator=(const PrecipitationTypeFilter &); // Not implemented.
};

#endif
