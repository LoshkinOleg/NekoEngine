#pragma once
#include "mathematics/vector.h"
#include "mathematics/aabb.h"

struct Plane
{
	Plane(neko::Vec3f point, neko::Vec3f normal)
	{
		_point = point;
		_normal = normal;
	}
	Plane(neko::Vec3f pointA, neko::Vec3f pointB, neko::Vec3f pointC) //TODO clean + optimize
	{
		_point = pointA;
		_normal = GetNormal(pointA, pointB, pointC);
	}
	neko::Vec3f GetNormal(neko::Vec3f pointA, neko::Vec3f pointB, neko::Vec3f pointC);
	float Distance(neko::Vec3f point);
	neko::Vec3f _normal; //Towards inside
	neko::Vec3f _point;
};

class Frustum
{
public:
	Frustum();
	Frustum(const Frustum&);
	~Frustum();

	void ConstructFrustum(neko::Vec3f position, neko::Vec3f direction, float nearPlaneDistance, float farPlaneDistance, neko::degree_t fovx, neko::degree_t fovy, neko::Vec3f up, neko::Vec3f right);

	bool ContainsPoint(neko::Vec3f point);
	bool ContainsCube(neko::Aabb3d aabb);
	bool ContainsSphere(neko::Vec3f center, float radius);

private:
	Plane m_Planes[6];
};