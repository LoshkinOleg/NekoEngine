#pragma once
/*
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include <ostream>

#include <mathematics/angle.h>
#include <mathematics/const.h>
#include <mathematics/trigo.h>

namespace neko
{
//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------
template<typename T>
struct Vec2;

template<typename T>
struct Vec3;

template<typename T>
struct alignas(4 * sizeof(T)) Vec4;

//-----------------------------------------------------------------------------
// Vec2
//-----------------------------------------------------------------------------
template<typename T>
struct Vec2
{
    union
    {
        struct
        {
            T x; ///< X coordinate of the vector
            T y; ///< Y coordinate of the vector

        };
        struct
        {
            T u; ///< X coordinate of the vector
            T v; ///< Y coordinate of the vector

        };
        T coord[2];
    };

    const static Vec2 zero;
    const static Vec2 one;
    const static Vec2 up;
    const static Vec2 down;
    const static Vec2 left;
    const static Vec2 right;

    //-----------------------------------------------------------------------------
    // Constructors
    //-----------------------------------------------------------------------------
    Vec2()
	    : x(0), y(0)
    {
    }

    explicit Vec2(T same)
	    : x(same), y(same)
    {
    }

    Vec2(const T& X, const T& Y)
	    : x(X), y(Y)
    {
    }

    explicit Vec2(const T* ptr)
	    : x(ptr[0]),
	      y(ptr[1])
    {
    }

    template<typename U>
    explicit Vec2(const Vec2<U>& vec2)
            : x(static_cast<T>(vec2.x)),
              y(static_cast<T>(vec2.y))
    {

    }

    template<typename U>
    explicit Vec2(const Vec3<U>& vector)
            : x(static_cast<T>(vector.x)),
              y(static_cast<T>(vector.y))
    {
    }

    template<typename U>
    explicit Vec2(const Vec4<U>& vec4)
            : x(static_cast<T>(vec4.x)),
              y(static_cast<T>(vec4.y))
    {
    }
	
    /**
     * \brief Adding explicit constructor for vector-like type
     */
    template <class U>
    explicit Vec2(const U& u) :
	    x(u.x),
	    y(u.y)
    {
    }

    //-----------------------------------------------------------------------------
    // Operators
    //-----------------------------------------------------------------------------
    Vec2<T> operator+(const Vec2<T>& rhs) const
    {
        return Vec2<T>(x + rhs.x, y + rhs.y);
    }

    Vec2<T>& operator+=(const Vec2<T>& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }

    Vec2<T> operator-(const Vec2<T>& rhs) const
    {
        return Vec2<T>(x - rhs.x, y - rhs.y);
    }

    Vec2<T>& operator-=(const Vec2<T>& rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        return *this;
    }

    Vec2<T> operator*(T rhs) const
    {
        return Vec2<T>(x * rhs, y * rhs);
    }

    Vec2<T> operator*(const Vec2<T>& rhs) const
    {
        return Vec2<T>(x * rhs.x, y * rhs.y);
    }

    Vec2<T>& operator*=(T rhs)
    {
        this->x *= rhs;
        this->y *= rhs;
        return *this;
    }

    Vec2<T> operator/(const Vec2<T>& rhs) const
    {
    	const T xx = x / rhs.x;
        return Vec2<T>(x / rhs.x, y / rhs.y);
    }

    Vec2<T> operator/(const T& rhs) const
    {
        return (*this) * (1.0f / rhs);
    }

    Vec2<T>& operator/=(const T& rhs)
    {
        this->x /= rhs;
        this->y /= rhs;
        return *this;
    }

    bool operator==(const Vec2<T>& right) const
    {
        return x == right.x && y == right.y;
    }

    bool operator!=(const Vec2<T>& right) const
    {
        return !(*this == right);
    }

    bool operator>(const Vec2<T>& right) const
    {
        return x > right.x && y > right.y;
    }
	
    bool operator>=(const Vec2<T>& right) const
    {
        return x >= right.x && y >= right.y;
    }

    bool operator<(const Vec2<T>& right) const
    {
        return x < right.x && y < right.y;
    }
	
    bool operator<=(const Vec2<T>& right) const
    {
        return x <= right.x && y <= right.y;
    }

    const T& operator[](size_t pAxis) const
    {

        return coord[pAxis];
    }

    T& operator[](size_t pAxis)
    {

        return coord[pAxis];
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec2<T>& dt)
    {
        os << "Vec2(" << dt.x << "," << dt.y << ")";
        return os;
    }

    //Used to specialize in case of other kind of vector
    template<typename U>
    explicit operator U() const;

    //-----------------------------------------------------------------------------
    // Formulas
    //-----------------------------------------------------------------------------
    /// \brief Calculates the dot product from two vectors.
    static T Dot(const Vec2<T>& v1, const Vec2<T>& v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }

    /// \brief Calculates the square magnitude.
    T SquareMagnitude() const
    {
        return Dot(*this, *this);
    }

    /// \brief Calculates the magnitude.
    T Magnitude() const
    {
        return std::sqrt(SquareMagnitude());
    }

    /// \brief Calculates the normalized vector.
    Vec2<T> Normalized() const
    {
        return (*this) / (*this).Magnitude();
    }

    /// \brief Interpolate between two vectors.
    /// \param t The interpolation amount.
    static Vec2<T> Lerp(const Vec2<T>& v1, const Vec2<T>& v2, T t)
    {
        return v1 + (v2 - v1) * t;
    }

    /// \brief Reflect the inVec using the normal given (doesn't need to be normalized).
    /// \param inVec The vector to reflect.
    /// \param normal The normal vector of the line to reflect off.
    static Vec2<T> Reflect(const Vec2<T>& inVec, const Vec2<T>& normal)
    {
        return inVec - normal * 2 * Dot(inVec, normal);
    }

    /// \brief Project v1 on v2 (does not need to be normalized).
    /// \param v1 The vector to project.
    /// \param v2 The vector to project on.
    static Vec2<T> Project(const Vec2<T>& v1, const Vec2<T>& v2)
    {
        const auto dot = Dot(v1, v2);
        const auto mag = v2.SquareMagnitude();
        return {(dot / mag) * v2.x,
                (dot / mag) * v2.y};
    }

    /// \brief Calculates the angle between two vectors.
    static radian_t AngleBetween(const Vec2& v1, const Vec2& v2)
    {
        const float dot = Vec2<T>::Dot(v1, v2) / v1.Magnitude() / v2.Magnitude();
        const float det = v1.x * v2.y - v1.y * v2.x;
        const radian_t angle = Atan2(det, dot);
        return angle;
    }

    /// \brief Rotates the Vec2 from the given angle (in degrees).
    Vec2<T> Rotate(const radian_t angle) const
    {
        return {x * Cos(angle) - y * Sin(angle),
                x * Sin(angle) + y * Cos(angle)};
    }
};
//-----------------------------------------------------------------------------
// Vec2 Aliases
//-----------------------------------------------------------------------------
using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec2u = Vec2<uint32_t>;
using Vec2u64 = Vec4<uint64_t>;
using Vec2u32 = Vec4<uint32_t>;
using Vec2u16 = Vec4<uint16_t>;
using Vec2u8 = Vec4<uint8_t>;

template<typename T>
inline Vec2<T> const Vec2<T>::zero = Vec2<T>(0, 0);
template<typename T>
inline Vec2<T> const Vec2<T>::one = Vec2<T>(1, 1);
template<typename T>
inline Vec2<T> const Vec2<T>::up = Vec2<T>(0, 1);
template<typename T>
inline Vec2<T> const Vec2<T>::down = Vec2<T>(0, -1);
template<typename T>
inline Vec2<T> const Vec2<T>::left = Vec2<T>(-1, 0);
template<typename T>
inline Vec2<T> const Vec2<T>::right = Vec2<T>(1, 0);

//-----------------------------------------------------------------------------
// Vec3
//-----------------------------------------------------------------------------
template<typename T>
struct Vec3
{
    union
    {
        struct
        {
            T x; ///< X coordinate of the vector
            T y; ///< Y coordinate of the vector
            T z;
        };
        T coord[3];
    };

    const static Vec3 zero;
    const static Vec3 one;
    const static Vec3 up;
    const static Vec3 down;
    const static Vec3 left;
    const static Vec3 right;
    const static Vec3 forward;
    const static Vec3 back;

    //-----------------------------------------------------------------------------
    // Constructors
    //-----------------------------------------------------------------------------
    Vec3()
	    : x(0), y(0), z(0)
    {
    }

    explicit Vec3(T same)
	    : x(same), y(same), z(same)
    {
    }

    Vec3(const T& X, const T& Y, const T& Z)
	    : x(X), y(Y), z(Z)
    {
    }

    explicit Vec3(const T* ptr)
	    : x(ptr[0]),
	      y(ptr[1]),
	      z(ptr[2])
    {
    }

    template<typename U>
    explicit Vec3(const Vec2<U>& vec2)
            : x(static_cast<T>(vec2.x)),
              y(static_cast<T>(vec2.y)),
              z(static_cast<T>(0))
    {

    }

    template<typename U>
    explicit Vec3(const Vec3<U>& vector)
            : x(static_cast<T>(vector.x)),
              y(static_cast<T>(vector.y)),
              z(static_cast<T>(vector.z))
    {
    }

    template<typename U>
    explicit Vec3(const Vec4<U>& vec4)
            : x(static_cast<T>(vec4.x)),
              y(static_cast<T>(vec4.y)),
              z(static_cast<T>(vec4.z))
    {
    }
	
    /**
     * \brief Adding explicit constructor for vector-like type
     */
    template <class U>
    explicit Vec3(const U& u) :
	    x(u.x), y(u.y), z(u.z)
    {
    }

    //-----------------------------------------------------------------------------
    // Operators
    //-----------------------------------------------------------------------------
    Vec3<T> operator+(const Vec3<T>& rhs) const
    {
        return Vec3<T>(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vec3<T>& operator+=(const Vec3<T>& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        return *this;
    }

    Vec3<T> operator-(const Vec3<T>& rhs) const
    {
        return Vec3<T>(x - rhs.x, y - rhs.y, z - rhs.z);
    }

	Vec3<T> operator-() const
    {
	    return Vec3<T>(-x, -y , -z );
    }

    Vec3<T>& operator-=(const Vec3<T>& rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
        return *this;
    }

    Vec3<T> operator*(T rhs) const
    {
        return Vec3<T>(x * rhs, y * rhs, z * rhs);
    }

    Vec3<T> operator*(const Vec3<T>& rhs) const
    {
        return Vec3<T>(x * rhs.x, y * rhs.y, z * rhs.z);
    }

    Vec3<T>& operator*=(T rhs)
    {
        this->x *= rhs;
        this->y *= rhs;
        this->z *= rhs;
        return *this;
    }
	
    Vec3<T> operator/(const Vec3<T>& rhs) const
    {
        return Vec2<T>(x / rhs.x, y / rhs.y, z / rhs.z);
    }

    Vec3<T> operator/(T rhs) const
    {
        return (*this) * (1.0f / rhs);
    }

    Vec3<T>& operator/=(T rhs)
    {
        *this = *this / rhs;
        return *this;
    }

    bool operator==(const Vec3<T>& right) const
    {
        return x == right.x && y == right.y && z == right.z;
    }

    bool operator!=(const Vec3<T>& right) const
    {
        return !(*this == right);
    }

    bool operator>(const Vec3<T>& right) const
    {
        return x > right.x && y > right.y && z > right.z;
    }
	
    bool operator>=(const Vec3<T>& right) const
    {
        return x >= right.x && y >= right.y && z >= right.z;
    }

    bool operator<(const Vec3<T>& right) const
    {
        return x < right.x && y < right.y && z < right.z;
    }
	
    bool operator<=(const Vec3<T>& right) const
    {
        return x <= right.x && y <= right.y && z <= right.z;
    }

    const T& operator[](size_t p_axis) const
    {
        return coord[p_axis];
    }

    T& operator[](size_t p_axis)
    {

        return coord[p_axis];
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec3<T>& dt)
    {
        os << "Vec3(" << dt.x << ", " << dt.y << ", " << dt.z << ")";
        return os;
    }

    //-----------------------------------------------------------------------------
    // Formulas
    //-----------------------------------------------------------------------------
    /// \brief Calculates the dot product from two vectors.
    static T Dot(const Vec3<T> v1, const Vec3<T> v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    /// \brief Calculates the cross product from two Vec3.
    static Vec3<T> Cross(const Vec3<T> v1, const Vec3<T> v2)
    {
        return Vec3<T>(v1.y * v2.z - v1.z * v2.y,
                       v1.z * v2.x - v1.x * v2.z,
                       v1.x * v2.y - v1.y * v2.x);
    }

    /// \brief Calculates the square magnitude.
    T SquareMagnitude() const
    {
        return Dot(*this, *this);
    }

    /// \brief Calculates the magnitude.
    template<typename ReturnT = float>
    ReturnT Magnitude() const
    {
        return std::sqrt(SquareMagnitude());
    }

    /// \brief Calculates the normalized vector.
    Vec3<T> Normalized() const
    {
        return (*this) / (*this).Magnitude();
    }

    /// \brief Interpolate between two vectors.
    /// \param t the interpolation amount.
    static Vec3<T> Lerp(const Vec3<T>& v1, const Vec3<T>& v2, T t)
    {
        return v1 + (v2 - v1) * t;
    }

    /// \brief Reflect the inVec using the normal given (does not need to be normalized).
    /// \param inVec the vector to reflect.
    /// \param normal the normal vector of the line to reflect off.
    static Vec3<T> Reflect(const Vec3<T>& inVec, const Vec3<T>& normal)
    {
        Vec3<T> normalized = normal.Normalized();
        return inVec - normalized * 2 * Dot(inVec, normalized);
    }

    /// \brief Project v1 on v2 (doesn't need to be normalized).
    /// \param v1 the vector to project.
    /// \param v2 the vector to project on.
    static Vec3<T> Project(const Vec3<T>& v1, const Vec3<T>& v2)
    {
        const auto dot = Dot(v1, v2);
        const auto mag = v2.SquareMagnitude();
        return {(dot / mag) * v2.x,
                (dot / mag) * v2.y,
                (dot / mag) * v2.z};
    }

    static radian_t AngleBetween(const Vec3& v1, const Vec3& v2)
    {
        const float dot = Vec3<T>::Dot(v1, v2) / v1.Magnitude() / v2.Magnitude();
        const float det = v1.x * v2.y - v1.y * v2.x;
        const radian_t angle = Atan2(det, dot);
        return angle;
    }
};
//-----------------------------------------------------------------------------
// Vec3 Aliases
//-----------------------------------------------------------------------------
using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;
using Vec3u = Vec3<uint32_t>;
using Vec3u64 = Vec3<uint64_t>;
using Vec3u32 = Vec3<uint32_t>;
using Vec3u16 = Vec3<uint16_t>;
using Vec3u8 = Vec3<uint8_t>;
using EulerAngles = Vec3<degree_t>;
using RadianAngles = Vec3<radian_t>;

template<typename T>
inline Vec3<T> const Vec3<T>::zero = Vec3<T>(0, 0, 0);
template<typename T>
inline Vec3<T> const Vec3<T>::one = Vec3<T>(1, 1, 1);
template<typename T>
inline Vec3<T> const Vec3<T>::up = Vec3<T>(0, 1, 0);
template<typename T>
inline Vec3<T> const Vec3<T>::down = Vec3<T>(0, -1, 0);
template<typename T>
inline Vec3<T> const Vec3<T>::left = Vec3<T>(-1, 0, 0);
template<typename T>
inline Vec3<T> const Vec3<T>::right = Vec3<T>(1, 0, 0);
template<typename T>
inline Vec3<T> const Vec3<T>::forward = Vec3<T>(0, 0, 1);
template<typename T>
inline Vec3<T> const Vec3<T>::back = Vec3<T>(0, 0, -1);

//-----------------------------------------------------------------------------
// Vec4
//-----------------------------------------------------------------------------
template<typename T>
struct alignas(4 * sizeof(T)) Vec4
{
    union
    {
        struct
        {
            T x, y, z, w;
        };
        struct
        {
            T r, g, b, a;
        };
        T coord[4];
    };

    const static Vec4 zero;
    const static Vec4 one;

    //-----------------------------------------------------------------------------
    // Constructors
    //-----------------------------------------------------------------------------
    Vec4() noexcept
	    : x(0), y(0), z(0), w(0)
    {
    }

    explicit Vec4(T same)
	    : x(same), y(same), z(same), w(same)
    {
    }

    Vec4(const T& X, const T& Y, const T& Z, const T& W)
	    : x(X), y(Y), z(Z), w(W)
    {
    }

    explicit Vec4(const T* ptr)
	    : x(ptr[0]),
	      y(ptr[1]),
	      z(ptr[2]),
	      w(ptr[3])
    {
    }

    template<typename U>
    explicit Vec4(const Vec2<U>& vec2)
            : x(static_cast<T>(vec2.x)),
              y(static_cast<T>(vec2.y)),
              z(static_cast<T>(0)),
              w(static_cast<T>(0))
    {

    }

    template<typename U>
    explicit Vec4(const Vec3<U>& vector)
            : x(static_cast<T>(vector.x)),
              y(static_cast<T>(vector.y)),
              z(static_cast<T>(vector.z)),
              w(static_cast<T>(0))
    {
    }

    template<typename U>
    explicit Vec4(const Vec4<U>& vec4)
            : x(static_cast<T>(vec4.x)),
              y(static_cast<T>(vec4.y)),
              z(static_cast<T>(vec4.z)),
              w(static_cast<T>(vec4.w))
    {
    }

    //-----------------------------------------------------------------------------
    // Operators
    //-----------------------------------------------------------------------------
    Vec4<T> operator+(const Vec4<T>& rhs) const
    {
        return Vec4<T>(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
    }

    Vec4<T>& operator+=(const Vec4<T>& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        this->w += rhs.w;
        return *this;
    }

    Vec4<T> operator-(const Vec4<T>& rhs) const
    {
        return Vec4<T>(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
    }

    Vec4<T>& operator-=(const Vec4<T>& rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
        this->w -= rhs.w;
        return *this;
    }

    Vec4<T> operator*(T rhs) const
    {
        return Vec4<T>(x * rhs, y * rhs, z * rhs, w * rhs);
    }

    Vec4<T> operator*(const Vec4<T>& rhs) const
    {
        return Vec4<T>(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
    }

    Vec4<T>& operator*=(T rhs)
    {
        this->x *= rhs;
        this->y *= rhs;
        this->z *= rhs;
        this->w *= rhs;
        return *this;
    }

    Vec4<T> operator/(T rhs) const
    {
        return (*this) * (1.0f / rhs);
    }

    Vec4<T>& operator/=(T rhs)
    {
        *this = *this / rhs;
        return *this;
    }

    bool operator==(const Vec4<T>& right) const
    {
        return x == right.x && y == right.y && z == right.z && w == right.w;
    }

    bool operator!=(const Vec4<T>& right) const
    {
        return !(*this == right);
    }

    bool operator>(const Vec4<T>& right) const
    {
        return x > right.x && y > right.y && z > right.z && w > right.w;
    }
	
    bool operator>=(const Vec4<T>& right) const
    {
        return x >= right.x && y >= right.y && z >= right.z && w >= right.w;
    }

    bool operator<(const Vec4<T>& right) const
    {
        return x < right.x && y < right.y && z < right.z && w < right.w;
    }
	
    bool operator<=(const Vec4<T>& right) const
    {
        return x <= right.x && y <= right.y && z <= right.z && w <= right.w;
    }

    const T& operator[](size_t p_axis) const
    {

        return coord[p_axis];
    }

    T& operator[](size_t p_axis)
    {
        return coord[p_axis];
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec4<T>& dt)
    {
        os << "Vec4(" << dt.x << "," << dt.y << "," << dt.z << "," << dt.w << ")";
        return os;
    }

    //-----------------------------------------------------------------------------
    // Formulas
    //-----------------------------------------------------------------------------
    /// \brief Calculates the dot product from two vectors.
    static T Dot(const Vec4<T> v1, const Vec4<T> v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
    }

    /// \brief Calculates the 3D dot product from two vectors.
    static T Dot3(const Vec4<T> v1, const Vec4<T> v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    /// \brief Calculates the square magnitude.
    T SquareMagnitude() const
    {
        return Dot(*this, *this);
    }

    /// \brief Calculates the magnitude.
    template<typename ReturnT = float>
    ReturnT Magnitude() const
    {
        return std::sqrt(SquareMagnitude());
    }

    /// \brief Calculates the normalized vector.
    Vec4<T> Normalized() const
    {
        return (*this) / (*this).Magnitude();
    }

    /// \brief Interpolate between two vectors.
    /// \param t the interpolation amount.
    static Vec4<T> Lerp(const Vec4<T>& v1, const Vec4<T>& v2, T t)
    {
        return v1 + (v2 - v1) * t;
    }

    /// \brief Project v1 on v2 (does not need to be normalized).
    /// \param v1 the vector to project.
    /// \param v2 the vector to project on.
    static Vec4<T> Project(const Vec4<T>& v1, const Vec4<T>& v2)
    {
        const auto dot = Dot(v1, v2);
        const auto mag = v2.SquareMagnitude();
        return {(dot / mag) * v2.x,
                (dot / mag) * v2.y,
                (dot / mag) * v2.z,
                (dot / mag) * v2.w};
    }
};
//-----------------------------------------------------------------------------
// Vec4 Aliases
//-----------------------------------------------------------------------------
using Vec4f = Vec4<float>;
using Vec4i = Vec4<int>;
using Vec4u = Vec4<uint32_t>;
using Vec4u64 = Vec4<uint64_t>;
using Vec4u32 = Vec4<uint32_t>;
using Vec4u16 = Vec4<uint16_t>;
using Vec4u8 = Vec4<uint8_t>;

template<typename T>
inline Vec4<T> const Vec4<T>::zero = Vec4<T>(0.0f);
template<typename T>
inline Vec4<T> const Vec4<T>::one = Vec4<T>(1.0f);
}
