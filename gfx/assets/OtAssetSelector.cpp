//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <vector>

#include "fmt/format.h"
#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "OtText.h"

#include "OtAssetSelector.h"
#include "OtMessageBus.h"
#include "OtPath.h"
#include "OtUi.h"


//
//	Constants
//

static constexpr ImU32 errorColor = IM_COL32(255, 32, 32, 128);


//
//	OtAssetSelector::showErrorPopup
//

void OtAssetSelector::showErrorPopup(Info& info) {
	std::string message("Error: ");

	if (info.isMissing) {
		message += fmt::format("Asset [{}] is missing", info.path);

	} else if (info.isInvalid) {
		message += fmt::format("Asset [{}] is invalid", info.path);
	}

	ImGui::PushStyleColor(ImGuiCol_Border, errorColor);
	ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 3.0f);
	ImGui::BeginTooltip();
	ImGui::TextUnformatted(message.c_str());
	ImGui::EndTooltip();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}


//
//	OtAssetSelector::renderUI
//

bool OtAssetSelector::renderUI(Info& info) {
	// determine button status
	bool showClearButton = !info.path.empty();
	bool showVirtualButton = info.virtualMode;
	bool showCreateButton = info.creator && info.path.empty();
	bool showEditButton = info.hasEditor && !info.path.empty() && OtPath::isRegularFile(info.path);

	// determine number of buttons
	int buttons =
		(showClearButton ? 1 : 0) +
		(showVirtualButton ? 1 : 0) +
		(showCreateButton ? 1 : 0) +
		(showEditButton ? 1 : 0);

	// calculate component sizes
	float itemWidth = ImGui::CalcItemWidth();
	float itemHeight = ImGui::GetFrameHeight();
	float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
	float buttonWidth = itemHeight + spacing;
	float pathWidth = itemWidth - buttonWidth * buttons;

	// determine real label and ID
	std::string realLabel;
	std::string id;
	OtUi::splitLabel(info.label, realLabel, id);

	// get file dialog information
	auto dialog = ImGuiFileDialog::Instance();
	auto dialogID = fmt::format("select-file-{}", info.id);
	std::string filter = info.supportedFileTypes;

	if (filter.find(',') != std::string::npos) {
		filter = "*{" + filter + "}," + filter;
	}

	// render widgets
	ImGui::PushID(id.c_str());
	bool changed = false;
	static bool creating = false;

	if (info.virtualMode && *info.virtualMode) {
		// render path as a textfield
		auto filename = OtText::from(info.path, 8);
		ImGui::SetNextItemWidth(pathWidth);

		auto backgroundColor = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, info.isMissing || info.isInvalid ? errorColor : backgroundColor);

		if (OtUi::inputText("##virtualpath", &filename) && info.path != "virtual:" + filename) {
			info.path = "virtual:" + filename;
			changed = true;
		}

		ImGui::PopStyleColor();

		if (ImGui::IsItemHovered() && (info.isMissing || info.isInvalid)) {
			showErrorPopup(info);
		}

	} else {
		// render path as a button
		auto filename = OtPath::getFilename(info.path);
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
		auto buttonColor = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Button]);
		ImGui::PushStyleColor(ImGuiCol_Button, info.isMissing || info.isInvalid ? errorColor : buttonColor);

		if (ImGui::Button((filename + "##path").c_str(), ImVec2(pathWidth, itemHeight))) {
			ImGui::PopStyleColor();

			IGFD::FileDialogConfig config;
			config.filePathName = info.path;
			config.countSelectionMax = 1;

			config.flags = ImGuiFileDialogFlags_Modal |
					ImGuiFileDialogFlags_DontShowHiddenFiles |
					ImGuiFileDialogFlags_ReadOnlyFileNameField;

			dialog->OpenDialog(dialogID.c_str(), "Select file...", filter.c_str(), config);

		} else {
			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && (info.isMissing || info.isInvalid)) {
				showErrorPopup(info);
			}
		}

		ImGui::PopStyleVar();
	}

	// render clear button (if required)
	if (showClearButton) {
		ImGui::SameLine(0.0f, spacing);

		if (ImGui::Button("x", ImVec2(itemHeight, itemHeight))) {
			info.path.clear();
			changed = true;
		}
	}

	// render virtual button (if required)
	if (showVirtualButton) {
		ImGui::SameLine(0.0f, spacing);
		bool colored = *info.virtualMode;

		if (colored) {
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) ImColor::HSV(0.45f, 0.5f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4) ImColor::HSV(0.45f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4) ImColor::HSV(0.45f, 0.7f, 0.7f));
		}

		if (ImGui::Button("v", ImVec2(itemHeight, itemHeight))) {
			if (!info.virtualOnly) {
				*info.virtualMode = !(*info.virtualMode);
			}
		}

		if (colored) {
			ImGui::PopStyleColor(3);
		}
	}

	// render create button (if required)
	if (showCreateButton) {
		ImGui::SameLine(0.0f, spacing);

		if (ImGui::Button("+", ImVec2(itemHeight, itemHeight))) {
			IGFD::FileDialogConfig config;
			config.path = ".";
			config.countSelectionMax = 1;

			config.flags = ImGuiFileDialogFlags_Modal |
					ImGuiFileDialogFlags_DontShowHiddenFiles |
					ImGuiFileDialogFlags_ConfirmOverwrite;

			dialog->OpenDialog(dialogID.c_str(), "Create file...", filter.c_str(), config);
			creating = true;
		}
	}

	// render edit button (if required)
	if (showEditButton) {
		ImGui::SameLine(0.0f, spacing);

		if (ImGui::Button("e", ImVec2(itemHeight, itemHeight))) {
			OtMessageBus::send("openinwindow " + info.path);
		}
	}

	// render label (if required)
	if (realLabel.size()) {
		ImGui::SameLine(0.0f, spacing);
		ImGui::TextUnformatted(realLabel.c_str());
	}

	// show file selector (if required)
	ImVec2 maxSize = ImGui::GetMainViewport()->Size;
	ImVec2 minSize = ImVec2(maxSize.x * 0.5f, maxSize.y * 0.5f);

	if (dialog->Display(dialogID.c_str(), ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
		if (dialog->IsOk()) {
			info.path = OtPath::join(dialog->GetCurrentPath(), dialog->GetCurrentFileName());
			OtPath::changeDirectory(OtPath::getParent(info.path));
			changed = true;

			if (creating) {
				// ensure correct file extension
				std::vector<std::string> extensions;
				OtText::split(info.supportedFileTypes, extensions, ',');
				info.path = OtPath::replaceExtension(info.path, extensions[0]);

				// create the file and open the editor
				info.creator(info.path);
				OtMessageBus::send("openinwindow " + info.path);
			}
		}

		creating = false;
		dialog->Close();
	}

	ImGui::PopID();
	return changed;
}
