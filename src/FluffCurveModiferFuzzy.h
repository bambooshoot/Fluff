#pragma once

#include <FluffCurveModiferBase.h>

#include <aiUtil.h>

class FluffCurveModiferFuzzy : public FluffCurveModiferBase
{
public:
	~FluffCurveModiferFuzzy() override {}
	void operator()(Curve& curve, const FolliclePoint& fp, const FluffData* fluffData) override
	{
		Vec		n, t;
		float	sampleStepLen = fp.attr.length * fluffData->fuzzy_seg_freq;

		float ratio2;
		std::vector<Vec> tList;
		for (uint i = 1; i < curve.pointList.size(); ++i) {
			t = curve.pointList.at(i) - curve.pointList.at(i - 1);
			t.normalize();
			tList.push_back(t);
		}

		AtVector noiseV;
		AtVector curNoiseSampleP((fp.uv.x - 0.5f) * fluffData->fuzzy_curve_freq, (fp.uv.y - 0.5f) * fluffData->fuzzy_curve_freq, 0);
		for (uint i = 1; i < curve.pointList.size(); ++i) {
			auto& p = curve.pointList.at(i);
			t = tList[i - 1];

			noiseV = AiVNoise3(
				curNoiseSampleP, 
				fluffData->fuzzy_octaves,
				fluffData->fuzzy_distortion,
				fluffData->fuzzy_lacunarity);

			n = AtVec2Vec(noiseV);
			float len = n.length();
			float dotTN = n.dot(t);
			n -= t * dotTN; // pare z direction off
			n.normalize();

			ratio2 = clamp(len, fluffData->fuzzy_min, fluffData->fuzzy_max);

			ratio2 *= fluffData->curveFuzzy.value(curve.zList[i]) * fp.attr.fuzzy;
			p += n * ratio2;

			curNoiseSampleP.z = sampleStepLen * curve.zList[i];
		}
	}
	void preModify(const FluffData* fluffData) override
	{
	}
	static FluffCurveModiferBase* create() {
		return new FluffCurveModiferFuzzy();
	}
	static NodeId id;
};