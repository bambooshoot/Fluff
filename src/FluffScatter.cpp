#include <FluffScatter.h>
#include <vector>

template<typename NODE, typename MESH>
void FluffMeshScatter<NODE, MESH>::scatterPoints(FolliclePointList& spList, cfloat density) const
{
	MESH restFluffMesh(mpRestMeshNode);

	uint polygonNum = restFluffMesh.polygonNum();

	float baseDen = density;

	KeyIds keyIds = { 0,0 };// matrix and point's key ids which are zero for scattering point on rest mesh.

	UIntList        polyIdList;
	TriangleIdList  tIdList;
	FloatList		areaList;
	for (uint polyId = 0; polyId < polygonNum; ++polyId) {
		TriangleIdList	tIdList2;
		FloatList		areaList2;

		restFluffMesh.getTrianglesWithArea(polyId, tIdList2, areaList2, keyIds);

		for (auto& tId : tIdList2) {
			tIdList.push_back(tId);
			polyIdList.push_back(polyId);
		}

		for (auto& area : areaList2) {
			areaList.push_back(area);
		}
	}

	float areaSum = std::accumulate(areaList.begin(),areaList.end(),0.0f);
	uint pointNum = uint(density * areaSum);

	areaSum = 1.0f / areaSum;
	for (auto& weight : areaList)
		weight *= areaSum;

	for (auto iter = areaList.begin(); iter != areaList.end() - 1; ++iter)
		*(iter + 1) += *iter;

	areaList.back() = 1;
	
	auto& levelList = areaList;
	std::vector<std::optional<FolliclePoint>> optSpList;
	optSpList.resize(pointNum);
	tbb::parallel_for(tbb::blocked_range<size_t>(0, pointNum), [&](tbb::blocked_range<size_t>& range) {
		RealDistrib weightRange(0, 1);
		for (size_t i = range.begin(); i < range.end(); ++i) {
			uint seed = i + mpFluffData->seed;
			RandGen rand_generator, rand_den_gen, rand_value_gen;

			rand_generator.seed(seed+121);
			rand_den_gen.seed(seed+3125);
			rand_value_gen.seed(seed+10231);

			float probability = weightRange(rand_generator);
			auto iter = std::find_if(levelList.begin(), levelList.end(), [&](cfloat& a) {
				return probability < a;
				});

			uint levelId = uint(iter - levelList.begin());
			uint polyId = polyIdList[levelId];
			TriangleId tId = tIdList[levelId];

			Vec2 w2, w2_1;
			w2[0] = weightRange(rand_generator);
			w2[1] = weightRange(rand_generator);
			w2_1[0] = 1 - w2[0];
			w2_1[1] = 1 - w2[1];
			float w2L = w2.length();
			float w2L_1 = w2_1.length();
			if (w2L_1 < w2L) {
				w2[0] = w2_1[0];
				w2[1] = w2_1[1];
			}

			Vec2 euv0, euv1, uv0;
			uv0 = restFluffMesh.uv(polyId, tId[0]);
			euv0 = restFluffMesh.uv(polyId, tId[1]) - uv0;
			euv1 = restFluffMesh.uv(polyId, tId[2]) - uv0;

			FolliclePoint sp;
			sp.uv = uv0 + euv0 * w2[0] + euv1 * w2[1];

			// density
			float den = mpFluffData->den.value(sp.uv.x, sp.uv.y, rand_value_gen) * weightRange(rand_den_gen);

			// length
			sp.attr.setLength(mpFluffData, sp.uv.x, sp.uv.y, rand_value_gen);
			if (den <= mpFluffData->minDensity || sp.attr.length < 1e-10f)
				continue;

			sp.polyId = polyId;
			sp.triIds = tId;
			sp.triWeights = { w2.x, w2.y };

			sp.frameRest = frame(restFluffMesh, polyId, tId, w2, 0);

			sp.attr.setValue(mpFluffData, sp.uv.x, sp.uv.y, rand_value_gen);

			optSpList[i] = sp;
		}
		});

	for (auto& opt : optSpList) {
		if(opt.has_value())
			spList.push_back(opt.value());
	}
}

template<typename NODE, typename MESH>
PointFrame FluffMeshScatter<NODE, MESH>::frame(const MESH & fluffMesh, cuint polyId, const TriangleId & tId, const Vec2 & w2, cuint keyId) const
{
	Vec	e0, e1, p0,
		en0, en1, n0,
		dPdu0, edPdu1, edPdu0;

	PointFrame pf;

	CKeyIds keyIds = fluffMesh.keyIds(keyId);
	p0 = fluffMesh.vertex(polyId, tId[0], keyIds);
	e0 = fluffMesh.vertex(polyId, tId[1], keyIds) - p0;
	e1 = fluffMesh.vertex(polyId, tId[2], keyIds) - p0;
	pf.p = p0 + e0 * w2[0] + e1 * w2[1];

	n0 = fluffMesh.normal(polyId, tId[0], keyIds);
	en0 = fluffMesh.normal(polyId, tId[1], keyIds) - n0;
	en1 = fluffMesh.normal(polyId, tId[2], keyIds) - n0;
	pf.n = n0 + en0 * w2[0] + en1 * w2[1];
	pf.n.normalize();

	dPdu0 = fluffMesh.dPdu(polyId, tId[0], keyIds);
	edPdu0 = fluffMesh.dPdu(polyId, tId[1], keyIds) - dPdu0;
	edPdu1 = fluffMesh.dPdu(polyId, tId[2], keyIds) - dPdu0;
	
	pf.dPdu = dPdu0 + edPdu0 * w2[0] + edPdu1 * w2[1];
	pf.dPdu.normalize();

	pf.dPdv = pf.n.cross(pf.dPdu);
	pf.dPdv.normalize();

	return pf;
}

template<typename NODE, typename MESH>
void FluffMeshScatter<NODE, MESH>::relocate(FolliclePointList& spList) const
{
	MESH deformedFluffMesh(mpDeformedMeshNode);
	uint keyNum = deformedFluffMesh.keyNum();
	
	for (auto& sp : spList) {
		for (uint i = 0; i < keyNum; ++i) {
			sp.frames.push_back( frame(deformedFluffMesh, sp.polyId, sp.triIds, sp.triWeights, i) );
		}
	}
}

template<typename NODE, typename MESH>
FolliclePointList FluffMeshScatter<NODE, MESH>::operator()(cfloat density) const
{
	FolliclePointList spList;
	scatterPoints(spList, density);
	relocate(spList);
	return spList;
}