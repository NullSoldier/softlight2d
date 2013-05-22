#include "Geom.h"
#include "GL/glfw.h"
#include "Light.h"
#include "Vector.h"

#include <string>
#include <iostream>
#include <tuple>

void buildWorld();
void clearAlpha();
bool isEdgeFacingLight (Vector2 curr, Vector2 prev, Light light);

void draw();
void drawGeometry();
void drawLight (Light light);
void drawShadows (Light light);

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

	for (auto iter = lights.begin(); iter != lights.end(); iter++) {
		clearAlpha();
		drawLight (*iter);
		drawShadows(*iter);
		drawGeometry();
	}	
	
	glfwSwapBuffers();
}

void drawLight (Light light)
{
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
        iter->Draw(lastStartEdge, lastEndEdge);
	}
}

void drawShadows (Light light)
{
	for (auto iter=geom.begin(); iter != geom.end(); iter++) {
		if (!iter->castShadows)
			continue;

        int startIndex=-1;
        int endIndex=-1;
        int edgeCount = iter->verts.size();
        std::vector<bool> edges (edgeCount);

		// Calculate which edges are front/back
		for (int i=0; i < edgeCount; i++) {
			Vector3 last = iter->verts[i];
			Vector3 current = iter->verts[(i+1) % edgeCount];
			std::cout << "Current i=" << ((i+1)%edgeCount) << ", x=" << current.X << ", y=" << current.Y << "\n";
			

			edges[i] = isEdgeFacingLight (current, last, light);
			std::cout << "Edge " << i << ": " << (edges[i] ? "Forward" : "Backward") << "\n";
		}

		// Calculate start/end points
		for (int i=1; i<= edgeCount; i++) {
			int currIndex = i == edgeCount ? 0 : i;
			bool current = edges[currIndex];
			bool last = edges[i-1];

			if (!current && last) {
				startIndex = lastStartEdge = currIndex;
				std::cout << "Found start " << lastStartEdge << ": " << current <<  "\n";
			}
			else if (current && !last) {
				endIndex = lastEndEdge = currIndex;
				std::cout << "Found end " << lastEndEdge  << "\n";
			}
            
            //std::cout << "Current " << currIndex << ": "  << current << "\n";
            //std::cout << "Last " << (i-1) << ": " << last << "\n";
		}
		
		glEnable (GL_DEPTH_TEST);
		glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
		glBegin (GL_TRIANGLE_STRIP);
		glColor4f (0, 0, 0, 0);
        
        const float SCALE = 100;
        int i = startIndex;
        
        while (true) {
            Vector3 vert = iter->verts[i];
            float dx = vert.X - light.X;
            float dy = vert.Y - light.Y;
            
            //std::cout << "Drawing index " << i << "\n";
            //std::cout << "dx=" << dx << ", dy=" << dy << "\n";
            std::cout << "i=" << i << ", x=" << vert.X << ", y=" << vert.Y << "\n";
            std::cout << "i=" << i << ", fx=" << (vert.X + (dx*SCALE)) << ", fy=" << (vert.Y + (dx*SCALE)) << "\n";
            
            glVertex3f (vert.X, vert.Y, vert.Z);
            glVertex3f (vert.X + (dx * SCALE), vert.Y + (dx * SCALE), vert.Z);
            
            if (i == endIndex)
                break;
            if (++i == edgeCount)
                i = 0;
        }
        glEnd();
    }
}

bool isEdgeFacingLight (Vector2 curr, Vector2 prev, Light light)
{
	Vector2 normal (
		curr.Y - prev.Y,
		-(curr.X - prev.X)
	);
	Vector2 to (
		curr.X - light.X,
		curr.Y - light.Y
	);

	std::cout << "Normal x=" << normal.X << ", y=" << normal.Y << "\n";
	std::cout << "To x=" << to.X << ", y=" << to.Y << "\n";
	std::cout << "Light x=" << light.X << ", y=" << light.Y << "\n";
	std::cout << "Prev x=" << prev.X << ", y=" << prev.Y << "\n";
	std::cout << "Curr x=" << curr.X << ", y=" << curr.Y << "\n";
	std::cout << "Results\n";
	std::cout << Vector2::Dot (normal, to) << "\n";
	
	return Vector2::Dot (normal, to) < 0;
}

void buildWorld()
{
	Geom box (0, 0.5, 1, 1);
	box.AddVertex (0.3, 0.3, 0.8);
	box.AddVertex (0.8, 0.3, 0.8);
	box.AddVertex (0.8, 0.8, 0.8);
	box.AddVertex (0.3, 0.8, 0.8);
	box.castShadows = true;
	geom.push_back (box);

	Geom box2 (0.5, 1, 1, 1);
	box2.AddVertex (-0.3, -0.8, 0.8);
	box2.AddVertex (0, -0.8, 0.8);
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

	Light light (0, 0, 0, 2, 0.5);
	lights.push_back (light);
}
