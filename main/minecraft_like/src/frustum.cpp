#include "frustum.h"

float Plane::Distance(neko::Vec3f point)
{
	float distance = _normal.Dot(point - _point, _normal);
	return distance;
}

neko::Vec3f Plane::GetNormal(neko::Vec3f pointA, neko::Vec3f pointB, neko::Vec3f pointC)
{
	neko::Vec3f vecA = pointA - pointB;
	neko::Vec3f vecB = pointC - pointB;
	return pointA.Cross(vecA, vecB).Normalized();
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
