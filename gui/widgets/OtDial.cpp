//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include "imgui.h"

#include "OtFunction.h"
#include "OtLog.h"

#include "OtBlit.h"
#include "OtPass.h"
#include "OtTransientVertexBuffer.h"
#include "OtUi.h"
#include "OtVertex.h"

#include "OtDial.h"


//
//	OtDialClass::init
//

void OtDialClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 4:
			setVerticalAlignment(OtUi::Alignment(parameters[3]->operator int()));
			[[fallthrough]];

		case 3:
			setHorizontalAlignment(OtUi::Alignment(parameters[2]->operator int()));
			[[fallthrough]];

		case 2:
			setNeedle(parameters[1]->operator std::string());
			[[fallthrough]];

		case 1:
			setBackground(parameters[0]->operator std::string());
			[[fallthrough]];

		case 0:
			break;

		default:
		OtLogError("[Dial] constructor expects up to 4 arguments (not {})", count);
	}
}


//
//	OtDialClass::render
//

void OtDialClass::render() {
	// sanity check
	if (background.isNull()) {
		OtLogError("[Dial] does not have a background");
	}

	// wait until background is ready
	if (background.isReady()) {
		// do we need to redraw widget
		if (redraw) {
			// determine framebuffer dimensions
			auto& backgroundTexture = background->getTexture();
			auto w = backgroundTexture.getWidth();
			auto h = backgroundTexture.getHeight();
			framebuffer.update(w, h);

			// render background
			OtBlit blit;
			blit.render(backgroundTexture, framebuffer);

			// render needle (if required)
			if (needle.isReady()) {
				// determine needle rotation
				auto ratio = (value - minValue) / (maxValue - minValue);
				auto rotation = minRotation + ratio * (maxRotation - minRotation);

				// configure rendering pass
				OtPass pass;
				pass.setRectangle(0, 0, w, h);
				pass.setFrameBuffer(framebuffer);
				glm::mat4 projMatrix = glm::ortho(0.0f, static_cast<float>(w), static_cast<float>(h), 0.0f);
				pass.setTransform(glm::mat4(1.0f), projMatrix);

				// render needle
				auto& needleTexture = needle->getTexture();
				w = needleTexture.getWidth();
				h = needleTexture.getHeight();

				// submit vertices
				static float vertices[] = {
					// pos      // tex
					0.0f, 1.0f, 0.0f, 1.0f,
					1.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 0.0f,

					0.0f, 1.0f, 0.0f, 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f, 0.0f, 1.0f, 0.0f
				};

				OtTransientVertexBuffer tvb;
				tvb.submit(vertices, sizeof(vertices) / sizeof(*vertices), OtVertexPosUv2D::getLayout());

				// bind needle to sampler
				sampler.submit(0, needleTexture);

				// determine transformation
				glm::mat4 model{1.0f};
				model = glm::translate(model, glm::vec3(bx, by, 0.0f));
				model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	 			model = glm::translate(model, glm::vec3(-nx, -ny, 0.0f));
				model = glm::scale(model, glm::vec3(w, h, 1.0f));

				// run the program
				pass.setTransform(model);

				pass.setState(
					OtStateWriteRgb |
					OtStateWriteA |
					OtStateBlendAlpha);

				pass.runShaderProgram(program);
			}

			// reset flag
			redraw = false;
		}

		auto size = ImVec2(framebuffer.getWidth() * scale, framebuffer.getHeight() * scale);
		OtUi::align(size, horizontalAlign, verticalAlign);
		ImGui::Image(framebuffer.getColorTextureID(), size);
	}
}


//
//	OtDialClass::getMeta
//

OtType OtDialClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtDialClass>("Dial", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtDialClass::init));
		type->set("setBackground", OtFunction::create(&OtDialClass::setBackground));
		type->set("setNeedle", OtFunction::create(&OtDialClass::setNeedle));
		type->set("setValue", OtFunction::create(&OtDialClass::setValue));
		type->set("getValue", OtFunction::create(&OtDialClass::getValue));
		type->set("setValueLimits", OtFunction::create(&OtDialClass::setValueLimits));
		type->set("setRotationLimits", OtFunction::create(&OtDialClass::setRotationLimits));
		type->set("setScale", OtFunction::create(&OtDialClass::setScale));
		type->set("setHorizontalAlignment", OtFunction::create(&OtDialClass::setHorizontalAlignment));
		type->set("setVerticalAlignment", OtFunction::create(&OtDialClass::setVerticalAlignment));
	}

	return type;
}
