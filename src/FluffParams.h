#pragma once

#include <fluffdef.h>
#include <ArTexture.h>
#include <curveAttr.h>
#include <FluffVariantParams.h>

enum class FluffRenderMode {
	root,
	fur,
	clumpPoint
};

#define IS_ENABLE_TEXTURE_FUNCTION(FUNC_NAME, ARMULTIEXR_LAYER) \
	inline bool FUNC_NAME(const uint textureEnables) { \
		return ( 1 << ARMULTIEXR_LAYER ) & textureEnables; \
	}

#define ENABLE_VIS_SHAPE		0
#define ENABLE_VIS_COLLISION	1

IS_ENABLE_TEXTURE_FUNCTION(isEnableVisShape, ENABLE_VIS_SHAPE)
IS_ENABLE_TEXTURE_FUNCTION(isEnableVisCollision, ENABLE_VIS_COLLISION)

IS_ENABLE_TEXTURE_FUNCTION(isEnableDensityTex, ARMULTIEXR_LAYER_DENSITY)
IS_ENABLE_TEXTURE_FUNCTION(isEnableGradientTex, ARMULTIEXR_LAYER_GRADIENT)
IS_ENABLE_TEXTURE_FUNCTION(isEnableSplitTex, ARMULTIEXR_LAYER_SPLIT)
IS_ENABLE_TEXTURE_FUNCTION(isEnableNormalTex, ARMULTIEXR_LAYER_NORMAL)
IS_ENABLE_TEXTURE_FUNCTION(isEnableLengthTex, ARMULTIEXR_LAYER_LENGTH)
IS_ENABLE_TEXTURE_FUNCTION(isEnableDisplaceTex, ARMULTIEXR_LAYER_DISPLACE)
IS_ENABLE_TEXTURE_FUNCTION(isEnableWidthTex, ARMULTIEXR_LAYER_WIDTH)
IS_ENABLE_TEXTURE_FUNCTION(isEnableFuzzyTex, ARMULTIEXR_LAYER_FUZZY)
IS_ENABLE_TEXTURE_FUNCTION(isEnableClumpTex, ARMULTIEXR_LAYER_CLUMP)

struct FluffData
{
	std::string			name;
	std::string			restSkinMesh, deformedSkinMesh, collisionSkinMesh;
	FluffRenderMode     mode;
	uint                seed;

	uint				segmentNum;
	float				min_pixel_width;
	float               minDensity;
	std::string         curveMode;

	float               ringWidth;

	//VariantParamTex<TextureDisplace, float>		disp;
	VariantParamTex<TextureDensity, float>			den;
	VariantParamTex<TextureLength, float>			len;
	VariantParamTex<TextureWidth, float>			width;

	VariantParamTex<TextureClump, float>			clump;
	float											clumpDensity;
	uint											clumpMethod;

	VariantParamTex<TextureGradient, Vec>			grad;
	VariantParamTex<TextureSplit, float>			split;
	float               splitScale;
	float               splitOffset;
	float				splitUVScale;

	VariantParamTex<TextureNormal, Vec>				normal;
	float               normalRatio;
	float				normalUVScale;
	
	VariantParamTex<TextureFuzzy, float>			fuzzy;

	int					fuzzy_octaves;
	float				fuzzy_distortion;
	float               fuzzy_lacunarity;
	float				fuzzy_seg_freq;
	float				fuzzy_curve_freq;
	float				fuzzy_min;
	float				fuzzy_max;

	float               collision;
	float               surfaceValue;

	float				vdbBandWidth;
	float               vdbVoxelSize;

	bool                visShape;
	bool				visCollision;
	bool				exrFileExists;

	// curve multiplier
	CurveAttrF				curveMultiRadius;
	VariantParamFlt<float>	multiRadius;
	uint					multiNum;
	float					multiLengthRand;

	NodeId              curveUnitMode;

	CurveAttrF			curveWidth;
	CurveAttrF			curveNormal;
	CurveAttrF			curveSplit;
	CurveAttrF			curveFuzzy;
	CurveAttrF			curveCollision;
	CurveAttrF          curveClump;

	std::shared_ptr<ArMultiExr>	multiExr;

	FluffData(const char* texPathName) :
		mode(FluffRenderMode::fur),
		segmentNum(10),
		min_pixel_width(1.0f),
		minDensity(0.5f),
		normalUVScale(1.0f),
		normalRatio(0.0f),
		splitOffset(0),
		splitScale(1),
		splitUVScale(1.0f),
		name(""),
		restSkinMesh(""),
		deformedSkinMesh(""),
		fuzzy_octaves(8),
		fuzzy_distortion(1),
		fuzzy_lacunarity(1.92f),
		fuzzy_seg_freq(10),
		fuzzy_curve_freq(4),
		fuzzy_min(0.2f),
		fuzzy_max(0.8f),
		curveMode("robbin"),
		vdbBandWidth(1.0f),
		vdbVoxelSize(0.2f),
		collision(1.0f),
		surfaceValue(0.0f),
		visShape(true),
		visCollision(false),
		clumpDensity(100.0f),
		clumpMethod(1),
		multiNum(1),
		curveUnitMode(0),
		ringWidth(0.2f),
		seed(0)
	{
		exrFileExists = _access_s(texPathName, 0) == 0;
		std::string exrFile(texPathName);
		std::regex r(".exr",std::regex::icase);
		exrFileExists &= std::regex_search(exrFile, r);
		if (!exrFileExists)
			return;

		multiExr.reset(new ArMultiExr(texPathName));
		auto pExr = &*multiExr;
		//disp.setTex(pExr);
		den.setTex(pExr);
		len.setTex(pExr);
		width.setTex(pExr);
		split.setTex(pExr);
		grad.setTex(pExr);
		normal.setTex(pExr);
		fuzzy.setTex(pExr);
		clump.setTex(pExr);
	}
	void setRenderMode(const char* renderMode)
	{
		if ( strcmp(renderMode, "root") == 0 )
			mode = FluffRenderMode::root;
		else if ( strcmp(renderMode, "fur") == 0 )
			mode = FluffRenderMode::fur;
		else
			mode = FluffRenderMode::clumpPoint;
	}
	void setUnitMode(const char* unitMode)
	{
		if ( strcmp(unitMode, "line") == 0)
			curveUnitMode = 1;
		else if ( strcmp(unitMode, "ring") == 0 )
			curveUnitMode = 2;
	}
};