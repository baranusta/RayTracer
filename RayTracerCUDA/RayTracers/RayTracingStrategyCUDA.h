#ifndef _RAYTRACINGSTRATEGYCUDA_H_
#define _RAYTRACINGSTRATEGYCUDA_H_

#include "RayTracingStrategy.h"
#include "Utils\Vec3.h"

class RayTracingStrategyCUDA : public RayTracingStrategy
{
public:
	RayTracingStrategyCUDA(float d, float p)
		:RayTracingStrategy(d,p)
	{
		this->name = "CUDA";
	}

	void DrawNextFrame(World* w, int* dst, Vec3 ViewPort)
	{

	}
};

#endif