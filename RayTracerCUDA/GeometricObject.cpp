#include "GeometricObject.h"


int GeometricObject::Count = 0;

void GeometricObject::setRect(RectWorld* c)
{
	container = c;
}
int GeometricObject::getId()
{
	return id;
}


Vec3 GeometricObject::getDiffuse()
{
	return ColorDiffuse;
}
Vec3 GeometricObject::getAmbient()
{
	return ColorDiffuse;
}
Vec3 GeometricObject::getSpecular()
{
	return ColorDiffuse;
}