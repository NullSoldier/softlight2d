#ifndef LIGHT_H
#define LIGHT_H
class Light
{
public:
	float X;
	float Y;
	float Radius;
	float Intensity;
	
	Light (float x, float y, float radius, float intensity);
	void DrawAlpha() const;
};
#endif
