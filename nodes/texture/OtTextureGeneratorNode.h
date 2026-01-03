//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

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

	// execute the node by generating a new version of the output
	inline void onExecute() override {
		// ensure output texture has correct setup
		texture.update(width, height, OtTexture::Format::rgba8, OtTexture::Usage::rwDefault);

		// run the generator
		onGenerate(texture);
		texture.setVersion(version++);
	}

	// the actual generator (to be overriden by subclasses)
	virtual void onGenerate(OtTexture& output) = 0;

protected:
	int width = 256;
	int height = 256;
	OtTexture texture;
	int version = 1;
};
