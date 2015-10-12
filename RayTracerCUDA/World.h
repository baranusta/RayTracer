#ifndef _WORLD_H_
#define _WORLD_H_

#include <vector>
#include <string>

#include <GL/glew.h>

#include "Utils\Utils.h"
#include "Objects\WorldObjects\GeometricObject.h"
#include "Objects\Light\Light.h"

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
	World(Color);
	~World();
	void UpdateObjects();
	void SetUpdateType(int type);
	void CopyToGPUArray(GLfloat * obj);
	void AddObject(GeometricObject*);
	Light GetLight();
	int GetObjectSize();
	Color GetAmbient();

	void AddLight(Vec3 pos, Color Ambient, Color Diffuse, Color Specular);
	void AddLight(Light light);
	//This will be removed
	//when the raytracing strategies implemented
	std::vector<GeometricObject*>* GetObjects(){
		return Objects;
	}
private:
	void UpdateObjectsSequential();
	void UpdateObjectsOpenMP();

	int updateType = 1;
	Color AmbientColor;

	//Later This Will hold Light Objects
	std::vector<Light> lights;
	std::vector<GeometricObject*>* Objects;
};

#endif