/*
 * TerrainHeightTransform.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: bbq
 */

#include <Filter/TerrainHeightTransform.hpp>
#include <Utils/Misc/Exceptions.hpp>

TerrainHeightTransform* TerrainHeightTransform::New(bool clamped, bool forward) {
	return new TerrainHeightTransform(clamped, forward);
}

TerrainHeightTransform::TerrainHeightTransform(bool clamped, bool forward) :
	clamped(clamped),
	transformForward(forward) {
}

TerrainHeightTransform::~TerrainHeightTransform() {
}

void TerrainHeightTransform::setClampingEnabled(bool clamped) {
	this->clamped = clamped;
	this->Modified();
}

bool TerrainHeightTransform::isClampingEnabled() const {
	return this->clamped;
}

void TerrainHeightTransform::Inverse() {
	this->transformForward = !this->transformForward;
	this->Modified();
}

void TerrainHeightTransform::InternalTransformPoint(const float in[3], float out[3]) {
	Vector3f outV = Vector3f(out);
	if (this->transformForward) {
		adjustByTerrainHeight(Vector3f(in), outV);
	} else {
		throw NoBackwardTransformationException("no backward transformation supported");
	}
	copyVectorToArray(outV, out);
}

void TerrainHeightTransform::InternalTransformPoint(const double in[3], double out[3]) {
	Vector3d outV = Vector3d(out);
	if (this->transformForward) {
		adjustByTerrainHeight(Vector3d(in), outV);
	} else {
		throw NoBackwardTransformationException("no backward transformation supported");
	}
	copyVectorToArray(outV, out);
}

void TerrainHeightTransform::InternalTransformDerivative(const float in[3], float out[3],
        float derivative[3][3]) {
	Vector3f outV = Vector3f(out);
	if (this->transformForward) {
		adjustByTerrainHeightWithDerivatives(Vector3f(in), outV, derivative);
	} else {
		throw NoBackwardTransformationException("no backward transformation supported");
	}
	copyVectorToArray(outV, out);
}

void TerrainHeightTransform::InternalTransformDerivative(const double in[3], double out[3],
        double derivative[3][3]) {
	Vector3d outV = Vector3d(out);
	if (this->transformForward) {
		adjustByTerrainHeightWithDerivatives(Vector3d(in), outV, derivative);
	} else {
		throw NoBackwardTransformationException("no backward transformation supported");
	}
	copyVectorToArray(outV, out);
}

vtkAbstractTransform* TerrainHeightTransform::MakeTransform() {
	TerrainHeightTransform* geoTrans = TerrainHeightTransform::New(this->clamped,
	                                   this->transformForward);
	return geoTrans;
}
