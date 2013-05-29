#include "Vector.h"
#include <cmath>
#include <string>
#include <sstream>

Vector2::Vector2 (float x, float y) :
    X (x),
    Y (y)
{
}

Vector2::Vector2 (const Vector3 &v) :
    X (v.X),
    Y (v.Y)
{
}

float Vector2::GetMagnitude() const
{
    return sqrt (X*X + Y*Y);
}

void Vector2::Normalize()
{
    float length = GetMagnitude();
    X /= length;
    Y /= length;
}

void Vector2::Reverse()
{
    *this = *this * -1.0f;
}

float Vector2::Dot (const Vector2 &a, const Vector2 &b)
{
    return (a.X*b.X) + (a.Y*b.Y);
}

float Vector2::AngleBetween (const Vector2& v1, const Vector2& v2)
{
    float dot = Dot (v1, v2);
    float mags = v1.GetMagnitude() * v2.GetMagnitude();
    float cos = dot / mags;
    return acos (cos);
}

Vector2 operator+ (const Vector2& v1, const Vector2& v2)
{
    return Vector2 (v1.X + v2.X, v1.Y + v2.Y);
}

Vector2 operator* (const Vector2& v1, const Vector2& v2)
{
    return Vector2 (v1.X * v2.X, v1.Y * v2.Y);
}

Vector2 operator* (const Vector2& v, const float val)
{
    return Vector2 (v.X * val, v.Y * val);
}

std::ostream& operator<< (std::ostream& s, const Vector2 &v)
{
    return s << "{x=" << v.X << ", y=" << v.Y << "}";
}

Vector3::Vector3 (float x, float y, float z) :
    X (x),
    Y (y),
    Z (z)
{
}

float Vector3::Dot (const Vector3 &a, const Vector3 &b)
{
    return (a.X*b.X) + (a.Y*b.Y) + (a.Z*b.Z);
}

Vector4::Vector4 (float x, float y, float z, float w) :
    X (x),
    Y (y),
    Z (z),
    W (w)
{
}
