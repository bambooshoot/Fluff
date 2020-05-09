#include <FluffPoints.h>

AtNode* FluffPoints::operator()(const AtNode* parentNode) const {
	AtNode* pRestSkinMeshNode = nodeLookUpByName(AtString(mpFluffData->restSkinMesh.c_str()));
	if (!pRestSkinMeshNode)
		return 0;

	AtNode* pDeformedSkinMeshNode = nodeLookUpByName(AtString(mpFluffData->deformedSkinMesh.c_str()));
	if (!pDeformedSkinMeshNode)
		return 0;

	float motion_start = AiNodeGetFlt(pDeformedSkinMeshNode, AtString("motion_start"));
	float motion_end = AiNodeGetFlt(pDeformedSkinMeshNode, AtString("motion_end"));

	FluffMeshScatter fms(pRestSkinMeshNode, pDeformedSkinMeshNode, mpFluffData);
	FolliclePointList spList = fms(density);

	AtNode* pPointNode = AiNode("points", mpFluffData->name.c_str(), parentNode);
	AiNodeSetFlt(pPointNode, "min_pixel_width", mpFluffData->min_pixel_width);
	AiNodeSetFlt(pPointNode, "motion_start", motion_start);
	AiNodeSetFlt(pPointNode, "motion_end", motion_end);

	uint pointNum = uint(spList.size());

	ArMesh deformedFluffMesh(pDeformedSkinMeshNode);
	uint keyNum = deformedFluffMesh.keyNum();

	AtArray* point_array = AiArrayAllocate(pointNum, keyNum, AI_TYPE_VECTOR);
	for (uint keyId = 0; keyId < keyNum; ++keyId) {
		AtVector* pSubList = (AtVector*)AiArrayMapKey(point_array, keyId);
		for (uint i = 0; i < pointNum; ++i) {
			pSubList[i] = Vec2AtVec(spList[i].frames.at(keyId).p);
		}
		AiArrayUnmap(point_array);
	}
	AiNodeSetArray(pPointNode, "points", point_array);

	AtArray* radius_array = AiArrayAllocate(pointNum, 1, AI_TYPE_FLOAT);
	for (uint i = 0; i < pointNum; ++i) {
		AiArraySetFlt(radius_array, i, mpFluffData->width.baseValue());
	}
	AiNodeSetArray(pPointNode, "radius", radius_array);

	return pPointNode;
}

AtNode* FluffPoints::virtualization(const AtNode* parentNode) const
{
	return 0;
}