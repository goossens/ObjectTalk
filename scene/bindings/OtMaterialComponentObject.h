//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtComponentObject.h"
#include "OtMaterialComponent.h"


//
//	Helper macros
//

#define declareMaterialProperty(T, N)												\
	inline void set##N(T value) { component().material->set##N(value); }			\
	inline T get##N() { return component().material->get##N(); }

#define declareMaterialVectorProperty(D, N)											\
	inline void set##N(size_t count, OtObject* parameters) {						\
		component().material->set##N(toVec##D(count, parameters));					\
	}																				\
																					\
	inline glm::vec##D get##N() { return component().material->get##N(); }


//
//	OtMaterialComponentObjectClass
//

class OtMaterialComponentObjectClass;
using OtMaterialComponentObject = OtObjectPointer<OtMaterialComponentObjectClass>;

class OtMaterialComponentObjectClass : public OtComponentObjectClass {
public:
	// access the properties
	declareMaterialVectorProperty(4, Albedo)
	declareMaterialProperty(float, Metallic)
	declareMaterialProperty(float, Roughness)
	declareMaterialVectorProperty(3, Emissive)
	declareMaterialProperty(float, Ao)
	declareMaterialVectorProperty(2, TextureOffset)
	declareMaterialProperty(float, TextureScale)

	declareMaterialProperty(std::string, AlbedoTexture)
	declareMaterialProperty(std::string, NormalTexture)
	declareMaterialProperty(std::string, MetallicRoughnessTexture)
	declareMaterialProperty(std::string, EmissiveTexture)
	declareMaterialProperty(std::string, AoTexture)

	// get type definition
	static inline OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtMaterialComponentObjectClass>("MaterialComponent", OtComponentObjectClass::getMeta());

			metaProperty(Material, Metallic)
			metaProperty(Material, Roughness)
			metaProperty(Material, Emissive)
			metaProperty(Material, Ao)
			metaProperty(Material, TextureOffset)
			metaProperty(Material, TextureScale)

			metaProperty(Material, AlbedoTexture)
			metaProperty(Material, NormalTexture)
			metaProperty(Material, MetallicRoughnessTexture)
			metaProperty(Material, EmissiveTexture)
			metaProperty(Material, AoTexture)
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtMaterialComponentObjectClass>;
	OtMaterialComponentObjectClass() = default;
	OtMaterialComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtMaterialComponent& component() { return OtComponentObjectClass::component<OtMaterialComponent>(scene, entity); }
};
