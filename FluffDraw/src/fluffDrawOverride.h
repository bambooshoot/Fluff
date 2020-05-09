#pragma once

#include <maya/MFnDependencyNode.h>
#include <maya/MPxGeometryOverride.h>

#include <FluffParams.h>

class FluffDrawOverride : public MHWRender::MPxGeometryOverride
{
public:
	static MHWRender::MPxGeometryOverride* Creator(const MObject& obj)
	{
		return new FluffDrawOverride(obj);
	}
	~FluffDrawOverride() override
	{
		cleanUp();
		mpFluff = NULL;
	};

	MHWRender::DrawAPI supportedDrawAPIs() const override
	{
		return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);
	};

	void updateDG() override;

	void updateRenderItems(const MDagPath& path,
		MHWRender::MRenderItemList& list) override;

	void populateGeometry(const MHWRender::MGeometryRequirements& requirements,
		const MHWRender::MRenderItemList& renderItems,
		MHWRender::MGeometry& data) override;

	void cleanUp()  override { 
		if (mpFluffData)
			delete mpFluffData;
	}

	void updateSelectionGranularity(const MDagPath& path,
		MHWRender::MSelectionContext& selectionContext) override;

private:
	FluffDrawOverride(const MObject& obj) : MPxGeometryOverride(obj) {
		mpFluff = &obj;
	}

	const MObject* mpFluff;

	FluffData *mpFluffData;
};