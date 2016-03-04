#ifndef GEOMETRYTRANSFORM
#define GEOMETRYTRANSFORM

#include <vtkAbstractTransform.h>

#include "Utils/Math/Vector3.hpp"
#include "Utils/Math/GeographicFunctions.hpp"
#include "Utils/Misc/Macros.hpp"
#include "Utils/Misc/Exceptions.hpp"

#include <exception>
#include <QString>


/**
 * transforms from gps to world coodinates
 */
class GeometryTransform : public vtkAbstractTransform {
private:
	//inidcates if transformation should be done.
	bool transform;
	//indictes direction in which we transform (normally forward and backward transformation are supported). We only support forward transformation.
	bool transformForward;

	//globe radius
	float globeRadius;

	/**
	 * transforms gps cooridinates (lat, long, height) to world/cartesian coodinate systen
	 */
	template<typename T> void gpsToWorldCoordinates(const Vector3<T>& gps, Vector3<T>& cartesian) {
		if (transform) {
			// Wrap around globe
			cartesian = sphericalToCartesian(gps);
		} else {
			cartesian = sphericalToCartesianFlat(gps);
		}
	}

	/**
	 * transforms gps cooridinates (lat, long, height) to world/cartesian coodinate systen, and also derivates
	 */
	template<typename T> void gpsToWorldAndDerivatives(const Vector3<T>& gps, Vector3<T>& cartesian,
	        T derivatives[3][3]) {

		gpsToWorldCoordinates(gps, cartesian);

		if (transform) {
			sphericalToCartesianJacobian(gps, derivatives);
		} else {
			sphericalToCartesianFlatJacobian(gps, derivatives);
		}
	}

	/**
	 * copies a vector to an array
	 */
	template<typename T> void copyVectorToArray(const Vector3<T>& vector, T array[3]) {
		array[0] = vector.x;
		array[1] = vector.y;
		array[2] = vector.z;
	}

public:
	static GeometryTransform* New(bool transform = true, bool forward = true) {
		return new GeometryTransform(transform, forward);
	}

	GeometryTransform(bool transform = true, bool forward = true) {
		this->transform = transform;
		this->transformForward = forward;
		this->globeRadius = Configuration::getInstance().getFloat("globe.radius");
	}

	~GeometryTransform() {
	}

	/**
	 * transformation is active by default, can be (de)actived with this method
	 */
	void setTransform(bool transform = true) {
		this->transform = transform;
	}

	/**
	 * change from forward transformation to backward transformation or the other way round. We only support forward transformation.
	 */
	void Inverse() override {
		this->transformForward = !this->transformForward;
		this->Modified();
	}

	/**
	 * InternalTransformPoint for floats
	 */
	void InternalTransformPoint(const float in[3], float out[3]) override {

		Vector3f outV = Vector3f(out);
		if (this->transformForward) {
			gpsToWorldCoordinates(Vector3f(in), outV);
		} else {
			throw NoBackwardTransformationException("no backward transformation supported");
		}
		copyVectorToArray(outV, out);
	}

	/**
	 * InternalTransformPoint for doubles
	 */
	void InternalTransformPoint(const double in[3], double out[3]) override {

		Vector3d outV = Vector3d(out);
		if (this->transformForward) {
			gpsToWorldCoordinates(Vector3d(in), outV);
		} else {
			throw NoBackwardTransformationException("no backward transformation supported");
		}
		copyVectorToArray(outV, out);
	}

	/**
	 * InternalTransformDerivative for floats (transforms point AND deriactive)
	 */
	void InternalTransformDerivative(const float in[3], float out[3], float derivative[3][3]) override {

		Vector3f outV = Vector3f(out);

		if (this->transformForward) {
			gpsToWorldAndDerivatives(Vector3f(in), outV, derivative);
		} else {
			throw NoBackwardTransformationException("no backward transformation supported");
		}
		copyVectorToArray<float>(outV, out);
	}

	/**
	 * InternalTransformDerivative for doubles (transforms point AND deriactive)
	 */
	void InternalTransformDerivative(const double in[3], double out[3],
	                                 double derivative[3][3]) override {

		Vector3d outV = Vector3d(out);

		if (this->transformForward) {
			gpsToWorldAndDerivatives(Vector3d(in), outV, derivative);
		} else {
			throw NoBackwardTransformationException("no backward transformation supported");
		}
		copyVectorToArray(outV, out);
	}

	vtkAbstractTransform* MakeTransform() override {
		GeometryTransform* geoTrans = GeometryTransform::New(this->transform, this->transformForward);
		return geoTrans;
	}
};

#endif // GEOMETRYTRANSFORM

