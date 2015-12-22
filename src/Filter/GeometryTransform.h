#ifndef GEOMETRYTRANSFORM
#define GEOMETRYTRANSFORM

#include <vtkAbstractTransform.h>

// FIXME: This is just copy pasted from vtkGeoSphereTransform

class GeometryTransform : public vtkAbstractTransform {
private:
    bool transform;
    bool ToRectangular;
    double BaseAltitude;
public:
    static GeometryTransform *New(bool transform = true) {
        return new GeometryTransform(transform);
    }

    GeometryTransform(bool transform = true) {
        this->transform = transform;
        this->ToRectangular = true;
        this->BaseAltitude = 0.0;
    }

    ~GeometryTransform() {
    }

    void Inverse() {
      this->ToRectangular = !this->ToRectangular;
      this->Modified();
    }

    void InternalTransformPoint(const float in[3], float out[3]) {
      double ind[3];
      double oud[3];
      int i;
      for ( i = 0; i < 3; ++ i )
        ind[i] = in[i];
      this->InternalTransformPoint( ind, oud );
      for ( i = 0; i < 3; ++ i )
        out[i] = static_cast<float>(oud[i]);
    }

    void InternalTransformPoint(const double in[3], double out[3]) {
      if ( this->ToRectangular )
        {
        //vtkGlobeSource::ComputeGlobePoint(in[0], in[1], vtkGeoMath::EarthRadiusMeters() + in[2] + this->BaseAltitude, out);
        }
      else
        {
        //vtkGlobeSource::ComputeLatitudeLongitude(const_cast<double*>(in), out[0], out[1]);
        //out[2] = vtkMath::Norm(in) - vtkGeoMath::EarthRadiusMeters() - this->BaseAltitude;
        }
    }

    void InternalTransformDerivative(const float in[3], float out[3], float derivative[3][3]) {
      double ind[3];
      double oud[3];
      double drd[3][3];
      int i;
      for ( i = 0; i < 3; ++ i )
        ind[i] = in[i];
      this->InternalTransformDerivative( ind, oud, drd );
      for ( i = 0; i < 3; ++ i )
        {
        out[i] = static_cast<float>(oud[i]);
        for ( int j = 0; j < 3; ++ j )
          {
          derivative[i][j] = drd[i][j];
          }
        }
    }

    void InternalTransformDerivative(const double in[3], double out[3], double derivative[3][3]) {
      // FIXME: Compute derivatives here
      (void) in;
      (void) out;
      (void) derivative;
    }

    vtkAbstractTransform* MakeTransform() {
      GeometryTransform* geoTrans = GeometryTransform::New(this->transform);
      return geoTrans;
    }
};

#endif // GEOMETRYTRANSFORM

