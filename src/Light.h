#ifndef LIGHT_H
#define LIGHT_H
class Light
{
public:
	float X;
	float Y;
	float Z;
    float PhysicalSize;
	float Range;
	float Intensity;
	
	Light (float x, float y, float z, float range, float intensity, float physicalSize);
	void DrawAlpha() const;
};
#endif
