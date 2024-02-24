//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtImage.h"
#include "OtTexture.h"

#include "OtNodesFactory.h"


//
//	OtImageToTextureNode
//

class OtImageToTextureNode : public OtNodeClass {
public:
	// constructor
	inline OtImageToTextureNode() : OtNodeClass(name, OtNodeClass::transformer) {}

	inline void configure() override {
		addInputPin("Input", image);
		addOutputPin("Output", texture);
	}

	// when the input changes, we read back the texture into a CPU buffer
	void onExecute() override {
		if (image.isValid()) {
			texture.loadFromImage(image);

		} else {
			texture.clear();
		}
	}

	static constexpr const char* name = "Image to Texture";

protected:
	OtImage image;
	OtTexture texture;
};

static OtNodesFactoryRegister<OtImageToTextureNode> type("Transformers");
