//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"

#include "OtGui.h"


//
//	OtMaterial
//

class OtMaterialClass;
typedef std::shared_ptr<OtMaterialClass> OtMaterial;

class OtMaterialClass : public OtGuiClass {
public:
	// destructor
	~OtMaterialClass();

	// determines which side of faces will be rendered
	OtObject setFrontSide() { frontside = true; backside = false; return shared(); }
	OtObject setBackSide() { frontside = false; backside = true; return shared(); }
	OtObject setDoubleSided() { frontside = true; backside = true; return shared(); }
	bool isFrontSided() { return frontside; }
	bool isBackSided() { return backside; }
	bool isDoubleSided() { return frontside && backside; }

	// access transparency
	OtObject setTransparent() { transparent = true; return shared(); }
	OtObject setOpaque() { transparent = false; return shared(); }
	bool isTransparent() { return transparent; }
	bool isOpaque() { return transparent; }

	// submit to GPU
	void submit(bgfx::ViewId view, bool instancing);

	// get type definition
	static OtType getMeta();

protected:
	virtual size_t getNumberOfUniforms() { return 0; }
	virtual void getUniforms(glm::vec4* uniforms) {}

	virtual size_t getNumberOfSamplers() { return 0; }
	virtual bgfx::TextureHandle getSamplerTexture(size_t index) { return BGFX_INVALID_HANDLE; }

	virtual bgfx::ProgramHandle createShader() { return BGFX_INVALID_HANDLE; }
	virtual bgfx::ProgramHandle createInstancingShader() { return BGFX_INVALID_HANDLE; }

private:
	// properties
	bool frontside = true;
	bool backside = false;
	bool transparent = false;

	// uniforms
	bool initialized = false;
	bgfx::UniformHandle uniforms = BGFX_INVALID_HANDLE;
	glm::vec4* uniformValues = nullptr;

	// samplers
	bgfx::UniformHandle* samplers = nullptr;

	// shaders
	bgfx::ProgramHandle shader;
	bgfx::ProgramHandle instancingShader;
};
