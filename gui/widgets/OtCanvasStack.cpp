//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "imgui.h"

#include "OtException.h"
#include "OtVM.h"

#include "OtPass.h"
#include "OtUi.h"

#include "OtCanvasStack.h"


//
//	OtCanvasStackClass::init
//

void OtCanvasStackClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 2:
			setVerticalAlignment(OtUi::Alignment(parameters[1]->operator int()));

		case 1:
			setHorizontalAlignment(OtUi::Alignment(parameters[0]->operator int()));

		case 0:
			break;

		default:
			OtLogFatal("[Board] constructor expects 0, 1 or 2 arguments (not {})", count);
	}
}


//
//	OtCanvasStackClass::addCanvas
//

int OtCanvasStackClass::addCanvas(OtObject canvas) {
	// sanity check
	if (!canvas.isKindOf<OtCanvasClass>()) {
		OtError("Object is not derived from [Canvas]");
	}

	if (!canvas->hasByName("render")) {
		OtError("Object does not have a [render] method");
	}

	int id = layerID++;
	canvases.emplace_back(id, canvas);
	return id;
}


//
//	OtCanvasStackClass::deleteCanvas
//

void OtCanvasStackClass::deleteCanvas(int id) {
	for (auto it = canvases.begin(); it < canvases.end(); it++) {
		if (it->id == id) {
			canvases.erase(it);
			return;
		}
	}
}


//
//	OtCanvasStackClass::enableCanvas
//

void OtCanvasStackClass::enableCanvas(int id) {
	findCanvas(id)->enabled = true;
}


//
//	OtCanvasStackClass::disableCanvas
//

void OtCanvasStackClass::disableCanvas(int id) {
	findCanvas(id)->enabled = false;
}


//
//	OtCanvasStackClass::rerenderCanvas
//

void OtCanvasStackClass::rerenderCanvas(int id) {
	findCanvas(id)->dirty = true;
}


//
//	OtCanvasStackClass::render
//

void OtCanvasStackClass::render() {
	// determine size and scale
	auto available = ImGui::GetContentRegionAvail();
	auto scale = std::min(available.x / width, available.y / height);
	ImVec2 size{width * scale, height * scale};

	bool dirty = false;

	for (auto& canvas : canvases) {
		if (canvas.enabled && canvas.dirty) {
			// update framebuffer size (if required)
			canvas.framebuffer.update(size.x, size.y);

			// render the canvas by calling the script
			OtCanvas(canvas.canvas)->render(canvas.framebuffer, scale, [&]() {
				OtVM::callMemberFunction(canvas.canvas, "render");
			});

			canvas.dirty = false;
			dirty = true;
		}
	}

	if (dirty) {
		// (re)-composite all layers
		framebuffer.update(size.x, size.y);

		OtPass pass;
		pass.setFrameBuffer(framebuffer);
		pass.setRectangle(0, 0, int(size.x), int(size.y));
		pass.setClear(true, true);
		pass.touch();

		for (auto& canvas : canvases) {
			pass.submitQuad(int(size.x), int(size.y));
			sampler.submit(0, canvas.framebuffer.getColorTextureHandle());
			program.setState(OtStateWriteRgb | OtStateWriteA | OtStateBlendAlpha);
			pass.runShaderProgram(program);
		}
	}

	ImGui::PushID(this);
	auto texture = framebuffer.getColorTexture();
	OtUi::align(size, horizontalAlign, verticalAlign);
	ImGui::Image((ImTextureID)(intptr_t) texture.getIndex(), size);
	ImGui::PopID();
}


//
//	OtCanvasStackClass::findCanvas
//

OtCanvasStackClass::Canvas* OtCanvasStackClass::findCanvas(int id) {
	for (auto& canvas : canvases) {
		if (canvas.id == id) {
			return &canvas;
		}
	}

	OtError("Unknown canvas ID [{}]", id);
	return nullptr;
}


//
//	OtCanvasStackClass::getMeta
//

OtType OtCanvasStackClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtCanvasStackClass>("CanvasStack", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtCanvasStackClass::init));
		type->set("setSize", OtFunction::create(&OtCanvasStackClass::setSize));
		type->set("setHorizontalAlignment", OtFunction::create(&OtCanvasStackClass::setHorizontalAlignment));
		type->set("setVerticalAlignment", OtFunction::create(&OtCanvasStackClass::setVerticalAlignment));

		type->set("addCanvas", OtFunction::create(&OtCanvasStackClass::addCanvas));
		type->set("deleteCanvas", OtFunction::create(&OtCanvasStackClass::deleteCanvas));
		type->set("enableCanvas", OtFunction::create(&OtCanvasStackClass::enableCanvas));
		type->set("disableCanvas", OtFunction::create(&OtCanvasStackClass::disableCanvas));
		type->set("rerenderCanvas", OtFunction::create(&OtCanvasStackClass::rerenderCanvas));
	}

	return type;
}
