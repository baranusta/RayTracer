#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "..\..\Utils\Vec3.h"

class Light
{
public:
	Light(Vec3 pos, Color Ambient, Color Diffuse, Color Specular) :
	status(true), AmbientLight(Ambient), DiffuseLight(Diffuse), SpecularhighLight(Specular), position(pos)
	{};
	~Light();
	void changeStatus(bool);
	void changePosition(Vec3 pos);
	
private:
	bool status;
	Color AmbientLight;
	Color DiffuseLight;
	Color SpecularhighLight;
	Vec3 position;
};

#endif