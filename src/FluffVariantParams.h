#pragma once

#include <fluffdef.h>
#include <ArTexture.h>

template <class TEX_OP, class T>
class VariantParamTex
{
public:
	VariantParamTex() :
		mValue(0),
		mRandRange(0.1f),
		mOffset(0)
	{
		valueFunc = &VariantParamTex<TEX_OP, T>::valueWithTex;
	};

	void setTex(const ArMultiExr* multiExr)
	{
		mTexOp.reset(new TEX_OP(multiExr));
	}
	void set(const float value,
		const float offset,
		const float randRange,
		const bool enableTex)
	{
		mValue = value;
		mOffset = offset;
		mRandRange = randRange;
		if (enableTex)
			valueFunc = &VariantParamTex<TEX_OP, T>::valueWithTex;
		else
			valueFunc = &VariantParamTex<TEX_OP, T>::valueWithoutTex;
	}
	T value(const float u, const float v, RandGen & randGen) const
	{
		return (this->*valueFunc)(u, v, randGen);
	}
	T valueWithTex(const float u, const float v, RandGen& randGen) const
	{
		if constexpr (texture_data_type<TEX_OP>::value == 0) {
			RealDistrib dist_n1p1(-1.0, 1.0);
			return mOffset + mValue * (1 + mRandRange * dist_n1p1(randGen)) * (*mTexOp)(u, v);
		}
		else
			return (*mTexOp)(u, v);
	}
	T valueWithoutTex(const float u, const float v, RandGen& randGen) const
	{
		if constexpr (texture_data_type<TEX_OP>::value == 0) {
			RealDistrib dist_n1p1(-1.0, 1.0);
			return mOffset + mValue * (1 + mRandRange * dist_n1p1(randGen));
		}
		else if constexpr (texture_data_type<TEX_OP>::value == 1)
			return T(0, 0, 1);
	}
	float baseValue() const
	{
		return mValue;
	}

private:
	float mValue, mRandRange, mOffset;
	std::shared_ptr<TEX_OP> mTexOp;
	T(VariantParamTex<TEX_OP, T>::* valueFunc)(const float u, const float v, RandGen & randGen) const;
};

template <class T>
class VariantParamFlt
{
public:
	VariantParamFlt() :
		mValue(0),
		mRandRange(0.1f),
		mOffset(0)
	{
	};
	void set(const float value,
		const float offset,
		const float randRange)
	{
		mValue = value;
		mOffset = offset;
		mRandRange = randRange;
	}
	T value(std::mt19937 &rand_gen) const
	{
		RealDistrib dist_n1p1(-1.0, 1.0);
		return mOffset + mValue * (1 + mRandRange * dist_n1p1(rand_gen));
	}
	float baseValue() const
	{
		return mValue;
	}

private:
	float mValue, mRandRange, mOffset;
};