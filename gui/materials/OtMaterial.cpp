//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFormat.h"
#include "OtFunction.h"

#include "OtFramework.h"
#include "OtMaterial.h"


//
//	OtMaterialClass::~OtMaterialClass
//

OtMaterialClass::~OtMaterialClass() {
	if (uniformValues) {
		delete [] uniformValues;
	}

	if (samplers) {
		delete [] samplers;
	}
}


//
//	OtMaterialClass::submit
//

void OtMaterialClass::submit(bgfx::ViewId view, bool instancing) {
	// get uniform counts
	auto uniformCount = getNumberOfUniforms();
	auto samplerCount = getNumberOfSamplers();

	// initialize uniforms (if required)
	if (!initialized) {
		// create uniforms (if required)
		if (uniformCount) {
			// create uniforms (which are automatically destroyed by the framework)
			uniforms = OtFrameworkClass::instance()->getUniform(
				"u_material", bgfx::UniformType::Vec4, uniformCount);

			uniformValues = new glm::vec4[uniformCount];
		}

		// create samplers (if required)
		if (samplerCount) {
			samplers = new bgfx::UniformHandle[samplerCount];

			for (auto c = 0; c < samplerCount; c++) {
				auto name = OtFormat("s_material%d", c + 1);

				samplers[c] = OtFrameworkClass::instance()->getUniform(
					name.c_str(), bgfx::UniformType::Sampler);
			}
		}

		// create shaders (which are automatically destroyed by the framework)
		shader = createShader();
		instancingShader = createInstancingShader();

		// set flag
		initialized = true;
	}

	// submit uniforms (if required)
	if (uniformCount) {
		getUniforms(uniformValues);
		bgfx::setUniform(uniforms, uniformValues, uniformCount);
	}

	// submit samplers (if required)
	if (samplerCount) {
		for (auto c = 0; c < samplerCount; c++) {
			bgfx::setTexture(c + 1, samplers[c], getSamplerTexture(c + 1));
		}
	}
	// run appropriate shader
	if (instancing) {
		bgfx::submit(view, instancingShader);

	} else {
		bgfx::submit(view, shader);
	}
}


//
//	OtMaterialClass::getMeta
//

OtType OtMaterialClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtMaterialClass>("Material", OtGuiClass::getMeta());

		type->set("setFrontSide", OtFunctionClass::create(&OtMaterialClass::setFrontSide));
		type->set("setBackSide", OtFunctionClass::create(&OtMaterialClass::setBackSide));
		type->set("setDoubleSided", OtFunctionClass::create(&OtMaterialClass::setDoubleSided));
		type->set("isFrontSided", OtFunctionClass::create(&OtMaterialClass::isFrontSided));
		type->set("isBackSided", OtFunctionClass::create(&OtMaterialClass::isBackSided));
		type->set("isDoubleSided", OtFunctionClass::create(&OtMaterialClass::isDoubleSided));

		type->set("setTransparent", OtFunctionClass::create(&OtMaterialClass::setTransparent));
		type->set("setOpaque", OtFunctionClass::create(&OtMaterialClass::setOpaque));
		type->set("isTransparent", OtFunctionClass::create(&OtMaterialClass::isTransparent));
		type->set("isOpaque", OtFunctionClass::create(&OtMaterialClass::isOpaque));
	}

	return type;
}
