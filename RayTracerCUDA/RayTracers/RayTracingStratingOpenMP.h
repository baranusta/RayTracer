#ifndef _RAYTRACINGSTRATEGYOPENMP_H_
#define _RAYTRACINGSTRATEGYOPENMP_H_

#include "RayTracingStrategyCPU.h"

class RayTracingStrategyOpenMP : public RayTracingStrategyCPU
{
public:

	RayTracingStrategyOpenMP(float d, float p)
		: RayTracingStrategyCPU(d, p)
	{
		this->name = "OpenMP";
	}

	void DrawNextFrame(World* w, int * dst, Vec3 ViewPort)
	{
		int i, k;
		int min = -ViewPort.getY() / 2;
		int max = ViewPort.getY() / 2;
		#pragma omp parallel for num_threads(4) private(i,k)
			for (i = min; i < max; i++)
			{
				k = i + max;
				RayTracingStrategyCPU::IterateInnerLoop(w,dst,ViewPort, i, k);
			}
	}
};
#endif