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
	neko::Vec3f direction = -camera.reverseDirection;
	neko::Vec3f position = camera.position + direction;//*10;
	neko::Vec3f right = camera.GetRight();
	neko::Vec3f up = camera.GetUp();
	float nearPlaneDistance = camera.nearPlane;
	float farPlaneDistance = camera.farPlane;
	neko::degree_t fovY = camera.fovY;
	neko::degree_t fovX = camera.GetFovX();

	Plane nearP = Plane(position + direction.Normalized() * nearPlaneDistance, direction.Normalized());
	Plane farP = Plane(position + direction.Normalized() * farPlaneDistance, -direction.Normalized());

	float heightNear = neko::Sin(fovY / 2) * nearPlaneDistance*2;
	float widthNear = neko::Sin(fovY / 2)* nearPlaneDistance*2;
	float heightFar = neko::Sin(fovX / 2) * farPlaneDistance*2;
	float widthFar = neko::Sin(fovY / 2) * farPlaneDistance*2;

	neko::Vec3f ntr = nearP.point_ + up * heightNear + right * widthNear; //Near Top Right
	neko::Vec3f nbr = nearP.point_ - up * heightNear + right * widthNear;
	neko::Vec3f nbl = nearP.point_ - up * heightNear - right * widthNear;
	neko::Vec3f ftr = farP.point_ + up * heightFar + right * widthFar;
	neko::Vec3f ftl = farP.point_ + up * heightFar - right * widthFar;
	neko::Vec3f fbl = farP.point_ - up * heightFar - right * widthFar; //Far Bottom Left

	Plane rightP = Plane(ntr, nbr, ftr);
	Plane leftP = Plane(ftl, fbl, nbl);
	Plane topP = Plane(ntr, ftr, ftl);
	Plane bottomP = Plane(nbr, nbl, fbl);

	planes_[0] = nearP;
	planes_[1] = farP;
	planes_[2] = rightP;
	planes_[3] = leftP;
	planes_[4] = topP;
	planes_[5] = bottomP;

	neko::Vec3f ntl = nearP.point_ + up * heightNear - right * widthNear;
	neko::Vec3f fbr = farP.point_ - up * heightFar + right * widthFar;

	fbr_ = fbr;
	ftr_ = ftr;
	ntl_ = ntl;
	nbl_ = nbl;
	fbl_ = fbl;

	axis[0] = (ntr - camera.position).Normalized();
	axis[1] = (ntl - camera.position).Normalized();
	axis[2] = (nbl - camera.position).Normalized();
	axis[3] = (nbr - camera.position).Normalized();
	axis[4] = camera.GetRight();
	axis[5] = camera.GetUp();

	cameraPosition = position;
	
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

bool Frustum::Contains(const neko::Aabb3d& aabb)
{
	
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

	//TODO Fix SAT
	/*
	//Projects frustum on aabb axis
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

	
	
	//Projects aabb on frustum axis
	std::array<neko::Vec3f, 6> aabbBounds;

	aabbBounds[0] = aabb.lowerLeftBound;
	aabbBounds[1] = neko::Vec3f(aabb.lowerLeftBound.x, aabb.upperRightBound.y, aabb.upperRightBound.z);
	aabbBounds[2] = neko::Vec3f(aabb.lowerLeftBound.x, aabb.upperRightBound.y, aabb.lowerLeftBound.z);
	aabbBounds[3] = neko::Vec3f(aabb.upperRightBound.x, aabb.lowerLeftBound.y, aabb.lowerLeftBound.z);
	aabbBounds[4] = neko::Vec3f(aabb.upperRightBound.x, aabb.lowerLeftBound.y, aabb.upperRightBound.z);
	aabbBounds[5] = aabb.upperRightBound;
	
	//top right axis
	for (int i = 0; i < 6;i++)
	{
		if (!(neko::Vec3f::Dot(aabbBounds[i] - planes_[5].point_, axis[0]) > neko::Vec3f::Dot(ftr_ - planes_[5].point_, axis[0])) &&
			(!(neko::Vec3f::Dot(aabbBounds[i] - ftr_, -axis[0]) > neko::Vec3f::Dot(planes_[5].point_ - ftr_, -axis[0]))))
			break;
		if (i == 5)
			return false;
	}
	//top left axis
	for (int i = 0; i < 6;i++)
	{
			if (!(neko::Vec3f::Dot(aabbBounds[i] - nbl_, axis[1]) > neko::Vec3f::Dot(planes_[3].point_ - nbl_, axis[1])) &&
				(!(neko::Vec3f::Dot(aabbBounds[i] - planes_[3].point_, -axis[1]) > neko::Vec3f::Dot(ntl_ - planes_[3].point_, -axis[1]))))
				break;
			if (i == 5)
				return false;
	}
	//bottom left axis
	for (int i = 0; i < 6;i++)
	{
			
		if (!(neko::Vec3f::Dot(aabbBounds[i] - ntl_, axis[2]) > neko::Vec3f::Dot(fbl_ - ntl_, axis[2])) &&
			(!(neko::Vec3f::Dot(aabbBounds[i] - fbl_, -axis[2]) > neko::Vec3f::Dot(ntl_ - fbl_, -axis[2]))))
			break;
		if (i == 5)
			return false;
	}
	//bottom right axis
	for (int i = 0; i < 6;i++)
	{
		if (!(neko::Vec3f::Dot(aabbBounds[i] - planes_[2].point_, axis[3]) > neko::Vec3f::Dot(fbr_ - planes_[2].point_, axis[3])) &&
			(!(neko::Vec3f::Dot(aabbBounds[i] - fbr_, -axis[3]) > neko::Vec3f::Dot(planes_[2].point_ - fbr_, -axis[3]))))
			break;;
		if (i == 5)
			return false;
	}
	//horizontal axis
	for (int i = 0; i < 6;i++)
	{
		if (!(neko::Vec3f::Dot(aabbBounds[i] - fbl_, axis[4]) > neko::Vec3f::Dot(fbr_ - fbl_, axis[4])) &&
			(!(neko::Vec3f::Dot(aabbBounds[i] - fbr_, -axis[4]) > neko::Vec3f::Dot(fbl_ - fbr_, -axis[4]))))
			break;
		if (i == 5)
			return false;
	}
	//vertical axis
	for (int i = 0; i < 6;i++)
	{
		if (!(neko::Vec3f::Dot(aabbBounds[i] - fbl_, axis[5]) > neko::Vec3f::Dot(planes_[3].point_ - fbl_, axis[5])) && 
			(!(neko::Vec3f::Dot(aabbBounds[i] - planes_[3].point_, -axis[5]) > neko::Vec3f::Dot(fbl_ - planes_[3].point_, -axis[5]))))
			break;
		if (i == 5)
			return false;
	}	
	return true;
	*/
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