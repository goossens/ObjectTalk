//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <fstream>

#include "nlohmann/json.hpp"

#include "OtPathTools.h"
#include "OtVM.h"

#include "OtCanvas.h"
#include "OtColor.h"
#include "OtScriptAsset.h"

#include "OtNodesFactory.h"
#include "OtTextureGeneratorNode.h"


//
//	New script template
//

static const char* scriptTemplate = "\
function render(canvas) {\n\
}\n\
";


//
//	OtCanvasNode
//

class OtCanvasNode : public OtTextureGeneratorNode {
public:
	// configure node
	inline void configure() override {
		OtTextureGeneratorNode::configure();
		script.onChange([&]() { onScriptChange(); });
	}

	// render custom fields
	void customRendering(float width) override {
		ImGui::SetNextItemWidth(width);
		auto old = serialize().dump();

		bool changed = script.renderUI("##ScriptPath", [](const std::string& path) {
			// create a new script file
			std::ofstream stream(path);
			stream << scriptTemplate;
			stream.close();
		});

		// do some state management if we have a new script specification
		if (changed) {
			oldState = old;
			newState = serialize().dump();
			needsEvaluating = true;
			needsSaving = true;
		}
	}

	float getCustomRenderingWidth() override {
		return 200.0f;
	}

	float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["path"] = OtPathRelative(script.getPath(), basedir);
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		script = OtPathGetAbsolute(*data, "path", basedir);
	}

	// run the texture generator
	void onGenerate(OtFrameBuffer& output) override {
		// is our script ready?
		if (script.isReady() && renderer) {
			// yes, create a canvas (if required)
			if (!canvas) {
				canvas = OtCanvas::create();
			}

			// render the canvas by calling the script
			canvas->render(output, [this](OtCanvas canvas) {
				OtVM::callMemberFunction(renderer, "__call__", canvas);
			});

		} else {
			// no script available so we clear the output
			output.clear();
		}
	}

	// gets called when script is specified or changed
	void onScriptChange() {
		if (script.isReady()) {
			script->compile();
			auto module = script->getModule();

			if (module->hasByName("render")) {
				renderer = module->getByName("render");

			} else {
				renderer = nullptr;
				OtError("Script [{}] does not contain function [render]", script.getPath());
			}

		} else {
			renderer = nullptr;
		}

		needsEvaluating = true;
	}

	static constexpr const char* nodeName = "Canvas Generator";
	static constexpr int nodeCategory = OtNodeClass::generate;
	static constexpr int nodeKind = OtNodeClass::fixed;

protected:
	OtAsset<OtScriptAsset> script;
	OtObject renderer;
	OtCanvas canvas;
};

static OtNodesFactoryRegister<OtCanvasNode> type;
