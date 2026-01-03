//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

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

	// get output format (none means use the same format as the incoming texture)
	inline virtual OtTexture::Format getOutputFormat() { return OtTexture::Format::none; }

	// run the filter
	inline void onExecute() override {
		// do we have a valid input
		if (inputTexture.isValid()) {
			// ensure output is the correct size and format
			auto format = getOutputFormat() == OtTexture::Format::none ? inputTexture.getFormat() : getOutputFormat();
			outputTexture.update(inputTexture.getWidth(), inputTexture.getHeight(), format, OtTexture::Usage::rwDefault);

			// run filter
			onFilter(inputTexture, outputTexture);
			outputTexture.setVersion(version++);

		} else {
			outputTexture.clear();
		}
	}

	// the actual filter execution (to be overriden by subclasses)
	virtual void onFilter(OtTexture& input, OtTexture& output) = 0;

	// properties
	OtTexture inputTexture;
	OtTexture outputTexture;
	int version = 1;
};
