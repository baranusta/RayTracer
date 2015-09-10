#ifndef _RAYTRACINGSTRATEGY_H_
#define _RAYTRACINGSTRATEGY_H_

#include "..\Utils\Vec3.h"
#include "..\World.h"

class RayTracingStrategy{
public:
	virtual void DrawNextFrame(World *w,int* dst, Vec3 ViewPort) = 0;
	char* GetName()
	{
		return name;
	}
protected:
	char* name;
};

#endif