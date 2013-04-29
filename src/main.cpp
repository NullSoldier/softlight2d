#include "Geom.h"
#include "Light.h"
#include "GL/glfw.h"
#include <string>
#include <iostream>

void draw();
void clearAlpha();
void buildWorld();
void drawGeometry();
void drawLight (Light light);

std::vector<Geom> geom;
std::vector<Light> lights;

int main (int argc, const char* argv[])
{
	if (!glfwInit()) {
		std::cout << "Failed to init glfw";
		return -1;
	}

	if (!glfwOpenWindow (640, 480, 8, 8, 8, 0, 24, 0, GLFW_WINDOW)) {
		std::cout << "Couldn't create glfw window";
		return -1;
	}

	std::string title = "Some Window Title";
	glfwSetWindowTitle (title.c_str());
	
	buildWorld();
	while (glfwGetWindowParam (GLFW_OPENED)) {
		draw();
	}
	return 0;
}

void draw() 
{
	glDepthMask (GL_TRUE);
	glClearDepth (1);
	glClearColor (0, 0, 0, 0);
	glClear (GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode (GL_TEXTURE);
	glLoadIdentity();
	
	glDisable (GL_CULL_FACE);

	// Fill the depth buffer	
	glDepthMask (GL_TRUE);
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	drawGeometry();
	glDepthMask (GL_FALSE);

	for (auto iter = lights.begin(); iter != lights.end(); iter++)
	{
		clearAlpha();
		drawLight (*iter);
		glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		drawGeometry();
	}	
	
	glfwSwapBuffers();
}

void drawLight (Light light)
{
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	glDisable (GL_BLEND);
	glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
	light.DrawAlpha();
}

void clearAlpha()
{
	glDisable (GL_BLEND);
	glDisable (GL_DEPTH_TEST);
	glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
	glBegin (GL_QUADS);
	glColor4f (0, 0, 0, 1);
	glVertex3f (-1, -1, 1);
	glVertex3f (1, -1, 1);
	glVertex3f (1, 1, 1);
	glVertex3f (-1, 1, 1);
	glEnd();	
}

void drawGeometry()
{
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	glEnable (GL_BLEND);
	glBlendFunc (GL_DST_ALPHA, GL_ONE);

	for (auto iter = geom.begin(); iter != geom.end(); iter++) {
		std::cout << "Drawing geometry with size " << iter->VertexCount() << std::endl;
		glBegin (GL_POLYGON);
		glColor4f (iter->r, iter->g, iter->b, iter->a);
		for (int i=0; i<iter->VertexCount(); i++) {
			auto vert = iter->GetVertex (i);
			glVertex3f (
				std::get<0> (vert),
				std::get<1> (vert),
				std::get<2> (vert)
			);
			//std::cout <<  std::get<0> (vert) << ' ' << std::get<1> (vert) << ' ' <<  std::get<2> (vert) << '\n';
		}
		glEnd();
	}
}

void buildWorld()
{
	Geom box (0, 0.5, 1, 1);
	box.AddVertex (0.3, 0.3, 0.8);
	box.AddVertex (0.7, 0.3, 0.8);
	box.AddVertex (0.7, 0.7, 0.8);
	box.AddVertex (0.3, 0.7, 0.8);
	geom.push_back (box);
		
	Geom floor (1, 1, 1, 1);
	floor.AddVertex (-1, -1, 1);
	floor.AddVertex (1, -1, 1);
	floor.AddVertex (1, 1, 1);
	floor.AddVertex (-1, 1, 1);
	geom.push_back (floor);

	Light light (0, 0, 0.5, 0.5);
	lights.push_back (light);
}
