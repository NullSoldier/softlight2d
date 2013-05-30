#include "Geom.h"
#include "GL/glfw.h"
#include "Light.h"
#include "ShadowFin.h"
#include "SOIL.h"
#include "Vector.h"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>
#include <tuple>

const std::string WINDOW_TITLE = "Some Window Title";

int init();
void update (float dt);
void buildWorld();
void clearAlpha();
bool isEdgeFacingLight (Vector2 curr, Vector2 prev, Light light);
void keyStatusChanged (int key, int status);

std::unique_ptr<ShadowFin> findShadowFin (const Light& light, const Geom& geom, int edgeIndex);
std::vector<Vector2> genFinTriangle (const ShadowFin& fin, const float scale);

void draw();
void drawGeometry();
void drawShadows (const Light& light);

std::vector<Geom> geom;
std::vector<std::unique_ptr<Light>> lights;
int lastStartEdge = -1;
int lastEndEdge = -1;
int shadowTexID = -1;

float camx=0.61, camy=0.11;
Light *plight;

int main (int argc, const char* argv[])
{
    if (init() != 0)
        return;
   
   shadowTexID = SOIL_load_OGL_texture (
	   "texture.png",
	   SOIL_LOAD_AUTO,
	   SOIL_CREATE_NEW_ID,
	   SOIL_FLAG_MIPMAPS
	); 
	buildWorld();
    
	while (glfwGetWindowParam (GLFW_OPENED)) {
        update (0.01);
		draw();
	}
    return 0;
}

int init()
{
    if (!glfwInit()) {
		std::cout << "Failed to init glfw";
		return -1;
	}
	if (!glfwOpenWindow (640, 480, 8, 8, 8, 0, 24, 0, GLFW_WINDOW)) {
		std::cout << "Couldn't create glfw window";
		return -1;
	}
    
    glfwSetWindowTitle (WINDOW_TITLE.c_str());
    glfwSetKeyCallback (keyStatusChanged);
    glfwEnable (GLFW_STICKY_KEYS);
    return 0;
}

void keyStatusChanged (int keyID, int status)
{
    std::cout << "Key " << keyID << ": " << status << "\n";
}

void update (float dt)
{
    if (glfwGetKey(GLFW_KEY_UP))
        camy += 1 * dt;
    else if (glfwGetKey (GLFW_KEY_DOWN))
        camy -= 1 * dt;
    if (glfwGetKey (GLFW_KEY_RIGHT))
        camx += 1 * dt;
    else if (glfwGetKey(GLFW_KEY_LEFT))
        camx -= 1 * dt;
    
    plight->X = camx;
    plight->Y = camy;
    
    //std::cout << "CAM x=" << camx << ", y=" << camy << "\n";
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
    glOrtho (-1.0, 1.0, -1.0, 1.0, 0.0, -1.0);
    
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
    glTranslated(-camx, -camy, 0);
    
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

	for (const auto &light : lights) {
		clearAlpha();
        light->DrawAlpha();
		drawShadows(*light);
		drawGeometry();
	}
	
	glfwSwapBuffers();
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
    glDisable (GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);
	glEnable (GL_BLEND);
	glBlendFunc (GL_DST_ALPHA, GL_ONE);
    glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	for (const Geom &g : geom) {
        g.Draw (lastStartEdge, lastEndEdge);
	}
}

void drawShadows (const Light& light)
{
	for (const Geom &g : geom) {
		if (!g.castShadows)
			continue;

        int startIndex=-1;
        int endIndex=-1;
        int edgeCount = g.verts.size();
        bool edges[edgeCount];

		// Calculate which edges are front/back
		for (int i=0; i < edgeCount; i++) {
			const Vector3 &last = g.verts[i];
			const Vector3 &current = g.verts[(i+1) % edgeCount];
			//std::cout << "Current i=" << ((i+1)%edgeCount) << ", x=" << current.X << ", y=" << current.Y << "\n";

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

        auto finRight = findShadowFin (light, g, startIndex);
        auto finLeft = findShadowFin (light, g, endIndex);

        // #SHADOW FIN DRAWING MODE
        glEnable (GL_TEXTURE_2D);
        glEnable (GL_DEPTH_TEST);
        glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
        
        const int depth = g.verts[startIndex].Z;
        
        auto triRight = genFinTriangle (*finRight, 100.0);
        glBindTexture (GL_TEXTURE_2D, shadowTexID);
        glBegin (GL_TRIANGLES);
        glColor4f (1, 1, 1, 1);
        glTexCoord2f (0, 0);
        glVertex3f (triRight[0].X, triRight[0].Y, depth);
        glTexCoord2f (0, 1);
        glVertex3f (triRight[1].X, triRight[1].Y, depth);
        glTexCoord2f (1, 1);
        glVertex3f (triRight[2].X, triRight[2].Y, depth);
        glEnd();
        
        auto triLeft = genFinTriangle (*finLeft, 100.0);
        glBindTexture (GL_TEXTURE_2D, shadowTexID);
        glBegin (GL_TRIANGLES);
        glColor4f (1, 1, 1, 1);
        glTexCoord2f (0, 0);
        glVertex3f (triLeft[0].X, triLeft[0].Y, depth);
        glTexCoord2f (0, 1);
        glVertex3f (triLeft[1].X, triLeft[1].Y, depth);
        glTexCoord2f (1, 1);
        glVertex3f (triLeft[2].X, triLeft[2].Y, depth);
        glEnd();
        
        //continue;
        
        // #SHADOW GEOM DRAWING MODE
        glDisable (GL_TEXTURE_2D);
		glEnable (GL_DEPTH_TEST);
		glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
		
        glBegin (GL_TRIANGLE_STRIP);
		glColor4f (0, 0, 0, 0);
        
        const float SCALE = 100;
        int i = startIndex;
        while (true) {
            Vector3 vert = g.verts[i];
            glVertex3f (vert.X, vert.Y, vert.Z);

            if (i == startIndex) {
                glVertex3f (
                    vert.X + (finRight->Center.X * SCALE),
                    vert.Y + (finRight->Center.Y * SCALE),
                    vert.Z
                );
            } else if (i == endIndex) {
                glVertex3f (
                    vert.X + (finLeft->Center.X * SCALE),
                    vert.Y + (finLeft->Center.Y * SCALE),
                    vert.Z
                );
            } else {
                Vector2 v (vert.X - light.X, vert.Y - light.Y);
                v.Normalize();
                glVertex3f (
                    vert.X + (v.X * SCALE),
                    vert.Y + (v.Y * SCALE),
                    vert.Z
                );
            }
            
            //std::cout << "Drawing index " << i << "\n";
            //std::cout << "dx=" << dx << ", dy=" << dy << "\n";
            //std::cout << "i=" << i << ", x=" << vert.X << ", y=" << vert.Y << "\n";
            //std::cout << "i=" << i << ", fx=" << (vert.X + (dx*SCALE)) << ", fy=" << (vert.Y + (dy*SCALE)) << "\n";
            
            if (i == endIndex)
                break;
            if (++i == edgeCount)
                i = 0;
        }
        glEnd();
    }
}

std::unique_ptr<ShadowFin> findShadowFin (const Light& light, const Geom& geom, int edgeIndex)
{
    const Vector2& root = geom.verts[edgeIndex]; // the point at the edge of the hull
    const Vector2& centroid = geom.GetCentroid();

    Vector2 center (root.X - light.X, root.Y - light.Y); // The vector from the edge -> light
    Vector2 perp (center.Y, -center.X); // the perpandicular vector from edge -> light
    Vector2 toEdge (center.X - centroid.X, center.Y - centroid.Y); // centroid -> edge
    

    center.Normalize();
    perp.Normalize();
    toEdge.Normalize();

    // Reverse the perp if it's the wrong way
    if (Vector2::Dot (toEdge, perp) < 0) {
        perp.Reverse();
        std::cout << "REVERSED PERP VECTOR\n";
        if (Vector2::Dot (toEdge, perp) < 0) {
            std::cout << "DID NOT REVERSE CORRECTLY\n";
        }
    }
    
    Vector2 inner = center + (perp * -1);
    Vector2 outer = center + perp;
    
    inner.Normalize();
    outer.Normalize();
    
    std::cout << "Inner vector " << inner << "\n";
    std::cout << "Center vector " << center << "\n";
    std::cout << "Outer vector " << outer << "\n";
    
    auto fin = new ShadowFin (root, center, outer, inner);
    std::cout << "True inner " << fin->Inner << "\n";
    return std::unique_ptr<ShadowFin> (fin);
}

std::vector<Vector2> genFinTriangle (const ShadowFin& fin, const float scale)
{
    /* h___x
       |  /
       | / x_mag
       |/
       root
     */
    
    Vector2 center_scaled = fin.Center * scale;
    
    // Find outer magnitude so we can form a 90 degree angle between triangle rhx
    float x_mag = center_scaled.GetMagnitude() / cos (Vector2::AngleBetween (fin.Center, fin.Outer));
    
    Vector2 x = fin.Root + (fin.Outer * x_mag);
    Vector2 h = fin.Root + center_scaled;
    
    std::vector<Vector2> tri;
    tri.push_back (fin.Root);
    tri.push_back (h);
    tri.push_back (x);
    
    return std::move (tri);
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

	Geom triangle (0.5, 1, 1, 1);
	triangle.AddVertex (-0.3, -0.8, 0.8);
	triangle.AddVertex (0, -0.8, 0.8);
	triangle.AddVertex (-0.5, -0.5, 0.8);
	triangle.castShadows = true;
	//geom.push_back (triangle);
    
    Geom box3 (0, 0.5, 1, 1);
    box3.AddVertex(0, 0, 0.8);
    box3.AddVertex(0.1, 0, 0.8);
    box3.AddVertex(0.1, 0.1, 0.8);
    box3.AddVertex(0, 0.1, 0.8);
    box3.castShadows = true;
    //geom.push_back (box3);
		
	Geom floor (1, 1, 1, 1);
	floor.AddVertex (-1, -1, 1);
	floor.AddVertex (1, -1, 1);
	floor.AddVertex (1, 1, 1);
	floor.AddVertex (-1, 1, 1);
	floor.castShadows = false;
	geom.push_back (floor);

	//Light light (0, 0, 0, 2, 0.5);
	//lights.push_back (light);
    
    plight = new Light (0, 0, 0, 2, 0.5);
    lights.push_back (std::unique_ptr<Light> (plight));
}
