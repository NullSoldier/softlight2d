#include "Light.h"
#include "GL/glfw.h"
#include <cmath>
#include <iostream>

Light::Light (float x, float y, float z, float r, float intensity) :
	X (x),
	Y (y),
	Z (z),
	Radius (r),
	Intensity (intensity)
{
}

void Light::DrawAlpha() const
{
	glBegin (GL_TRIANGLE_FAN);
	
	glColor4f (0.0f, 0.0f, 0.0f, Intensity);
	glVertex3f (X, Y, Z);

	const int NUM_OF_DIVISIONS = 32;
	const float STEP = (M_PI*2/NUM_OF_DIVISIONS);

	glColor4f (0, 0, 0, 0);
	
	for (int i=0; i<NUM_OF_DIVISIONS; i++)
	{
		float angle = i * STEP;
		float x = Radius * std::cos (angle) + X;
		float y = Radius * std::sin (angle) + Y;
		glVertex3f (x, y, this->Z);
		//std::cout << "Angle: " << angle << ", Vert[" << i << "] " << x << ", " << y << "\n";	
	}
	
	glVertex3f (X + Radius, Y, 1);
	glEnd();
}
