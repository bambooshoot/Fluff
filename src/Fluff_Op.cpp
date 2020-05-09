#include <ai.h>
#include <sstream>
#include <random>

AI_OPERATOR_NODE_EXPORT_METHODS(OpMethods);

struct FluffOpData
{
	FluffOpData(int max_instances = 50) :
		max_instances(max_instances),
		num_instances(0)
	{}

	int max_instances;
	int num_instances;

	AtString		name;
	AtString		skinMesh;

	float			density;
	float			length;
	float		    length_random;
	float			root_width;
	float			tip_width;
	float			width_random;
	float			fuzzy;
	float			segmentLen;

	int				fill_mode;
	AtRGB			color1;
	float			opacity1;
	AtRGB			color2;
	float			opacity2;
};

namespace
{
	std::uniform_real_distribution<float> dist(0.0, 1.0);
	std::mt19937 rand_generator;

	float random_value()
	{
		return dist(rand_generator);
	}
}

node_parameters
{
	// scatter params
	AiParameterStr("skin_mesh", "");
	AiParameterFlt("segment_length", 0.1f);
	AiParameterFlt("seed", 0);

	AiParameterFlt("density", 1.0f);

	// curve params
	AiParameterFlt("length", 1.0f);
	AiParameterFlt("length_random", 0.2f);

	AiParameterFlt("root_width", 0.2f);
	AiParameterFlt("tip_width", 0.1f);
	AiParameterFlt("width_random", 0.2f);

	AiParameterFlt("fuzzy", 0.2f);

	// shader params
	AiParameterInt("fill_mode", 0);
	AiParameterRGB("color1", 1.0f, 1.0f, 1.0f);
	AiParameterFlt("opacity1", 1.0f);
	AiParameterRGB("color2", 1.0f, 1.0f, 1.0f);
	AiParameterFlt("opacity2", 1.0f);
}

operator_init
{
	return true;
}

operator_cleanup
{
	return true;
}

bool cleanup_child_data(void* child_data)
{
	delete static_cast<FluffOpData*>(child_data);
	return true;
}

operator_cook
{
	FluffOpData* pData = static_cast<FluffOpData*>(child_data);
	pData->name = AiNodeGetStr(node, "name");
	pData->skinMesh = AiNodeGetStr(node, "skin_mesh");

#define ASSIGN_FLT_PARAM(PARAMNAME) pData->PARAMNAME = AiNodeGetFlt(node, "PARAMNAME")

	ASSIGN_FLT_PARAM(density);
	ASSIGN_FLT_PARAM(length);
	ASSIGN_FLT_PARAM(length_random);
	ASSIGN_FLT_PARAM(root_width);
	ASSIGN_FLT_PARAM(tip_width);
	ASSIGN_FLT_PARAM(width_random);
	ASSIGN_FLT_PARAM(fuzzy);
	ASSIGN_FLT_PARAM(segmentLen);
	ASSIGN_FLT_PARAM(opacity1);
	ASSIGN_FLT_PARAM(opacity2);

	pData->fill_mode = AiNodeGetInt(node, "fill_mode");

#define ASSIGN_RGB_PARAM(PARAMNAME) pData->PARAMNAME = AiNodeGetRGB(node, "PARAMNAME")

	ASSIGN_RGB_PARAM(color1);
	ASSIGN_RGB_PARAM(color2);

	return true;
}

operator_post_cook
{
	return true;
}