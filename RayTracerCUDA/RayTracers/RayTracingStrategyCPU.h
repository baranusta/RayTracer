#ifndef _RAYTRACINGSTRATEGYCPU_H_
#define _RAYTRACINGSTRATEGYCPU_H_

#include "RayTracingStrategy.h"
#include "..\WorldObjects\GeometricObject.h"
#include <algorithm>
#include <vector>

class RayTracingStrategyCPU: public RayTracingStrategy
{
public:
	void IterateInnerLoop(World *w, int* dst, Vec3 ViewPort, int i, int k);
private:
	int rgbToInt(float r, float g, float b);
	int DetermineColor(Vec3 Light, Vec3 ray, Vec3 Normal, Vec3 hitPoint, int objId, float ka, std::vector<GeometricObject*>* Objects);
};

#endif