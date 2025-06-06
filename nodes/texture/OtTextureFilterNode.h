//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
	// configure node
	inline void configure() override {
		addInputPin("Input", inputTexture);
		addOutputPin("Output", outputTexture);
	}

	// get output format (noTexture means use the same format as the incoming texture)
	inline virtual int getOutputFormat() { return OtTexture::noTexture; }

	// run the filter
	inline void onExecute() override {
		// do we have a valid input
		if (inputTexture.isValid()) {
			// determine output texture format for framebuffer
			auto format = getOutputFormat();
			format = format == OtTexture::noTexture ? inputTexture.getFormat() : format;

			// reinitialize framebuffer if format changed
			if (format != framebuffer.getColorTextureType()) {
				framebuffer.initialize(format);
			}

			// ensure framebuffer has the right size
			framebuffer.update(inputTexture.getWidth(), inputTexture.getHeight());

			// run filter
			onFilter(inputTexture, framebuffer);

			// manage output (versions number are used to detect changes down the line)
			outputTexture = framebuffer.getColorTexture();
			outputTexture.setVersion(version++);

		} else {
			outputTexture.clear();
		}
	}

	// the actual filter execution (to be overriden by subclasses)
	virtual void onFilter(OtTexture& input, OtFrameBuffer& output) = 0;

	// properties
	OtFrameBuffer framebuffer;
	OtTexture inputTexture;
	OtTexture outputTexture;
	int version = 1;
};
