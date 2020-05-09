#include <Fluff.h>
#include <fluffDrawOverride.h>

#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>

MString Fluff::drawRegistrantId("fluffDrawOverride");
MString Fluff::drawDbClassification("drawdb/geometry/Fluff");

MStatus initializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj, "zhusun", "", "");

	status = plugin.registerNode("Fluff",
		Fluff::id,
		Fluff::creator,
		Fluff::initialize,
		MPxNode::kLocatorNode,
		&Fluff::drawDbClassification);

	status = MHWRender::MDrawRegistry::registerGeometryOverrideCreator(
		Fluff::drawDbClassification,
		Fluff::drawRegistrantId,
		FluffDrawOverride::Creator);

	return status;
}

// The unitializePlugin is called when Maya needs to unload the plugin.
// It basically does the opposite of initialize by calling
// the deregisterCommand to remove it.
//
MStatus uninitializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj);

	status = plugin.deregisterNode(Fluff::id);
	if (!status) {
		status.perror("Failed to deregister node : Fluff");
		return status;
	}

	status = MHWRender::MDrawRegistry::deregisterGeometryOverrideCreator(
		Fluff::drawDbClassification,
		Fluff::drawRegistrantId);

	return status;
}
