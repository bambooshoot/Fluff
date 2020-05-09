#include <MayaMesh.h>

MayaMesh::MayaMesh(const MFnMesh* pFnMesh) :m_pFnMesh(pFnMesh)
{
	pFnMesh->getVertices(vertexCountList, vertexIdList);
	int faceCount = vertexCountList.length();
	int curveVertexBegin = 0;
	for (int i = 0; i < faceCount; ++i) {
		vertexBeginIdList.append(curveVertexBegin);
		curveVertexBegin += vertexCountList[i];
	}

	pFnMesh->getTriangles(triangleCounts, triangleVertices);
	curveVertexBegin = 0;
	for (int i = 0; i < faceCount; ++i) {
		triangleVerticesBeginList.append(curveVertexBegin);
		curveVertexBegin += triangleCounts[i] * 3;
	}
}

uint MayaMesh::vertexId(cint face_idx, cint v_loc_idx) const
{
	return vertexIdList[vertexBeginIdList[face_idx] + v_loc_idx];
}

uint MayaMesh::pointNum() const
{
	return m_pFnMesh->numVertices();
}

uint MayaMesh::polygonNum() const
{
	return m_pFnMesh->numPolygons();
}

KeyIds MayaMesh::keyIds(cuint keyId) const
{
	return { 0,0 };
}

cuint MayaMesh::keyNum() const
{
	return 1;
}

Vec MayaMesh::point(cuint idx, CKeyIds& kId) const
{
	MPoint pos;
	m_pFnMesh->getPoint(idx, pos);
	return Vec(pos.x, pos.y, pos.z);
}

Vec MayaMesh::vertex(cuint fid, cuint vid, CKeyIds& kId) const
{
	return point(vertexId(fid, vid), kId);
}

Vec2 MayaMesh::uv(cuint fid, cuint vid) const
{
	int uvId;
	m_pFnMesh->getPolygonUVid(fid, vid, uvId);
	Vec2 uv;
	m_pFnMesh->getUV(uvId, uv.x, uv.y);
	return uv;
}

Vec MayaMesh::normal(cuint fid, cuint vid, CKeyIds& kId) const
{
	MVector n;
	m_pFnMesh->getFaceVertexNormal(fid, vid, n);
	return Vec(n.x, n.y, n.z);
}

void MayaMesh::getTriangles(cint fid, TriangleIdList& triIdList) const
{
	int beginId = triangleVerticesBeginList[fid];
	int i3 = 0;
	triIdList.clear();
	for (int i = 0; i < triangleCounts[fid]; ++i, i3 += 3) {
		TriangleId tid = { 
			triangleVertices[beginId + i3],
			triangleVertices[beginId + i3 + 1],
			triangleVertices[beginId + i3 + 2] 
		};

		triIdList.push_back(tid);
	}
}

void MayaMesh::getTrianglesWithArea(cint fid, TriangleIdList& triIdList, FloatList& areaList, CKeyIds& kId) const
{
	getTriangles(fid, triIdList);

	for (auto& triId : triIdList)
		areaList.push_back(triArea(fid, triId[0], triId[1], triId[2]));
}

Vec MayaMesh::dPdu(cuint fid, cuint vid, CKeyIds& kId) const
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

Vec MayaMesh::dPdv(cuint fid, cuint vid, CKeyIds& kId) const
{
	Vec n = normal(fid, vid, kId);
	Vec du = dPdu(fid, vid, kId).cross(n);
	du.normalize();
	return du;
}

Vec MayaMesh::pEdgeVec(cuint fid, cuint edgeId, CKeyIds& kId) const
{
	uint p0Id = 0, p1Id = (edgeId == 0) ? 1 : edgeId;

	Vec p0 = vertex(fid, p0Id, kId);
	Vec p1 = vertex(fid, p1Id, kId);
	return (p1 - p0).normalize();
}

Vec2 MayaMesh::uvEdgeVec(cuint fid, cuint edgeId) const
{
	uint p0Id = 0, p1Id = (edgeId == 0) ? 1 : edgeId;

	Vec2 uv0 = uv(fid, p0Id);
	Vec2 uv1 = uv(fid, p1Id);
	Vec2 edgeUV = uv1 - uv0;
	float lenInv = 1.0f / edgeUV.length();
	edgeUV *= lenInv;
	return edgeUV;
}

float MayaMesh::triArea(cuint fid, cuint vid0, cuint vid1, cuint vid2) const
{
	Vec p0, p1, p2, v0, v1;
	KeyIds kId = { 0,0 };
	p0 = vertex(fid, vid0, kId);
	p1 = vertex(fid, vid1, kId);
	p2 = vertex(fid, vid2, kId);
	v0 = p1 - p0;
	v1 = p2 - p0;
	return v0.cross(v1).length() * 0.5f;
}