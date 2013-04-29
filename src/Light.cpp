#include "Light.h"
#include "GL/glfw.h"
#include <cmath>
#include <iostream>

Light::Light (float x, float y, float radius, float intensity) :
	X (x),
	Y (y),
	Radius (radius),
	Intensity (intensity)
{
}

void Light::DrawAlpha() const
{
	glBegin (GL_TRIANGLE_FAN);
	
	glColor4f (0.0f, 0.0f, 0.0f, Intensity);
	glVertex3f (X, Y, 1.0f);

	const int NUM_OF_DIVISIONS = 32;
	const float STEP = (M_PI*2/NUM_OF_DIVISIONS);

	glColor4f (0, 0, 0, 0);
	
	for (int i=0; i<NUM_OF_DIVISIONS; i++)
	{
		float angle = i * STEP;
		float x = Radius * std::cos (angle) + X;
		float y = Radius * std::sin (angle) + Y;
		glVertex3f (x, y, 1.0f);
		//std::cout << "Angle: " << angle << ", Vert[" << i << "] " << x << ", " << y << "\n";	
	}
	
	glVertex3f (X + Radius, 0, 1);
	glEnd();
}
