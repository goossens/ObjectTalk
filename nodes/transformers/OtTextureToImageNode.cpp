//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

	// update state
	inline bool onUpdate() override {
		if (texture.isValid() && image.getVersion() != buffer.getImage().getVersion()) {
			image = buffer.getImage();
			return true;

		} else {
			return false;
		}
	}

	// when the input changes, we read back the texture into a CPU buffer
	void onExecute() override {
		if (texture.isValid()) {
			buffer.readback(texture);

		} else {
			image.clear();
		}
	}

	static constexpr const char* nodeName = "Texture to Image";
	static constexpr int nodeCategory = OtNodeClass::transformer;
	static constexpr int nodeKind = OtNodeClass::fixed;

protected:
	OtTexture texture;
	OtImage image;
	OtReadBackBuffer buffer;
};

static OtNodesFactoryRegister<OtTextureToImageNode> type;
