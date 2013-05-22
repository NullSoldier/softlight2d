#ifndef VECTOR_H
#define VECTOR_H

struct Vector3;

struct Vector2
{
    Vector2 (float x, float y);
    Vector2 (Vector3);
    float X, Y;
    
    static float Dot (Vector2, Vector2);
};

struct Vector3
{
    Vector3 (float x, float y, float z);
    float X, Y, Z;
    
    static float Dot (Vector3, Vector3);
};

struct Vector4
{
    Vector4 (float x, float y, float z, float w);
    float X, Y, Z, W;
};
#endif