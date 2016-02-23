/*
 * TerrainHeightTransform.hpp
 *
 *  Created on: Feb 23, 2016
 *      Author: bbq
 */

#ifndef SRC_FILTER_TERRAINHEIGHTTRANSFORM_HPP_
#define SRC_FILTER_TERRAINHEIGHTTRANSFORM_HPP_

#include <Globe/HeightmapSampler.hpp>
#include <Utils/Math/Vector3.hpp>
#include <vtkAbstractTransform.h>

/**
 * transforms data by terrain height.
 */
class TerrainHeightTransform: public vtkAbstractTransform
{
private:
	//inidcates if transformation should be clamped so that heights below 0 are treated as 0.
	bool clamped;
	//indictes direction in which we transform (normally forward and backward transformation are supported). We only support forward transformation.
	bool transformForward;

	template<typename T>
	void adjustByTerrainHeight(const Vector3<T>& input, Vector3<T>& output)
	{
		output.x = input.x;
		output.y = input.y;

		if (clamped)
		{
			output.z = input.z
			        + std::max<T>(0, HeightmapSampler::getInstance().sample(input.x, input.y));
		}
		else
		{
			output.z = input.z + HeightmapSampler::getInstance().sample(input.x, input.y);
		}
	}

	template<typename T>
	void adjustByTerrainHeightWithDerivatives(const Vector3<T>& input, Vector3<T>& output,
	        T derivatives[3][3])
	{
		adjustByTerrainHeight(input, output);
		// TODO: wtf is derivative
	}

	/**
	 * copies a vector to an array
	 */
	template<typename T> void copyVectorToArray(const Vector3<T>& vector, T array[3])
	{
		array[0] = vector.x;
		array[1] = vector.y;
		array[2] = vector.z;
	}

public:
	static TerrainHeightTransform* New(bool clamped = true, bool forward = true);

	TerrainHeightTransform(bool clamped = true, bool forward = true);

	~TerrainHeightTransform();

	void setClampingEnabled(bool clamped = true);

	bool isClampingEnabled() const;

	/**
	 * change from forward transformation to backward transformation or the other way round. We only support forward transformation.
	 */
	void Inverse() override;

	/**
	 * InternalTransformPoint for floats
	 */
	void InternalTransformPoint(const float in[3], float out[3]) override;

	/**
	 * InternalTransformPoint for doubles
	 */
	void InternalTransformPoint(const double in[3], double out[3]) override;

	/**
	 * InternalTransformDerivative for floats (transforms point AND deriactive)
	 */
	void InternalTransformDerivative(const float in[3], float out[3], float derivative[3][3])
	        override;

	/**
	 * InternalTransformDerivative for doubles (transforms point AND deriactive)
	 */
	void InternalTransformDerivative(const double in[3], double out[3], double derivative[3][3])
	        override;

	vtkAbstractTransform* MakeTransform() override;
};

#endif /* SRC_FILTER_TERRAINHEIGHTTRANSFORM_HPP_ */
