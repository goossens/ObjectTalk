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

#include "OtFunction.h"

#include "OtGeometry.h"


//
//	OtGeometryClass::computeTangents
//

OtObject OtGeometryClass::computeTangents() {
	if (!tangent) {
		tangent = true;
		refreshBuffers = true;
	}

	return OtObject(this);
}


//
//	OtGeometryClass::getAABB
//

OtAABB &OtGeometryClass::getAABB() {
	validateGeometry();

	if (displacer.isActive()) {
		return displacer.getAABB();

	} else {
		return aabb;
	}
}


//
//	OtGeometryClass::updateGeometry
//

void OtGeometryClass::updateGeometry() {
	// first clear to existing geometry
	aabb.clear();
	vertices.clear();
	triangles.clear();
	lines.clear();

	// create new geometry
	fillGeometry();

	// set correct flags
	refreshGeometry = false;
	refreshDisplacement = true;
	refreshBuffers = true;
}


//
//	OtGeometryClass::validateGeometry
//

void OtGeometryClass::validateGeometry() {
	if (refreshGeometry) {
		updateGeometry();
	}
}


//
//	OtGeometryClass::updateDisplacement
//

void OtGeometryClass::updateDisplacement() {
	// perform the displacement
	displacer.displace(vertices, triangles);

	// set correct flags
	refreshDisplacement = false;
	refreshBuffers = true;
}


//
//	OtGeometryClass::validateDisplacement
//

void OtGeometryClass::validateDisplacement() {
	if (refreshDisplacement) {
		updateDisplacement();
	}
}


//
//	OtGeometryClass::updateBuffers
//

void OtGeometryClass::updateBuffers() {
	// determine vertex source
	std::vector<OtVertex>& source = displacer.isActive() ? displacer.getVertices() : vertices;

	// do we need to generate the tangents?
	if (tangent) {
		// clear tangents
		for (auto& vertex : source) {
			vertex.tangent = glm::vec3(0.0);
			vertex.bitangent = glm::vec3(0.0);
		}

		// generate new tangents
		for (auto i = 0; i < triangles.size(); i += 3) {
			OtVertex& v0 = source[triangles[i]];
			OtVertex& v1 = source[triangles[i + 1]];
			OtVertex& v2 = source[triangles[i + 2]];

			glm::vec3 edge1 = v1.position - v0.position;
			glm::vec3 edge2 = v2.position - v0.position;

			float deltaU1 = v1.uv.x - v0.uv.x;
			float deltaV1 = v1.uv.y - v0.uv.y;
			float deltaU2 = v2.uv.x - v0.uv.x;
			float deltaV2 = v2.uv.y - v0.uv.y;

			float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

			glm::vec3 tangent;
			tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
			tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
			tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);

			v0.tangent += tangent;
			v1.tangent += tangent;
			v2.tangent += tangent;

			glm::vec3 bitangent;
			bitangent.x = f * (-deltaU2 * edge1.x + deltaU1 * edge2.x);
			bitangent.y = f * (-deltaU2 * edge1.y + deltaU1 * edge2.y);
			bitangent.z = f * (-deltaU2 * edge1.z + deltaU1 * edge2.z);

			v0.bitangent += bitangent;
			v1.bitangent += bitangent;
			v2.bitangent += bitangent;
		}

		// normalize the tangents
		for (auto& vertex : source) {
			vertex.tangent = glm::normalize(vertex.tangent);
			vertex.bitangent = glm::normalize(vertex.bitangent);
		}
	}

	// update the buffers
	vertexBuffer.set(source.data(), source.size(), OtVertex::getLayout());
	triangleIndexBuffer.set(triangles.data(), triangles.size());
	lineIndexBuffer.set(lines.data(), lines.size());
	refreshBuffers = false;
}


//
//	OtGeometryClass::validateBuffers
//

void OtGeometryClass::validateBuffers() {
	if (refreshBuffers) {
		updateBuffers();
	}
}


//
//	OtGeometryClass::submitTriangles
//

void OtGeometryClass::submitTriangles() {
	validateGeometry();
	validateDisplacement();
	validateBuffers();

	vertexBuffer.submit();
	triangleIndexBuffer.submit();
}


//
//	OtGeometryClass::submitLines
//

void OtGeometryClass::submitLines() {
	validateGeometry();
	validateDisplacement();
	validateBuffers();

	vertexBuffer.submit();
	lineIndexBuffer.submit();
}


//
//	OtGeometryClass::renderGUI
//

bool OtGeometryClass::renderGUI() {
	bool changed = displacer.renderGUI();

	if (changed) {
		refreshDisplacement = displacer.isActive();
	}

	return changed;
}


//
//	OtGeometryClass::serialize
//

nlohmann::json OtGeometryClass::serialize() {
	auto data = nlohmann::json::object();
	data["displacement"] = displacer.serialize();
	return data;
}


//
//	Class::deserialize
//

void OtGeometryClass::deserialize(nlohmann::json data) {
	if (data.contains("displacement")) {
		displacer.deserialize(data["displacement"]);
	}

	refreshDisplacement = displacer.isActive();
}


//
//	OtGeometryClass::getMeta
//

OtType OtGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtGeometryClass>("Geometry", OtObjectClass::getMeta());

		type->set("computeTangents", OtFunction::create(&OtGeometryClass::computeTangents));
	}

	return type;
}
