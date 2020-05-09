#pragma once

#include <FluffCurveModiferBase.h>

class FluffCurveModiferRadius : public FluffCurveModiferBase
{
public:
	~FluffCurveModiferRadius() override {}
	void operator()(Curve& curve, const FolliclePoint& fp, const FluffData* fluffData) override
	{
		float r = fp.attr.width;
		uint i = 0;
		for (auto& radius : curve.radiusList) {
			radius = fluffData->curveWidth.value(curve.zList[i++]) * r;
		}
	}
	void preModify(const FluffData* fluffData) override
	{
	}
	static FluffCurveModiferBase* create() {
		return new FluffCurveModiferRadius();
	}
	static NodeId id;
};