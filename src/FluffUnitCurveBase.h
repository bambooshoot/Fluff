#pragma once

#include <FluffFolliclePoint.h>
#include <curve.h>
#include <FluffCurveMultiplier.h>

class FluffUnitCurveBase
{
public:
	virtual ~FluffUnitCurveBase() {};
	virtual Curve operator()(FolliclePoint& sp, const FluffData* pFluffData) const = 0;
};