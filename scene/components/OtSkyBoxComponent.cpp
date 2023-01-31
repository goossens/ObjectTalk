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

#include "OtUi.h"

#include "OtComponentTools.h"
#include "OtSkyBoxComponent.h"


//
//	OtSkyBoxComponent::renderGUI
//

bool OtSkyBoxComponent::renderGUI() {
	bool changed = false;

	changed |= OtUiFileSelector("Right", right);
	changed |= OtUiFileSelector("Left", left);
	changed |= OtUiFileSelector("Top", top);
	changed |= OtUiFileSelector("Bottom", bottom);
	changed |= OtUiFileSelector("Front", front);
	changed |= OtUiFileSelector("Back", back);

	if (changed) {
		update = true;
	}

	return changed;
}


//
//	OtSkyBoxComponent::serialize
//

nlohmann::json OtSkyBoxComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["right"] = OtComponentGetRelativePath(right, basedir);
	data["left"] = OtComponentGetRelativePath(left, basedir);
	data["top"] = OtComponentGetRelativePath(top, basedir);
	data["bottom"] = OtComponentGetRelativePath(bottom, basedir);
	data["front"] = OtComponentGetRelativePath(front, basedir);
	data["back"] = OtComponentGetRelativePath(back, basedir);
	return data;
}


//
//	OtSkyBoxComponent::deserialize
//

void OtSkyBoxComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	right = OtComponentGetAbsolutePath(data, "right", basedir);
	left = OtComponentGetAbsolutePath(data, "left", basedir);
	top = OtComponentGetAbsolutePath(data, "top", basedir);
	bottom = OtComponentGetAbsolutePath(data, "bottom", basedir);
	front = OtComponentGetAbsolutePath(data, "front", basedir);
	back = OtComponentGetAbsolutePath(data, "back", basedir);
	update = true;
}


//
//	OtSkyBoxComponent::render
//

void OtSkyBoxComponent::render(OtPass & pass) {
	// update the cubemap (if required)
	if (update) {
		update = false;

		// wait until all sides have valid files
		if (std::filesystem::is_regular_file(right) &&
			std::filesystem::is_regular_file(left) &&
			std::filesystem::is_regular_file(top) &&
			std::filesystem::is_regular_file(bottom) &&
			std::filesystem::is_regular_file(front) &&
			std::filesystem::is_regular_file(back)) {

			// load the files into the cubemap
			cubemap.load(right.string(), left.string(), top.string(), bottom.string(), front.string(), back.string());

		} else {
			// invalidate cubemap just in case
			cubemap.clear();
		}
	}

	// only continue if we have a valid cubemap
	if (cubemap.isValid()) {
		// submit texture via sampler
		sampler.submit(0, cubemap, "s_cubemap");

		// setup the mesh
		if (!geometry) {
			geometry = OtBoxGeometryClass::create();
		}

		geometry->submitTriangles();

		// load the shader (if required)
		if (!shader.isValid()) {
			shader.initialize("OtSkyboxVS", "OtSkyboxFS");
		}

		// run the shader
		shader.setState(OtShader::noDepth);
		pass.runShader(shader);
	}
}
