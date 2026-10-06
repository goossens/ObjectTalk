//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <vector>

#include "assimp/Exporter.hpp"
#include "assimp/Importer.hpp"
#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "OtLog.h"
#include "OtPath.h"
#include "OtText.h"

#include "OtManifold.h"


//
//	OtManifold::OtManifold
//

OtManifold::OtManifold(const manifold::Manifold& m) {
	manifold = std::make_shared<manifold::Manifold>(m);
	incrementVersion();
	validate();
}


//
//	OtManifold::clear
//

void OtManifold::clear() {
	manifold = nullptr;
	incrementVersion();
}


//
//	OtManifold::load
//

void OtManifold::load(const std::string& filepath) {
	// configure importer
	Assimp::Importer importer;

	importer.SetPropertyInteger(
		AI_CONFIG_PP_RVC_FLAGS,
		aiComponent_NORMALS |
		aiComponent_TANGENTS_AND_BITANGENTS |
		aiComponent_COLORS |
		aiComponent_TEXCOORDS |
		aiComponent_BONEWEIGHTS |
		aiComponent_ANIMATIONS |
		aiComponent_TEXTURES |
		aiComponent_LIGHTS |
		aiComponent_CAMERAS |
		aiComponent_MATERIALS);

	importer.SetPropertyInteger(
		AI_CONFIG_PP_SBP_REMOVE,
		aiPrimitiveType_POINT |
		aiPrimitiveType_LINE);

	unsigned int flags =
		aiProcess_Triangulate |
		aiProcess_RemoveComponent |
		aiProcess_PreTransformVertices |
		aiProcess_SortByPType;

	auto ext = OtPath::getExtension(filepath);

	if (ext == ".stl") {
		flags |=
			aiProcess_JoinIdenticalVertices |
			aiProcess_OptimizeMeshes;
	}

	// read the file
	const aiScene* scene = importer.ReadFile(filepath, flags);

	// sanity check
	if (scene == nullptr) {
		OtLogError("Can't read manifold from [{}], error: {}", filepath, importer.GetErrorString());
	}

	// extract mesh from scene
	manifold::MeshGL mesh;
	mesh.numProp = 3;

	for (size_t i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* aMash = scene->mMeshes[i];

		for (size_t j = 0; j < aMash->mNumVertices; j++) {
			aiVector3D vert = aMash->mVertices[j];
			mesh.vertProperties.insert(mesh.vertProperties.end(), {vert.x, vert.z, -vert.y});
		}

		for (size_t j = 0; j < aMash->mNumFaces; j++) {
			aiFace face = aMash->mFaces[j];

			if (face.mNumIndices != 3) {
				OtLogError("Non-triangular face in [{}]", filepath);
			}

			mesh.triVerts.insert(mesh.triVerts.end(), {face.mIndices[0], face.mIndices[1], face.mIndices[2]});
		}
	}

	// create manifold
	manifold = std::make_shared<manifold::Manifold>(mesh);
	incrementVersion();
	validate();
}


//
//	OtManifold::save
//

void OtManifold::save(const std::string& filepath) {
	// create mesh from manifold
	OtMesh mesh;
	createMesh(mesh);
	mesh.save(filepath);
}


//
//	OtManifold::cube
//

OtManifold OtManifold::cube(float width, float height, float depth, bool center) {
	return OtManifold(manifold::Manifold::Cube(manifold::vec3(width, height, depth), center).CalculateNormals(0));
}


//
//	OtManifold::cylinder
//

OtManifold OtManifold::cylinder(float height, float bottomRadius, float topRadius, int segments, bool center) {
	return OtManifold(manifold::Manifold::Cylinder(height, bottomRadius, topRadius, segments, center).CalculateNormals(0));
}


//
//	OtManifold::sphere
//

OtManifold OtManifold::sphere(float radius, int segments) {
	return OtManifold(manifold::Manifold::Sphere(radius, segments).CalculateNormals(0));
}


//
//	OtManifold::decompose
//

std::vector<OtManifold> OtManifold::decompose() {
	std::vector<OtManifold> result;

	for (auto& m : manifold->Decompose()) {
		result.emplace_back(m);
	}

	return result;
}


//
//	OtManifold::compose
//

OtManifold OtManifold::compose(std::vector<OtManifold>& manifolds) {
	std::vector<manifold::Manifold> mans;

	for (auto& manifold : manifolds) {
		if (manifold.isValid()) {
			mans.emplace_back(*manifold.manifold);
		}
	}

	return OtManifold(manifold::Manifold::BatchBoolean(mans, manifold::OpType::Add));
}


//
//	OtManifold::unionManifolds
//

OtManifold OtManifold::unionManifolds(OtManifold& other) {
	return OtManifold(*manifold + *other.manifold);
}


//
//	OtManifold::differenceManifolds
//

OtManifold OtManifold::differenceManifolds(OtManifold& other) {
	return OtManifold(*manifold - *other.manifold);
}


//
//	OtManifold::intersectManifolds
//

OtManifold OtManifold::intersectManifolds(OtManifold& other) {
	return OtManifold(*manifold ^ *other.manifold);
}


//
//	shapeToPolygons
//

static void shapeToPolygons(manifold::Polygons& polygons, OtShape& shape, float tolerance) {
	// get the shape's path segments
	std::vector<glm::vec2> points;
	std::vector<size_t> sizes;
	shape.getSegments(points, sizes, tolerance);

	// convert shape to polygons
	size_t start = 0;

	for (auto size : sizes) {
		auto& polygon = polygons.emplace_back();

		for (auto j = start; j < start + size - 1; j++) {
			polygon.emplace_back(static_cast<double>(points[j].x), static_cast<double>(points[j].y));
		}

		start += size;
	}
}


//
//	polygonsToShape
//

static void polygonsToShape(OtShape& shape, manifold::Polygons& polygons) {
	for (auto& polygon : polygons) {
		bool first = true;

		for (auto& point : polygon) {
			if (first) {
				shape.moveTo(static_cast<float>(point.x), static_cast<float>(point.y));
				first = false;

			} else {
				shape.lineTo(static_cast<float>(point.x), static_cast<float>(point.y));
			}
		}

		shape.close();
	}
}


//
//	OtManifold::extrude
//

OtManifold OtManifold::extrude(OtShape& shape, float height, int divisions, float twistDegrees, float scaleTop, float tolerance) {
	// convert shape to polygons
	manifold::Polygons polygons;
	shapeToPolygons(polygons, shape, tolerance);

	// extrude and create manifold
	return OtManifold(manifold::Manifold::Extrude(
		polygons,
		height,
		divisions,
		twistDegrees,
		manifold::vec2(scaleTop, scaleTop)).CalculateNormals(0));
}


//
//	OtManifold::revolve
//

OtManifold OtManifold::revolve(OtShape& shape, int segments, float revolveDegrees, float tolerance) {
	// convert shape to polygons
	manifold::Polygons polygons;
	shapeToPolygons(polygons, shape, tolerance);

	// extrude and create manifold
	return OtManifold(manifold::Manifold::Revolve(polygons, segments, revolveDegrees).CalculateNormals(0));
}


//
//	OtManifold::slice
//

OtShape OtManifold::slice(float height) {
	auto polygons = manifold->Slice(static_cast<double>(height));

	OtShape shape;
	polygonsToShape(shape, polygons);
	return shape;
}


//
//	OtManifold::project
//

OtShape OtManifold::project() {
	auto polygons = manifold->Project();
	auto crossSection = manifold::CrossSection(polygons);
	polygons = crossSection.ToPolygons();

	OtShape shape;
	polygonsToShape(shape, polygons);
	return shape;
}


//
//	OtManifold::simplify
//

OtManifold OtManifold::simplify(float tolerance) {
	return OtManifold(manifold->Simplify(static_cast<double>(tolerance)));
}


//
//	OtManifold::refine
//

OtManifold OtManifold::refine(int tolerance) {
	return OtManifold(manifold->Refine(tolerance));
}


//
//	OtManifold::translate
//

OtManifold OtManifold::translate(float x, float y, float z) {
	return OtManifold(manifold->Translate(manifold::vec3(x, y, z)));
}


//
//	OtManifold::rotate
//

OtManifold OtManifold::rotate(float x, float y, float z) {
	return OtManifold(manifold->Rotate(x, y, z));
}


//
//	OtManifold::scale
//

OtManifold OtManifold::scale(float x, float y, float z) {
	return OtManifold(manifold->Scale(manifold::vec3(x, y, z)));
}


//
//	OtManifold::mirror
//

OtManifold OtManifold::mirror(float x, float y, float z) {
	return OtManifold(manifold->Mirror(manifold::vec3(x, y, z)));
}


//
//	OtManifold::hull
//

OtManifold OtManifold::hull() {
	return OtManifold(manifold->Hull().CalculateNormals(0));
}


//
//	OtManifold::split
//

std::pair<OtManifold, OtManifold> OtManifold::split(glm::vec3 normal, float offset) {
	auto result = manifold->SplitByPlane(
		manifold::vec3(
			static_cast<double>(normal.x),
			static_cast<double>(normal.y),
			static_cast<double>(normal.z)),
		static_cast<double>(offset));

	return std::pair<OtManifold, OtManifold>(result.first, result.second);
}


//
//	OtManifold::createMesh
//

void OtManifold::createMesh(OtMesh& mesh) {
	// get manifold information
	auto m = manifold->GetMeshGL();
	auto numberOfVertices = static_cast<size_t>(m.NumVert());
	auto numberOfTriangles = static_cast<size_t>(m.NumTri());
	auto numberOfProperties = static_cast<size_t>(m.numProp);

	// clear mesh and start converting
	mesh.clear();
	size_t offset = 0;

	// see if manifold includes normals
	if (numberOfProperties >= 6) {
		for (size_t v = 0; v < numberOfVertices; v++) {
			mesh.addVertex(OtVertex(
				glm::vec3(m.vertProperties[offset], m.vertProperties[offset + 2], -m.vertProperties[offset + 1]),
				glm::vec3(m.vertProperties[offset + 3], m.vertProperties[offset + 5], -m.vertProperties[offset + 4])));

			offset += numberOfProperties;
		}

	} else {
		for (size_t v = 0; v < numberOfVertices; v++) {
			mesh.addVertex(glm::vec3(m.vertProperties[offset], m.vertProperties[offset + 2], -m.vertProperties[offset + 1]));
			offset += numberOfProperties;
		}
	}

	// generate triangles
	for (size_t t = 0; t < numberOfTriangles; t++) {
		auto triangles = m.GetTriVerts(t);
		mesh.addTriangle(triangles[0], triangles[1], triangles[2]);
	}

	mesh.generateAABB();

	// if normals were not included
	if (numberOfProperties < 6) {
		mesh.generateNormals();
	}
}


//
//	OtManifold::validate
//

void OtManifold::validate() {
	using error = manifold::Manifold::Error;
	auto status = manifold->Status();

	if (status != error::NoError) {
		manifold = nullptr;

		switch(status) {
			case error::NoError: OtLogError("Manifold error: No Error"); break;
			case error::NonFiniteVertex: OtLogError("Manifold error: Non Finite Vertex"); break;
			case error::NotManifold: OtLogError("Manifold error: Not Manifold"); break;
			case error::VertexOutOfBounds: OtLogError("Manifold error: Vertex Out Of Bounds"); break;
			case error::PropertiesWrongLength: OtLogError("Manifold error: Properties Wrong Length"); break;
			case error::MissingPositionProperties: OtLogError("Manifold error: Missing Position Properties"); break;
			case error::MergeVectorsDifferentLengths: OtLogError("Manifold error: Merge Vectors Different Lengths"); break;
			case error::MergeIndexOutOfBounds: OtLogError("Manifold error: Merge Index Out Of Bounds"); break;
			case error::TransformWrongLength: OtLogError("Manifold error: Transform Wrong Length"); break;
			case error::RunIndexWrongLength: OtLogError("Manifold error: Run Index Wrong Length"); break;
			case error::FaceIDWrongLength: OtLogError("Manifold error: Face ID Wrong Length"); break;
			case error::InvalidConstruction: OtLogError("Manifold error: Invalid Construction"); break;
			case error::ResultTooLarge: OtLogError("Manifold error: Result Too Large"); break;
			default: OtLogError("Manifold error: Unknown Error"); break;
		}
	}
}
