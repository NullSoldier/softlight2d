#ifndef GEOM_H
#define GEOM_H

#include "Vector.h"

#include <vector>
#include <tuple>
#include <memory>

class Geom
{
public:
    Geom (float r, float g, float b, float a);
    void Draw (int lastStartEdge, int lastEndEdge) const;
    void AddVertex (float x, float y, float z);
    std::vector<Vector3> verts;
    Vector4 color;
	bool castShadows;
};
#endif
