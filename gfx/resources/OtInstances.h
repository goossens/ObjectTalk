//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <string>
#include <vector>

#include "glm/glm.hpp"


//
//	OtInstances
//

class OtInstances {
public:
	// constructor
	OtInstances();

	// clear list of instances
	void clear();

	// see if we have any instances (i.e. is this a valid list)
	inline bool isValid() { return instances->size() != 0; }

	// instance I/O
	void load(const std::string& path);
	void save(const std::string& path);

	// add an instance
	void add(const glm::mat4& instance, bool updateVersion=true);

	// access individual instances
	glm::mat4& operator[](int i) { return instances->operator[](i); }
	size_t size() { return instances->size(); }

	// submit instances to GPU
	bool submit();

	// version management
	inline void setVersion(int v) { version = v; }
	inline int getVersion() { return version; }
	inline void incrementVersion() { version++; }

	// see if instances are identical
	inline bool operator==(OtInstances& rhs) {
		return instances == rhs.instances && version == rhs.version;
	}

	inline bool operator!=(OtInstances& rhs) {
		return !operator==(rhs);
	}

private:
	// list of transformations (for the instances)
	std::shared_ptr<std::vector<glm::mat4>> instances;
	int version = 0;
};
