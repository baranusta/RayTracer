#ifndef _RAYTRACINGSTRATEGY_H_
#define _RAYTRACINGSTRATEGY_H_

#include "..\Utils\Vec3.h"
#include "..\World.h"

class RayTracingStrategy{
public:
	RayTracingStrategy(float d,float p)
		:diffuseConst(d), phongPower(p)
	{
	}

	virtual void DrawNextFrame(World *w,int* dst, Vec3 ViewPort) = 0;
	char* GetName()
	{
		return name;
	}
protected:
	const float diffuseConst;
	const float phongPower;
	char* name;
};

#endif