#ifndef LIGHT_H
#define LIGHT_H
class Light
{
public:
	float X;
	float Y;
	float Z;
	float Radius;
	float Intensity;
	
	Light (float x, float y, float z, float r, float intensity);
	void DrawAlpha() const;
};
#endif
