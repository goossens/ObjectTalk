//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <format>
#include <vector>

#include "imgui.h"
#include "FileSelector.h"

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
		message += std::format("Asset [{}] is missing", info.path);

	} else if (info.isInvalid) {
		message += std::format("Asset [{}] is invalid:\n{}", info.path, info.errorMessage);
	}

	ImGui::PushStyleColor(ImGuiCol_Border, errorColor);
	ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 3.0f);
	ImGui::BeginTooltip();
	OtUi::text(message);
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

	// get file selector information
	auto& dialog = FileSelector::Instance();

	// render widgets
	ImGui::PushID(id.c_str());
	auto changed = false;
	static bool creating = false;
	static bool selectorOpen = false;

	if (info.virtualMode && *info.virtualMode) {
		// render path as a textfield
		auto filename = OtText::from(info.path, 8);
		ImGui::SetNextItemWidth(pathWidth);
		auto backgroundColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
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
		auto buttonColor = ImGui::GetColorU32(ImGuiCol_Button);
		ImGui::PushStyleColor(ImGuiCol_Button, info.isMissing || info.isInvalid ? errorColor : buttonColor);

		if (ImGui::Button((filename + "##path").c_str(), ImVec2(pathWidth, itemHeight))) {
			ImGui::PopStyleColor();

			dialog.SetCurrentPath(OtPath::getParent(info.path));
			dialog.OpenFile(info.supportedFileTypes);
			selectorOpen = true;

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
			dialog.SaveAs();
			selectorOpen = true;
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
		OtUi::text(realLabel);
	}

	// show file selector (if required)
	if (selectorOpen && dialog.Render()) {
		if (dialog.SelectedOpenFile() || dialog.SelectedSaveAs()) {
			info.path = OtPath::toString(dialog.GetSelectedPath());
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

			selectorOpen = false;
		}

		creating = false;
	}

	ImGui::PopID();
	return changed;
}
