#pragma once

#include <FluffDef.h>
#include <ai.h>

inline AtVector Vec2AtVec(CVec &vec)
{
	return AtVector(vec.x, vec.y, vec.z);
}

inline AtVector2 Vec22AtVec(CVec2& vec)
{
	return AtVector2(vec.x, vec.y);
}


inline AtNode* nodeLookUpByName(const AtString& name)
{
	std::string nodeName(name);
	if (nodeName.empty())
		return nullptr;

	AtNode* pNode;
	if constexpr (AI_VERSION_ARCH_NUM == 6) {
		if (nodeName.at(0) != '|' && nodeName.find_first_of('|') < nodeName.size())
			nodeName = nodeName.insert(0, "|");
		else if (nodeName.at(0) != '/' && nodeName.find_first_of('/') < nodeName.size())
			nodeName = nodeName.insert(0, "/");

		pNode = AiNodeLookUpByName(AtString(nodeName.c_str()));

		if (pNode)
			return pNode;

		std::replace(nodeName.begin(), nodeName.end(), '|', '/');
	}

	pNode = AiNodeLookUpByName(AtString(nodeName.c_str()));
	return pNode;
}
