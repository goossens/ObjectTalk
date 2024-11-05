//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>
#include <cstring>
#include <fstream>
#include <sstream>

#include "bgfx/bgfx.h"
#include "nlohmann/json.hpp"

#include "OtException.h"

#include "OtGlm.h"
#include "OtInstances.h"


//
//	OtInstances::OtInstances
//

OtInstances::OtInstances() {
	instances = std::make_shared<std::vector<glm::mat4>>();
}


//
//	OtInstances::clear
//

void OtInstances::clear() {
	instances = std::make_shared<std::vector<glm::mat4>>();
	incrementVersion();
}


//
//	OtInstances::load
//

void OtInstances::load(const std::string &path) {
	// load instances from file
	std::stringstream buffer;

	try {
		std::ifstream stream(path.c_str());

		if (stream.fail()) {
			OtError("Can't read from file [{}]", path);
		}

		buffer << stream.rdbuf();
		stream.close();

	} catch (std::exception& e) {
		OtError("Can't read from file [{}], error: {}", path, e.what());
	}

	// parse json
	auto data = nlohmann::json::parse(buffer.str());

	// clear list and add instances
	instances = std::make_shared<std::vector<glm::mat4>>();

	for (auto element : data.items()) {
		glm::mat4 instance;
		glm::from_json(element.value(), instance);
		instances->emplace_back(instance);
	}

	// increment version number
	incrementVersion();
}


//
//	OtInstances::save
//

void OtInstances::save(const std::string& path) {
	auto data = nlohmann::json::array();

	for (auto& instance : *instances) {
		data.emplace_back(instance);
	}

	// write instances to file
	try {
		std::ofstream stream(path.c_str());

		if (stream.fail()) {
			OtError("Can't open file [{}] for writing", path);
		}

		stream << data.dump(1, '\t');
		stream.close();

	} catch (std::exception& e) {
		OtError("Can't write to file [{}], error: {}", path, e.what());
	}
}


//
//	OtInstances::add
//

void OtInstances::add(const glm::mat4 &instance, bool updateVersion) {
	instances->emplace_back(instance);

	if (updateVersion) {
	 	incrementVersion();
	}
}


//
//	OtInstances::submit
//

bool OtInstances::submit(OtFrustum& frustum, OtAABB& aabb) {
	if (instances->size()) {
		// filter instances based on visibility
		std::vector<glm::mat4> tmp;

		for (auto& instance : *instances) {
			if (frustum.isVisibleAABB(aabb.transform(instance))) {
				tmp.emplace_back(instance);
			}
		}

		// get number of instances and size of instance data
		uint32_t count = (uint32_t) tmp.size();
		uint32_t stride = sizeof(glm::mat4);

		if (count) {
			// create instance data buffer and submit it to the GPU
			bgfx::InstanceDataBuffer idb;
			count = bgfx::getAvailInstanceDataBuffer(count, stride);
			bgfx::allocInstanceDataBuffer(&idb, count, stride);
			std::memcpy(idb.data, tmp.data(), idb.size);
			bgfx::setInstanceDataBuffer(&idb);
			return true;

		} else {
			return false;
		}

	} else {
		return false;
	}
}
