#ifndef _WORLD_H_
#define _WORLD_H_

#include <vector>
#include <string>

#include <GL/glew.h>

#include "Utils\Vec3.h"
#include "WorldObjects\GeometricObject.h"

/*
This class is responsible for all the objects in the scene
1. all Objects
2. Light
3. Camera
*/

class World
{
public:
	World();
	~World();
	void UpdateObjects();
	void SetUpdateType(int type);
	void CopyToGPUArray(GLfloat * obj);
	int GetObjectSize();
	void AddObject(GeometricObject*);
	float GetKA();
	Vec3 GetLight();
	//This will be removed
	//when the raytracing strategies implemented
	std::vector<GeometricObject*>* GetObjects(){
		return Objects;
	}
private:
	void UpdateObjectsSequential();
	void UpdateObjectsOpenMP();

	int updateType = 1;
	float ka = 0.2f;

	//Later This Will hold Light Objects
	std::vector<Vec3> Light;
	std::vector<GeometricObject*>* Objects;
};

#endif