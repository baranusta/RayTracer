#ifndef _RAYTRACINGSTRATEGYSEQUENTIAL_H_
#define _RAYTRACINGSTRATEGYSEQUENTIAL_H_

#include "RayTracingStrategyCPU.h"


class RayTracingStrategySequential : public RayTracingStrategyCPU
{
public:

	RayTracingStrategySequential(float d, float p)
		: RayTracingStrategyCPU(d, p)
	{
		this->name = "CPU";
	}

	void DrawNextFrame(World* w, int *dst, Vec3 ViewPort)
	{
		for (int i = -ViewPort.getY() / 2, k = 0; i < ViewPort.getY() / 2; i++, k++)
		{
			RayTracingStrategyCPU::IterateInnerLoop(w, dst, ViewPort, i, k);
		}
	}
};

#endif