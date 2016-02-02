#ifndef GEOMETRYTRANSFORM
#define GEOMETRYTRANSFORM

#include <vtkAbstractTransform.h>

#include "Utils/Math/Vector3.hpp"
#include "Utils/Math/SphericalCoordinateFunctions.h"
#include "Utils/Misc/Macros.hpp"

class GeometryTransform : public vtkAbstractTransform {
private:
	bool transform;
	bool transformForward;
	double baseAltitude;

    template<typename T> void gpsToWorldCoordinates(const Vector3<T>& gps, Vector3<T>& world) {
        world = sphericalToCartesian(gps);
	}

    template<typename T> void gpsToWorldAndDerivatives(const Vector3<T>& gps, Vector3<T>& world,
	        T** derivatives) {
        
        gpsToWorldCoordinates(gps, world);
        sphericalToCartesianJacobian(gps, derivatives);
	}

	template<typename T> void worldToGPSCoordinates(const Vector3<T> world, Vector3<T>& gps) {
		// TODO
	}

	template<typename T> void worldToGPSAndDerivatives(const Vector3<T> in, Vector3<T>& out,
	        T* derivatives) {
		// TODO
	}

	template<typename T> void copyVectorToArray(const Vector3<T>& vector, T array[3]) {
		array[0] = vector.x;
		array[1] = vector.y;
		array[2] = vector.z;
	}

	template<typename T> void copyVectorToArray(const Vector3<T>& v1,
	        T a1[3],
	        const Vector3<Vector3<T>>& v2,
	        T a2[3][3]) {
		copyVectorToArray(v1, a1);
		copyVectorToArray(v2.x, a2[0]);
		copyVectorToArray(v2.y, a2[1]);
		copyVectorToArray(v2.z, a2[2]);
	}

public:
	static GeometryTransform* New(bool transform = true, bool forward = true,
	                              double baseAltitude = 100.0) {
		return new GeometryTransform(transform, forward, baseAltitude);
	}

	GeometryTransform(bool transform = true, bool forward = true, double baseAltitude = 100.0) {
		this->transform = transform;
		this->transformForward = forward;
		this->baseAltitude = baseAltitude;
	}

	~GeometryTransform() {
	}

	void setTransform(bool transform = true) {
		this->transform = transform;
	}

	void Inverse() override {
		this->transformForward = !this->transformForward;
		this->Modified();
    }

	void InternalTransformPoint(const float in[3], float out[3]) override {
		if (!transform) {
			return;
		}
		Vector3f outV = Vector3f(out);
		if (this->transformForward) {
			gpsToWorldCoordinates(Vector3f(in), outV);
		} else {
			worldToGPSCoordinates(Vector3f(in), outV);
		}
		copyVectorToArray(outV, out);
	}

	void InternalTransformPoint(const double in[3], double out[3]) override {
		if (!transform) {
			return;
		}
		Vector3d outV = Vector3d(out);
		if (this->transformForward) {
			gpsToWorldCoordinates(Vector3d(in), outV);
		} else {
			worldToGPSCoordinates(Vector3d(in), outV);
		}
		copyVectorToArray(outV, out);
	}

	void InternalTransformDerivative(const float in[3], float out[3], float derivative[3][3]) override {
		if (!transform) {
			return;
		}
		Vector3f outV = Vector3f(out);
		
		if (this->transformForward) {
			gpsToWorldAndDerivatives(Vector3f(in), outV, derivative);
		} else {
			worldToGPSAndDerivatives(Vector3f(in), outV, derivative);
		}
		copyVectorToArray<float>(outV, out);
	}

	void InternalTransformDerivative(const double in[3], double out[3],
	                                 double derivative[3][3]) override {
		if (!transform) {
			return;
		}
		Vector3d outV = Vector3d(out);

		if (this->transformForward) {
			gpsToWorldAndDerivatives(Vector3d(in), outV, derivative);
		} else {
			worldToGPSAndDerivatives(Vector3d(in), outV, derivative);
		}
		copyVectorToArray(outV, out);
	}

	vtkAbstractTransform* MakeTransform() override {
		GeometryTransform* geoTrans = GeometryTransform::New(this->transform, this->transformForward,
		                              this->baseAltitude);
		return geoTrans;
	}
};

#endif // GEOMETRYTRANSFORM

