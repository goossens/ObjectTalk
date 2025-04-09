//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtImage.h"
#include "OtReadBackBuffer.h"
#include "OtTexture.h"

#include "OtNodesFactory.h"


//
//	OtTextureToImageNode
//

class OtTextureToImageNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", texture);
		addOutputPin("Output", image);
	}

	// when the input changes, we read back the texture into a CPU buffer
	inline void onExecute() override {
		if (texture.isValid()) {
			buffer.readback(texture, [&]() {
				image = buffer.getImage();
			});

		} else {
			image.clear();
		}
	}

	static constexpr const char* nodeName = "Texture to Image";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::transform;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtTexture texture;
	OtImage image;
	OtReadBackBuffer buffer;
};

static OtNodesFactoryRegister<OtTextureToImageNode> registration;
