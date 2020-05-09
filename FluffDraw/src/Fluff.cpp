#include <Fluff.h>

#include <MayaHandy.h>

#include <maya/MFnNumericAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>

MTypeId Fluff::id(0x00090001);

MObject Fluff::mRestSkinMesh;
MObject Fluff::mDeformedSkinMesh;
MObject Fluff::mCollisionSkinMesh;
MObject Fluff::mRenderMode;
MObject Fluff::mSegmentNum;
MObject Fluff::mMinPixelWidth;
MObject Fluff::mSeed;
MObject Fluff::mCurveMode;
MObject Fluff::mUnitMode;
MObject Fluff::mEnableVisual;
MObject Fluff::mDensity;
MObject Fluff::mMinDensity;
MObject Fluff::mTexture;
MObject Fluff::mTextureEnable;
MObject Fluff::mLength;
MObject Fluff::mLengthRandom;
MObject Fluff::mLengthOffset;
MObject Fluff::mRingWidth;
MObject Fluff::mWidth;
MObject Fluff::mWidthRandom;
MObject Fluff::mMultiNum;
MObject Fluff::mMultiRadius;
MObject Fluff::mMultiRadiusRand;
MObject Fluff::mMultiLengthRand;
MObject Fluff::mClumpMethod;
MObject Fluff::mClumpDensity;
MObject Fluff::mClump;
MObject Fluff::mClumpOffset;
MObject Fluff::mSplit;
MObject Fluff::mSplitScale;
MObject Fluff::mSplitOffset;
MObject Fluff::mSplitUVScale;
MObject Fluff::mNormalRatio;
MObject Fluff::mNormalScale;
MObject Fluff::mNormalUVScale;
MObject Fluff::mFuzzy;
MObject Fluff::mFuzzyOctaves;
MObject Fluff::mFuzzyDistortion;
MObject Fluff::mFuzzyLacunarity;
MObject Fluff::mFuzzySeqFreq;
MObject Fluff::mFuzzyCurveFreq;
MObject Fluff::mFuzzyMin;
MObject Fluff::mFuzzyMax;
MObject Fluff::mCollision;
MObject Fluff::mSurfaceValue;
MObject Fluff::mVdbBandWidth;
MObject Fluff::mVdbVoxelSize;

MBoundingBox Fluff::boundingBox() const
{
	MBoundingBox bbox;
	return bbox;
}

void* Fluff::creator()
{
	return new Fluff();
}

void Fluff::draw(M3dView& view, const MDagPath& path,
	M3dView::DisplayStyle style, M3dView::DisplayStatus)
{
}

MStatus Fluff::initialize()
{
	MFnTypedAttribute   tAttr;
	MFnGenericAttribute nGAttr;
	MFnNumericAttribute	nAttr;
	MFnEnumAttribute	eAttr;
	
	mRestSkinMesh = tAttr.create("inRestSkinMesh", "rsm", MFnData::kMesh);
	mDeformedSkinMesh = tAttr.create("inDeformedSkinMesh", "dsm", MFnData::kMesh);
	mCollisionSkinMesh = tAttr.create("inCollisionSkinMesh", "csm", MFnData::kMesh);

	mRenderMode = eAttr.create("renderMode", "rmd");
	eAttr.addField("fur", 0);
	eAttr.addField("root", 1);
	eAttr.addField("clumpPoint", 2);

	mSegmentNum = nAttr.create("segment_num", "snm", MFnNumericData::kInt, 10);
	mMinPixelWidth = nAttr.create("min_pixel_width", "mpw", MFnNumericData::kDouble, 0.5f);
	mSeed = nAttr.create("seed", "sed", MFnNumericData::kInt, 0);
	
	mCurveMode = eAttr.create("curveMode", "cuv");
	eAttr.addField("robbin", 0);
	eAttr.addField("thick", 1);

	mUnitMode = eAttr.create("unitMode", "umd");
	eAttr.addField("line", 0);
	eAttr.addField("ring", 1);

	mEnableVisual = nAttr.create("enableVisual", "evl", MFnNumericData::kInt, 1);
	mDensity = nAttr.create("density", "den", MFnNumericData::kDouble, 1000.0);
	mMinDensity = nAttr.create("minDensity", "mdn", MFnNumericData::kDouble, 0.5);

	mTexture = nAttr.create("texture", "tex", MFnNumericData::kDouble, 0.5);
	mTextureEnable = nAttr.create("textureEnable", "tee", MFnNumericData::kInt, 0);

	// curve params
	mLength = nAttr.create("length", "len", MFnNumericData::kDouble, 0.010);
	mLengthRandom = nAttr.create("length_random","lrd",MFnNumericData::kDouble, 0.5);
	mLengthOffset = nAttr.create("length_offset", "lof", MFnNumericData::kDouble, 0.5);

	// ring
	mRingWidth = nAttr.create("ringWidth", "rwd", MFnNumericData::kDouble, 0.2);

	// width
	mWidth = nAttr.create("width", "wid", MFnNumericData::k2Double, 0.005);
	mWidthRandom = nAttr.create("width_random", "wrd", MFnNumericData::kDouble, 0.5);

	// multiplier
	mMultiNum = nAttr.create("multiNum", "mnm", MFnNumericData::kInt, 1);
	mMultiRadius = nAttr.create("multiRadius", "mrd", MFnNumericData::kDouble, 0.1);
	mMultiRadiusRand = nAttr.create("multiRadiusRand", "mrr", MFnNumericData::kDouble, 0.5);
	mMultiLengthRand = nAttr.create("multiLengthRand", "mlr", MFnNumericData::kDouble, 0.5);

	// modifier
	// clump
	mClumpMethod = eAttr.create("clumpMethod","clm");
	eAttr.addField("tree", 0);
	eAttr.addField("bruteForce", 1);

	mClumpDensity = nAttr.create("clumpDensity", "cde", MFnNumericData::kDouble, 100.0);
	mClump = nAttr.create("clump", "clm", MFnNumericData::kDouble, 1.0);
	mClumpOffset = nAttr.create("clumpOffset", "clf", MFnNumericData::kDouble, 0.0);

	// split
	mSplit = nAttr.create("split", "spl", MFnNumericData::kDouble, 1.0);
	mSplitScale = nAttr.create("splitScale", "sps", MFnNumericData::kDouble, 1.0);
	mSplitOffset = nAttr.create("splitOffset", "sos", MFnNumericData::kDouble, 0.0);
	mSplitUVScale = nAttr.create("splitUVScale", "suv", MFnNumericData::kDouble, 1.0);

	// normal
	mNormalRatio = nAttr.create("normalRatio", "nra", MFnNumericData::kDouble, 0.0);
	mNormalScale = nAttr.create("normalScale", "nsl", MFnNumericData::kDouble, 1.0);
	mNormalUVScale = nAttr.create("normalUVScale", "nuv", MFnNumericData::kDouble, 1.0);

	// fuzzy
	mFuzzy = nAttr.create("fuzzy", "fuz", MFnNumericData::kDouble, 0.2);
	mFuzzyOctaves = nAttr.create("fuzzy_octaves", "fuo", MFnNumericData::kInt, 8);
	mFuzzyDistortion = nAttr.create("fuzzy_distortion", "fud", MFnNumericData::kDouble, 1.0);
	mFuzzyLacunarity = nAttr.create("fuzzy_lacunarity", "ful", MFnNumericData::kDouble, 1.0);
	mFuzzySeqFreq = nAttr.create("fuzzy_seg_freq", "fus", MFnNumericData::kDouble, 1.0);
	mFuzzyCurveFreq = nAttr.create("fuzzy_curve_freq", "fuc", MFnNumericData::kDouble, 1.0);
	mFuzzyMin = nAttr.create("fuzzy_min", "fun", MFnNumericData::kDouble, 1.0);
	mFuzzyMax = nAttr.create("fuzzy_max", "fux", MFnNumericData::kDouble, 1.0);

	// collision free
	mCollision = nAttr.create("collision", "col", MFnNumericData::kDouble, 0.0);
	mSurfaceValue = nAttr.create("surfaceValue", "sur", MFnNumericData::kDouble, 0.0);
	mVdbBandWidth = nAttr.create("vdbBandWidth", "vbw", MFnNumericData::kDouble, 1.0);
	mVdbVoxelSize = nAttr.create("vdbVoxelSize", "vvs", MFnNumericData::kDouble, 0.2);

	//graph attribute
	GraphAttribCreate(mCurveWidthPosition, mCurveWidthValue, mCurveWidthInterp, mCurveWidth, "curveWidth", "cwh");
	GraphAttribCreate(mCurveNormalPosition, mCurveNormalValue, mCurveNormalInterp, mCurveNormal, "curveNormal", "cnl");
	GraphAttribCreate(mCurveSplitPosition, mCurveSplitValue, mCurveSplitInterp, mCurveSplit, "curveSplit", "csp");
	GraphAttribCreate(mCurveFuzzyPosition, mCurveFuzzyValue, mCurveFuzzyInterp, mCurveFuzzy, "curveFuzzy", "cfz");
	GraphAttribCreate(mCurveCollisionPosition, mCurveCollisionValue, mCurveCollisionInterp, mCurveCollision, "curveCollision", "ccn");
	GraphAttribCreate(mCurveClumpPosition, mCurveClumpValue, mCurveClumpInterp, mCurveClump, "curveClump", "ccl");
	GraphAttribCreate(mCurveMultiRadiusPosition, mCurveMultiRadiusValue, mCurveMultiRadiusInterp, mCurveMultiRadius, "curveMultiRadius", "cmr");

	return MS::kSuccess;
}

MStatus Fluff::compute(const MPlug& plug, MDataBlock& data)
{
	return MS::kSuccess;
}

void Fluff::GetPv2Data(FluffVp2Data& vp2Data)
{

}