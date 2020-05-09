#include <FluffPoints.h>
#include <FluffCurves.h>
#include <sstream>
#include <vector>

AI_PROCEDURAL_NODE_EXPORT_METHODS(ProceduralMtd);

static const char* primtiveModes[] = { "fur", "root", "clumpPoint", NULL };
static const char* curveModes[] = { "robbin", "thick", NULL };
static const char* clumpMethods[] = { "tree", "bruteForce", NULL };
static const char* unitModes[] = { "line", "ring", NULL };

static const char* curveAttrNames[] = {
		"curve_width",
		"curve_normal",
		"curve_split",
		"curve_fuzzy",
		"curve_collision",
		"curve_clump",
		"curve_multiRadius"
};

node_parameters
{
	// scatter params
	AiParameterStr("rest_skin_mesh", "" );
	AiParameterStr("deformed_skin_mesh", "");
	AiParameterStr("collision_skin_mesh", ""); 
	AiParameterInt("segment_num", 10);
	AiParameterFlt("min_pixel_width", 1.0f);
	AiParameterInt("seed", 0);
	AiParameterEnum("curveMode", 0, curveModes);
	AiParameterEnum("renderMode", 0, primtiveModes);
	AiParameterEnum("unitMode", 0, unitModes);
	
	AiParameterUInt("enableVisual", 1);

	AiParameterFlt("density", 1000.f);
	AiParameterFlt("minDensity", 0.5f);

	AiParameterStr("texture", "");
	AiParameterUInt("textureEnable", 0);

	// curve params
	AiParameterFlt("length", 0.010f);
	AiParameterFlt("length_random", 0.5f);
	AiParameterFlt("length_offset", 0.0f);

	// ring
	AiParameterFlt("ringWidth", 0.2f);

	// width
	AiParameterFlt("width", 0.005f);
	AiParameterFlt("width_random", 0.5f);

	// multiplier
	AiParameterUInt("multiNum", 1);
	AiParameterFlt("multiRadius", 0.1f);
	AiParameterFlt("multiRadiusRand", 0.5f);
	AiParameterFlt("multiLengthRand", 0.5f);

	// modifier
	// clump
	AiParameterEnum("clumpMethod", 0, clumpMethods);
	AiParameterFlt("clumpDensity", 100.0f);
	AiParameterFlt("clump", 1.0f);
	AiParameterFlt("clumpOffset", 0.0f);

	// split
	AiParameterFlt("split", 1.0f);
	AiParameterFlt("splitScale", 1.0f);
	AiParameterFlt("splitOffset", 0.0f); 
	AiParameterFlt("splitUVScale", 1.0f);

	// normal
	AiParameterFlt("normalRatio", 0.0f);
	AiParameterFlt("normalScale", 1.0f);
	AiParameterFlt("normalUVScale", 1.0f);

	// fuzzy
	AiParameterFlt("fuzzy", 0.2f);
	AiParameterInt("fuzzy_octaves", 8);
	AiParameterFlt("fuzzy_distortion", 1);
	AiParameterFlt("fuzzy_lacunarity", 1.92f);
	AiParameterFlt("fuzzy_seg_freq", 5.0f);
	AiParameterFlt("fuzzy_curve_freq", 2.0f);
	AiParameterFlt("fuzzy_min", 0.2f);
	AiParameterFlt("fuzzy_max", 0.8f);

	// collision free
	AiParameterFlt("collision", 0.0f);
	AiParameterFlt("surfaceValue", 0.0f);
	AiParameterFlt("vdbBandWidth", 1.0f);
	AiParameterFlt("vdbVoxelSize", 0.2f);

	for (auto attrName : curveAttrNames) {
		AtArray* a_array = AiArrayAllocate(0, 1, AI_TYPE_VECTOR);
		AiParameterArray(attrName, a_array);
	}
}

procedural_init
{
	AtString texPathName = AiNodeGetStr(node, "texture");
	FluffData *pData = new FluffData(texPathName);
	*user_ptr = pData;

	pData->name = AiNodeGetStr(node, "name");
	pData->restSkinMesh = AiNodeGetStr(node, "rest_skin_mesh");
	pData->deformedSkinMesh = AiNodeGetStr(node, "deformed_skin_mesh");
	pData->collisionSkinMesh = AiNodeGetStr(node, "collision_skin_mesh");
	pData->min_pixel_width = AiNodeGetFlt(node, "min_pixel_width");
	pData->seed = AiNodeGetInt(node, "seed");

	pData->normalRatio = AiNodeGetFlt(node, "normalRatio");

	pData->curveMode = AiNodeGetStr(node, "curveMode");
	pData->collision = AiNodeGetFlt(node, "collision");
	pData->surfaceValue = AiNodeGetFlt(node, "surfaceValue");

	pData->vdbVoxelSize = AiNodeGetFlt(node, "vdbVoxelSize");
	pData->vdbBandWidth = AiNodeGetFlt(node, "vdbBandWidth");

	uint textureEnables = AiNodeGetUInt(node, "textureEnable");
	if (!pData->exrFileExists)
		textureEnables = 0;

	uint enableVisual = AiNodeGetUInt(node, "enableVisual");

	pData->visShape = isEnableVisShape(enableVisual);
	pData->visCollision = isEnableVisCollision(enableVisual);

	pData->ringWidth = AiNodeGetFlt(node, "ringWidth");

	pData->multiNum = AiNodeGetUInt(node, "multiNum");
	pData->multiLengthRand = AiNodeGetFlt(node, "multiLengthRand");
	pData->multiRadius.set(AiNodeGetFlt(node, "multiRadius"), 0, AiNodeGetFlt(node, "multiRadiusRand"));

	pData->clumpDensity = AiNodeGetFlt(node, "clumpDensity");
	pData->clump.set(AiNodeGetFlt(node, "clump"), AiNodeGetFlt(node, "clumpOffset"), 0, isEnableClumpTex(textureEnables));
	AtString clumpMethod = AiNodeGetStr(node, "clumpMethod");
	pData->clumpMethod = (clumpMethod == AtString("tree")) ? 1 : 0;
	
	pData->normal.set(AiNodeGetFlt(node, "normalScale"), 0, 0, isEnableNormalTex(textureEnables));
	pData->normalUVScale = AiNodeGetFlt(node, "normalUVScale");

	float density = AiNodeGetFlt(node, "density");

	pData->minDensity = AiNodeGetFlt(node, "minDensity") * density;
	
	pData->splitScale = AiNodeGetFlt(node, "splitScale");
	pData->splitOffset = AiNodeGetFlt(node, "splitOffset");
	pData->splitUVScale = AiNodeGetFlt(node, "splitUVScale");
	pData->split.set(AiNodeGetFlt(node, "split"),
		0, 
		0, 
		isEnableSplitTex(textureEnables));

	pData->grad.set(1, 0, 0, isEnableGradientTex(textureEnables));

	pData->den.set(density, 0, 0, isEnableDensityTex(textureEnables));
	pData->len.set(AiNodeGetFlt(node, "length"), AiNodeGetFlt(node, "length_offset"), AiNodeGetFlt(node, "length_random"), isEnableLengthTex(textureEnables));
	pData->segmentNum = AiNodeGetInt(node, "segment_num");

	float widthRandom = AiNodeGetFlt(node, "width_random");
	pData->width.set(AiNodeGetFlt(node, "width"), 0, widthRandom, isEnableWidthTex(textureEnables));

	pData->fuzzy.set(AiNodeGetFlt(node, "fuzzy"), 0, 0, isEnableFuzzyTex(textureEnables));
	pData->fuzzy_octaves = AiNodeGetInt(node, "fuzzy_octaves");
	pData->fuzzy_distortion = AiNodeGetFlt(node, "fuzzy_distortion");
	pData->fuzzy_lacunarity = AiNodeGetFlt(node, "fuzzy_lacunarity");
	pData->fuzzy_seg_freq = AiNodeGetFlt(node, "fuzzy_seg_freq");
	pData->fuzzy_curve_freq = AiNodeGetFlt(node, "fuzzy_curve_freq");
	pData->fuzzy_min = AiNodeGetFlt(node, "fuzzy_min");
	pData->fuzzy_max = AiNodeGetFlt(node, "fuzzy_max");

	CurveAttrF* cafList[] = {
		&pData->curveWidth,
		&pData->curveNormal,
		&pData->curveSplit,
		&pData->curveFuzzy,
		&pData->curveCollision,
		&pData->curveClump,
		&pData->curveMultiRadius
	};

	uint ci = 0;
	for (auto attrName : curveAttrNames) {
		AtArray* curve_array = AiNodeGetArray(node, attrName);
		uint dataNum = AiArrayGetNumElements(curve_array);
		auto caf = cafList[ci];
		AtVector *vList = (AtVector*)AiArrayMap(curve_array);
		for (uint i = 0; i < dataNum; ++i) {
			caf->push(vList[i].y, vList[i].x);
		}
		AiArrayUnmap(curve_array);
		++ci;
	}
	
	AtString renderMode = AiNodeGetStr(node, "renderMode");
	pData->setRenderMode(renderMode.c_str());

	AtString unitMode = AiNodeGetStr(node, "unitMode");
	if (unitMode == AtString("line"))
		pData->curveUnitMode = 1;
	else if (unitMode == AtString("ring"))
		pData->curveUnitMode = 2;

	return true;
}

procedural_cleanup
{
	FluffData *pData = (FluffData*)user_ptr;
	delete pData;
	return true;
}

procedural_num_nodes
{
	// return how many nodes to generate
	const FluffData* pData = (const FluffData*)user_ptr;

	int nodeNum = 0;
	AtNode* pNode = 0;
	switch (pData->mode) {
	case FluffRenderMode::root:
	case FluffRenderMode::clumpPoint:
		nodeNum = 1;
		break;
	case FluffRenderMode::fur:
		nodeNum += pData->visShape;
		nodeNum += pData->visCollision;
		break;
	}

	return nodeNum;
}

procedural_get_node
{
	FluffData *pData = (FluffData*)user_ptr;

	AtNode* pNode=0;
	if (i == 0 && pData->visShape) {
		switch (pData->mode) {
		case FluffRenderMode::root: 
		{
			pNode = FluffPoints(pData, pData->den.baseValue())(node);
			AiMsgInfo("Fluff: FluffPoints root mode");
		}
			break;
		case FluffRenderMode::fur:
		{
			auto fluffCurve = FluffCurves(pData);
			pNode = fluffCurve(node);
			AiMsgInfo("Fluff: FluffCurves fur mode");
		}
			break;
		case FluffRenderMode::clumpPoint:
		{
			pNode = FluffPoints(pData, pData->clumpDensity)(node);
			AiMsgInfo("Fluff: FluffPoints clump mode");
		}
			break;
		}
	}
	else if ((i == 1 || i == 0) && pData->visCollision) {
		if (pData->mode == FluffRenderMode::fur) {
			auto fluffCurve = FluffCurves(pData);
			pNode = fluffCurve.virtualization(node);
		}
	}

	return pNode;
}