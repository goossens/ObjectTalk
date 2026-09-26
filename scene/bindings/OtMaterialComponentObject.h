//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "glm/glm.hpp" // IWYU pragma: keep

#include "OtComponentObject.h"
#include "OtMaterialComponent.h"


//
//	OtMaterialComponentObjectClass
//

class OtMaterialComponentObjectClass;
using OtMaterialComponentObject = OtObjectPointer<OtMaterialComponentObjectClass>;

class OtMaterialComponentObjectClass : public OtComponentObjectClass {
public:
	// set the properties
	inline void setAlbedo(glm::vec4 albedo) { component().material->setAlbedo(albedo); }
	inline void setMetallic(float metallic) { component().material->setMetallic(metallic); }
	inline void setRoughness(float roughness) { component().material->setRoughness(roughness); }
	inline void setEmissive(glm::vec3 emissive) { component().material->setEmissive(emissive); }
	inline void setAo(float ao) { component().material->setAo(ao); }
	inline void setTextureOffset(glm::vec2 offset) { component().material->setTextureOffset(offset); }
	inline void setTextureScale(float scale) { component().material->setTextureScale(scale); }

	// set the textures
	inline void setAlbedoTexture(const std::string& path) { component().material->setAlbedoTexture(path); }
	inline void setNormalTexture(const std::string& path) { component().material->setNormalTexture(path); }
	inline void setMetallicRoughnessTexture(const std::string& path) { component().material->setMetallicRoughnessTexture(path); }
	inline void setEmissiveTexture(const std::string& path) { component().material->setEmissiveTexture(path); }
	inline void setAoTexture(const std::string& path) { component().material->setAoTexture(path); }

	// get type definition
	static OtType getMeta();

protected:
	// return class name
	inline const char* getClassName() override {return "MaterialComponent"; }

	// constructor
	friend class OtObjectPointer<OtMaterialComponentObjectClass>;
	OtMaterialComponentObjectClass() = default;
	OtMaterialComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtMaterialComponent& component() { return OtComponentObjectClass::component<OtMaterialComponent>(scene, entity); }
};
