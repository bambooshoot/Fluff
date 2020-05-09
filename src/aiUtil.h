#pragma once

#include <ai.h>

inline Vec AtVec2Vec(AtVector& vec)
{
	return Vec(vec.x, vec.y, vec.z);
}

inline Vec2 AtVec22Vec(AtVector2& vec)
{
	return Vec2(vec.x, vec.y);
}