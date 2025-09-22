#pragma once
#include <vector>

class nTreeNode {
public:
	void* data = nullptr;
	nTreeNode* parent = nullptr;
	std::vector<nTreeNode*> children;

	nTreeNode() {}
	nTreeNode(nTreeNode* parent) { this->parent = parent; }
	~nTreeNode() {
		for (auto& child : children) {
			child->~nTreeNode();
			free(child);
		}
		free(data);
	}

};