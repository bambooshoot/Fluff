#pragma once

#include <FluffGeo.h>
#include <FluffFolliclePoint.h>
#include <FluffUnitCurveBase.h>
#include <FluffLine.h>
#include <FluffRing.h>
#include <curve.h>
#include <FluffCurveMultiplier.h>
#include <FluffScatter.h>

template<typename NODE, typename MESH>
class FluffCurvesBase : public FluffGeo
{
public:
	using CurveList = std::vector<Curve>;
	using CurveListList = std::vector<CurveList>;

	FluffCurvesBase(const FluffData* pData) : FluffGeo(pData) {
		FluffFactory<FluffCurveModiferBase> curvefactory;
		curvefactory.registerClass(FluffCurveModiferRadius::id, FluffCurveModiferRadius::create);
		curvefactory.registerClass(FluffCurveModiferFuzzy::id, FluffCurveModiferFuzzy::create);
		curvefactory.registerClass(FluffCurveModiferNormal::id, FluffCurveModiferNormal::create);
		curvefactory.registerClass(FluffCurveModiferSplit::id, FluffCurveModiferSplit::create);
		curvefactory.registerClass(FluffCurveModiferClump::id, FluffCurveModiferClump::create);

		curveModiferList.push_back(curvefactory.create(FluffCurveModiferRadius::id));

		if (pData->clump.baseValue() > 0)
			curveModiferList.push_back(curvefactory.create(FluffCurveModiferClump::id));

		if (pData->fuzzy.baseValue() > 0)
			curveModiferList.push_back(curvefactory.create(FluffCurveModiferFuzzy::id));

		if (pData->split.baseValue() > 0)
			curveModiferList.push_back(curvefactory.create(FluffCurveModiferSplit::id));

		if (pData->normal.baseValue() > 0)
			curveModiferList.push_back(curvefactory.create(FluffCurveModiferNormal::id));

		FluffFactory<FluffUnitCurveBase> curveMakerfactory;
		curveMakerfactory.registerClass(FluffLine::id, FluffLine::create);
		curveMakerfactory.registerClass(FluffRing::id, FluffRing::create);
		curveMaker = curveMakerfactory.create(pData->curveUnitMode);
	};
	virtual ~FluffCurvesBase() {};
	virtual Curve operator()(FolliclePoint& sp, const FluffData* pFluffData) const = 0;

	void GenerateLocalSpace(CurveList &curveList, FolliclePointList &spList, UIntList &spIdList, MESH & deformedFluffMesh) const {
		FluffMeshScatter<NODE, MESH> fms(pRestSkinMeshNode, pDeformedSkinMeshNode, mpFluffData);
		spList = fms(mpFluffData->den.baseValue());

		for (auto modifer : curveModiferList)
			(*modifer).preModify(mpFluffData);

		CurveListList curveListList(spList.size());
		size_t curveNum = curveListList.size();
		tbb::parallel_for(tbb::blocked_range<size_t>(0, curveNum), [&](tbb::blocked_range<size_t>& range) {
			for (size_t i = range.begin(); i < range.end(); ++i) {
				auto& sp = spList[i];
				auto& curveList = curveListList[i];
				auto curve = (*curveMaker)(sp, mpFluffData);

				FluffCurveMultiplier multi;
				curveList = multi(sp, curve, i, mpFluffData);

				for (auto& curv : curveList) {
					for (auto modifer : curveModiferList)
						(*modifer)(curv, spList[i], mpFluffData);
				}
			}
			});

		uint spId = 0;
		for (auto& curveList2 : curveListList) {
			for (auto& curve : curveList2) {
				curveList.push_back(curve);
				spIdList.push_back(spId);
			}
			++spId;
		}
	}

private:
	std::vector<FluffCurveModiferBase*> curveModiferList;
	FluffUnitCurveBase*					curveMaker;
};