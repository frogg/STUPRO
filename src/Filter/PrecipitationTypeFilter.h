#ifndef KRONOS_PRECIPITATION_TYPE_FILTER_HPP
#define KRONOS_PRECIPITATION_TYPE_FILTER_HPP

#include <vtkExtractSelection.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

#include <QMap>

#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>

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
	int FillOutputPortInformation(int port, vtkInformation *info) override;
	int FillInputPortInformation(int port, vtkInformation *info) override;
	
	void enableUndefined(int enabled);
	void enableRain(int enabled);
	void enableSnow(int enabled);
	void enableSleet(int enabled);
	void enableHail(int enabled);

private:
	PrecipitationTypeFilter();
	~PrecipitationTypeFilter();

	PrecipitationTypeFilter(const PrecipitationTypeFilter &); // Not implemented.
    void operator=(const PrecipitationTypeFilter &); // Not implemented.
    
    void displayPrecipitationType(PrecipitationDataPoint::PrecipitationType type, bool display);
	
	vtkSmartPointer<vtkSelection> selection;
	vtkSmartPointer<vtkSelectionNode> selectionNode;
	
	QMap<PrecipitationDataPoint::PrecipitationType, bool> precipitationTypeVisibilities;
};

#endif
