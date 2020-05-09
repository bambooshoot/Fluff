#include <FluffCurves.h>

FluffCurves::FluffCurves(const FluffData* pData) :
	FluffCurvesBase<AtNode, ArMesh>(pData) {

	FluffFactory<FluffCurvePostModiferBase> curvePostFactory;
	curvePostFactory.registerClass(FluffCurvePostModiferCollisionFree::id, FluffCurvePostModiferCollisionFree::create);

	curvePostModiferList.push_back(curvePostFactory.create(FluffCurvePostModiferCollisionFree::id));
}

AtNode * FluffCurves::operator()(const AtNode * parentNode) const {
	AtNode * pRestSkinMeshNode = nodeLookUpByName(AtString(mpFluffData->restSkinMesh.c_str()));
	if (!pRestSkinMeshNode)
		return 0;

	AtNode* pDeformedSkinMeshNode = nodeLookUpByName(AtString(mpFluffData->deformedSkinMesh.c_str()));
	if (!pDeformedSkinMeshNode)
		return 0;

	ArMesh deformedFluffMesh(pDeformedSkinMeshNode);
	uint keyNum = deformedFluffMesh.keyNum();
	CurveList curveList;
	FolliclePointList spList;
	UIntList  spIdList;
	GenerateLocalSpace(curveList, spList, spIdList, deformedFluffMesh);

	float motion_start = AiNodeGetFlt(pDeformedSkinMeshNode, AtString("motion_start"));
	float motion_end = AiNodeGetFlt(pDeformedSkinMeshNode, AtString("motion_end"));

	AtNode * pCurveNode = AiNode("curves", AtString(mpFluffData->name.c_str()), parentNode);
	AiNodeSetStr(pCurveNode, "basis", "catmull-rom");
	AiNodeSetStr(pCurveNode, "mode", mpFluffData->curveMode.c_str());
	AiNodeSetFlt(pCurveNode, "min_pixel_width", mpFluffData->min_pixel_width);
	AiNodeSetFlt(pCurveNode, "motion_start", motion_start);
	AiNodeSetFlt(pCurveNode, "motion_end", motion_end);

	pushPointNumArray(pCurveNode, curveList);

	pushPointArray(pCurveNode, spList, spIdList, curveList, keyNum);
	pushRadiusArray(pCurveNode, curveList);
	pushZCoordArray(pCurveNode, curveList);
	pushUVArray(pCurveNode, curveList);

	AtNode* pCollisionSkinMeshNode = nodeLookUpByName(AtString(mpFluffData->collisionSkinMesh.c_str()));
	if (!pCollisionSkinMeshNode)
		return pCurveNode;

	ArMesh collisionFluffMesh(pCollisionSkinMeshNode);
	for (auto modifer : curvePostModiferList)
		(*modifer)(pCurveNode, collisionFluffMesh, mpFluffData);

	return pCurveNode;
}

AtNode* FluffCurves::virtualization(const AtNode* parentNode) const
{
	AtNode* pDeformedSkinMeshNode = nodeLookUpByName(AtString(mpFluffData->deformedSkinMesh.c_str()));
	if (!pDeformedSkinMeshNode)
		return 0;

	return curvePostModiferList[0]->visualize(parentNode, pDeformedSkinMeshNode, mpFluffData);
}

void FluffCurves::pushPointNumArray(AtNode * pCurveNode, CurveList& curveList) const
{
	uint curveNum = uint(curveList.size());
	AtArray * num_point_array = AiArrayAllocate(curveNum, 1, AI_TYPE_UINT);
	for (uint i = 0; i < curveNum; ++i) {
		AiArraySetUInt(num_point_array, i, uint(curveList[i].pointList.size()) + 2);
	}
	AiNodeSetArray(pCurveNode, "num_points", num_point_array);
}

void  FluffCurves::pushPointArray(AtNode * pCurveNode, FolliclePointList& fpList, CUIntList &spIdList, CurveList& curveList, cuint keyNum) const
{
	size_t curveNum = curveList.size();
	PointSizeSum pointNum(&curveList);
	tbb::parallel_reduce(tbb::blocked_range<size_t>(0, curveNum), pointNum);
	pointNum.value += curveNum * 2;

	AiMsgInfo("point data num : %u", pointNum.value);

	AtArray * point_array = AiArrayAllocate((uint)pointNum.value, keyNum, AI_TYPE_VECTOR);
	uint pi;
	uint pId, curveId;
	Vec p;
	PointFrame pf;
	for (uint keyId = 0; keyId < keyNum; ++keyId) {
		AtVector * pList = (AtVector*)AiArrayMapKey(point_array, keyId);
		curveId = 0;
		pi = 0;
		for (auto& curve : curveList) {
			pId = 0;
			pf = fpList[spIdList[curveId]].frames[keyId];
			CMatrix &mtx = pf.matrix();
			p = curve.transformedP(mtx, pId);
			pList[pi++] = Vec2AtVec(p);
			for (uint i = 0; i < curve.pointList.size(); ++i) {
				p = curve.transformedP(mtx, pId);
				pList[pi++] = Vec2AtVec(p);
				++pId;
			}
			pList[pi++] = Vec2AtVec(p);
			++curveId;
		}
		AiArrayUnmap(point_array);
	}
	AiNodeSetArray(pCurveNode, "points", point_array);
}

void  FluffCurves::pushRadiusArray(AtNode * pCurveNode, CurveList& curveList) const
{
	PointSizeSum pointNum(&curveList);
	tbb::parallel_reduce(tbb::blocked_range<size_t>(0, curveList.size()), pointNum);

	AiMsgInfo("radius data num : %u", pointNum.value);

	AtArray * radius_array = AiArrayAllocate((uint)pointNum.value, 1, AI_TYPE_FLOAT);
	uint pi = 0;
	for (auto & curve : curveList) {
		for (auto & r : curve.radiusList) {
			AiArraySetFlt(radius_array, pi++, r);
		}
	}
	AiNodeSetArray(pCurveNode, "radius", radius_array);
}

void  FluffCurves::pushZCoordArray(AtNode * pCurveNode, CurveList& curveList) const
{
	PointSizeSum pointNum(&curveList);
	tbb::parallel_reduce(tbb::blocked_range<size_t>(0, curveList.size()), pointNum);

	AiMsgInfo("zCoord data num : %u", pointNum.value);

	AiNodeDeclare(pCurveNode, "zCoord", "varying FLOAT");
	AtArray * z_array = AiArrayAllocate((uint)pointNum.value, 1, AI_TYPE_FLOAT);
	uint pi = 0;
	for (auto & curve : curveList) {
		for (auto & c : curve.zList) {
			AiArraySetFlt(z_array, pi++, c);
		}
	}
	AiNodeSetArray(pCurveNode, "zCoord", z_array);
}

void  FluffCurves::pushUVArray(AtNode * pCurveNode, CurveList& curveList) const
{
	AtArray * uv_array = AiArrayAllocate((uint32_t)curveList.size(), 1, AI_TYPE_VECTOR2);
	uint pi = 0;
	for (auto & curve : curveList) {
		AiArraySetVec2(uv_array, pi++, Vec22AtVec(curve.uv));
	}
	AiNodeSetArray(pCurveNode, "uvs", uv_array);

	AiMsgInfo("uv data num : %u", curveList.size());
}