#pragma once

#include <FluffFollicleModiferBase.h>
#include <Singleton.h>

class FluffFollicleModiferFactory {
public:
	FluffFollicleModiferBase* create(NodeId nodeId)
	{
		auto iter = funcMap.find(nodeId);
		if (iter != funcMap.end()) {
			return (*iter->second)();
		}
		return nullptr;
	}
	typedef FluffFollicleModiferBase* CreateFuncType();
	void registerClass(NodeId id, CreateFuncType * createFunc)
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

using FluffFollicleModiferFactory_Single = Singleton<FluffFollicleModiferFactory>;