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
    const Vector2 & GetCentroid() const;
    void AddVertex (float x, float y, float z);
    void Draw (int lastStartEdge, int lastEndEdge) const;
    
    std::vector<Vector3> verts;
    Vector4 color;
	bool castShadows;
    
private:
    void calculateCentroid() const;
    mutable Vector2 centroid;
    mutable bool dirtyCentroid;
};
#endif
