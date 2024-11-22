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

#include "OtClass.h"
#include "OtIdentifier.h"
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
var canvas = import(\"canvas\");\n\
\n\
class Renderer : canvas.Canvas {\n\
	function render(this) {\n\
	}\n\
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
		if (script.isReady() && instance && hasRenderMethod) {
			// get access to the canvas
			auto canvas = OtCanvas(instance);

			// render the canvas by calling the script
			canvas->render(output, [this]() {
				OtVM::callMemberFunction(instance, renderID);
			});

		} else {
			// no script available so we clear the output
			output.clear();
		}
	}

	// gets called when canvas script is specified or changed
	void onScriptChange() {
		instance = nullptr;
		hasRenderMethod = false;

		if (script.isReady()) {
			script->compile();
			auto path = script.getPath();
			auto module = script->getModule();

			if (module->has(rendererID)) {
				auto classObject = module->get(rendererID);

				// ensure it is a class object
				if (classObject.isKindOf<OtClassClass>()) {
					// create instance of class
					instance = OtClass(classObject)->instantiate();

					// ensure the class is derived from Canvas
					if (instance.isKindOf<OtCanvasClass>()) {
						hasRenderMethod = instance->has(renderID);

					} else {
						OtError("Class [Renderer] in script [{}] is not derived from [Canvas]", path);
					}

				} else {
					OtError("Object [Renderer] in script [{}] is not a class", path);
				}

			} else {
				OtError("Script [{}] does not contain class [Renderer]", path);
			}
		}

		needsEvaluating = true;
	}

	static constexpr const char* nodeName = "Canvas Generator";
	static constexpr int nodeCategory = OtNodeClass::generate;
	static constexpr int nodeKind = OtNodeClass::fixed;

protected:
	OtAsset<OtScriptAsset> script;
	OtObject instance;

	OtID renderID = OtIdentifier::create("render");
	OtID rendererID = OtIdentifier::create("Renderer");
	bool hasRenderMethod = false;
};

static OtNodesFactoryRegister<OtCanvasNode> type;
