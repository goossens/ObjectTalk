//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "glm/glm.hpp"

#include "OtObject.h"

#include "OtMaterialComponent.h"


//
//	OtMaterialComponentObjectClass
//

class OtMaterialComponentObjectClass;
using OtMaterialComponentObject = OtObjectPointer<OtMaterialComponentObjectClass>;

class OtMaterialComponentObjectClass : public OtObjectClass {
public:
	// set the properties
	inline void setAlbedo(glm::vec4 albedo) { material->material->setAlbedo(albedo); }
	inline void setMetallic(float metallic) { material->material->setMetallic(metallic); }
	inline void setRoughness(float roughness) { material->material->setRoughness(roughness); }
	inline void setEmissive(glm::vec3 emissive) { material->material->setEmissive(emissive); }
	inline void setAo(float ao) { material->material->setAo(ao); }
	inline void setTextureOffset(glm::vec2 offset) { material->material->setTextureOffset(offset); }
	inline void setTextureScale(float scale) { material->material->setTextureScale(scale); }

	// set the textures
	inline void setAlbedoTexture(const std::string& path) { material->material->setAlbedoTexture(path); }
	inline void setNormalTexture(const std::string& path) { material->material->setNormalTexture(path); }
	inline void setMetallicRoughnessTexture(const std::string& path) { material->material->setMetallicRoughnessTexture(path); }
	inline void setEmissiveTexture(const std::string& path) { material->material->setEmissiveTexture(path); }
	inline void setAoTexture(const std::string& path) { material->material->setAoTexture(path); }

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtMaterialComponentObjectClass>;
	OtMaterialComponentObjectClass() = default;
	OtMaterialComponentObjectClass(OtMaterialComponent* component) : material(component) {}

private:
	// connection to component
	OtMaterialComponent* material;
};
