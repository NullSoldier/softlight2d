#include "Geom.h"

void Geom::AddVertex (float x, float y, float z)
{
	vertices.push_back (x);
	vertices.push_back (y);
	vertices.push_back (z);
}

std::tuple<float,float,float> Geom::GetVertex (int index) const
{
	int start = index/3;
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
