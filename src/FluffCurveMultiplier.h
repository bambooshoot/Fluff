#pragma once

#include <FluffFolliclePoint.h>
#include <curve.h>

class FluffCurveMultiplier
{
public:
	using CurveList = std::vector<Curve>;
	CurveList operator()(
		const FolliclePoint& fp, 
		const Curve & curve,
		cuint id,
		const FluffData* pFluffData ) const {
		
		cuint & curveNum = pFluffData->multiNum;
		CurveList curveList;
		if (curveNum <= 1) {
			curveList.push_back(curve);
			return curveList;
		}

		RealDistrib dist_n1p1(-1.0, 1.0);
		RealDistrib dist_01(0, 1.0);
		RandGen rand_generator01(id + pFluffData->seed + 13245), rand_generator_n1p1(id + pFluffData->seed + 11213);

		RandGen rand_gen(id + pFluffData->seed + 43002);
		float radius = pFluffData->multiRadius.value(rand_gen);

		for (uint i = 0; i < curveNum; ++i) {
			
			float len = dist_01(rand_generator01);
			len = 1 - pFluffData->multiLengthRand * len;

			Vec2  offset2d(dist_n1p1(rand_generator_n1p1), dist_n1p1(rand_generator_n1p1));
			offset2d.normalize();
			offset2d *= radius;
			Curve curCurve = curve;
			float curveRadiusWeight;
			uint ii = 0;
			for (auto & p : curCurve.pointList) {
				curveRadiusWeight = pFluffData->curveMultiRadius.value(curCurve.zList[ii++]);
				p.x += offset2d.x * curveRadiusWeight;
				p.y += offset2d.y * curveRadiusWeight;
				p.z *= len;
			}
			curveList.push_back(curCurve);
		}
		return curveList;
	}
};