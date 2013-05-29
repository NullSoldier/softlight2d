#ifndef SHADOWFIN_H
#define SHADOWFIN_H

#include "Vector.h"

class ShadowFin
{
public:
	ShadowFin (Vector2 root, Vector2 center, Vector2 outer, Vector2 inner);
	Vector2 Root;
    Vector2 Center;
	Vector2 Inner;
	Vector2 Outer;
};
#endif
