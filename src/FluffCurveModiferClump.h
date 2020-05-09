#pragma once

#include <FluffCurveModiferBase.h>
#include <ArMesh.h>
#include <FluffScatter.h>
#include <PointKdTree.h>

class FluffCurveModiferClump : public FluffCurveModiferBase
{
public:
	~FluffCurveModiferClump() override {}
	void operator()(Curve& curve, const FolliclePoint& fp, const FluffData* fluffData) override
	{
		if (fp.attr.clump == 0 || spList.empty() || posList.empty())
			return;

		Vec2 p(fp.uv.x, fp.uv.y);
		
		uint id = (this->*pSNF)(p);

		FolliclePoint& cfp = spList[id];
		Vec clumpDir = cfp.frameRest.p - fp.frameRest.p;
		clumpDir *= fp.attr.clump;
		Matrix mat = fp.frameRest.matrix();
		mat = mat.inverse();
		mat.multDirMatrix(clumpDir,clumpDir);
		uint i = 0;
		for (auto& cv : curve.pointList) {
			cv += clumpDir * fluffData->curveClump.value(curve.zList[i++]);
		}
	}
	void preModify(const FluffData* fluffData) override
	{
		if (fluffData->clump.baseValue() == 0)
			return;

		AtNode * pRestMeshNode = nodeLookUpByName(AtString(fluffData->restSkinMesh.c_str()));
		AtNode * pDeformedSkinMeshNode = nodeLookUpByName(AtString(fluffData->deformedSkinMesh.c_str()));

		ArMesh restMesh(pRestMeshNode);
		ArMesh deformedMesh(pDeformedSkinMeshNode);

		FluffMeshScatter<AtNode, ArMesh> fms(pRestMeshNode, pDeformedSkinMeshNode, fluffData);
		spList = fms(fluffData->clumpDensity);

		posList.clear();
		for (auto& sp : spList) {
			posList.push_back(KDTree2::Vec(sp.uv.x,sp.uv.y));
		}

		pSNF = &FluffCurveModiferClump::searchNearest_BruteForce;
		if (fluffData->clumpMethod) {
			kdTree.build(posList);
			pSNF = &FluffCurveModiferClump::searchNearest_Tree;

			maxSearchedNum = 0;
			avgSearchedNum = 0;
			mSearchedNum = 0;
			minSearchedNum = uint(posList.size());
		}
	}

private:
	uint searchNearest_Tree(CVec2 & p)
	{
		uint searchedNum = 0;
		uint id = kdTree.searchNearest(posList, p, searchedNum);

		tbb::spin_mutex::scoped_lock lock;
		lock.acquire(mtx);
		minSearchedNum = std::min<uint>(searchedNum, minSearchedNum);
		maxSearchedNum = std::max<uint>(searchedNum, maxSearchedNum);
		avgSearchedNum += searchedNum;
		++mSearchedNum;
		lock.release();

		return id;
	}

	uint searchNearest_BruteForce(CVec2& p)
	{
		uint id = 0, i = 0;
		float dist2, nearestDist2 = 1e10f;
		Vec2 uv2;
		for (auto& p2d : spList) {
			uv2.x = p2d.uv.x;
			uv2.y = p2d.uv.y;
			dist2 = (uv2 - p).length2();
			if (dist2 > nearestDist2) {
				++i;
				continue;
			}

			nearestDist2 = dist2;
			id = i;
			++i;
		}

		return id;
	}

	typedef uint(FluffCurveModiferClump::*PSearchNearestFunc)(CVec2& p);
	PSearchNearestFunc pSNF;

private:
	FolliclePointList spList;
	KDTree2 kdTree;
	KDTree2::VecList posList;

	uint minSearchedNum, maxSearchedNum, avgSearchedNum, mSearchedNum;
	static tbb::spin_mutex mtx;

public:
	static FluffCurveModiferBase* create() {
		return new FluffCurveModiferClump();
	}
	static NodeId id;
	
};