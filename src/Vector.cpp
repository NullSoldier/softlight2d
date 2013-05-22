#include "Vector.h"

Vector2::Vector2 (float x, float y) :
    X (x),
    Y (y)
{
}

Vector2::Vector2 (Vector3 v) :
    X (v.X),
    Y (v.Y)
{
}

float Vector2::Dot (const Vector2 a, const Vector2 b)
{
    return (a.X*b.X) + (a.Y*b.Y);
}

Vector3::Vector3 (float x, float y, float z) :
    X (x),
    Y (y),
    Z (z)
{
}

Vector4::Vector4 (float x, float y, float z, float w) :
    X (x),
    Y (y),
    Z (z),
    W (w)
{
}
