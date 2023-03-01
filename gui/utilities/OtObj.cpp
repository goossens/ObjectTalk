//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//


#include <filesystem>
#include <string>
#include <vector>

#include "tiny_obj_loader.h"

#include "OtException.h"
#include "OtFormat.h"

#include "OtColoredMaterial.h"
#include "OtCustomGeometry.h"
#include "OtGroup.h"
#include "OtMesh.h"
#include "OtObj.h"


//
//	OtObjLoad
//

OtObject OtObjLoad(const std::string& filename) {
	// load the object model
	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(filename)) {
		if (!reader.Error().empty()) {
			OtExcept("Error while loading model [%s]: %s",
				filename.c_str(),
				reader.Error().c_str());
		}
	}

	if (!reader.Warning().empty()) {
		OT_DEBUG(reader.Warning());
	}

	auto& attrs = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();

	// OT_DEBUG(OtFormat("# of shapes    = %d", (int) shapes.size()));
	// OT_DEBUG(OtFormat("# of materials = %d", (int) materials.size()));
	// OT_DEBUG(OtFormat("# of vertices  = %d", (int) (attrs.vertices.size()) / 3));
	// OT_DEBUG(OtFormat("# of normals   = %d", (int) (attrs.normals.size()) / 3));
	// OT_DEBUG(OtFormat("# of texcoords = %d", (int) (attrs.texcoords.size()) / 2));

	// process all materials
	OtColoredMaterial defaultMaterial = OtColoredMaterial::create();
	std::vector<OtMaterial> materialList;

	std::filesystem::path path = filename;
	path = path.parent_path();

	for (auto i = materials.begin(); i < materials.end(); i++) {
		// do we have a texture?
		if (i->diffuse_texname.length() > 0) {
			// create a textured material
			// material->setTexture((path / i->diffuse_texname).string());

		} else {
			// create a colored material object
			OtColoredMaterial material = OtColoredMaterial::create();

			// set colors
			material->setAmbientRGB(i->ambient[0], i->ambient[1], i->ambient[2]);
			material->setDiffuseRGB(i->diffuse[0], i->diffuse[1], i->diffuse[2]);
			material->setSpecularRGB(i->specular[0], i->specular[1], i->specular[2]);
			material->setShininess(i->shininess);
			material->setOpacity(i->dissolve);

			// add material to registry
			materialList.push_back(material);
		}
	}

	// process all shapes in model
	OtGroup group = OtGroup::create();

	for (auto& shape : shapes) {
		// create a new mesh
		OtMesh mesh = OtMesh::create();

		// create geometry
		OtCustomGeometry geometry = OtCustomGeometry::create();

		// see if we have texture coordinates
		bool hasUV = attrs.texcoords.size() > 0;

		// add all vertices
		int vertices = 0;

		for (auto& index : shape.mesh.indices) {
			geometry->addVertex(OtVertex(
				glm::vec3(
					attrs.vertices[3 * index.vertex_index + 0],
					attrs.vertices[3 * index.vertex_index + 1],
					attrs.vertices[3 * index.vertex_index + 2]),
				glm::normalize(glm::vec3(
					attrs.normals[3 * index.normal_index + 0],
					attrs.normals[3 * index.normal_index + 1],
					attrs.normals[3 * index.normal_index + 2])),
				glm::vec2(
					hasUV ? attrs.texcoords[2 * index.texcoord_index + 0] : 0.0,
					hasUV ? attrs.texcoords[2 * index.texcoord_index + 1] : 0.0)));

			if ((++vertices % 3) == 0) {
				geometry->addTriangle(vertices - 3, vertices - 2, vertices - 1);
				geometry->addLine(vertices - 3, vertices - 2);
				geometry->addLine(vertices - 2, vertices - 1);
				geometry->addLine(vertices - 1, vertices - 3);
			}
		}

		mesh->setGeometry(geometry);

		// set material
		auto materialID = shape.mesh.material_ids[0];

		if (materialID < 0 || materialID >= materialList.size()) {
			mesh->setMaterial(defaultMaterial);

		} else {
			mesh->setMaterial(materialList[materialID]);
		}

		// add mesh to group
		group->add(mesh);
	}

	// return all meshes as a group
	return group;
}
