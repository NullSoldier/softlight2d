#include "Geom.h"
#include "Light.h"
#include "GL/glfw.h"
#include <string>
#include <iostream>

void render();
void clearFrameBufferAlpha();
void buildWorld();
void drawGeometry();

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
		render();
	}

	return 0;
}

void render() 
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
	
	for (auto iter = lights.begin(); iter != lights.end(); iter++)
	{
		// fill z buffer with geom

		//clearFrameBufferAlpha();	
		//iter->DrawAlpha();
		drawGeometry();
	}	
	
	glfwSwapBuffers();
}

void clearFrameBufferAlpha()
{
}

void drawGeometry()
{
	glEnable (GL_BLEND);
	glBlendFunc (GL_DST_ALPHA, GL_ONE);

	std::cout << "Drawing geometry" << std::endl;
	for (auto iter = geom.begin(); iter != geom.end(); iter++) {
		std::cout << "Drawing geometry with size " << iter->VertexCount() << std::endl;
		glBegin (GL_POLYGON);
		for (int i=0; i<iter->VertexCount(); i++) {
			auto vert = iter->GetVertex (i);
			glVertex3f (
				std::get<0> (vert),
				std::get<1> (vert),
				std::get<2> (vert)
			);
		}
		glEnd();
	}
}

void buildWorld()
{
	auto box = Geom();
	box.AddVertex (20, 20, 0);
	box.AddVertex (40, 20, 0);
	box.AddVertex (40, 40, 0);
	box.AddVertex (20, 40, 0);
	geom.push_back (box);

	auto light = Light (0, 0, 100, 0.5);
	lights.push_back (light);

}
