#include "World.h"



World::World()
{
	Light.push_back(Vec3(0, 0, 500));
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

float World::GetKA()
{
	return ka;
}

Vec3 World::GetLight()
{
	return Light[0];
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
		Vec3 Pos, Color;
		float r;
		bool toRight;
		(*Objects)[i]->getInfo(Pos, r, Color, toRight);
		obj[i * 8] = Pos.getX();
		obj[i * 8 + 1] = Pos.getY();
		obj[i * 8 + 2] = Pos.getZ();
		obj[i * 8 + 3] = r;
		obj[i * 8 + 4] = Color.getX();
		obj[i * 8 + 5] = Color.getY();
		obj[i * 8 + 6] = Color.getZ();
		obj[i * 8 + 7] = toRight ? 1.0f : -1.0f;
	}
}