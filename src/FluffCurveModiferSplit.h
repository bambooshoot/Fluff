#pragma once

#include <FluffCurveModiferBase.h>

class FluffCurveModiferSplit : public FluffCurveModiferBase
{
public:
	~FluffCurveModiferSplit() override {}
	void operator()(Curve& curve, const FolliclePoint& fp, const FluffData* fluffData) override
	{
		if (fp.attr.split < 1e-10f)
			return;

		Vec  n(0, 0, 1);
		Vec splitN = fp.attr.splitDir(fluffData->splitUVScale);
		float splitRatio = clamp<float>(fp.attr.split, 0, 1);
		splitN = n * (1 - splitRatio) + splitN * splitRatio;
		Vec splitN2;

		float ratio2;
		float splitLevel = clamp<float>(fp.attr.grad.z, 0, 1);
		float curveSplitScale = fluffData->splitScale;

		for (uint i = 1; i < curve.pointList.size(); ++i) {
			ratio2 = fluffData->curveSplit.value(curve.zList[i]);
			ratio2 = clamp<float>(ratio2 * curveSplitScale * ( 1 - splitLevel ) + fluffData->splitOffset, 0, 1);
			splitN2 = n * (1 - ratio2) + splitN * ratio2;
			Matrix mtx(
				1, 0, 0, 0,
				0, 1, 0, 0,
				splitN2.x, splitN2.y, splitN2.z, 0,
				0, 0, 0, 1
			); 

			auto& p = curve.pointList.at(i);
			p *= mtx;
		}
	}
	void preModify(const FluffData* fluffData) override
	{
	}
	static FluffCurveModiferBase* create() {
		return new FluffCurveModiferSplit();
	}
	static NodeId id;
};