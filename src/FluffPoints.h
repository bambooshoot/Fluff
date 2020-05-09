#pragma once

#include <FluffGeo.h>

class FluffPoints : public FluffGeo
{
public:
	FluffPoints(const FluffData* pData, cfloat _density) :
		FluffGeo(pData),
		density(_density) {}

	AtNode* operator ()(const AtNode* parentNode) const override;
	AtNode* virtualization(const AtNode* parentNode) const override;

private:
	float density;
};