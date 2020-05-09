#pragma once

#include <FluffDef.h>

struct Curve {
	VectorList	pointList;
	FloatList	radiusList;
	FloatList	zList;
	Vec2    	uv;

	Vec transformedP(CMatrix& rotMat, const uint id)
	{
		return pointList[id] * rotMat;
	}
};