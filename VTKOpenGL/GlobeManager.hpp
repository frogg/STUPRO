#ifndef STUPRO_GLOBEMANAGER_HPP
#define STUPRO_GLOBEMANAGER_HPP

class GlobeManager
{
public:
	GlobeManager();
	virtual ~GlobeManager();

	void setResolution(Vector2u resolution);
	Vector2u getResolution() const;

	
private:
	
	vtkSmartPointer<vtkPlaneSource> myPlaneSource;
};

#endif
