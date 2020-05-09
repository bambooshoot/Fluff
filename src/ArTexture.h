#pragma once

#include <ExrTexture.h>

template<class T>
class TextureBase
{
public:
	TextureBase(const ArMultiExr* _pMultiExr) : pMultiExr(_pMultiExr), pp(0){};
	virtual T operator()(cfloat u, cfloat v) const = 0;

protected:
	const ArMultiExr* pMultiExr;
	PixelPicker* pp;
};

#define ARMULTIEXR_LAYER_DENSITY	0
#define ARMULTIEXR_LAYER_GRADIENT   1
#define ARMULTIEXR_LAYER_SPLIT		2
#define ARMULTIEXR_LAYER_NORMAL		3
#define ARMULTIEXR_LAYER_LENGTH		4
#define ARMULTIEXR_LAYER_DISPLACE	5
#define ARMULTIEXR_LAYER_WIDTH		6
#define ARMULTIEXR_LAYER_FUZZY		7
#define ARMULTIEXR_LAYER_CLUMP		8

#define TEXTURE_FLOAT_DEFINATION(TEX_FLOAT_CLASS, ARMULTIEXR_LAYER) \
class TEX_FLOAT_CLASS : public TextureBase<float> \
{ \
public: \
	TEX_FLOAT_CLASS(const ArMultiExr* _pMultiExr) : TextureBase<float>(_pMultiExr) { \
		pp = PixelPicker::create(pMultiExr->getImage(ARMULTIEXR_LAYER), pMultiExr->getPixelType(ARMULTIEXR_LAYER)); \
	} \
	float operator()(cfloat u, cfloat v) const override \
	{ \
		Pixel pix; \
		pix = pp->getPixel_Interp(u, v); \
		return pix.r; \
	} \
};

TEXTURE_FLOAT_DEFINATION(TextureDensity, ARMULTIEXR_LAYER_DENSITY)
TEXTURE_FLOAT_DEFINATION(TextureSplit, ARMULTIEXR_LAYER_SPLIT)
TEXTURE_FLOAT_DEFINATION(TextureLength, ARMULTIEXR_LAYER_LENGTH)
TEXTURE_FLOAT_DEFINATION(TextureDisplace, ARMULTIEXR_LAYER_DISPLACE)
TEXTURE_FLOAT_DEFINATION(TextureWidth, ARMULTIEXR_LAYER_WIDTH)
TEXTURE_FLOAT_DEFINATION(TextureFuzzy, ARMULTIEXR_LAYER_FUZZY)
TEXTURE_FLOAT_DEFINATION(TextureClump, ARMULTIEXR_LAYER_CLUMP)

#define TEXTURE_VEC_DEFINATION(TEX_VEC_CLASS, ARMULTIEXR_LAYER, VEC_TYPE) \
class TEX_VEC_CLASS : public TextureBase<VEC_TYPE> \
{ \
public: \
	TEX_VEC_CLASS(const ArMultiExr* _pMultiExr) : TextureBase<VEC_TYPE>(_pMultiExr) { \
		pp = PixelPicker::create(pMultiExr->getImage(ARMULTIEXR_LAYER), pMultiExr->getPixelType(ARMULTIEXR_LAYER)); \
	} \
	VEC_TYPE operator()(cfloat u, cfloat v) const override \
	{ \
		Pixel pix; \
		pix = pp->getPixel_Interp(u, v); \
		VEC_TYPE vec(pix.r, pix.g, pix.b); \
		return vec; \
	} \
};

TEXTURE_VEC_DEFINATION(TextureNormal, ARMULTIEXR_LAYER_NORMAL, Vec)

class TextureGradient : public TextureBase<Vec>
{
public:
	TextureGradient(const ArMultiExr* _pMultiExr) : TextureBase<Vec>(_pMultiExr) {
		pp = PixelPicker::create(pMultiExr->getImage(ARMULTIEXR_LAYER_GRADIENT), pMultiExr->getPixelType(ARMULTIEXR_LAYER_GRADIENT));
	}
	Vec operator()(cfloat u, cfloat v) const override
	{
		Pixel pix;
		pix = pp->getGradient(u, v);

		return Vec(pix.r, pix.g, pix.b);
	}
};

template<class T>
struct texture_data_type {};

#define TEXTURE_DATA_TYPE_FLT(TEX_OBJ) \
template<> \
struct texture_data_type<TEX_OBJ> { \
	static const int value = 0; \
};

TEXTURE_DATA_TYPE_FLT(TextureDensity)
TEXTURE_DATA_TYPE_FLT(TextureSplit)
TEXTURE_DATA_TYPE_FLT(TextureLength)
TEXTURE_DATA_TYPE_FLT(TextureDisplace)
TEXTURE_DATA_TYPE_FLT(TextureWidth)
TEXTURE_DATA_TYPE_FLT(TextureFuzzy)
TEXTURE_DATA_TYPE_FLT(TextureClump)

#define TEXTURE_DATA_TYPE_VEC(TEX_OBJ) \
template<> \
struct texture_data_type<TEX_OBJ> { \
	static const int value = 1; \
};

TEXTURE_DATA_TYPE_VEC(TextureNormal)
TEXTURE_DATA_TYPE_VEC(TextureGradient)