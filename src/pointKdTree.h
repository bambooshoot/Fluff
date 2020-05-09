#pragma once

#include <FluffDef.h>

template<class VEC>
class PointKD_Node
{
public:
	typedef VEC Vec;
	typedef const Vec CVec;
	typedef std::vector<Vec> VecList;
	typedef const VecList CVecList;

	uint childId[2];
	uint pointId;
	uint dim;

	PointKD_Node() {
		childId[0] = UINT_MAX;
		childId[1] = UINT_MAX;
		pointId = UINT_MAX;
		dim = UINT_MAX;
	}
	bool touchLeft(CVec& p, cfloat r, CVecList& posList)
	{
		if (!hasChild(0))
			return false;

		float valueDim = posList[pointId][dim];
		return p[dim] - r <= valueDim;
	}
	bool touchRight(CVec& p, cfloat r, CVecList& posList)
	{
		if (!hasChild(1))
			return false;

		float valueDim = posList[pointId][dim];
		return valueDim <= p[dim] + r;
	}
	bool touchPoint(CVec& p, cfloat r, CVecList& posList)
	{
		return (posList[pointId] - p).length2() < r * r;
	}
	float distance2(CVec& p, CVecList& posList)
	{
		return (posList[pointId] - p).length2();
	}
	CVec& point(CVecList& posList)
	{
		return posList[pointId];
	}
	bool hasChild(cuint id)
	{
		return childId[id] != UINT_MAX;
	}
	bool isBranch()
	{
		return childId[0] != UINT_MAX || childId[1] != UINT_MAX;
	}
};

template<class VEC>
class PointKD_Tree
{
public:
	using Vec = VEC;
	using CVec = const Vec;
	using VecList = std::vector<Vec>;
	using CVecList = const std::vector<Vec>;

private:
	using KDNode = PointKD_Node<Vec>;
	using Box = Imath::Box<Vec>;

	std::vector<KDNode> nodeList;

	void buildBranch(CVecList& posList, UIntList& idList)
	{
		struct NodeStack {
			uint beginId, endId, parentId, branchId;
		};

		std::vector<NodeStack> stack;
		NodeStack nodeStack, nodeStack0, nodeStack1;
		nodeStack.beginId = 0;
		nodeStack.endId = (uint)posList.size();
		nodeStack.parentId = 0;
		nodeStack.branchId = 3;
		stack.push_back(nodeStack);

		while (stack.size()) {
			nodeStack = stack.back();
			stack.pop_back();

			if (nodeStack.endId - nodeStack.beginId < 2) {
				KDNode kdNode;
				kdNode.dim = 0;
				kdNode.pointId = idList[nodeStack.beginId];

				if (nodeStack.branchId < 2)
					nodeList[nodeStack.parentId].childId[nodeStack.branchId] = (uint)nodeList.size();

				nodeList.push_back(kdNode);
			}
			else {
				uint dim = majorDim(posList, idList, nodeStack.beginId, nodeStack.endId);
				uint midId = nodeStack.beginId + (nodeStack.endId - nodeStack.beginId) / 2;
				std::nth_element(idList.begin() + nodeStack.beginId,
					idList.begin() + midId,
					idList.begin() + nodeStack.endId, [&](cuint id0, cuint id1) {
						return posList[id0][dim] < posList[id1][dim];
					});

				KDNode kdNode;
				kdNode.dim = dim;
				kdNode.pointId = idList[midId];

				if (nodeStack.branchId < 2)
					nodeList[nodeStack.parentId].childId[nodeStack.branchId] = (uint)nodeList.size();

				nodeList.push_back(kdNode);

				nodeStack0.beginId = nodeStack.beginId;
				nodeStack0.endId = std::max<uint>(midId, nodeStack.beginId);
				nodeStack0.parentId = (uint)nodeList.size() - 1;
				nodeStack0.branchId = 0;

				stack.push_back(nodeStack0);

				nodeStack1.beginId = midId + 1;
				if (nodeStack1.beginId < nodeStack.endId) {
					nodeStack1.endId = nodeStack.endId;
					nodeStack1.parentId = nodeStack0.parentId;
					nodeStack1.branchId = 1;
					stack.push_back(nodeStack1);
				}
			}
		}
	}

	uint majorDim(CVecList& posList, UIntList& idList, cuint beginId, cuint endId)
	{
		Box box;
		for (uint i = beginId; i < endId; ++i) {
			box.extendBy(posList[idList[i]]);
		}
		return box.majorAxis();
	}

public:
	void clear()
	{
		nodeList.clear();
	}
	void build(CVecList& posList)
	{
		nodeList.clear();

		uint pointNum = (uint)posList.size();
		if (pointNum == 0)
			return;

		UIntList idList;
		for (uint i = 0; i < pointNum; ++i) {
			idList.push_back(i);
		}

		buildBranch(posList, idList);
	}
	bool search(UIntList& idList, CVecList& posList, CVec& p, float radius)
	{
		UIntList nodeIdList;
		nodeIdList.push_back(0);
		while (nodeIdList.size()) {
			KDNode& node = nodeList[nodeIdList.back()];
			nodeIdList.pop_back();

			if (node.isBranch()) {
				if (node.touchLeft(p, radius, posList))
					nodeIdList.push_back(node.childId[0]);

				if (node.touchRight(p, radius, posList))
					nodeIdList.push_back(node.childId[1]);
			}

			if (node.touchPoint(p, radius, posList))
				idList.push_back(node.pointId);
		}
		return idList.size() > 0;
	}
	uint searchNearest(CVecList& posList, CVec& p, uint& searchedNum)
	{
		uint nearestId = 0;
		float	nearestDist = 1e10f,
			nearestDist2 = nearestDist * nearestDist,
			curDist2;

		UIntList nodeIdList;
		nodeIdList.push_back(0);
		searchedNum = 0;
		while (nodeIdList.size()) {
			KDNode& node = nodeList[nodeIdList.back()];
			nodeIdList.pop_back();

			curDist2 = node.distance2(p, posList);
			if (curDist2 < nearestDist2) {
				nearestId = node.pointId;
				nearestDist2 = curDist2;
				nearestDist = sqrtf(curDist2);
			}

			if (node.isBranch()) {
				if (node.touchLeft(p, nearestDist, posList))
					nodeIdList.push_back(node.childId[0]);

				if (node.touchRight(p, nearestDist, posList))
					nodeIdList.push_back(node.childId[1]);
			}
			++searchedNum;
		}

		return nearestId;
	}
	
};

typedef PointKD_Tree<Vec2> KDTree2;