#pragma once
#include "mathematics/vector.h"
#include "mathematics/aabb.h"

struct Plane //TODO change syntax to project standards
{
	Plane();
	explicit Plane(neko::Vec3f point, neko::Vec3f normal)
	{
		_point = point;
		_normal = normal;
	}
	explicit Plane(neko::Vec3f pointA, neko::Vec3f pointB, neko::Vec3f pointC) //TODO clean + optimize
	{
		_point = pointA;
		_normal = CalculateNormalFrom(pointA, pointB, pointC);
	}
	[[nodiscard]] neko::Vec3f CalculateNormalFrom(neko::Vec3f pointA, neko::Vec3f pointB, neko::Vec3f pointC) const;
	[[nodiscard]] float Distance(neko::Vec3f point) const;
	neko::Vec3f _normal; //Towards inside
	neko::Vec3f _point;
};

class Frustum
{
public:
	Frustum();
	//TODO add explicit constructor with camera
	explicit Frustum(const neko::Vec3f & position, const neko::Vec3f & direction, float nearPlaneDistance, float farPlaneDistance, neko::degree_t fovx, neko::degree_t fovy,const neko::Vec3f & up, const neko::Vec3f & right);

	bool Contains(const neko::Vec3f & point);
	bool Contains(const neko::Aabb3d & aabb);
	bool Contains(const neko::Vec3f & center, float radius);

private:
	std::array<Plane,6> planes_;
};