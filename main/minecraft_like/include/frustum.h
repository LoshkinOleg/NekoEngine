#pragma once
#include "mathematics/vector.h"
#include "mathematics/aabb.h"

struct Plane
{
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

	void ConstructFrustum();

	bool ContainsPoint(neko::Vec3f point);
	bool ContainsCube(neko::Aabb3d aabb);
	bool ContainsSphere(neko::Vec3f center, float radius);

private:
	Plane m_Planes[6];
};