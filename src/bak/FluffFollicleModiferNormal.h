#pragma once

#include <FluffFollicleModiferBase.h>

class FluffFollicleModiferNormal : public FluffFollicleModiferBase
{
public:
	~FluffFollicleModiferNormal() override {}
	void operator()(FolliclePoint& fp, const FluffData* fluffData) override
	{
		if (fluffData->normalRatio == 0)
			return;

		auto normal = fp.attr.normalDir(fp.frame0, fluffData->normalUVScale);
		fp.frame0.n = fp.frame0.n * (1 - fluffData->normalRatio) + normal * fluffData->normalRatio;
	}
	static FluffFollicleModiferBase* create() {
		return new FluffFollicleModiferNormal();
	}
	static NodeId id;
};