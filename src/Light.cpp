#include "Light.h"
#include "GL/glfw.h"
#include <cmath>

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
	
	glColor4f (0, 0, 0, Intensity);
	glVertex3f (X, Y, 1.0);
	
	const int NUM_OF_SUBDIVISIONS = 32;
	const int STEP = (M_PI/NUM_OF_SUBDIVISIONS);

	glColor4f (0, 0, 0, 0);
	
	for (int i=0; i<NUM_OF_SUBDIVISIONS; i++)
	{
		float angle = i * STEP;
		float x = Radius * std::cos (angle) + X;
		float y = Radius * std::sin (angle) + Y;
		glVertex3f (x, y, 1);
	}
	
	glVertex3f (X + Radius, 0, 1);
	glEnd();
}
