#include "World.h"

World::World()
{
	AmbientColor = Color(0,0,0);
	Objects = new std::vector<GeometricObject*>();
}

World::World(Color c)
{
	AmbientColor = c;
	Objects = new std::vector<GeometricObject*>();
}

World::~World()
{
	for (GeometricObject* obj : *Objects)
			delete obj;
	Objects->clear();
	delete Objects;
}

void World::AddObject(GeometricObject* p)
{
	Objects->push_back(p);
}

Color World::GetAmbient()
{
	return AmbientColor;
}

void World::AddLight(Vec3 pos, Color Ambient, Color Diffuse, Color Specular)
{
	AddLight(Light(pos,Ambient,Diffuse,Specular));
}

void World::AddLight(Light light)
{
	lights.push_back(light);
}

Light World::GetLight()
{
	return lights[0];
}

void World::UpdateObjects()
{
	if (updateType == 1)
		UpdateObjectsSequential();
	else
		UpdateObjectsOpenMP();
}

void World::UpdateObjectsOpenMP()
{
	#pragma omp parallel for schedule(static)
	for (int i = 0; i< Objects->size(); i++)
		(*Objects)[i]->Move();
}

void World::UpdateObjectsSequential()
{
	for (int i = 0; i< Objects->size(); i++)
		(*Objects)[i]->Move();
}

void World::SetUpdateType(int type)
{
	updateType = type;
}

int World::GetObjectSize()
{
	return Objects->size();
}

void World::CopyToGPUArray(GLfloat *obj)
{
	for (int i = 0; i < Objects->size(); i++)
	{
		Vec3 pos;
		Color color;
		float r;
		bool toRight;
		(*Objects)[i]->getInfo(pos, r, color, toRight);
		obj[i * 8] = pos.getX();
		obj[i * 8 + 1] = pos.getY();
		obj[i * 8 + 2] = pos.getZ();
		obj[i * 8 + 3] = r;
		obj[i * 8 + 4] = color.getR();
		obj[i * 8 + 5] = color.getG();
		obj[i * 8 + 6] = color.getB();
		obj[i * 8 + 7] = toRight ? 1.0f : -1.0f;
	}
}