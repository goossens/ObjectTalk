//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtBlendMap.h"
#include "OtFramework.h"


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

void OtBlendMapClass::init(const std::string& bm, const std::string& tn, const std::string& tr, const std::string& tg, const std::string& tb) {
	auto framework = OtFrameworkClass::instance();
	blendmap = framework->getTexture(bm);
	textureN = framework->getTexture(tn);
	textureR = framework->getTexture(tr);
	textureG = framework->getTexture(tg);
	textureB = framework->getTexture(tb);
}


//
//	OtBlendMapClass::submit
//

void OtBlendMapClass::submit() {
	bgfx::setTexture(1, blendmapUniform, blendmap);
	bgfx::setTexture(2, textureUniformN, textureN);
	bgfx::setTexture(3, textureUniformR, textureR);
	bgfx::setTexture(4, textureUniformG, textureG);
	bgfx::setTexture(5, textureUniformB, textureB);
}


//
//	OtBlendMapClass::getMeta
//

OtType OtBlendMapClass::getMeta() {
	static OtType type;

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
