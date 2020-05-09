#pragma once

#include <FluffCurvesBase.h>

class FluffCurves : public FluffCurvesBase<AtNode, ArMesh>
{
public:
	struct PointSizeSum {
		const CurveList* pCurveList;
		size_t value;
		PointSizeSum(const CurveList* _pCurveList) : value(0), pCurveList(_pCurveList) {}
		PointSizeSum(PointSizeSum& s, tbb::split) : value(0), pCurveList(s.pCurveList) {}
		void operator()(const tbb::blocked_range<size_t>& r) {
			for (size_t i = r.begin(); i < r.end(); ++i)
				value += (*pCurveList)[i].pointList.size();
		}
		void join(PointSizeSum& rhs) { value += rhs.value; }
	};

	FluffCurves(const FluffData* pData);

	AtNode* operator ()(const AtNode* parentNode) const override;
	AtNode* virtualization(const AtNode* parentNode) const override;

private:
	std::vector<FluffCurvePostModiferBase*> curvePostModiferList;

	void pushPointNumArray(AtNode* pCurveNode, CurveList& curveList) const;
	void pushPointArray(AtNode* pCurveNode, FolliclePointList& fpList, CUIntList& spIdList, CurveList& curveList, cuint keyNum) const;
	void pushRadiusArray(AtNode* pCurveNode, CurveList& curveList) const;
	void pushZCoordArray(AtNode* pCurveNode, CurveList& curveList) const;
	void pushUVArray(AtNode* pCurveNode, CurveList& curveList) const;
};