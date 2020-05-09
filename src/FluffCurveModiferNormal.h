#pragma once

#include <FluffCurveModiferBase.h>

class FluffCurveModiferNormal : public FluffCurveModiferBase
{
public:
	~FluffCurveModiferNormal() override {}
	void operator()(Curve& curve, const FolliclePoint& fp, const FluffData* fluffData) override
	{
		Vec  n(0, 0, 1);
		Vec normal = fp.attr.normalDir(fluffData->normalUVScale);
		normal = n * (1 - fluffData->normalRatio) + normal * fluffData->normalRatio;
		Vec normal2;

		float ratio2;
		size_t pointNum = curve.pointList.size();
		for (uint i = 1; i < pointNum; ++i) {
			ratio2 = fluffData->curveNormal.value(curve.zList[i]);
			normal2 = n * (1 - ratio2) + normal * ratio2;
			Matrix mtx(
				1, 0, 0, 0,
				0, 1, 0, 0,
				normal2.x, normal2.y, normal2.z, 0,
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
		return new FluffCurveModiferNormal();
	}
	static NodeId id;
};