#include "frustum.h"

Frustum::Frustum(const neko::MoveableCamera3D & camera)
{
	neko::Vec3f direction = -camera.reverseDirection;
	neko::Vec3f position = camera.position - direction * cameraRecoil;
	neko::Vec3f right = camera.GetRight();
	neko::Vec3f up = camera.GetUp();
	float nearPlaneDistance = camera.nearPlane;
	float farPlaneDistance = camera.farPlane;
	neko::degree_t fovY = camera.fovY;
	neko::degree_t fovX = camera.GetFovX();

	float heightNear = neko::Sin(fovY / 2) * nearPlaneDistance * 2;
	float widthNear = neko::Sin(fovY / 2)* nearPlaneDistance * 2;
	float heightFar = neko::Sin(fovX / 2) * farPlaneDistance * 2;
	float widthFar = neko::Sin(fovY / 2) * farPlaneDistance * 2;

	enum planes {NEAR,FAR,RIGHT,LEFT,TOP,BOTTOM};
	
	planes_[NEAR] = Plane(position + direction.Normalized() * nearPlaneDistance, direction.Normalized());
	planes_[FAR] = Plane(position + direction.Normalized() * farPlaneDistance, -direction.Normalized());
	
	neko::Vec3f ntr = planes_[0].point_ + up * heightNear + right * widthNear; //Near Top Right
	neko::Vec3f nbr = planes_[0].point_ - up * heightNear + right * widthNear;
	neko::Vec3f nbl = planes_[0].point_ - up * heightNear - right * widthNear;
	neko::Vec3f ftr = planes_[1].point_ + up * heightFar + right * widthFar;
	neko::Vec3f ftl = planes_[1].point_ + up * heightFar - right * widthFar;
	neko::Vec3f fbl = planes_[1].point_ - up * heightFar - right * widthFar; //Far Bottom Left
	//TODO enum

	planes_[RIGHT] = Plane(ntr, nbr, ftr);
	planes_[LEFT] = Plane(ftl, fbl, nbl);
	planes_[TOP] = Plane(ntr, ftr, ftl);
	planes_[BOTTOM] = Plane(nbr, nbl, fbl);

	neko::Vec3f ntl = planes_[NEAR].point_ + up * heightNear - right * widthNear;
	neko::Vec3f fbr = planes_[FAR].point_ - up * heightFar + right * widthFar;
}

bool Frustum::Contains(const neko::Aabb3d& aabb)
{
	std::array<neko::Vec3f, 8> aabbBounds;

	aabbBounds[0] = aabb.lowerLeftBound;
	aabbBounds[1] = neko::Vec3f(aabb.lowerLeftBound.x, aabb.lowerLeftBound.y, aabb.upperRightBound.z);
	aabbBounds[2] = neko::Vec3f(aabb.lowerLeftBound.x, aabb.upperRightBound.y, aabb.lowerLeftBound.z);
	aabbBounds[3] = neko::Vec3f(aabb.lowerLeftBound.x, aabb.upperRightBound.y, aabb.upperRightBound.z);
	aabbBounds[4] = neko::Vec3f(aabb.upperRightBound.x, aabb.lowerLeftBound.y, aabb.lowerLeftBound.z);
	aabbBounds[5] = neko::Vec3f(aabb.upperRightBound.x, aabb.lowerLeftBound.y, aabb.upperRightBound.z);
	aabbBounds[6] = neko::Vec3f(aabb.upperRightBound.x, aabb.upperRightBound.y, aabb.lowerLeftBound.z);
	aabbBounds[7] = aabb.upperRightBound;
	
	for(int i =0; i < 6; i++)
	{
		float min = planes_[i].Distance(aabbBounds[0]);
		float max = min;
		for (int j = 1; j < 8;j++)
		{
			if (planes_[i].Distance(aabbBounds[j]) < min)
				min = planes_[i].Distance(aabbBounds[j]);
			if (planes_[i].Distance(aabbBounds[j]) > max)
				max = planes_[i].Distance(aabbBounds[j]);
		}
		if (max < 0.0f) //On the outside of the axis
			return false;
	}
	return true;
}