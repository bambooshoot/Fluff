#pragma once

#include <FluffUnitCurveBase.h>

class FluffRing : public FluffUnitCurveBase
{
public:
	virtual ~FluffRing() {};
	Curve operator()(FolliclePoint& fp, const FluffData* pFluffData) const override {
		Curve curve;

		float d = fp.attr.length;
		float r = fp.attr.length * 0.5f;
		uint pointNum = pFluffData->segmentNum * 2;
		float stepTheta = PI2 / float(pointNum);
		float curTheta = 0;

		Vec curP(0, 0, 0);
		for (uint i = 0; i < pointNum; ++i) {
			curP.x = pFluffData->ringWidth * sin(curTheta);
			curP.z = r * (cos(curTheta + PI) + 1);

			curve.pointList.push_back(curP);
			curTheta += stepTheta;
		}

		curve.uv = fp.uv;
		curve.radiusList.resize(curve.pointList.size(), 0.01f);

		float stepLen = 1.0f / pointNum;
		float ratio = 0, ratio1;

		curve.zList.resize(curve.pointList.size(), 1);
		for (float& z : curve.zList) {
			ratio1 = 1 - abs(ratio * 2 - 1);
			z = ratio1;
			ratio += stepLen;
		}

		return curve;
	}

	static FluffUnitCurveBase* create() {
		return new FluffRing();
	}
	static NodeId id;
};