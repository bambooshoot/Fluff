#pragma once

#include <FluffParams.h>

struct PointFrame
{
	Vec p, dPdu, dPdv, n;

	PointFrame() {
		p = Vec(0, 0, 0);
		dPdu = Vec(1, 0, 0);
		dPdv = Vec(0, 1, 0);
		n = Vec(0, 0, 1);
	}
	Matrix matrix() const {
		return Matrix(
			dPdu.x, dPdu.y, dPdu.z, 0,
			dPdv.x, dPdv.y, dPdv.z, 0,
			n.x,    n.y,    n.z,    0,
			p.x,    p.y,    p.z,    1
		);
	}
};

struct PointAttributes
{
	PointAttributes() :
		displace(0),
		length(1),
		width(1),
		fuzzy(0),
		split(0),
		clump(0)
	{
		grad.setValue(0, 0, 1);
		normal.setValue(0, 0, 1);
	}

	float	displace,
			length,
			width,
			split,
			fuzzy,
			clump;

	Vec	grad, normal;

	void setValue(const FluffData* fd, cfloat u, cfloat v, RandGen & randGen) {
		//displace = fd->disp.value(u, v);
		width = fd->width.value(u, v, randGen);
		fuzzy = fd->fuzzy.value(u, v, randGen);
		split = fd->split.value(u, v, randGen);
		grad = fd->grad.value(u, v, randGen);
		normal = fd->normal.value(u, v, randGen);
		clump = fd->clump.value(u, v, randGen);
	}
	void setLength(const FluffData* fd, cfloat u, cfloat v, RandGen& randGen) {
		length = fd->len.value(u, v, randGen);
	}
	Vec splitDir(const float uvsplitScale) const {
		Vec split_loc(grad.x * uvsplitScale, grad.y * uvsplitScale, sqrtf(grad.x * grad.x + grad.y * grad.y));
		split_loc.normalize();
		if (split_loc.length2() < 1e-20f)
			split_loc.setValue(0, 0, 1);

		return split_loc;
	}
	Vec normalDir(const float uvNormalScale) const {
		Vec normal_loc((normal.x - 0.5f) * 2.0f * uvNormalScale, (normal.y - 0.5f) * 2.0f * uvNormalScale, normal.z);
		if (normal.length2() < 1e-20f)
			normal_loc.setValue(0, 0, 1);

		return normal_loc;
	}
};

struct FolliclePoint
{
	FolliclePoint() :
		polyId(0)
	{
		uv.setValue(0, 0);
		triIds = { 0,0,0 };
		triWeights.setValue(0, 0);
	}

	uint					polyId;
	TriangleId				triIds;
	Vec2					triWeights;
	//PointFrame				frame0; // local space where curve generated
	PointFrame				frameRest;//rest world space where root point sampled
	std::vector<PointFrame> frames; // deformed world space where curve finally is
	Vec2					uv;
	PointAttributes			attr;
};

typedef std::vector<FolliclePoint> FolliclePointList;