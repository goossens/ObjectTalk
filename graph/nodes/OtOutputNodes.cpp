//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "OtImageAsset.h"
#include "OtPathTools.h"
#include "OtTextureAsset.h"

#include "OtGraphNode.h"
#include "OtOutputNodes.h"


//
//	OtGraphNodeImageToPngOutput
//

class OtGraphNodeImageToPngOutput : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNodeImageToPngOutput() : OtGraphNodeClass(name, OtGraphNodeClass::output) {}

	// configure node
	inline void configure() override {
		addInputPin("Input", image)->addRenderer([&](float width) {
			// render button to save image if image is valid
			if (!image.isValid()) {
				ImGui::BeginDisabled();
			}

			if (ImGui::Button("Save As...", ImVec2(width, 0.0f))) {
				ImGuiFileDialog::Instance()->OpenDialog(
					"image-saveas-png",
					"Save Image as...",
					".*",
					OtPathGetCurrentWorkingDirectory(),
					"",
					1,
					nullptr,
					ImGuiFileDialogFlags_Modal |
						ImGuiFileDialogFlags_DontShowHiddenFiles |
						ImGuiFileDialogFlags_ConfirmOverwrite);
			}

			if (!image.isValid()) {
				ImGui::EndDisabled();
			}

			// handle saveas dialog
			ImVec2 maxSize = ImGui::GetIO().DisplaySize;
			ImVec2 minSize = ImVec2(maxSize.x * 0.5, maxSize.y * 0.5);

			if (ImGuiFileDialog::Instance()->Display("image-saveas-png", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
				// open selected file if required
				if (ImGuiFileDialog::Instance()->IsOk()) {
					auto path = ImGuiFileDialog::Instance()->GetFilePathName();
					path = OtPathReplaceExtension(path, ".png");
					OtPathChangeDirectory(OtPathGetParent(path));
					image.saveToPNG(path);
				}

				// close dialog
				ImGuiFileDialog::Instance()->Close();
			}
		});
	}

	static constexpr const char* name = "Save Image To PNG";

protected:
	OtImage image;
};


//
//	OtGraphNodeImageToDdsOutput
//

class OtGraphNodeImageToDdsOutput : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNodeImageToDdsOutput() : OtGraphNodeClass(name, OtGraphNodeClass::output) {}

	// configure node
	inline void configure() override {
		addInputPin("Input", image)->addRenderer([&](float width) {
			// render button to save image if image is valid
			if (!image.isValid()) {
				ImGui::BeginDisabled();
			}

			if (ImGui::Button("Save As...", ImVec2(width, 0.0f))) {
				ImGuiFileDialog::Instance()->OpenDialog(
					"image-saveas-dds",
					"Save Image as...",
					".*",
					OtPathGetCurrentWorkingDirectory(),
					"",
					1,
					nullptr,
					ImGuiFileDialogFlags_Modal |
						ImGuiFileDialogFlags_DontShowHiddenFiles |
						ImGuiFileDialogFlags_ConfirmOverwrite);
			}

			if (!image.isValid()) {
				ImGui::EndDisabled();
			}

			// handle saveas dialog
			ImVec2 maxSize = ImGui::GetIO().DisplaySize;
			ImVec2 minSize = ImVec2(maxSize.x * 0.5, maxSize.y * 0.5);

			if (ImGuiFileDialog::Instance()->Display("image-saveas-dds", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
				// open selected file if required
				if (ImGuiFileDialog::Instance()->IsOk()) {
					auto path = ImGuiFileDialog::Instance()->GetFilePathName();
					path = OtPathReplaceExtension(path, ".dds");
					OtPathChangeDirectory(OtPathGetParent(path));
					image.saveToDDS(path);
				}

				// close dialog
				ImGuiFileDialog::Instance()->Close();
			}
		});
	}

	static constexpr const char* name = "Save Image To DDS";

protected:
	OtImage image;
};


//
//	OtGraphNodeVirtualImageOutput
//

class OtGraphNodeVirtualImageOutput : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNodeVirtualImageOutput() : OtGraphNodeClass(name, OtGraphNodeClass::output) {}

	// configure node
	inline void configure() override {
		addInputPin("Input", image)->addRenderer([&](float width) {
			auto old = serialize().dump();

			OtImageAsset* oldAsset = nullptr;

			if (!asset.isNull()) {
				oldAsset = &(*asset);
			}

			ImGui::SetNextItemWidth(width);

			if (asset.renderVirtualUI("##name")) {
				if (oldAsset && asset.isNull()) {
					oldAsset->clearImage();
				}

				oldState = old;
				newState = serialize().dump();
				needsEvaluating = true;
				needsSaving = true;
			}
		}, fieldWidth);
	}

	// synchronize the asset with the incoming image
	void onExecute() override {
		if (asset.isReady()) {
			asset->setImage(image);

		} else {
			asset.clear();
		}
	}

	// (de)serialize input
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		auto t = asset.getPath();
		(*data)["path"] = asset.getPath();
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		asset = data->value("path", "");
	}

	static constexpr const char* name = "Save Image To Virtual";
	static constexpr float fieldWidth = 170.0f;

protected:
	OtImage image;
	OtAsset<OtImageAsset> asset;
};


//
//	OtGraphNodeVirtualTextureOutput
//

class OtGraphNodeVirtualTextureOutput : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNodeVirtualTextureOutput() : OtGraphNodeClass(name, OtGraphNodeClass::output) {}

	// configure node
	inline void configure() override {
		addInputPin("Input", texture)->addRenderer([&](float width) {
			auto old = serialize().dump();

			OtTextureAsset* oldAsset = nullptr;

			if (!asset.isNull()) {
				oldAsset = &(*asset);
			}

			ImGui::SetNextItemWidth(width);

			if (asset.renderVirtualUI("##name")) {
				if (oldAsset && asset.isNull()) {
					oldAsset->clearTexture();
				}

				oldState = old;
				newState = serialize().dump();
				needsEvaluating = true;
				needsSaving = true;
			}
		}, fieldWidth);
	}

	// synchronize the asset with the incoming texture
	void onExecute() override {
		if (asset.isReady()) {
			asset->setTexture(texture);

		} else {
			asset.clear();
		}
	}

	// (de)serialize input
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		auto t = asset.getPath();
		(*data)["path"] = asset.getPath();
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		asset = data->value("path", "");
	}

	static constexpr const char* name = "Save Texture To Virtual";
	static constexpr float fieldWidth = 170.0f;

protected:
	OtTexture texture;
	OtAsset<OtTextureAsset> asset;
};


//
//	OtOutputNodesRegister
//

#define REGISTER(CLASS) \
	graph.registerNodeType<CLASS>("Output", CLASS::name)

void OtOutputNodesRegister(OtGraph& graph) {
	REGISTER(OtGraphNodeImageToPngOutput);
	REGISTER(OtGraphNodeImageToDdsOutput);
	REGISTER(OtGraphNodeVirtualImageOutput);
	REGISTER(OtGraphNodeVirtualTextureOutput);
}
