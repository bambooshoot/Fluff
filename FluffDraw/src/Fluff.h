#pragma once

#include <maya/MTypes.h>
#include <maya/MTypeId.h> 
#include <maya/MPxLocatorNode.h> 

struct FluffVp2Data {

};

class Fluff : public MPxLocatorNode
{
public:
	Fluff() {};
	virtual	~Fluff() {};
	virtual bool isBounded() const { return true; };
	virtual MBoundingBox boundingBox() const;

	virtual void draw(M3dView& view, const MDagPath& path,
		M3dView::DisplayStyle style, M3dView::DisplayStatus);

	static  void*		creator();
	static  MStatus     initialize();

	virtual MStatus		compute(const MPlug& plug, MDataBlock& data);

	void GetPv2Data(FluffVp2Data& vp2Data);

public:
	static	MTypeId		id;
	static  MString     drawDbClassification, drawRegistrantId;

	static MObject mRestSkinMesh, mDeformedSkinMesh, mCollisionSkinMesh,
		mRenderMode, mSegmentNum, mMinPixelWidth, mSeed, mCurveMode, mUnitMode,
		mEnableVisual, mDensity, mMinDensity, mTexture, mTextureEnable,
		mLength, mLengthRandom, mLengthOffset, mRingWidth, mWidth, mWidthRandom,
		mMultiNum, mMultiRadius, mMultiRadiusRand, mMultiLengthRand,
		mClumpMethod, mClumpDensity, mClump, mClumpOffset,
		mSplit, mSplitScale, mSplitOffset, mSplitUVScale,
		mNormalRatio, mNormalScale, mNormalUVScale,
		mFuzzy, mFuzzyOctaves, mFuzzyDistortion, mFuzzyLacunarity, mFuzzySeqFreq, mFuzzyCurveFreq, mFuzzyMin, mFuzzyMax,
		mCollision, mSurfaceValue, mVdbBandWidth, mVdbVoxelSize,

		mCurveWidth, mCurveWidthPosition, mCurveWidthValue, mCurveWidthInterp,
		mCurveNormalPosition, mCurveNormalValue, mCurveNormalInterp, mCurveNormal,
		mCurveSplitPosition, mCurveSplitValue, mCurveSplitInterp, mCurveSplit,
		mCurveFuzzyPosition, mCurveFuzzyValue, mCurveFuzzyInterp, mCurveFuzzy,
		mCurveCollisionPosition, mCurveCollisionValue, mCurveCollisionInterp, mCurveCollision,
		mCurveClumpPosition, mCurveClumpValue, mCurveClumpInterp, mCurveClump,
		mCurveMultiRadiusPosition, mCurveMultiRadiusValue, mCurveMultiRadiusInterp, mCurveMultiRadius;
};