#ifndef _RAYTRACINGCONTROLLER_H_
#define _RAYTRACINGCONTROLLER_H_

#include <map>
#include <string>

#include "RayTracingStrategy.h"
#include "..\World.h"

class RayTracingController{
public:
	RayTracingController();
	~RayTracingController();
	void AddStrategy(int key,RayTracingStrategy* strategy);
	void Apply(World *w,int *dst, Vec3 ViewPort);
	bool SetStrategy(int key,void (*fnc)(char*));

private:
	RayTracingStrategy* selectedStrategy;
	std::map<int, RayTracingStrategy*> Strategies;
};


#endif