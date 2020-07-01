#include "frustum.h"

float Plane::Distance(neko::Vec3f point)
{
	float distance = _normal.Dot(point - _point, _normal);
	return distance;
}

void Frustum::ConstructFrustum()
{

}

bool Frustum::ContainsPoint(neko::Vec3f point)
{
	for (int i = 0; i < 6;i++)
	{
		if (m_Planes[i].Distance(point) < 0)
		{
			return false;
		}
	}
	return true;
}

bool Frustum::ContainsCube(neko::Aabb3d aabb)
{
	return true;
}

bool Frustum::ContainsSphere(neko::Vec3f center, float radius)
{
	for(int i =0; i<6;i++)
	{
		if (abs(m_Planes[i].Distance(center))-radius < 0)
		{
			return false;
		}
	}
	return true;
}
