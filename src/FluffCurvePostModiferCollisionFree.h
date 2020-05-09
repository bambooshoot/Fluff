#pragma once

#include <FluffCurvePostModiferBase.h>

#include <openvdb/openvdb.h>
#include <openvdb/tools/MeshToVolume.h>
#include <openvdb/tools/VolumeToMesh.h>
#include <openvdb/tools/LevelSetUtil.h>
#include <openvdb/tools/Interpolation.h>
#include <openvdb/tools/GridOperators.h>

class FluffCurvePostModiferCollisionFree : public FluffCurvePostModiferBase
{
private:
	using FloatGridType = openvdb::FloatGrid;
	using FloatSamplerType = openvdb::tools::GridSampler<FloatGridType, openvdb::tools::BoxSampler>;
	using VectorGridType = openvdb::tools::ScalarToVectorConverter<FloatGridType>::Type;
	using VectorSamplerType = openvdb::tools::GridSampler<VectorGridType, openvdb::tools::BoxSampler>;

	std::vector<FloatGridType::Ptr>		sdfGridList;
	std::vector<VectorGridType::Ptr>	gradGridList;
	float surfaceValue;

	std::optional<AtVector> collidedVec(const AtVector& p, CKeyIds keyId) const {
		openvdb::Vec3R sampleP(p.x, p.y, p.z);
		FloatSamplerType sdfSampler(*sdfGridList[keyId.pKeyId]);
		FloatGridType::ValueType sdfValue = sdfSampler.wsSample(sampleP);
		if (sdfValue >= surfaceValue)
			return std::nullopt;

		VectorSamplerType gradSampler(*gradGridList[keyId.pKeyId]);
		VectorGridType::ValueType gradValue = gradSampler.wsSample(sampleP);
		gradValue.normalize();
		gradValue *= surfaceValue - sdfValue;

		return AtVector(gradValue.x(), gradValue.y(), gradValue.z());
	}
	void initVDB(const ArMesh& deformedFluffMesh, const float bandWidth, const float voxelSize) {
		uint polyNum = deformedFluffMesh.polygonNum();
		cuint keyNum = deformedFluffMesh.keyNum();

		sdfGridList.resize(2);
		gradGridList.resize(2);

		for (uint keyId = 0; keyId < keyNum; ++keyId) {
			CKeyIds keyIds = deformedFluffMesh.keyIds(keyId);
			std::vector<openvdb::Vec4I> primList;
			for (uint fid = 0; fid < polyNum; ++fid) {
				TriangleIdList triIdList;
				deformedFluffMesh.getTriangles(fid, triIdList);
				for (auto& triId : triIdList) {
					openvdb::Vec4I prim(
						deformedFluffMesh.vertexId(fid, triId[0]),
						deformedFluffMesh.vertexId(fid, triId[1]),
						deformedFluffMesh.vertexId(fid, triId[2]),
						openvdb::util::INVALID_IDX);

					primList.push_back(prim);
				}
			}

			openvdb::math::Transform::Ptr transform;
			transform = openvdb::math::Transform::createLinearTransform(voxelSize);

			uint pointNum = deformedFluffMesh.pointNum();
			std::vector<openvdb::Vec3s> pointList(pointNum);
			Vec p;
			openvdb::Vec3d pos;
			for (uint pi = 0; pi < pointNum; ++pi) {
				p = deformedFluffMesh.point(pi, keyIds);
				pos[0] = double(p.x);
				pos[1] = double(p.y);
				pos[2] = double(p.z);

				pos = transform->worldToIndex(pos);

				pointList[pi][0] = float(pos[0]);
				pointList[pi][1] = float(pos[1]);
				pointList[pi][2] = float(pos[2]);
			}

			openvdb::tools::QuadAndTriangleDataAdapter<openvdb::Vec3s, openvdb::Vec4I>
				theMesh(pointList, primList);

			sdfGridList[keyId] = openvdb::tools::meshToVolume<FloatGridType>(
				theMesh, *transform, bandWidth, bandWidth);

			gradGridList[keyId] = openvdb::tools::gradient(*sdfGridList[keyId], true);
		}
	}
	
public:
	~FluffCurvePostModiferCollisionFree() override {};
	void operator()(AtNode* pCurveNode, const ArMesh& deformedFluffMesh, const FluffData* pFluffData) override {
		if (pFluffData->collision < 1e-10f)
			return;

		initVDB(deformedFluffMesh, pFluffData->vdbBandWidth, pFluffData->vdbVoxelSize);

		surfaceValue = pFluffData->surfaceValue;
		AtArray * numArray = AiNodeGetArray(pCurveNode, "num_points");
		const uint * numList = (const uint *)AiArrayMap(numArray);
		const uint curveNum = AiArrayGetNumElements(numArray);
		uint pointNum;
		AtArray* pointArray = AiNodeGetArray(pCurveNode, "points");
		uint keyNum = AiArrayGetNumKeys(pointArray);
		for (uint keyId = 0; keyId < keyNum; ++keyId) {
			CKeyIds keyIds = deformedFluffMesh.keyIds(keyId);
			AtVector* pointList = (AtVector*)AiArrayMapKey(pointArray, keyId);
			uint gPId = 0;
			float ratio, ratio2, step;
			for (uint curveId = 0; curveId < curveNum; ++curveId) {
				ratio = 0;
				pointNum = numList[curveId];
				step = 1.0f / float(pointNum);
				for (uint pId = 0; pId < numList[curveId]; ++pId) {
					auto & p = pointList[gPId++];
					std::optional<AtVector> collidedFreeVec = collidedVec(p, keyIds);
					if (collidedFreeVec.has_value()) {
						ratio2 = pFluffData->curveCollision.value(ratio);
						ratio2 *= pFluffData->collision;
						p = p + collidedFreeVec.value() * ratio2;
					}
					ratio += step;
				}
			}
			AiArrayUnmap(pointArray);
		}
		AiArrayUnmap(numArray);
	};
	AtNode* visualize(const AtNode* parentNode, const ArMesh& deformedFluffMesh, const FluffData* pFluffData) override
	{
		initVDB(deformedFluffMesh, pFluffData->vdbBandWidth, pFluffData->vdbVoxelSize);

		AtNode* node = AiNode("polymesh", "collisionMeshVisualization", parentNode);

		cuint keyNum = deformedFluffMesh.keyNum();
		std::vector<std::vector<openvdb::Vec3s>> pointList(keyNum);
		std::vector<std::vector<openvdb::Vec4I>> quadList(keyNum);
		size_t quadNum = 0;
		size_t pointNum = 0;
		for (uint keyId = 0; keyId < keyNum; ++keyId) {
			openvdb::tools::volumeToMesh<FloatGridType>(*sdfGridList[keyId], pointList[keyId], quadList[keyId], pFluffData->surfaceValue);
			quadNum += quadList[keyId].size();
			pointNum += pointList[keyId].size();
		}

		AtArray* num_point_array = AiArrayAllocate((uint)quadNum, 1, AI_TYPE_UINT);
		uint idx = 0;
		for (auto & quads : quadList)
			for (auto& quadIds : quads)
				AiArraySetUInt(num_point_array, idx++, 4);
				
		AiNodeSetArray(node, "nsides", num_point_array);

		AtArray* idx_array = AiArrayAllocate((uint)quadNum * 4, 1, AI_TYPE_UINT);
		idx = 0;
		for (auto& quads : quadList)
			for (auto& quadIds : quads)
				for (uint i = 0; i < 4; ++i)
					AiArraySetUInt(idx_array, idx++, quadIds[i]);

		AiNodeSetArray(node, "vidxs", idx_array);

		AtArray* pointArray = AiArrayAllocate((uint)pointNum, 1, AI_TYPE_VECTOR);
		idx = 0;
		for (auto & points : pointList)
			for (auto& p : points)
				AiArraySetVec(pointArray, idx++, AtVector(p.x(), p.y(), p.z()));

		AiNodeSetArray(node, "vlist", pointArray);
		
		return node;
	}
	static FluffCurvePostModiferBase* create() {
		return new FluffCurvePostModiferCollisionFree();
	}
	static NodeId id;
};