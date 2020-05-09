#pragma once

#include <curve.h>
#include <FluffUnitCurveBase.h>
#include <FluffFolliclePoint.h>

class FluffCurveModiferBase
{
public:
	virtual ~FluffCurveModiferBase() {};
	virtual void operator()(Curve& curve, 
		const FolliclePoint& fp,
		const FluffData* fluffData) = 0;
	virtual void preModify(const FluffData* fluffData) = 0;
};