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

#include "OtFrameBuffer.h"
#include "OtTexture.h"
#include "OtUi.h"

#include "OtNode.h"


//
//	OtTextureGeneratorNode
//

class OtTextureGeneratorNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Width", width);
		addInputPin("Height", height);
		addOutputPin("Texture", texture);
	}

	// configure the framebuffer
	inline virtual void configureFrameBuffer() { framebuffer.initialize(OtTexture::rgba8Texture); }

	// execute the node by generating a new version of the output
	inline void onExecute() override {
		// initialize framebuffer (if required)
		if (!framebuffer.isValid()) {
			configureFrameBuffer();
		}

		// ensure framebuffer has right size
		framebuffer.update(width, height);

		// run the generator
		onGenerate(framebuffer);

		// manage output (version numbers are used to detect changes down the line)
		texture = framebuffer.getColorTexture();
		texture.setVersion(version++);
	}

	// the actual generator (to be overriden by subclasses)
	virtual void onGenerate(OtFrameBuffer& output) = 0;

protected:
	int width = 256;
	int height = 256;
	OtFrameBuffer framebuffer;
	OtTexture texture;
	int version = 1;
};
