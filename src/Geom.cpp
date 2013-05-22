#include "Vector.h"
#include "Geom.h"
#include "GL/glfw.h"

#include <memory>
#include <iostream>

Geom::Geom (float r, float g, float b, float a)
    : color (r, g, b, a)
{
}

void Geom::AddVertex (float x, float y, float z)
{
    verts.push_back (Vector3 (x, y, z));
}

void Geom::Draw (int lastStartEdge, int lastEndEdge) const
{
    glBegin (GL_POLYGON);
    
    for (int i=0; i < verts.size(); i++) {
        Vector3 vert = verts[i];
        
        float vertR =
            castShadows && lastStartEdge == i ? 1 :
            castShadows && lastEndEdge ==   i ? 0 :
            color.X;
        float vertG =
            castShadows && lastStartEdge == i ? 0 :
            castShadows && lastEndEdge ==   i ? 1 :
            color.Y;
        float vertB =
            castShadows && lastStartEdge == i ? 0 :
            castShadows && lastEndEdge ==   i ? 0 :
            color.Z;
        
        glColor4f (vertR, vertG, vertB, 1);
        glVertex3f (
            vert.X,
            vert.Y,
            vert.Z
        );
    }
    glEnd();
}