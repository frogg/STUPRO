#ifndef KRONOS_READER_HPP
#define KRONOS_READER_HPP

#include <vtkPolyDataAlgorithm.h>
#include <memory>
#include <QString>

#include <Reader/DataReader/JsonReader.hpp>
#include <Utils/Math/Vector3.hpp>

class vtkKronosReader : public vtkPolyDataAlgorithm {
public:
	vtkTypeMacro(vtkKronosReader, vtkPolyDataAlgorithm);

	/**
	 * Set the name of the file this reader should use.
	 * @param name Path of the file that should be read
	 */
	void SetFileName(std::string name);

	/**
	 * Set the camera position this reader uses to calculate the level of detail.
	 * @param x X value of the camera position
	 * @param y Y value of the camera position
	 * @param z Z value of the camera position
	 */
	void SetCameraPosition(double x, double y, double z);

	/**
	 * Initialize a new Kronos reader.
	 */
	static vtkKronosReader* New();

protected:
	vtkKronosReader();
	~vtkKronosReader();

	int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
	int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*);
private:
	vtkKronosReader(const vtkKronosReader&); // Not implemented
	void operator=(const vtkKronosReader&);   // Not implemented

	/**
	 * Recalculate the zoom level of this reader using its currently stored camera position.
	 */
	void recalculateZoomLevel();

	/**
	 * Display an error message and remember that this reader does not hold valid data.
	 * @param message The error message to be shown to the user
	 */
	void fail(QString message);

	/**
	 * Boolean flag denoting whether there was an error while reading the data.
	 */
	bool error;

	/**
	 * Current camera position as used for calculating the zoom level.
	 */
	Vector3d cameraPosition;

	/**
	 * The globe radius as read from the configuration file.
	 */
	double globeRadius;

	/**
	 * The camera altitude where the smallest zoom level will be displayed.
	 */
	double highestAltitude;

	/**
	 * The highest possible zoom priority.
	 */
	int maximumPriority;

	/**
	 * The current zoom level at which this reader will output points.
	 */
	int zoomLevel;

	/**
	 * Path and name of the file currently read.
	 */
	QString fileName;

	/**
	 * Unique pointer to the `JsonReader` that is used with this reader.
	 */
	std::unique_ptr<JsonReader> jsonReader;

};

#endif
