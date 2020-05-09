#pragma once

#include <FluffArDef.h>
#include <array>
#include <vector>

class ArMesh
{
public:
	ArMesh(const AtNode * pNode);
	~ArMesh();
	
	uint vertexId(cint face_idx, cint v_loc_idx) const;
	uint pointNum() const;
	uint polygonNum() const;
	KeyIds keyIds(cuint keyId) const;
	cuint keyNum() const;

	Vec point(cuint idx, CKeyIds& kId) const;
	Vec vertex(cuint fid, cuint vid, CKeyIds & kId) const;
	Vec2 uv(cuint fid, cuint vid) const;
	Vec normal(cuint fid, cuint vid, CKeyIds & kId) const;

	void getTriangles(cint fid, TriangleIdList& triIdList) const;
	
	void getTrianglesWithArea(cint fid, TriangleIdList & triIdList, FloatList & areaList, CKeyIds & kId) const;
	
	Vec dPdu(cuint fid, cuint vid, CKeyIds & kId) const;
	Vec dPdv(cuint fid, cuint vid, CKeyIds & kId) const;
	
private:
	uint sideNum(cint idx) const;
	uint uvId(cint face_idx, cint v_loc_idx) const;
	uint normalId(cint face_idx, cint v_loc_idx) const;
	float polygonArea(cint fid, CKeyIds& kId) const;
	
	cuint mtxKeyNum() const;
	cuint pointKeyNum() const;

	void reset(const AtNode * pNode);
	cuint vertexBegin(cuint fid) const;
	float triArea(cuint fid, cuint vid0, cuint vid1, cuint vid2, CKeyIds & kId) const;
	Vec pEdgeVec(cuint fid, cuint edgeId, CKeyIds & kId) const;
	Vec2 uvEdgeVec(cuint fid, cuint edgeId) const;

	uint    mPointKeyNum, mMtxKeyNum;
	AtArray *nsides,
			*nsidesBegin,

			*vidxs,
			*vlist,

			*uvidxs,
			*uvlist,

			*nidxs,
			*nlist,

			*mtxlist;
};