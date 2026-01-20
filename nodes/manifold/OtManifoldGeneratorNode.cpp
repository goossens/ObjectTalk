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

#include "OtScriptAsset.h"
#include "OtManifoldModule.h"

#include "OtNodesFactory.h"


//
//	New script template
//

static const char* scriptTemplate = "\
var manifold = import(\"manifold\");\n\
\n\
class Generator : Object {\n\
	function generate(this) {\n\
		return manifold.Manifold();\n\
	}\n\
}\n\
";


//
//	OtManifoldGeneratorNode
//

class OtManifoldGeneratorNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Manifold", manifold);
		script.onChange([&]() { onScriptChange(); });
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		ImGui::SetNextItemWidth(itemWidth);
		auto old = serialize().dump();

		auto changed = script.renderUI("##script", [](const std::string& path) {
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

	// execute the node by generating a new version of the manifold
	inline void onExecute() override {
		if (script.isReady() && instance && hasRenderMethod) {
			try {
				error.clear();
				auto result = OtVM::callMemberFunction(instance, GenerateID);

				if (!OtManifoldValidateObject(result)) {
					OtLogError("[generate] method returned a [{}], not a [Manifold]", result.getTypeName());
				}

				manifold = OtManifoldFromObject(result);

			} catch (OtException& e) {
				onError(e);
			}

		} else {
			// no script available so we clear the manifold
			manifold.clear();
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

	static constexpr const char* nodeName = "Manifold Generator";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::manifold;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtAsset<OtScriptAsset> script;
	OtObject instance;
	OtManifold manifold;

	OtID GenerateID = OtIdentifier::create("generate");
	OtID GeneratorID = OtIdentifier::create("Generator");
	bool hasRenderMethod = false;
};

static OtNodesFactoryRegister<OtManifoldGeneratorNode> registration;
