#include "RayTracingStrategyCPU.h"

#include <algorithm>

void RayTracingStrategyCPU::IterateInnerLoop(World *w, int* dst, Vec3 ViewPort, int i, int k)
{
	for (int j = -ViewPort.getZ() / 2, t = 0; j < ViewPort.getZ() / 2; j++, t++)
	{
		Vec3 src(ViewPort.getX(), i, j);
		Vec3 ray = Vec3(1, 0, 0);
		/*Vec3 ray = Vec3(ViewPort.getX(), i, j) - eyePos;
		ray.rotate(ViewPortAngle);
		ray.Normalize();*/
		float minDist = 1000000;
		Vec3 Normal, hitPoint;
		int objectId;
		std::vector<GeometricObject*>* Objects = w->GetObjects();
		for (int id = 0,size = Objects->size(); id < size; id++)
		{
			float dist;
			Vec3 N, hP;
			if ((*Objects)[id]->isRayIntersects(ray, src, N, hP, dist))
			{
				if (dist < minDist)
				{
					minDist = dist;
					Normal = N;
					hitPoint = hP;
					objectId = id;
				}
			}
		}

		if (minDist != 1000000)
			dst[t * (int)ViewPort.getY() + k] = DetermineColor(w->GetLight(), ray, Normal, hitPoint, objectId, w->GetKA(), Objects);
		else
			dst[t * (int)ViewPort.getY() + k] = rgbToInt(0.8f, 0.8f, 0.8f);
		//World->SendRay(collidedObj,size,ray,eyePos,Objects);
		//Color pixelC = DetermineColor(collidedObj,size);
	}
}

int RayTracingStrategyCPU::rgbToInt(float r, float g, float b)
{
	if (r > 1.0f)
		r = 1.0f;
	if (g > 1.0f)
		g = 1.0f;
	if (b > 1.0f)
		b = 1.0f;
	return (int(r * 255) << 16) | (int(g * 255) << 8) | int(b * 255);
}


int RayTracingStrategyCPU::DetermineColor(Vec3 Light, Vec3 ray, Vec3 Normal, Vec3 hitPoint, int objId, float ka, std::vector<GeometricObject*>* Objects)
{
	Vec3 SourceToLight = Light - hitPoint;
	SourceToLight.Normalize();
	Normal.Normalize();
	bool willBeShaded = false;

	for (int id = 0; id < Objects->size(); id++)
	{
		Vec3 N, hP;
		float dist;
		if (id != objId && (*Objects)[id]->isRayIntersects(SourceToLight, hitPoint, N, hP, dist))
		{
			willBeShaded = true;
			break;
		}
	}

	Vec3 amb = (*Objects)[objId]->getDiffuse();
	Vec3 baseColor = Vec3(amb.getX()*ka, amb.getY()*ka, amb.getZ()*ka);
	if (willBeShaded)
	{
		return rgbToInt(baseColor.getX(), baseColor.getY(), baseColor.getZ());
	}
	else{
		float kd = std::max(0.f, SourceToLight.dotProduct(Normal));
		Vec3 h = SourceToLight - ray;
		h.Normalize();
		float ks = pow(std::max(0.f, h.dotProduct(Normal)), 20.0);
		baseColor = baseColor + Vec3(amb.getX()*kd, amb.getY()*kd, amb.getZ()*kd);
		baseColor = baseColor + Vec3(amb.getX()*ks, amb.getY()*ks, amb.getZ()*ks);
		return rgbToInt(baseColor.getX(), baseColor.getY(), baseColor.getZ());
	}
}