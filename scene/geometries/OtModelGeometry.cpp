//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtFunction.h"

#include "OtUi.h"

#include "OtModelGeometry.h"
#include "OtPathTools.h"


//
//	OtModelGeometryClass::init
//

void OtModelGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 1:
				modelPath = parameters[0]->operator std::string();
				break;

			default:
				OtError("Too many parameters [%ld] for [ModelGeometry] constructor (max 1)", count);
		}

		refreshGeometry = true;
	}
}


//
//	OtModelGeometryClass::setModel
//

OtObject OtModelGeometryClass::setModel(const std::string& name) {
	modelPath = name;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtModelGeometryClass::fillGeometry
//

static inline glm::vec2 ToVec2(const aiVector3D& v) {
	return glm::vec2(v.x, v.y);
}

static inline glm::vec3 ToVec3(const aiVector3D& v) {
	return glm::vec3(v.x, v.y, v.z);
}

void OtModelGeometryClass::fillGeometry() {
	// do we have a valid file?
	if (OtPathIsRegularFile(modelPath)) {
		// create an asset importer
		Assimp::Importer importer;

		// determine the import flags
		static constexpr uint32_t flags =
			aiProcessPreset_TargetRealtime_Quality |
			aiProcess_OptimizeMeshes |
			aiProcess_PreTransformVertices |
			aiProcess_FlipUVs;

		// read the model file
		const aiScene* scene = importer.ReadFile(modelPath, flags);

		// ensure model was loaded correctly
		if (scene == nullptr) {
			OtError("Unable to load model [%s], error: %s", modelPath.c_str(), importer.GetErrorString());
		}

		// ensure scene is complete
		if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
			OtError("Incomplete model [%s]", modelPath.c_str());
		}

		// load all the meshes
		int offset = 0;

		for (auto i = 0; i < scene->mNumMeshes; i++) {
			auto mesh = scene->mMeshes[i];

			// see if we have 2-dimensional UV coordinates
			bool hasUV = mesh->mNumUVComponents[0] >= 2 && mesh->mTextureCoords[0] != nullptr;

			// process all vertices
			for (auto i = 0; i < mesh->mNumVertices; i++) {
				// get position
				glm::vec3 pos = ToVec3(mesh->mVertices[i]);

				// add a new vertex
				addVertex(OtVertex(
					pos,
					ToVec3(mesh->mNormals[i]),
					hasUV ? ToVec2(mesh->mTextureCoords[0][i]) : glm::vec2(),
					ToVec3(mesh->mTangents[i]),
					ToVec3(mesh->mBitangents[i])));
			}

			// process all triangles and lines
			for(auto i = 0; i < mesh->mNumFaces; i++) {
				auto i0 = mesh->mFaces[i].mIndices[0] + offset;
				auto i1 = mesh->mFaces[i].mIndices[1] + offset;
				auto i2 = mesh->mFaces[i].mIndices[2] + offset;
				addTriangle(i0, i1, i2);
				addLine(i0, i1);
				addLine(i1, i2);
				addLine(i2, i0);
			}

			// update the index offset
			offset += mesh->mNumVertices;
		}

	} else {
		// generate a dummy triangle to keep everybody happy
		addVertex(OtVertex(glm::vec3(-0.0001f, 0.0f, 0.0f)));
		addVertex(OtVertex(glm::vec3(0.0001f, 0.0f, 0.0f)));
		addVertex(OtVertex(glm::vec3(0.0f, 0.0f, -0.0001f)));
		addTriangle(0, 1, 2);
		addLine(0, 1);
		addLine(1, 2);
		addLine(2, 0);
	}
}


//
//	OtModelGeometryClass::renderUI
//

bool OtModelGeometryClass::renderUI() {
	bool changed = false;
	changed |= OtUiFileSelector("Model", modelPath);
	return changed;
}


//
//	OtModelGeometryClass::serialize
//

nlohmann::json OtModelGeometryClass::serialize(std::string* basedir) {
	auto data = OtGeometryClass::serialize(basedir);
	data["type"] = name;
	data["modelPath"] = OtPathRelative(modelPath, basedir);
	return data;
}


//
//	OtModelGeometryClass::deserialize
//

void OtModelGeometryClass::deserialize(nlohmann::json data, std::string* basedir) {
	modelPath = OtPathGetAbsolute(data, "modelPath", basedir);
	refreshGeometry = true;
}


//
//	OtModelGeometryClass::getMeta
//

OtType OtModelGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtModelGeometryClass>("ModelGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunction::create(&OtModelGeometryClass::init));
		type->set("setModel", OtFunction::create(&OtModelGeometryClass::setModel));
	}

	return type;
}
