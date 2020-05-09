#pragma once

#include <FluffDef.h>
#include <FluffParams.h>
#include <ArMesh.h>

class FluffCurvePostModiferBase
{
public:
	virtual ~FluffCurvePostModiferBase() {};
	virtual void operator()(AtNode* pCurveNode, const ArMesh &deformedFluffMesh, const FluffData* pFluffData) = 0;
	virtual AtNode* visualize(const AtNode* parentNode, const ArMesh& deformedFluffMesh, const FluffData* pFluffData) = 0;
};