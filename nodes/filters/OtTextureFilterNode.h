//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFrameBuffer.h"
#include "OtTexture.h"

#include "OtNode.h"


//
//	OtTextureFilterNode
//

class OtTextureFilterNode : public OtNodeClass {
public:
	// constructor
	inline OtTextureFilterNode(const char* name) : OtNodeClass(name, OtNodeClass::filter) {}

	// get output format (noTexture means use the same format as the incoming texture)
	inline virtual int getOutputFormat() { return OtTexture::noTexture; }

	// configure node
	inline void configure() override {
		addInputPin("Input", inputTexture);
		addOutputPin("Output", outputTexture);
	}

	// running the HeightMap to NormalMap filter
	void onExecute() override {
		// do we have a valid input
		if (inputTexture.isValid()) {
			// determine output texture format for framebuffer
			auto format = getOutputFormat();
			format = format == OtTexture::noTexture ? inputTexture.getFormat() : format;
			framebuffer.initialize(format);

			// ensure framebuffer has the right size
			framebuffer.update(inputTexture.getWidth(), inputTexture.getHeight());

			// run filter
			onFilter();

			// manage output (versions number are used to detect changes down the line)
			outputTexture = framebuffer.getColorTexture();
			outputTexture.setVersion(version++);

		} else {
			outputTexture.clear();
		}
	}

	// the actual filter execution (to be overriden by subclasses)
	virtual void onFilter() = 0;

	// properties
	OtFrameBuffer framebuffer;
	OtTexture inputTexture;
	OtTexture outputTexture;
	int version = 1;
};
