#include "RayTracingController.h"
#include <string>

RayTracingController::RayTracingController()
{

}

RayTracingController::~RayTracingController()
{
	Strategies.clear();
}

void RayTracingController::AddStrategy(int key, RayTracingStrategy* strategy)
{
	Strategies[key] =  strategy;
}

bool RayTracingController::SetStrategy(int key, void (*fnc)(char*))
{
	if (Strategies.count(key) == 1)
	{
		selectedStrategy = Strategies[key];

		char* mode = "Mode: ";
		char* name = selectedStrategy->GetName();
		char* text = new char[std::strlen(name) + 7];
		char* itr = text;
		while (*mode) *itr++ = *mode++;
		while (*name) *itr++ = *name++;
		*itr = '\0';

		fnc(text);
		delete text;
		return true;
	}
	return false;
}

void RayTracingController::Apply(World *w, int *dst, Vec3 ViewPort)
{
	this->selectedStrategy->DrawNextFrame(w,dst,ViewPort);
}
