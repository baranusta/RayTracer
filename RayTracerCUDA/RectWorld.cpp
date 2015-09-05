#include "RectWorld.h"

RectWorld::RectWorld(RectWorld* Parent, Vec3 max, Vec3 min)
{
	Construct(Parent, max, min);
}

RectWorld::RectWorld(RectWorld* Parent, Boundary b)
{
	Construct(Parent, b.max, b.min);
}

void RectWorld::Construct(RectWorld* Parent, Vec3 max, Vec3 min)
{
	parent = Parent;
	SubWorlds = new RectWorld*[8]();
	for (int i = 0; i < 8; i++)
	{
		SubWorlds[i] = NULL;
	}
	bound = new Boundary(max, min);
}

RectWorld::~RectWorld()
{
	delete bound;
	for (int i = 0; i < 8; i++)
	{
		delete SubWorlds[i];
	}
}

void RectWorld::AddObject(GeometricObject *obj){
	if (!bound->isSmallest() && !obj->isIntersects(bound->getCenter()))
	{
		Vec3 center = bound->getCenter();
		int nextPos = obj->nextPos(center);
		Boundary newBound = getNewBoundary(nextPos, center);

		SubWorlds[nextPos] = new RectWorld(this, newBound);
		SubWorlds[nextPos]->AddObject(obj);
	}
	else
	{
		obj->setRect(this);
		objects.push_back(obj->getId());
	}
}

void RectWorld::SendRay(collisionInfo arr[], int & size, Vec3 ray, Vec3 src, std::vector<GeometricObject*>* obj){

}

Boundary RectWorld::getNewBoundary(int pos, Vec3 center){
	float xMax, yMax, zMax;
	float xMin, yMin, zMin;

	if (pos - 4 >= 0)
	{
		pos -= 4;
		xMax = bound->max.getX();
		xMin = center.getX();
	}
	else
	{
		xMax = center.getX();
		xMin = bound->min.getX();
	}
	if (pos - 2 >= 0)
	{
		pos -= 2;
		yMax = bound->max.getY();
		yMin = center.getY();
	}
	else
	{
		yMax = center.getY();
		yMin = bound->min.getY();
	}
	if (pos - 1 >= 0)
	{
		pos -= 1;
		zMax = bound->max.getZ();
		zMin = center.getZ();
	}
	else
	{
		zMax = center.getZ();
		zMin = bound->min.getZ();
	}
	return Boundary(Vec3(xMax, yMax, zMax), Vec3(xMin, yMin, zMin));
}