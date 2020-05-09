#pragma once

#include <FluffUnitCurveBase.h>

class FluffLine : public FluffUnitCurveBase
{
public:
	virtual ~FluffLine() {};
	Curve operator()(FolliclePoint& fp, const FluffData* pFluffData) const override {
		Curve curve;

		float len = fp.attr.length;
		float stepLen1 = 1.0f / float(pFluffData->segmentNum);
		float stepLen = len * stepLen1;

		Vec curP(0, 0, 0);

		curve.pointList.push_back(curP);

		for (uint i = 0; i < pFluffData->segmentNum; ++i) {
			curP.z += stepLen;
			curve.pointList.push_back(curP);
		}

		curve.uv = fp.uv;
		curve.radiusList.resize(curve.pointList.size(), 0.01f);
	
		float ratio = 0;
		curve.zList.resize(curve.pointList.size(), 0);
		for (float & z : curve.zList) {
			z = ratio;
			ratio += stepLen1;
		}

		return curve;
	}

	static FluffUnitCurveBase* create() {
		return new FluffLine();
	}
	static NodeId id;
};