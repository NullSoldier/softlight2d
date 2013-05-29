#ifndef VECTOR_H
#define VECTOR_H
#include <string>
#include <iosfwd>

struct Vector4
{
    Vector4 (float x, float y, float z, float w);
    float X, Y, Z, W;
};

struct Vector3
{
    Vector3 (float x, float y, float z);
    float X, Y, Z;
    
    static float Dot (const Vector3 &, const Vector3 &);
};

struct Vector2
{
    Vector2 (float x, float y);
    Vector2 (const Vector3 &);
    float X, Y;
    float GetMagnitude() const;
    
    void Normalize();
    void Reverse();
    static float Dot (const Vector2&, const Vector2&);
    static float AngleBetween (const Vector2&, const Vector2&);
};

std::ostream& operator<< (std::ostream& s, const Vector2 &v);
Vector2 operator+ (const Vector2&, const Vector2&);
Vector2 operator* (const Vector2&, const Vector2&);
Vector2 operator* (const Vector2&, const float);
#endif
