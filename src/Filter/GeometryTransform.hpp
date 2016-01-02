#ifndef GEOMETRYTRANSFORM
#define GEOMETRYTRANSFORM

#include <vtkAbstractTransform.h>

class GeometryTransform : public vtkAbstractTransform {
private:
    bool transform;
    bool transformForward;
    double baseAltitude;

    template<typename T> void gpsToWorldCoordinates(const T gps[3], T world[3]){
        double lon = gps[0] * M_PI / 180;
        double lat = gps[1] * M_PI / 180;
        double radius = baseAltitude + gps[2];

        world[2] = radius * cos(lat) * cos(lon);
        world[0] = radius * cos(lat) * sin(lon);
        world[1] = radius * sin(lat);
    }

    template<typename T> void worldToGPSCoordinates(const T world[3], T gps[3]) {
        // TODO
    }

    template<typename T> void derivativesToWorldCoordinates(const T in[3], T out[3], T derivatives[3][3]) {
        // TODO
    }

    template<typename T> void derivativesToGPSCoordinates(const T in[3], T out[3], T derivatives[3][3]) {
        // TODO
    }

public:
    static GeometryTransform *New(bool transform = true, bool forward = true, double baseAltitude = 0.0) {
        return new GeometryTransform(transform, forward, baseAltitude);
	}

    GeometryTransform(bool transform = true, bool forward = true, double baseAltitude = 0.0) {
        this->transform = transform;
        this->transformForward = forward;
        this->baseAltitude = baseAltitude;
	}

	~GeometryTransform() {
	}

	void Inverse() {
        this->transformForward = !this->transformForward;
		this->Modified();
	}

    void InternalTransformPoint(const float in[3], float out[3]) {
        if (!transform) return;
        if (this->transformForward) {
            gpsToWorldCoordinates(in, out);
        } else {
            worldToGPSCoordinates(in, out);
        }
	}

	void InternalTransformPoint(const double in[3], double out[3]) {
        if (!transform) return;
        if (this->transformForward) {
            gpsToWorldCoordinates(in, out);
        } else {
            worldToGPSCoordinates(in, out);
		}
	}

	void InternalTransformDerivative(const float in[3], float out[3], float derivative[3][3]) {
        if (!transform) return;
        if (this->transformForward) {
            derivativesToWorldCoordinates(in, out, derivative);
        } else {
            derivativesToGPSCoordinates(in, out, derivative);
        }
	}

	void InternalTransformDerivative(const double in[3], double out[3], double derivative[3][3]) {
        if (!transform) return;
        if (this->transformForward) {
            derivativesToWorldCoordinates(in, out, derivative);
        } else {
            derivativesToGPSCoordinates(in, out, derivative);
        }
	}

	vtkAbstractTransform *MakeTransform() {
        GeometryTransform *geoTrans = GeometryTransform::New(this->transform, this->transformForward, this->baseAltitude);
		return geoTrans;
	}
};

#endif // GEOMETRYTRANSFORM

