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

	neko::Vec3f ntl = nearP.point_ + up * heightNear / 2 - right * widthNear / 2;
	neko::Vec3f fbr = farP.point_ - up * heightFar / 2 + right * widthFar / 2;
	
	xMax = std::max(std::max(std::max(ntr.x, nbr.x), std::max(nbl.x, ftr.x)), std::max(std::max(ftl.x, fbl.x), std::max(ntl.x, fbr.x)));
	yMax = std::max(std::max(std::max(ntr.y, nbr.y), std::max(nbl.y, ftr.y)), std::max(std::max(ftl.y, fbl.y), std::max(ntl.y, fbr.y)));
	zMax = std::max(std::max(std::max(ntr.z, nbr.z), std::max(nbl.z, ftr.z)), std::max(std::max(ftl.z, fbl.z), std::max(ntl.z, fbr.z)));

	xMin = std::min(std::min(std::min(ntr.x, nbr.x), std::min(nbl.x, ftr.x)), std::min(std::min(ftl.x, fbl.x), std::min(ntl.x, fbr.x)));
	yMin = std::min(std::min(std::min(ntr.y, nbr.y), std::min(nbl.y, ftr.y)), std::min(std::min(ftl.y, fbl.y), std::min(ntl.y, fbr.y)));
	zMin = std::min(std::min(std::min(ntr.z, nbr.z), std::min(nbl.z, ftr.z)), std::min(std::min(ftl.z, fbl.z), std::min(ntl.z, fbr.z)));	
}

bool Frustum::Contains(const neko::Vec3f & point)
{
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
	/*
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
	*/
	
	if (aabb.upperRightBound.x < xMin)
		return false;
	if (aabb.upperRightBound.y < yMin)
		return false;
	if (aabb.upperRightBound.z < zMin)
		return false;

	if (aabb.lowerLeftBound.x > xMax)
		return false;
	if (aabb.lowerLeftBound.y > yMax)
		return false;
	if (aabb.lowerLeftBound.z > zMax)
		return false;
	return true;
}

bool Frustum::Contains(const neko::Vec3f & center, float radius)
{
	for(int i =0; i<6;i++)
	{
		if (planes_[i].Distance(center) < radius)
		{
			return true;
		}
	}
	return false;
}