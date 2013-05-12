#ifndef GEOM_H
#define GEOM_H

#include <vector>
#include <tuple>

class Geom
{
public:
	float r;
	float g;
	float b;
	float a;
	bool castShadows;

	Geom (float r, float g, float b, float a); 
	void AddVertex (float x, float y, float z);
	int VertexCount() const;
	std::tuple<float,float,float> GetVertex (int index) const;
private:
	std::vector<float> vertices;
};
#endif
