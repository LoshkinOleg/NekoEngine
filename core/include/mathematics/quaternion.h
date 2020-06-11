#pragma once
#include <engine/component.h>
#include <mathematics/vector.h>
#include "mathematics/trigo.h"


//DATE : 19.02.2020

namespace neko
{
struct Quaternion
{
	union
	{
		struct {
			float x;
			float y;
			float z;
			float w;
		};
		float coord[4];
	};
	const static Quaternion Identity;

	Quaternion()
	{
		x = y = z = w = 0;
	}

	explicit Quaternion(float same)
		: x(same), y(same), z(same), w(same)
	{
	}

	Quaternion(float X, float Y, float Z, float W) noexcept
		: x(X), y(Y), z(Z), w(W)
	{
	}


	const float& operator[](size_t p_axis) const
	{
		return coord[p_axis];
	}

	float& operator[](size_t p_axis)
	{

		return coord[p_axis];
	}

	//The dot product between two rotations.
	static float Dot(Quaternion a, Quaternion b)
	{
		return	a.x * b.x +
				a.y * b.y +
				a.z * b.z + 
				a.w * b.w;
	}

	//Converts this quaternion to one with the same orientation but with a magnitude of 1.
	static Quaternion Normalized(Quaternion quaternion)
	{
		return quaternion / Magnitude(quaternion);
	}

	static float Magnitude(Quaternion quaternion)
	{
		return std::sqrt(quaternion.x * quaternion.x +
					quaternion.y * quaternion.y +
					quaternion.z * quaternion.z +
					quaternion.w * quaternion.w);
	}

	//Rotates the Quaternion of angle degrees around axis.
	static Quaternion AngleAxis(radian_t rad, neko::Vec3f axis)
	{
		if (axis.SquareMagnitude() == 0.0f)
			return Identity;

		Quaternion result;
		axis = axis.Normalized();
		axis *= Sin(rad);
		result.x = axis.x;
		result.y = axis.y;
		result.z = axis.z;
		result.w = Cos(rad);

		return Normalized(result);
	}


	//Returns the angle in degrees between two rotations a and b.
	static degree_t Angle(Quaternion a, Quaternion b)
	{
		
		return 2.0f * Acos(std::abs(Dot(a, b)));
	}

	Quaternion Conjugate() const
	{
		return Quaternion(-x, -y, -z, w);
	}

	//Returns the Inverse of rotation.
	Quaternion Inverse() const
	{
		const Quaternion conj = Conjugate();
		const float mag = Magnitude(*this);

		return conj / (mag * mag);
	}

	/*
	Returns a rotation that rotates z degrees around the z axis,
	x degrees around the x axis, and y degrees around the y axis; 
	applied in that order
	*/
	static Quaternion FromEuler(const EulerAngles& angle)
	{
		const auto cy = Cos(angle.x * 0.5f);
		const auto sy = Sin(angle.x * 0.5f);
		const auto cp = Cos(angle.y * 0.5f);
		const auto sp = Sin(angle.y * 0.5f);
		const auto cr = Cos(angle.z * 0.5f);
		const auto sr = Sin(angle.z * 0.5f);

		return Quaternion(
			cy * cp * cr + sy * sp * sr,
			cy * cp * sr - sy * sp * cr,
			sy * cp * sr + cy * sp * cr,
			sy * cp * cr - cy * sp * sr
		);
	}

	EulerAngles ToEuler() const
	{
		/*
		reference
		http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
		http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
		*/
	
		EulerAngles eulerAngles;
	
		// Threshold for the singularities found at the north/south poles.
		const float SINGULARITY_THRESHOLD = 0.4999995f;
	
		const auto sqw = w * w;
		const auto sqx = x * x;
		const auto sqy = y * y;
		const auto sqz = z * z;
		const auto unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
		const auto singularityTest = x * z + w * y;
	
		if (singularityTest > SINGULARITY_THRESHOLD * unit)
		{
			eulerAngles.z = 2.0f * Atan2(x, w);
			eulerAngles.y = radian_t(PI / 2.0f);
			eulerAngles.x = degree_t(0);
		}
		else if (singularityTest < -SINGULARITY_THRESHOLD * unit)
		{
			eulerAngles.z = -2.0f * Atan2(x, w);
			eulerAngles.y = radian_t(-PI / 2.0f);
			eulerAngles.x = degree_t(0);
		}
		else
		{
			eulerAngles.z = Atan2(2 * (w * z - x * y), sqw + sqx - sqy - sqz);
			eulerAngles.y = Asin(2 * singularityTest / unit);
			eulerAngles.x = Atan2(2 * (w * x - y * z), sqw - sqx - sqy + sqz);
		}
	
		return eulerAngles;
	}
	
	//Operators
	Quaternion operator/(Quaternion rhs) const
	{
		return *this * rhs.Inverse();
	}

	Quaternion operator/(const float rhs) const {
		return Quaternion(
			x / rhs,
			y / rhs,
			z / rhs,
			w / rhs);
	}

	Quaternion& operator+=(const float rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		w /= rhs;
		return *this;
	}

	Quaternion operator-(const Quaternion& rhs) const
	{
		return Quaternion(
			x - rhs.x,
			y - rhs.y, 
			z - rhs.z, 
			w - rhs.w);
	}
	Quaternion& operator-=(const Quaternion& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;
		return *this;
	}

	Quaternion operator+(const Quaternion& rhs) const
	{
		return Quaternion(
			x + rhs.x,
			y + rhs.y,
			z + rhs.z,
			w + rhs.w);
	}

	Quaternion& operator+=(const Quaternion& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;
		return *this;
	}
	

	Quaternion operator*(const Quaternion& rhs) const
	{
		return Quaternion(
			w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
			w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
			w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x,
			w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z);
	}

	Quaternion operator*(const float rhs) const {
		return Quaternion(
			x * rhs,
			y * rhs,
			z * rhs,
			w * rhs);
	}
	
	Quaternion& operator*=(const Quaternion& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		w *= rhs.w;
		return *this;
	}
	
	bool operator==(const Quaternion& right) const
	{
		return x == right.x && y == right.y && z == right.z && w == right.w;
	}
	
	bool operator!=(const Quaternion& right) const
	{
		return !(*this == right);
	}

    friend std::ostream& operator<<(std::ostream& os, const Quaternion& quat)
    {
        os << "Quaternion(" << quat.x << "," << quat.y << "," << quat.z << "," << quat.w << ")";
        return os;
    }
};

const inline Quaternion Quaternion::Identity = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}