//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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

	// when the input changes, we read back the texture into a CPU image
	inline void onExecute() override {
		if (texture.isValid()) {
			image = buffer.readback(texture);

		} else {
			image.clear();
		}
	}

	static constexpr const char* nodeName = "Texture to Image";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::texture;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	OtTexture texture;
	OtImage image;
	OtReadBackBuffer buffer;
};

static OtNodesFactoryRegister<OtTextureToImageNode> registration;
