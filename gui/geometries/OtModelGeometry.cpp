//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"
#include "tiny_obj_loader.h"

#include "OtFunction.h"

#include "OtModelGeometry.h"


//
//	OtModelGeometryClass::init
//

void OtModelGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 2:
				scale = parameters[1]->operator float();

			case 1:
				modelName = parameters[0]->operator std::string();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [ModelGeometry] constructor (max 2)", count);
		}

		refreshGeometry = true;
	}
}


//
//	OtModelGeometryClass::setModel
//

OtObject OtModelGeometryClass::setModel(const std::string& name) {
	modelName = name;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtModelGeometryClass::setScale
//

OtObject OtModelGeometryClass::setScale(float s) {
	scale = s;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtModelGeometryClass::fillGeometry
//

void OtModelGeometryClass::fillGeometry() {
	// load the object model
	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(modelName)) {
		if (!reader.Error().empty()) {
			OtExcept("Error while loading model [%s]: %s",
				modelName.c_str(),
				reader.Error().c_str());
		}
	}

	if (!reader.Warning().empty()) {
		OT_DEBUG(reader.Warning());
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();

	// process all model shapes (and created vertices and indices)
	bool hasUV = attrib.texcoords.size() > 0;

	for (auto& shape : shapes) {
		for (auto& index : shape.mesh.indices) {
			addVertex(OtVertex(
				glm::vec3(
					scale * attrib.vertices[3 * index.vertex_index + 0],
					scale * attrib.vertices[3 * index.vertex_index + 1],
					scale * attrib.vertices[3 * index.vertex_index + 2]),
				glm::normalize(glm::vec3(
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2])),
				glm::vec2(
					hasUV ? attrib.texcoords[2 * index.texcoord_index + 0] : 0.0,
					hasUV ? attrib.texcoords[2 * index.texcoord_index + 1] : 0.0)));

			auto size = (uint32_t) vertices.size();

			if (size && ((size % 3) == 0)) {
				addTriangle(size -3, size -2, size - 1);
				addLine(size - 3, size - 2);
				addLine(size - 2, size - 1);
				addLine(size - 1, size - 3);
			}
		}
	}
}


//
//	OtModelGeometryClass::renderGUI
//

bool OtModelGeometryClass::renderGUI() {
	return false;
}


//
//	OtModelGeometryClass::serialize
//

nlohmann::json OtModelGeometryClass::serialize() {
	auto data = nlohmann::json::object();
	data["type"] = name;
	return data;
}


//
//	OtModelGeometryClass::deserialize
//

void OtModelGeometryClass::deserialize(nlohmann::json data) {
}


//
//	OtModelGeometryClass::getMeta
//

OtType OtModelGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtModelGeometryClass>("ModelGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunction::create(&OtModelGeometryClass::init));
		type->set("setModel", OtFunction::create(&OtModelGeometryClass::setModel));
		type->set("setScale", OtFunction::create(&OtModelGeometryClass::setScale));
	}

	return type;
}
