#include <ArMesh.h>

#include <aiUtil.h>

ArMesh::ArMesh(const AtNode * pNode) {
	reset(pNode);
}
ArMesh::~ArMesh() {
	AiArrayDestroy(nsidesBegin);
}
uint ArMesh::polygonNum() const
{
	return AiArrayGetNumElements(nsides);
}
uint ArMesh::sideNum(cint idx) const
{
	return AiArrayGetUInt(nsides, idx);
}
cuint ArMesh::mtxKeyNum() const
{
	return mMtxKeyNum;
}
cuint ArMesh::pointKeyNum() const
{
	return mPointKeyNum;
}
cuint ArMesh::keyNum() const
{
	return std::min<uint>(mPointKeyNum, mMtxKeyNum);
}  
uint ArMesh::pointNum() const
{
	return AiArrayGetNumElements(vlist);
}
KeyIds ArMesh::keyIds(cuint keyId) const
{
	KeyIds kids;
	kids.mtxKeyId = std::min<uint>(keyId, mMtxKeyNum - 1);
	kids.pKeyId = std::min<uint>(keyId, mPointKeyNum - 1);
	return kids;
}
Vec ArMesh::point(cuint idx, CKeyIds & kId) const
{
	AtVector p = AiArrayGetVec(vlist, idx);
	AtMatrix* subMat = (AtMatrix*)AiArrayMapKey(mtxlist, kId.mtxKeyId);
	p = AiM4PointByMatrixMult(*subMat, p);
	AiArrayUnmap(mtxlist);

	return AtVec2Vec(p);
}
uint ArMesh::vertexId(cint face_idx, cint v_loc_idx) const
{
	return AiArrayGetUInt(vidxs, vertexBegin(face_idx) + v_loc_idx);
}
uint ArMesh::uvId(cint face_idx, cint v_loc_idx) const
{
	return AiArrayGetUInt(uvidxs, vertexBegin(face_idx) + v_loc_idx);
}
uint ArMesh::normalId(cint face_idx, cint v_loc_idx) const
{
	return AiArrayGetUInt(nidxs, vertexBegin(face_idx) + v_loc_idx);
}
Vec ArMesh::vertex(cuint fid, cuint vid, CKeyIds & kId) const
{
	AtVector* subVList = (AtVector*)AiArrayMapKey(vlist, kId.pKeyId);
	AtVector p = subVList[vertexId(fid, vid)];
	AiArrayUnmap(vlist);

	AtMatrix* subMat = (AtMatrix*)AiArrayMapKey(mtxlist, kId.mtxKeyId);
	p = AiM4PointByMatrixMult(*subMat, p);
	AiArrayUnmap(mtxlist);

	return AtVec2Vec(p);
}
Vec2 ArMesh::uv(cuint fid, cuint vid) const
{
	AtVector2 _uv = AiArrayGetVec2(uvlist, uvId(fid, vid));
	return AtVec22Vec(_uv);
}
Vec ArMesh::normal(cuint fid, cuint vid, CKeyIds & kId) const
{
	AtVector* subNList = (AtVector*)AiArrayMapKey(nlist, kId.pKeyId);
	AtVector n = subNList[normalId(fid, vid)];
	AiArrayUnmap(nlist);

	AtMatrix* subMat = (AtMatrix*)AiArrayMapKey(mtxlist, kId.mtxKeyId);
	n = AiM4VectorByMatrixTMult(AiM4Invert(*subMat), n);
	AiArrayUnmap(mtxlist);

	return AtVec2Vec(n);
}
cuint ArMesh::vertexBegin(cuint fid) const
{
	return AiArrayGetUInt(nsidesBegin, fid);
}
float ArMesh::polygonArea(cint fid, CKeyIds & kId) const
{
	float area = 0;
	uint triNum = sideNum(fid) - 2;
	uint triVid[3];
	triVid[0] = 0;
	for (uint i = 0; i < triNum; ++i) {
		triVid[1] = 1 + i;
		triVid[2] = 2 + i;
		area += triArea(fid, triVid[0], triVid[1], triVid[2], kId);
	}
	return area;
}
void ArMesh::getTrianglesWithArea(cint fid, TriangleIdList & triIdList, FloatList & areaList, CKeyIds & kId) const
{
	uint triNum = sideNum(fid) - 2;
	TriangleId triId;
	triId[0] = 0;
	for (uint i = 0; i < triNum; ++i) {
		triId[1] = 1 + i;
		triId[2] = 2 + i;
		triIdList.push_back(triId);
		areaList.push_back(triArea(fid, triId[0], triId[1], triId[2], kId));
	}
}
void ArMesh::getTriangles(cint fid, TriangleIdList& triIdList) const
{
	uint triNum = sideNum(fid) - 2;
	TriangleId triId;
	triId[0] = 0;
	for (uint i = 0; i < triNum; ++i) {
		triId[1] = 1 + i;
		triId[2] = 2 + i;
		triIdList.push_back(triId);
	}
}
Vec ArMesh::dPdu(cuint fid, cuint vid, CKeyIds & kId) const
{
	Vec n = normal(fid, vid, kId);
	Vec2 uve = uvEdgeVec(fid, 0);
	Vec  pe = pEdgeVec(fid, 0, kId);
	Vec vpe(pe.x, pe.y, pe.z);
	float angle = atan2f(uve.y, uve.x);
	Quat q;
	q.setAxisAngle(Vec(n.x, n.y, n.z), -angle);
	vpe = q.rotateVector(vpe);
	pe.setValue(vpe.x, vpe.y, vpe.z);
	pe = pe.cross(n);
	pe = n.cross(pe);
	pe.normalize();
	return pe;
}
Vec ArMesh::dPdv(cuint fid, cuint vid, CKeyIds & kId) const
{
	Vec n = normal(fid, vid, kId);
	Vec du = dPdu(fid, vid, kId).cross(n);
	du.normalize();
	return du;
}
void ArMesh::reset(const AtNode * pNode)
{
	nsides = AiNodeGetArray(pNode, AtString("nsides"));
	vidxs = AiNodeGetArray(pNode, AtString("vidxs"));
	vlist = AiNodeGetArray(pNode, AtString("vlist"));
	uvidxs = AiNodeGetArray(pNode, AtString("uvidxs"));
	uvlist = AiNodeGetArray(pNode, AtString("uvlist"));
	nidxs = AiNodeGetArray(pNode, AtString("nidxs"));
	nlist = AiNodeGetArray(pNode, AtString("nlist"));
	mtxlist = AiNodeGetArray(pNode, AtString("matrix"));

	mPointKeyNum = AiArrayGetNumKeys(vlist);
	mMtxKeyNum = AiArrayGetNumKeys(mtxlist);

	cuint polyNum = polygonNum();
	nsidesBegin = AiArrayAllocate(polyNum, 1, AI_TYPE_UINT);

	if (polyNum <= 1)
		return;

	AiArraySetUInt(nsidesBegin, 0, 0);
	for (uint i = 1; i < polyNum; ++i) {
		uint offset = AiArrayGetUInt(nsides, i - 1) + AiArrayGetUInt(nsidesBegin, i - 1);
		AiArraySetUInt(nsidesBegin, i, offset);
	}

}
float ArMesh::triArea(cuint fid, cuint vid0, cuint vid1, cuint vid2, CKeyIds & kId) const
{
	Vec p0, p1, p2, v0, v1;
	p0 = vertex(fid, vid0, kId);
	p1 = vertex(fid, vid1, kId);
	p2 = vertex(fid, vid2, kId);
	v0 = p1 - p0;
	v1 = p2 - p0;
	return v0.cross(v1).length() * 0.5f;
}
Vec ArMesh::pEdgeVec(cuint fid, cuint edgeId, CKeyIds & kId) const
{
	uint p0Id = 0, p1Id = (edgeId == 0) ? 1 : edgeId;

	Vec p0 = vertex(fid, p0Id, kId);
	Vec p1 = vertex(fid, p1Id, kId);
	return (p1 - p0).normalize();
}
Vec2 ArMesh::uvEdgeVec(cuint fid, cuint edgeId) const
{
	uint p0Id = 0, p1Id = (edgeId == 0) ? 1 : edgeId;

	Vec2 uv0 = uv(fid, p0Id);
	Vec2 uv1 = uv(fid, p1Id);
	Vec2 edgeUV = uv1 - uv0;
	float lenInv = 1.0f / edgeUV.length();
	edgeUV *= lenInv;
	return edgeUV;
}