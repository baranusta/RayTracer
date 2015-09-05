#ifndef _GEOMETRICOBJECT_H_
#define _GEOMETRICOBJECT_H_


#include "Vec3.h"

class RectWorld;


class GeometricObject
{
public:
	static int Count;
	virtual float getMaxX() = 0;
	virtual float getMaxY() = 0;
	virtual float getMaxZ() = 0;
	virtual float getMinX() = 0;
	virtual float getMinY() = 0;
	virtual float getMinZ() = 0;
	virtual void Move() = 0;
	virtual void getInfo(Vec3&, float&, Vec3&, bool&) = 0;
	virtual bool isIntersects(Vec3 p) = 0;
	void setRect(RectWorld* c);
	Vec3 getDiffuse();
	Vec3 getAmbient();
	Vec3 getSpecular();
	int getId();
	virtual bool isRayIntersects(Vec3 ray, Vec3 src, Vec3&, Vec3 & Point, float& dist) = 0;

	virtual int nextPos(Vec3 p) = 0;
protected:
	int id;
	Vec3 ColorAmbient;
	Vec3 ColorSpecular;
	Vec3 ColorDiffuse;
	bool toRight;

private:
	RectWorld * container;
};


#endif