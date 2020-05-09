#pragma once

#include <random>

#include <FluffParams.h>
#include <FluffFolliclePoint.h>
#include <ArMesh.h>

template<typename NODE, typename MESH>
class FluffMeshScatter
{
public:
	FluffMeshScatter(
		const NODE* pRestNode,
		const NODE* pDeformedNode,
		const FluffData * pData ) :

		mpRestMeshNode(pRestNode),
		mpDeformedMeshNode(pDeformedNode),
		mpFluffData(pData) {};

	FolliclePointList operator()(cfloat density) const;

private:
	const NODE* mpRestMeshNode;
	const NODE* mpDeformedMeshNode;
	const FluffData	* mpFluffData;

	PointFrame frame(const MESH& fluffMesh, cuint polyId, const TriangleId& tId, const Vec2& w2, cuint keyId) const;
	void scatterPoints(FolliclePointList& spList, cfloat density) const;
	void relocate(FolliclePointList& spList) const;
};