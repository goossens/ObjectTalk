//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
			[[fallthrough]];

		case 1:
			setHorizontalAlignment(OtUi::Alignment(parameters[0]->operator int()));
			[[fallthrough]];

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
	OtCanvas(findCanvas(id)->canvas)->enable();
}


//
//	OtCanvasStackClass::disableCanvas
//

void OtCanvasStackClass::disableCanvas(int id) {
	OtCanvas(findCanvas(id)->canvas)->disable();
}


//
//	OtCanvasStackClass::rerenderCanvas
//

void OtCanvasStackClass::rerenderCanvas(int id) {
	OtCanvas(findCanvas(id)->canvas)->requestRerender();
}


//
//	OtCanvasStackClass::render
//

void OtCanvasStackClass::render() {
	// determine size and scale
	auto available = ImGui::GetContentRegionAvail();
	auto scale = std::min(available.x / width, available.y / height);
	auto w = int(width * scale);
	auto h = int(height * scale);

	bool sizeChanged = w != framebuffer.getWidth() || h != framebuffer.getHeight();
	bool dirty = false;

	for (auto& canvas : canvases) {
		auto cvs = OtCanvas(canvas.canvas);

		if (sizeChanged || cvs->needsRerender()) {
			// update framebuffer size (if required)
			canvas.framebuffer.update(w, h);

			// render the canvas
			cvs->render(canvas.framebuffer, scale, [&]() {
				OtVM::callMemberFunction(canvas.canvas, "render");
			});

			cvs->markClean();
			dirty = true;
		}
	}

	// (re)-composite all layers (if required)
	if (dirty) {
		framebuffer.update(w, h);

		OtPass clearPass;
		clearPass.setFrameBuffer(framebuffer);
		clearPass.setRectangle(0, 0, w, h);
		clearPass.setClear(true);
		clearPass.touch();

		for (auto& canvas : canvases) {
			if (OtCanvas(canvas.canvas)->isEnabled()) {
				OtPass overlayPass;
				overlayPass.setFrameBuffer(framebuffer);
				overlayPass.setRectangle(0, 0, w, h);

				overlayPass.submitQuad(w, h);
				sampler.submit(0, canvas.framebuffer.getColorTextureHandle());
				program.setState(OtStateWriteRgb | OtStateWriteA | OtStateBlendAlpha);
				overlayPass.runShaderProgram(program);
			}
		}
	}

	ImVec2 size{float(w), float(h)};
	auto texture = framebuffer.getColorTexture();

	ImGui::PushID(this);
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

#if !_WIN32
	return nullptr;
#endif
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
