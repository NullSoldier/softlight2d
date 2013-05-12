#include "Geom.h"
#include "Light.h"
#include "GL/glfw.h"
#include <string>
#include <iostream>
#include <tuple>

void draw();
void clearAlpha();
void buildWorld();
void drawGeometry();
void drawLight (Light light);
void drawShadows (Light light);
bool isEdgeFront (std::tuple<float,float,float> currPoint, std::tuple<float,float,float> prevPoint, Light light);
float dotProduct (float x1, float y1, float x2, float y2);

std::vector<Geom> geom;
std::vector<Light> lights;
int lastStartEdge = -1;
int lastEndEdge = -1;

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
		drawShadows(*iter);
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
	glColor4f (0, 0, 0, 0);
	glVertex3f (-1, -1, 1);
	glVertex3f (1, -1, 1);
	glVertex3f (1, 1, 1);
	glVertex3f (-1, 1, 1);
	glEnd();	
}

void drawGeometry()
{
	glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	glEnable (GL_BLEND);
	glBlendFunc (GL_DST_ALPHA, GL_ONE);

	for (auto iter = geom.begin(); iter != geom.end(); iter++) {
		//std::cout << "Drawing geometry with size " << iter->VertexCount() << std::endl;
		glBegin (GL_POLYGON);

		glColor4f (iter->r, iter->g, iter->b, iter->a);
		for (int i=0; i<iter->VertexCount(); i++) {
			auto vert = iter->GetVertex (i);
			
			float r = iter->r;
			float g = iter->g;
			float b = iter->b;

			if (i == lastStartEdge && iter->castShadows) {
				r = 1;
				g = 0;
				b = 0;
			}
			if (i == lastEndEdge && iter->castShadows) {
				r = 0;
				g = 1;
				b = 0;
			}
			
			glColor4f (r,g,b,1);
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

void drawShadows (Light light)
{
	for (auto iter=geom.begin(); iter != geom.end(); iter++)
	{
		if (!iter->castShadows)
			continue;

		int edgeCount = iter->VertexCount();
		std::vector<bool> edges;
		edges.resize (edgeCount); // this is stupid

		int startIndex=-1;
		int endIndex=-1;

		// Calculate which edges are front/back
		for (int i=0; i < edgeCount; i++) {
			//if (i != 1 && i != 0) continue;

			int currIndex = (i == edgeCount-1) ? 0 : i+1;
			auto current = iter->GetVertex (currIndex);
			auto last = iter->GetVertex (i);
			
			bool isFront = isEdgeFront (current, last, light);
			//std::cout << "Edge " << i << ": " << isFront << " used " << i << "->" << currIndex << "\n";
			edges[i] = isFront;
		}
		
		//return;

		// Calculate start/end points
		for (int i=1; i<= edgeCount; i++) {
			int currIndex = i == edgeCount ? 0 : i;
			bool current = edges[currIndex];
			bool last = edges[i-1];
			
			//std::cout << "Current " << currIndex << ": "  << current << "\n";
			//std::cout << "Last " << (i-1) << ": " << last << "\n";

			if (!current && last) {
				lastStartEdge = startIndex = currIndex;
				std::cout << "Found start " << lastStartEdge << ": " << current <<  "\n";
			}
			else if (current && !last) {
				lastEndEdge = endIndex = currIndex;
				std::cout << "Found end " << lastEndEdge  << "\n";
			}
		}
		
		//return;
		
		glEnable (GL_DEPTH_TEST);
		glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
		//glBegin (GL_TRIANGLES);
		glBegin (GL_TRIANGLE_STRIP);
		glColor4f (0, 0, 0, 0);	
		
		const float SCALE = 100;
		int i = startIndex;
		while (true) {
			//std::cout << "Drawing index " << i << "\n";
			auto vert = iter->GetVertex (i);
			float x = std::get<0> (vert);
			float y = std::get<1> (vert);
			float z = std::get<2> (vert);
			float dx = x - light.X;
			float dy = y - light.Y;
			//std::cout << "dx=" << dx << ", dy=" << dy << "\n";
			
			std::cout << "i=" << i << ", x=" << x << ", y=" << y << "\n";
			glVertex3f (x, y, z);
			std::cout << "i=" << i << ", fx=" << (x + (dx*SCALE)) << ", fy=" << (y + (dx*SCALE)) << "\n";
			glVertex3f (x + (dx * SCALE), y + (dx * SCALE), z);
			
			if (i == endIndex)
				break;
			if (++i == edgeCount)
				i = 0;
		}
		//glVertex3f (0.4, 0.3, 0.8);	
		glEnd();
	}
}

bool isEdgeFront (std::tuple<float,float,float> currPoint, std::tuple<float,float,float> prevPoint, Light light)
{
	float x1 = std::get<0> (prevPoint);
	float y1 = std::get<1> (prevPoint);
	float x2 = std::get<0> (currPoint);
	float y2 = std::get<1> (currPoint);

	//std::cout << "IsEdgeFront: (" << x2 << ", " << y2 << ") - (" << x1 << ", " << y1 << ")\n";
	float normalX = y1 - y2;
	float normalY = x2 - x1;

	float toX = x2 - light.X;
	float toY = y2 - light.Y;

	return dotProduct (normalX, normalY, toX, toY) > 0;
}

float dotProduct (float x1, float y1, float x2, float y2)
{
	//std::cout << "X * Y = (" << x1 << "*" << x2 << ") + ("<< y1 << "*" << y2 << ")\n";
	float result = (x1*x2) + (y1*y2);
	//std::cout << "dot result: " << result << "\n";
	return result;
}

void buildWorld()
{
	Geom box (0, 0.5, 1, 1);
	box.AddVertex (0.3, 0.3, 0.8);
	box.AddVertex (0.8, 0.3, 0.8);
	box.AddVertex (0.8, 0.8, 0.8);
	box.AddVertex (0.3, 0.8, 0.8);
	box.castShadows = true;
	//geom.push_back (box);

	Geom box2 (0.5, 1, 1, 1);
	box2.AddVertex (-0.3, -0.8, 0.8);
	box2.AddVertex (-0.1, -0.5, 0.8);
	box2.AddVertex (-0.5, -0.5, 0.8);
	box2.castShadows = true;
	geom.push_back (box2);
		
	Geom floor (1, 1, 1, 1);
	floor.AddVertex (-1, -1, 1);
	floor.AddVertex (1, -1, 1);
	floor.AddVertex (1, 1, 1);
	floor.AddVertex (-1, 1, 1);
	floor.castShadows = false;
	geom.push_back (floor);

	Light light (0.1, 0.1, 0, 2, 0.5);
	lights.push_back (light);
}
