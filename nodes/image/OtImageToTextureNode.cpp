//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtImage.h"
#include "OtTexture.h"

#include "OtNodesFactory.h"


//
//	OtImageToTextureNode
//

class OtImageToTextureNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", image);
		addOutputPin("Output", texture);
	}

	// when the input changes, we write the image to the GPUs texture
	inline void onExecute() override {
		if (image.isValid()) {
			texture.load(image);

		} else {
			texture.clear();
		}
	}

	static constexpr const char* nodeName = "Image to Texture";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::image;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtImage image;
	OtTexture texture;
};

static OtNodesFactoryRegister<OtImageToTextureNode> registration;
