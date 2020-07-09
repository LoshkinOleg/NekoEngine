#include "frustum.h"

Plane::Plane(){}

float Plane::Distance(const neko::Vec3f & point) const
{
	return neko::Vec3f::Dot(point - point_, normal_);
}

neko::Vec3f Plane::CalculateNormalFrom(const neko::Vec3f & pointA, const neko::Vec3f & pointB, const neko::Vec3f & pointC) const
{
	neko::Vec3f vecA = pointA - pointB;
	neko::Vec3f vecB = pointC - pointB;
	return pointA.Cross(vecA, vecB).Normalized();
}

Frustum::Frustum(){}

Frustum::Frustum(const neko::MoveableCamera3D & camera) //TODO clean and optimize
{
	neko::Vec3f position = camera.position;
	neko::Vec3f direction = -camera.reverseDirection;
	neko::Vec3f right = camera.GetRight();
	neko::Vec3f up = camera.GetUp();
	float nearPlaneDistance = camera.nearPlane;
	float farPlaneDistance = camera.farPlane;
	neko::degree_t fovY = camera.fovY;
	neko::degree_t fovX = camera.GetFovX();

	Plane nearP = Plane(position + direction.Normalized() * nearPlaneDistance, direction.Normalized());
	Plane farP = Plane(position + direction.Normalized() * farPlaneDistance, -direction.Normalized());

	float heightNear = neko::Cos(fovX / 2) * nearPlaneDistance;
	float widthNear = neko::Cos(fovY / 2) * nearPlaneDistance;
	float heightFar = neko::Cos(fovX / 2) * farPlaneDistance;
	float widthFar = neko::Cos(fovY / 2) * farPlaneDistance;

	neko::Vec3f ntr = nearP.point_ + up * heightNear/2 + right * widthNear/2; //Near Top Right
	neko::Vec3f nbr = nearP.point_ - up * heightNear/2 + right * widthNear/2;
	neko::Vec3f nbl = nearP.point_ - up * heightNear/2 - right * widthNear/2;
	neko::Vec3f ftr = farP.point_ + up * heightFar/2 + right * widthFar/2;
	neko::Vec3f ftl = farP.point_ + up * heightFar/2 - right * widthFar/2;
	neko::Vec3f fbl = farP.point_ - up * heightFar/2- right * widthFar/2; //Far Bottom Left

	Plane rightP = Plane(ntr, nbr, ftr);
	Plane leftP = Plane(ftl, fbl, nbl);
	Plane topP = Plane(ntr, ftr, ftl);
	Plane bottomP = Plane(nbr, nbl, fbl);

	planes_[0] = nearP;
	planes_[1] = farP;
	planes_[2] = rightP;
	planes_[3] = leftP;
	planes_[4] = topP;
	planes_[5] = rightP;
}

bool Frustum::Contains(const neko::Vec3f & point)
{
	//std::printf("Contains POINT \n");
	for (int i = 0; i < 6;i++)
	{
		if (planes_[i].Distance(point) < 0)
		{
			return false;
		}
	}
	return true;
}

bool Frustum::Contains(const neko::Aabb3d & aabb)
{
	//std::printf("Contains BOX \n");
	//TODO replace by SAT
	// /!\ DOESNT WORK YET WITH CHUNKS

	if (Contains(aabb.lowerLeftBound))
		return true;
	if (Contains(aabb.upperRightBound))
		return true;

	if (Contains(aabb.lowerLeftBound + neko::Vec3f(1, 0, 0)))
		return true;
	if (Contains(aabb.lowerLeftBound + neko::Vec3f(0, 1, 0)))
		return true;
	if (Contains(aabb.lowerLeftBound + neko::Vec3f(0, 0, 1)))
		return true;
	if (Contains(aabb.upperRightBound - neko::Vec3f(1, 0, 0)))
		return true;
	if (Contains(aabb.upperRightBound - neko::Vec3f(0, 1, 0)))
		return true;
	if (Contains(aabb.upperRightBound - neko::Vec3f(0, 0, 1)))
		return true;
	
	return false;
}

bool Frustum::Contains(const neko::Vec3f & center, float radius)
{
	//std::printf("Contains Sphere \n");
	for(int i =0; i<6;i++)
	{
		if (planes_[i].Distance(center) < radius)
		{
			return true;
		}
	}
	return false;
}
