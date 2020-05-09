#pragma once

template<typename NodeType>
class FluffFactory {
public:
	NodeType* create(NodeId nodeId)
	{
		auto iter = funcMap.find(nodeId);
		if (iter != funcMap.end()) {
			return (*iter->second)();
		}
		return nullptr;
	}
	typedef NodeType* CreateFuncType();
	void registerClass(NodeId id, CreateFuncType* createFunc)
	{
		funcMap.insert(std::pair<NodeId, CreateFuncType*>(id, createFunc));
	}
	void unregisterClass(NodeId id)
	{
		funcMap.erase(id);
	}
private:
	std::map<NodeId, CreateFuncType*> funcMap;
};