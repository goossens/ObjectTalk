//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <vector>
#include <unordered_map>

#include "assimp/scene.h"
#include "glm/glm.hpp"


//
//	OtModelNodes
//

class OtModelNodes {
public:
	// load the nodes into a hierarchy
	void load(const aiNode* node);

	// properties
	class Node {
	public:
		size_t id;
		std::string name;
		size_t parent;
		std::vector<size_t> children;
	};

	// access nodes
	inline bool hasNode(const std::string& name) { return index.find(name) != index.end(); }
	inline Node& getNode(const std::string& name) { return nodes[index[name]]; }
	inline Node& getNode(size_t id) { return nodes[id]; }

	// print debug information
	inline void debug() { printDebug(0, 0); }

private:
	// properties
	std::vector<Node> nodes;
	std::unordered_map<std::string, size_t> index;

	glm::mat4 globalInverseTransform;

	size_t addNode(const aiNode* node);
	void markParent(size_t id, size_t parent);
	void printDebug(size_t id, size_t indent);
};
