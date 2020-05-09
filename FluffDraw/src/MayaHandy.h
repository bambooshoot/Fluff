#pragma once

#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMatrixData.h>

#include <maya/MHWGeometry.h>
#include <maya/MShaderManager.h>
#include <maya/MFragmentManager.h>

#include <maya/MStatus.h>
#include <maya/MRampAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MRampAttribute.h>
#include <maya/MString.h>
#include <maya/MFloatArray.h>
#include <maya/MIntArray.h>
#include <maya/MGlobal.h>
#include <maya/MTime.h>

#include <vector>

inline double FrameTime()
{
	auto unit = MTime::uiUnit();
	double fps;
	switch (unit) {
	case MTime::k24FPS:
		fps = 24.0;
		break;
	case MTime::k25FPS:
		fps = 25.0;
		break;
	case MTime::k30FPS:
		fps = 30.0;
		break;
	}
	return 1.0f / fps;
}

inline bool ConnectedNodeFindPlug(MPlug & attrPlug, MPlug & inputPlug, MObject & attrObj)
{
	MPlugArray plugArray;
	bool connectedFlag = inputPlug.connectedTo(plugArray, 1, 0);
	if (connectedFlag) {
		MObject nodeObj = plugArray[0].node();
		MPlug foundAttrPlug(nodeObj, attrObj);
		attrPlug = foundAttrPlug;
	}
	return connectedFlag;
}


inline float GraphAttribValue(const float u, MObject & thisMObj, MObject & mWeight)
{
	MStatus status;
	MRampAttribute rampAttr(thisMObj, mWeight, &status);
	float fw;
	rampAttr.getValueAtPosition(u, fw, &status);
	return fw;
}

inline void GraphAttribCreate(MObject & mWeightPosition,
	MObject & mWeightValue,
	MObject & mWeightInterp,
	MObject & mWeight,
	const MString weightStr,
	const MString weightShortStr)
{
	const char* sInterpNone = "None";
	const char* sInterpLinear = "Linear";
	const char* sInterpSmooth = "Smooth";
	const char* sInterpSpline = "Spline";

	MFnEnumAttribute		enumAttrFn;
	MFnCompoundAttribute	compAttr;
	MFnNumericAttribute		nAttr;

	mWeightPosition = nAttr.create(weightStr + "_Position", weightShortStr + "p", MFnNumericData::kFloat, 0.0f);
	mWeightValue = nAttr.create(weightStr + "_FloatValue", weightShortStr + "v", MFnNumericData::kFloat, 0.0f);
	mWeightInterp = enumAttrFn.create(weightStr + "_Interp", weightShortStr + "i");
	enumAttrFn.addField(sInterpNone, 0);
	enumAttrFn.addField(sInterpLinear, 1);
	enumAttrFn.addField(sInterpSmooth, 2);
	enumAttrFn.addField(sInterpSpline, 3);
	enumAttrFn.setDefault(3);

	mWeight = compAttr.create(weightStr, weightShortStr);
	compAttr.addChild(mWeightPosition);
	compAttr.addChild(mWeightValue);
	compAttr.addChild(mWeightInterp);
	compAttr.setArray(true);
	compAttr.setStorable(true);

	compAttr.setUsesArrayDataBuilder(true);
}

inline MMatrix WorldMatrixFromShapePlug(MPlug & inHeadMeshPlug)
{
	MPlugArray plugArray;
	MMatrix worldMatrix;
	if (inHeadMeshPlug.connectedTo(plugArray, 1, 0)) {
		MFnDagNode fnMeshDagNode(plugArray[0].node());
		MFnDependencyNode fnTransformNode(fnMeshDagNode.parent(0));
		MString parName = fnTransformNode.name();
		worldMatrix = MFnMatrixData(fnTransformNode.findPlug("worldMatrix").elementByLogicalIndex(0).asMObject()).matrix();
	}
	return worldMatrix;
}

inline void vp2FragmentShaderList(MHWRender::MFragmentManager* fragMgr, std::vector<std::string> & shaderList)
{
	MStringArray shaderArray;
	fragMgr->fragmentList(shaderArray);
	for (unsigned int i = 0; i < shaderArray.length(); ++i) {
		shaderList.push_back(shaderArray[i].asChar());
	}
}

inline void vp2ShaderParamsList(MHWRender::MShaderInstance* mBlinnShader,std::vector<std::string> & params, std::vector<std::string> & paramTypes)
{
	MStringArray paramList;
	mBlinnShader->parameterList(paramList);
	for (unsigned int i = 0; i < paramList.length(); ++i) {
		params.push_back(paramList[i].asChar());
		std::string curType;
		switch (mBlinnShader->parameterType(paramList[i]))
		{
		case MHWRender::MShaderInstance::kInvalid:
			curType="Invalid";
			break;
		case MHWRender::MShaderInstance::kBoolean:
			curType = "Boolean";
			break;
		case MHWRender::MShaderInstance::kInteger:
			curType = "Integer"; 
			break;
		case MHWRender::MShaderInstance::kFloat:
			curType = "Float";
			break;
		case MHWRender::MShaderInstance::kFloat2:
			curType = "Float2"; 
			break;
		case MHWRender::MShaderInstance::kFloat3:
			curType = "Float3"; 
			break;
		case MHWRender::MShaderInstance::kFloat4:
			curType = "Float4"; 
			break;
		case MHWRender::MShaderInstance::kFloat4x4Row:
			curType = "Float4x4Row";
			break;
		case MHWRender::MShaderInstance::kFloat4x4Col:
			curType = "Float4x4Col";
			break;
		case MHWRender::MShaderInstance::kTexture1:
			curType = "1D Texture"; 
			break;
		case MHWRender::MShaderInstance::kTexture2:
			curType = "2D Texture"; 
			break;
		case MHWRender::MShaderInstance::kTexture3:
			curType = "3D Texture"; 
			break;
		case MHWRender::MShaderInstance::kTextureCube:
			curType = "Cube Texture"; 
			break;
		case MHWRender::MShaderInstance::kSampler:
			curType = "Sampler"; 
			break;
		default:
			curType = "Unknown";
			break;
		};
		paramTypes.push_back(curType);
	}
}

inline void vp2ShaderParamPrint(MHWRender::MShaderInstance* mBlinnShader)
{
	MStringArray paramList;
	mBlinnShader->parameterList(paramList);
	for (uint i = 0; i < paramList.length(); ++i)
		MGlobal::displayInfo(paramList[i]);
}

inline bool FileNameFromConnectedFileNode(MString & fileName,MPlug & plugConnectedByFileNode)
{
	MPlugArray plugArray;
	if (!plugConnectedByFileNode.connectedTo(plugArray, 1, 0))
		return false;

	MFnDependencyNode fnNode(plugArray[0].node());
	MStatus status;
	MPlug fileNamePlug = fnNode.findPlug("fileTextureName", &status);
	if (status != MStatus::kSuccess)
		return false;
	
	fileName = fileNamePlug.asString();
	return true;
}

inline void LoadHairShaderXML(MHWRender::MFragmentManager* fragMgr)
{
	static int shaderRegisterRunOnce = 0;
	if (shaderRegisterRunOnce > 0)
		return;

	//MString mayaPlugInPath(getenv("XML_SHADER_PATH"));
	MString xmlPath;
	MGlobal::executeCommand(MString("getenv \"XML_SHADER_PATH\""), xmlPath, false);
	MString info("XML_SHADER_PATH=");
	info += xmlPath;
	MGlobal::displayInfo(info);
	//fragMgr->addFragmentPath(xmlPath);
	MStringArray plugInPathArray;
	xmlPath.split(';', plugInPathArray);
	for (uint i = 0; i<plugInPathArray.length(); ++i)
		if (plugInPathArray[i] != "") {
			MString ss = plugInPathArray[i];
			fragMgr->addFragmentPath(plugInPathArray[i]);
		}

	static const char* fragList[] = {
		"nurbsShaderEmission",
		"nurbsfloatPassThrough",
		"nurbsfloat3PassThrough",
		"nurbsHairSpecular",
		"nurbsHairShaderBase",
		"nurbsHairShaderCombiner",
		"nurbsFresnel",
		"lerpFloat1",
		"multiplyFloat",
		"nurbsHairVSPc",
		"nurbsHairBwNw",
		"nurbsHairBwNwOut"
	};

	for (const char* curFrag : fragList) {
		MString fragAdded(curFrag);
		if (!fragMgr->hasFragment(fragAdded)) {
			fragAdded = fragMgr->addShadeFragmentFromFile(fragAdded + ".xml", 0);
		}

	}

	static const char* graphList[] = {
		"nurbsHairShader"
	};

	for (const char* curGraph : graphList) {
		MString shader(curGraph);
		if (!fragMgr->hasFragment(shader)) {
			shader = fragMgr->addFragmentGraphFromFile(shader + ".xml");
		}
	}

	shaderRegisterRunOnce = 1;
}

inline void LoadHairShaderXML_Reload(MHWRender::MFragmentManager* fragMgr)
{
	//MString mayaPlugInPath(getenv("XML_SHADER_PATH"));
	//MString xmlPath;
	//MGlobal::executeCommand(MString("getenv \"XML_SHADER_PATH\""), xmlPath, false);
	//MString info("XML_SHADER_PATH=");
	//info += xmlPath;
	//MGlobal::displayInfo(info);
	//MStringArray plugInPathArray;
	//xmlPath.split(';', plugInPathArray);
	//for (uint i = 0; i<plugInPathArray.length(); ++i)
	//	if (plugInPathArray[i] != "")
	//		fragMgr->addFragmentPath(plugInPathArray[i]);

	fragMgr->addFragmentPath("D:/asunlab/github/hairTools/xml");

	static const char* fragList[] = {
		"nurbsShaderEmission",
		"nurbsfloatPassThrough",
		"nurbsfloat3PassThrough",
		"nurbsHairSpecular",
		"nurbsHairShaderBase",
		"nurbsHairShaderCombiner",
		"nurbsFresnel",
		"lerpFloat1",
		"multiplyFloat",
		"nurbsHairVSPc",
		"nurbsHairBwNw",
		"nurbsHairBwNwOut"
	};

	for (const char* curFrag : fragList) {
		MString fragAdded(curFrag);
		if (fragMgr->hasFragment(fragAdded)) {
			fragMgr->removeFragment(fragAdded);
		}
		fragAdded = fragMgr->addShadeFragmentFromFile(fragAdded + ".xml", 0);
	}

	static const char* graphList[] = {
		"nurbsHairShader"
	};

	for (const char* curGraph : graphList) {
		MString shader(curGraph);
		if (fragMgr->hasFragment(shader)) {
			fragMgr->removeFragment(shader);
		}
		shader = fragMgr->addFragmentGraphFromFile(shader + ".xml");
	}
}