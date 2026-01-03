//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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

#include "OtShapeModule.h"
#include "OtScriptAsset.h"

#include "OtNodesFactory.h"


//
//	New script template
//

static const char* scriptTemplate = "\
var shape = import(\"shape\");\n\
\n\
class Generator : shape.Shape {\n\
	function generate(this) {\n\
	}\n\
}\n\
";


//
//	OtShapeGeneratorNode
//

class OtShapeGeneratorNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Shape", shape);
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

	// execute the node by generating a new version of the shape
	inline void onExecute() override {
		if (script.isReady() && instance && hasRenderMethod) {
			try {
				OtVM::callMemberFunction(instance, GenerateID);
				shape = OtShapeFromObject(instance);
				shape.close();
				shape.setVersion(version++);
				error.clear();

			} catch (OtException& e) {
				onError(e);
			}

		} else {
			// no script available so we clear the shape
			shape.clear();
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

				if (!module->has(GeneratorID)) {
					OtLogError("Script [{}] does not contain class [Generator]", path);
				}

				auto classObject = module->get(GeneratorID);

				// ensure it is a class object
				if (!classObject.isKindOf<OtClassClass>()) {
					OtLogError("Object [Generator] in script [{}] is not a class", path);
				}

				// create instance of class
				instance = OtClass(classObject)->instantiate();

				// ensure the class is derived from Shape
				if (!OtShapeValidateObject(instance)) {
					OtLogError("Class [Generator] in script [{}] is not derived from [Shape]", path);
				}

				// ensure class has a render method
				if (!instance->has(GenerateID)) {
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

	static constexpr const char* nodeName = "Shape Generator";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::shape;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtAsset<OtScriptAsset> script;
	OtObject instance;
	OtShape shape;
	int version = 1;

	OtID GenerateID = OtIdentifier::create("generate");
	OtID GeneratorID = OtIdentifier::create("Generator");
	bool hasRenderMethod = false;
};

static OtNodesFactoryRegister<OtShapeGeneratorNode> registration;
