#ifndef _RECTWORLD_H_
#define _RECTWORLD_H_

#include "Vec3.h"
#include "GeometricObject.h"
#include <vector>

struct collisionInfo{
	int id;
	Vec3 angleWithNormal;
};


struct Boundary
{
	Vec3 max;
	Vec3 min;
	Boundary(Vec3 MaxPoints, Vec3 MinPoint) :max(MaxPoints), min(MinPoint)
	{}
	bool isSmallest()
	{
		if (max.getX() - min.getX() <= 2)
		{
			return true;
		}
		else if (max.getY() - min.getY() <= 2)
		{
			return true;
		}
		else if (max.getZ() - min.getZ() <= 2)
		{
			return true;
		}
		else
			return false;
	}
	Vec3 getCenter()
	{
		return Vec3((max.getX() + min.getX()) / 2, (max.getY() + min.getY()) / 2, (max.getZ() + min.getZ()) / 2);
	}

};

class RectWorld
{
public:
	RectWorld(RectWorld* Parent, Vec3 max, Vec3 min);
	RectWorld(RectWorld* Parent, Boundary b);
	~RectWorld();
	void AddObject(GeometricObject *obj);
	void SendRay(collisionInfo arr[], int & size, Vec3 ray, Vec3 source, std::vector<GeometricObject*>*);

private:
	void Construct(RectWorld*, Vec3, Vec3);
	std::vector<int> objects;
	RectWorld ** SubWorlds;
	RectWorld * parent;
	Boundary *bound;

	Boundary getNewBoundary(int Pos, Vec3 center);
};

#endif