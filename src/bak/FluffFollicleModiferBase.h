#pragma once

#include <FluffParams.h>
#include <FluffFolliclePoint.h>

class FluffFollicleModiferBase
{
public:
	virtual ~FluffFollicleModiferBase() {};
	virtual void operator()(FolliclePoint& fp, const FluffData *fluffData) = 0;
};