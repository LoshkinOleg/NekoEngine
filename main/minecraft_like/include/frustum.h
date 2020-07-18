#pragma once
#include "mathematics/vector.h"
#include "mathematics/aabb.h"
#include <graphics/camera.h>

struct Plane
{
	Plane(){};
	explicit Plane(const neko::Vec3f & point, const neko::Vec3f & normal)
	{
		point_ = point;
		normal_ = normal;
	}
	explicit Plane(const neko::Vec3f & pointA, const neko::Vec3f & pointB, const neko::Vec3f & pointC) //TODO clean + optimize
	{
		point_ = pointA;
		normal_ = CalculateNormalFrom(pointA, pointB, pointC);
	}
	[[nodiscard]] static neko::Vec3f CalculateNormalFrom(const neko::Vec3f & pointA, const neko::Vec3f & pointB, const neko::Vec3f & pointC);
	[[nodiscard]] float Distance(const neko::Vec3f & point) const;
	neko::Vec3f normal_; //Towards inside
	neko::Vec3f point_;
};

class Frustum
{
public:
	Frustum(){};
	
	explicit Frustum(const neko::MoveableCamera3D & camera);

	bool Contains(const neko::Vec3f & point);
	bool Contains(const neko::Aabb3d & aabb);
	bool Contains(const neko::Vec3f & center, float radius);

	float cameraRecoil = 0.0f;
private:
	std::array<Plane,6> planes_;
};