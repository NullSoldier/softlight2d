#include "Light.h"
#include "GL/glfw.h"
#include <cmath>
#include <iostream>

Light::Light (float x, float y, float z, float range, float intensity, float physicalSize) :
	X (x),
	Y (y),
	Z (z),
	Range (range),
	Intensity (intensity),
    PhysicalSize (physicalSize)
{
}

void Light::DrawAlpha() const
{
    glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	glDisable (GL_BLEND);
	glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
    
	glBegin (GL_TRIANGLE_FAN);
	glColor4f (0.0f, 0.0f, 0.0f, Intensity);
	glVertex3f (X, Y, Z);

	const int NUM_OF_DIVISIONS = 32;
	const float STEP = (M_PI*2/NUM_OF_DIVISIONS);

	glColor4f (0, 0, 0, 0);
	
	for (int i=0; i<NUM_OF_DIVISIONS; i++)
	{
		float angle = i * STEP;
		float x = X + (Range * std::cos (angle));
		float y = Y + (Range * std::sin (angle));
		glVertex3f (x, y, this->Z);
	}
	
	glVertex3f (X + Range, Y, 1);
	glEnd();
}
