#pragma once

#include <FluffArDef.h>
#include <FluffScatter.h>
#include <curve.h>
#include <FluffFactory.h>

#include <FluffCurveModiferRadius.h>
#include <FluffCurveModiferFuzzy.h>
#include <FluffCurveModiferSplit.h>
#include <FluffCurveModiferNormal.h>
#include <FluffCurveModiferClump.h>

#include <FluffCurvePostModiferCollisionFree.h>

class FluffGeo
{
public:
	FluffGeo(const FluffData * pData) :
		mpFluffData(pData) {}

	virtual AtNode * operator ()(const AtNode * parentNode) const = 0;
	virtual AtNode* virtualization(const AtNode* parentNode) const = 0;

protected:
	const FluffData * mpFluffData;
};