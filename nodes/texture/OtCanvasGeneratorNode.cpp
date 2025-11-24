//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "fmt/format.h"
#include "nlohmann/json.hpp"

#include "OtClass.h"
#include "OtException.h"
#include "OtIdentifier.h"
#include "OtLog.h"
#include "OtMessageBus.h"
#include "OtText.h"
#include "OtVM.h"

#include "OtCanvasModule.h"
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
class Generator : canvas.Canvas {\n\
	function generate(this) {\n\
	}\n\
}\n\
";


//
//	OtCanvasGeneratorNode
//

class OtCanvasGeneratorNode : public OtTextureGeneratorNode {
public:
	// configure node
	inline void configure() override {
		OtTextureGeneratorNode::configure();
		script.onChange([&]() { onScriptChange(); });
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		ImGui::SetNextItemWidth(itemWidth);
		auto old = serialize().dump();

		bool changed = script.renderUI("##script", [](const std::string& path) {
			// create a new script file
			OtText::save(path, scriptTemplate);
		});

		// do some state management if we have a new script specification
		if (changed) {
			oldState = old;
			newState = serialize().dump();
			needsEvaluating = true;
			needsSaving = true;
		}
	}

	inline float getCustomRenderingWidth() override {
		return 200.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["path"] = OtAssetSerialize(script.getPath(), basedir);
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		script = OtAssetDeserialize(data, "path", basedir);
	}

	// run the texture generator
	inline void onGenerate(OtTexture& output) override {
		if (script.isReady() && instance && hasRenderMethod) {
			// get access to the canvas
			auto& canvas = OtCanvasObject(instance)->getCanvas();

			// render the canvas by calling the script
			try {
				framebuffer.update(width, height);

				canvas.render(framebuffer, 1.0f, [this]() {
					OtVM::callMemberFunction(instance, generateID);
				});

				output = framebuffer.getColorTexture();
				error.clear();

			} catch (OtException& e) {
				onError(e);
			}

		} else {
			// no script available so we clear the output
			output.clear();
		}
	}

	// gets called when script is specified or changed
	inline void onScriptChange() {
		instance = nullptr;
		hasRenderMethod = false;

		if (script.isReady()) {
			try {
				script->compile();
				auto path = script.getPath();
				auto module = script->getModule();

				if (!module->has(generatorID)) {
					OtLogError("Script [{}] does not contain class [Generator]", path);
				}

				auto classObject = module->get(generatorID);

				// ensure it is a class object
				if (!classObject.isKindOf<OtClassClass>()) {
					OtLogError("Object [Generator] in script [{}] is not a class", path);
				}

				// create instance of class
				instance = OtClass(classObject)->instantiate();

				// ensure the class is derived from Canvas
				if (!instance.isKindOf<OtCanvasClass>()) {
					OtLogError("Class [Generator] in script [{}] is not derived from [Canvas]", path);
				}

				// ensure class has a render method
				if (!instance->has(generateID)) {
					OtLogError("Class [Generator] in script [{}] does not have a [generate] method", path);
				}

				hasRenderMethod = true;

			} catch (OtException& e) {
				onError(e);
			}
		}

		needsEvaluating = true;
	}

	void onError(OtException& e) {
		instance = nullptr;
		hasRenderMethod = false;
		error = e.what();

		if (e.getModule().size()) {
			OtMessageBus::send(fmt::format("highlight {}", e.serialize()));
		}
	}

	static constexpr const char* nodeName = "Canvas Generator";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::texture;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	OtAsset<OtScriptAsset> script;
	OtObject instance;

	OtID generateID = OtIdentifier::create("generate");
	OtID generatorID = OtIdentifier::create("Generator");
	bool hasRenderMethod = false;

	OtFrameBuffer framebuffer{OtTexture::Format::rgba8, OtTexture::Format::d32s8};
};

static OtNodesFactoryRegister<OtCanvasGeneratorNode> registration;
