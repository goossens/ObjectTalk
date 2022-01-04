//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "blendmap.h"


//
//	OtBlendMapClass::OtBlendMapClass
//

OtBlendMapClass::OtBlendMapClass() {
	blendmapUniform = bgfx::createUniform("s_blendmap", bgfx::UniformType::Sampler);
	textureUniformN = bgfx::createUniform("s_texture_n", bgfx::UniformType::Sampler);
	textureUniformR = bgfx::createUniform("s_texture_r", bgfx::UniformType::Sampler);
	textureUniformG = bgfx::createUniform("s_texture_g", bgfx::UniformType::Sampler);
	textureUniformB = bgfx::createUniform("s_texture_b", bgfx::UniformType::Sampler);
}


//
//	OtBlendMapClass::~OtBlendMapClass
//

OtBlendMapClass::~OtBlendMapClass() {
	bgfx::destroy(blendmapUniform);
	bgfx::destroy(textureUniformN);
	bgfx::destroy(textureUniformR);
	bgfx::destroy(textureUniformG);
	bgfx::destroy(textureUniformB);
}


//
//	OtBlendMapClass::init
//

void OtBlendMapClass::init(OtObject bm, OtObject tn, OtObject tr, OtObject tg, OtObject tb) {
	blendmap = toTexture(bm);
	textureN = toTexture(tn);
	textureR = toTexture(tr);
	textureG = toTexture(tg);
	textureB = toTexture(tb);
}


//
//	OtBlendMapClass::toTexture
//

OtTexture OtBlendMapClass::toTexture(OtObject object) {
	// ensure object is a texture
	if (!object->isKindOf("Texture")) {
		OtExcept("Expected a [Texture] object, not a [%s]", object->getType()->getName().c_str());
	}

	return object->cast<OtTextureClass>();
}


//
//	OtBlendMapClass::submit
//

void OtBlendMapClass::submit() {
	blendmap->submit(0, blendmapUniform);
	textureN->submit(1, textureUniformN);
	textureR->submit(2, textureUniformR);
	textureG->submit(3, textureUniformG);
	textureB->submit(4, textureUniformB);
}


//
//	OtBlendMapClass::getMeta
//

OtType OtBlendMapClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtBlendMapClass>("BlendMap", OtGuiClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtBlendMapClass::init));
	}

	return type;
}


//
//	OtBlendMapClass::create
//

OtBlendMap OtBlendMapClass::create() {
	OtBlendMap blendmap = std::make_shared<OtBlendMapClass>();
	blendmap->setType(getMeta());
	return blendmap;
}
