#pragma once

#include <FluffDef.h>
#include <array>
#include <vector>

#include <maya/MFnMesh.h>
#include <maya/MIntArray.h>

class MayaMesh
{
public:
	MayaMesh(const MFnMesh* pFnMesh) :m_pFnMesh(pFnMesh){};

	uint vertexId(cint face_idx, cint v_loc_idx) const;
	uint pointNum() const;
	uint polygonNum() const;
	KeyIds keyIds(cuint keyId) const;
	cuint keyNum() const;

	Vec point(cuint idx, CKeyIds& kId) const;
	Vec vertex(cuint fid, cuint vid, CKeyIds& kId) const;
	Vec2 uv(cuint fid, cuint vid) const;
	Vec normal(cuint fid, cuint vid, CKeyIds& kId) const;

	void getTriangles(cint fid, TriangleIdList& triIdList) const;

	void getTrianglesWithArea(cint fid, TriangleIdList& triIdList, FloatList& areaList, CKeyIds& kId) const;

	Vec dPdu(cuint fid, cuint vid, CKeyIds& kId) const;
	Vec dPdv(cuint fid, cuint vid, CKeyIds& kId) const;

private:
	float triArea(cuint fid, cuint vid0, cuint vid1, cuint vid2) const;
	Vec pEdgeVec(cuint fid, cuint edgeId, CKeyIds& kId) const;
	Vec2 uvEdgeVec(cuint fid, cuint edgeId) const;

	const MFnMesh* m_pFnMesh;
	MIntArray vertexCountList, vertexBeginIdList, vertexIdList,
			  triangleCounts, triangleVerticesBeginList, triangleVertices;
};