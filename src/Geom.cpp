#include "Geom.h"
#include <iostream>

Geom::Geom (float r, float g, float b, float a) :
	r (r),
	g (g),
	b (b),
	a (a)	
{
}

void Geom::AddVertex (float x, float y, float z)
{
	vertices.push_back (x);
	vertices.push_back (y);
	vertices.push_back (z);
}

std::tuple<float,float,float> Geom::GetVertex (int index) const
{
	//std::cout << "Getting vertex with index " << index << "\n";
	int start = index*3;
	//std::cout << "Vertex index is " << start << "\n";
	return std::tuple<float,float,float> (
		vertices[start],
		vertices[start+1],
		vertices[start+2]
	);
}

int Geom::VertexCount() const
{
	return vertices.size() / 3;
}
