#ifndef _RAYTRACINGSTRATEGYCPU_H_
#define _RAYTRACINGSTRATEGYCPU_H_

#include "RayTracingStrategy.h"
#include "..\Objects\WorldObjects\GeometricObject.h"
#include <algorithm>
#include <vector>

class RayTracingStrategyCPU: public RayTracingStrategy
{
public:
	RayTracingStrategyCPU(float d, float p)
		: RayTracingStrategy(d, p)
	{}

	void IterateInnerLoop(World *w, int* dst, Vec3 ViewPort, int i, int k);
private:
	int DetermineColor(Light light, Vec3 ray, Vec3 Normal, Vec3 hitPoint, int objId, std::vector<GeometricObject*>* Objects);
};

#endif