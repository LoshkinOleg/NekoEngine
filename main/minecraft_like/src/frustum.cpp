#include "frustum.h"

Plane::Plane(){}

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

Frustum::Frustum(){}

void Frustum::ConstructFrustum(neko::Vec3f position, neko::Vec3f direction, float nearPlaneDistance, float farPlaneDistance, neko::degree_t fovx, neko::degree_t fovy, neko::Vec3f up, neko::Vec3f right) //TODO clean and optimize
{
	Plane nearP = Plane(position + direction.Normalized() * nearPlaneDistance, direction.Normalized());
	Plane farP = Plane(position + direction.Normalized() * farPlaneDistance, direction.Normalized());

	float heightNear = neko::Cos(fovx / 2) * nearPlaneDistance;
	float widthNear = neko::Cos(fovy / 2) * nearPlaneDistance;
	float heightFar = neko::Cos(fovx / 2) * farPlaneDistance;
	float widthFar = neko::Cos(fovy / 2) * farPlaneDistance;

	neko::Vec3f ntr = nearP._point + up * heightNear/2 + right * widthNear/2; //Near Top Right
	neko::Vec3f nbr = nearP._point - up * heightNear/2 + right * widthNear/2;
	neko::Vec3f nbl = nearP._point - up * heightNear/2 - right * widthNear/2;
	neko::Vec3f ftr = farP._point + up * heightFar/2 + right * widthFar/2;
	neko::Vec3f ftl = farP._point + up * heightFar / 2 - right * widthFar / 2;
	neko::Vec3f fbl = farP._point - up * heightFar / 2 - right * widthFar / 2; //Far Bottom Left

	Plane rightP = Plane(ntr, nbr, ftr);
	Plane leftP = Plane(nbl, ftl, fbl);
	Plane topP = Plane(ntr, ftr, ftl);
	Plane bottomP = Plane(nbr, nbl, fbl);

	m_Planes[0] = nearP;
	m_Planes[1] = farP;
	m_Planes[2] = rightP;
	m_Planes[3] = leftP;
	m_Planes[4] = topP;
	m_Planes[5] = rightP;
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
