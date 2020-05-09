#include <fluffDrawOverride.h>
#include <string>
#include <map>
#include <maya/MString.h>
#include <maya/MPlug.h>

template<typename T>
struct FluffParamSetter {
	template<typename T, typename std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	void operator()(MFnDependencyNode& fnNode, T* pValue, const char* attr) {
		MPlug plug = fnNode.findPlug(MString(attr));
		*pValue = (T)plug.asDouble();
	};

	template<typename T, typename std::enable_if_t<std::is_same_v<T, std::string>, int> = 0>
	void operator()(MFnDependencyNode& fnNode, T* pValue, const char* attr) {
		MPlug plug = fnNode.findPlug(MString(attr));
		*pValue = plug.asChar();
	};

	template<typename T, typename std::enable_if_t<std::is_integral_v<T>, int> = 0 >
	void operator()(MFnDependencyNode& fnNode, T* pValue, const char* attr) {
		MPlug plug = fnNode.findPlug(MString(attr));
		*pValue = plug.asInt();
	};
};

template<template <typename> class T, typename T1>
struct FluffParamSetter<T<T1>> {
	template<template <typename> class T, typename T1, typename std::enable_if_t<std::is_same_v<T<T1>, VariantParamFlt<float>>, int> = 0 >
	void operator()(MFnDependencyNode& fnNode, T<T1>* pValue, std::initializer_list<const char*> attr) {

		std::vector<const char*> mAttrNameList(attr);

		float value[3] = { 0 };
		for (int i = 0; i < 3; ++i) {
			if (strcmp(mAttrNameList[i], "") == 0)
				value[i] = fnNode.findPlug(MString(mAttrNameList[i])).asFloat();
		}
		pValue->set(value[0], value[1], value[2]);
	};
};

template< template<typename, typename> class T, typename T1, typename T2>
struct FluffParamSetter<T<T1,T2>> {
	template<template<typename, typename> class T, typename T1, typename T2, typename std::enable_if_t<std::is_same_v<T<T1,T2>, CurveAttrF>, int> = 0>
	void operator()(MFnDependencyNode& fnNode, T<T1, T2>* pValue, const char* attr) {
		std::vector<std::string> mAttrNameList;
		MPlug plug = fnNode.findPlug(MString(attr));
		uint dataNum = plug.numElements();
		for (uint i = 0; i < dataNum; ++i) {
			MPlug plug1 = plug.elementByPhysicalIndex(i);
			pValue->push(plug1.child(0).asFloat(), plug1.child(1).asFloat());
		}
	}

	template<template<typename, typename> class T, typename T1, typename T2, typename std::enable_if_t<std::is_same_v<T<T1,T2>,VariantParamTex<T1, T2>>, int> = 0 >
	void operator()(MFnDependencyNode& fnNode, T<T1,T2>* pValue, std::initializer_list<const char*> attr, bool texEnable) {

		std::vector<const char*> mAttrNameList(attr);

		float value[3] = {0};
		for (int i = 0; i < 3; ++i) {
			if (strcmp(mAttrNameList[i], "") == 0)
				value[i] = (float)fnNode.findPlug(MString(mAttrNameList[i])).asDouble();
		}
		pValue->set(value[0], value[1], value[2], texEnable);
	};
};

template< template<typename, typename> class T, typename T1, typename T2>
void FluffParamSetTex(MFnDependencyNode &fnNode, T<T1,T2>& data, const char* baseValue, const char* offsetValue, const char* randValue, bool enableTex)
{
	FluffParamSetter<T<T1, T2>> fluffParamSetter;
	fluffParamSetter(fnNode, &data, { baseValue, offsetValue, randValue }, enableTex);
}

void FluffDrawOverride::updateDG()
{
	MFnDependencyNode fnNode(*mpFluff);

	MString texPathName = fnNode.findPlug("texture").asString();
	mpFluffData = new FluffData(texPathName.asChar());

#define FLUFFPARAMSET(VAR) \
{ \
	FluffParamSetter<decltype(mpFluffData->VAR)> fluffParamSetter; \
	fluffParamSetter(fnNode, &mpFluffData->VAR,  #VAR); \
}

	FLUFFPARAMSET(restSkinMesh)
	FLUFFPARAMSET(deformedSkinMesh)
	FLUFFPARAMSET(collisionSkinMesh)
	FLUFFPARAMSET(seed)
	FLUFFPARAMSET(segmentNum)
	FLUFFPARAMSET(min_pixel_width)
	FLUFFPARAMSET(minDensity)
	FLUFFPARAMSET(curveMode)
	FLUFFPARAMSET(ringWidth)
	FLUFFPARAMSET(clumpDensity)
	FLUFFPARAMSET(clumpMethod)
	FLUFFPARAMSET(splitScale)
	FLUFFPARAMSET(splitOffset)
	FLUFFPARAMSET(splitUVScale)
	FLUFFPARAMSET(normalRatio)
	FLUFFPARAMSET(normalUVScale)
	FLUFFPARAMSET(fuzzy_octaves)
	FLUFFPARAMSET(fuzzy_distortion)
	FLUFFPARAMSET(fuzzy_lacunarity)
	FLUFFPARAMSET(fuzzy_seg_freq)
	FLUFFPARAMSET(fuzzy_curve_freq)
	FLUFFPARAMSET(fuzzy_min)
	FLUFFPARAMSET(fuzzy_max)
	FLUFFPARAMSET(collision)
	FLUFFPARAMSET(surfaceValue)
	FLUFFPARAMSET(vdbBandWidth)
	FLUFFPARAMSET(vdbVoxelSize)
	FLUFFPARAMSET(visShape)
	FLUFFPARAMSET(visCollision)
	FLUFFPARAMSET(exrFileExists)

	FLUFFPARAMSET(curveWidth)
	FLUFFPARAMSET(curveNormal)
	FLUFFPARAMSET(curveSplit)
	FLUFFPARAMSET(curveFuzzy)
	FLUFFPARAMSET(curveCollision)
	FLUFFPARAMSET(curveClump)

	uint enableTexMask = (uint)fnNode.findPlug("textureEnable").asInt();
	FluffParamSetTex(fnNode, mpFluffData->den, "density", "", "", isEnableDensityTex(enableTexMask));
	FluffParamSetTex(fnNode, mpFluffData->len, "length", "length_offset", "length_random", isEnableLengthTex(enableTexMask));
	FluffParamSetTex(fnNode, mpFluffData->width, "width", "", "width_random", isEnableWidthTex(enableTexMask));
	FluffParamSetTex(fnNode, mpFluffData->fuzzy, "fuzzy", "", "", isEnableFuzzyTex(enableTexMask));
	FluffParamSetTex(fnNode, mpFluffData->normal, "normalScale", "", "", isEnableNormalTex(enableTexMask));
	FluffParamSetTex(fnNode, mpFluffData->split, "split", "", "", isEnableSplitTex(enableTexMask));
	FluffParamSetTex(fnNode, mpFluffData->clump, "clump", "clumpOffset", "", isEnableClumpTex(enableTexMask));

	mpFluffData->grad.set(1, 0, 0, isEnableGradientTex(enableTexMask));

{
	FluffParamSetter<decltype(mpFluffData->multiRadius)> fluffParamSetter;
	fluffParamSetter(fnNode, &mpFluffData->multiRadius, { "multiRadius", "", "multiRadiusRand" });
}

}

void FluffDrawOverride::updateRenderItems(const MDagPath& path,
	MHWRender::MRenderItemList& list)
{
}

void FluffDrawOverride::populateGeometry(const MHWRender::MGeometryRequirements& requirements,
	const MHWRender::MRenderItemList& renderItems,
	MHWRender::MGeometry& data)
{
}

void FluffDrawOverride::updateSelectionGranularity(const MDagPath& path,
	MHWRender::MSelectionContext& selectionContext)
{
}