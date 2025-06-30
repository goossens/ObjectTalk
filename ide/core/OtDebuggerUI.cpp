//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtText.h"

#include "OtUi.h"

#include "OtObjectTalkLanguage.h"
#include "OtDebuggerUI.h"


//
//	OtDebuggerUI::update
//

void OtDebuggerUI::update(const std::string& message) {
	debugState.deserialize(message);
	stackFrame = &debugState.frames.back();
}


//
//	OtDebuggerUI::render
//

void OtDebuggerUI::render(OtSubProcess& subprocess, OtConsole& console) {
	// create debugger window
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin(
		"Workspace",
		nullptr,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus);

	// split the screen between the debugger and the console
	determinePanelHeights();
	auto spacing = ImGui::GetStyle().ItemSpacing;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	ImGui::BeginChild("Debugger", ImVec2(0.0f, contentsHeight));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);

	// render the debugger elements
	renderControlBar(subprocess);
	determinePanelWidths();
	renderSourceCode();
	OtUi::splitterHorizontal(&sourceCodeWidth, sourceCodeMinWidth, sourceCodeMaxWidth);
	renderVariables();

	// split the screen between the editors and the console
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::PopStyleVar();
	OtUi::splitterVertical(&contentsHeight, contentsMinHeight, contentsMaxHeight);

	// render the console
	console.render();

	ImGui::End();
	ImGui::PopStyleVar();
}


//
//	button
//

static bool button(const char* label, const char* tooltip) {
	auto result = ImGui::Button(label, ImVec2(ImGui::GetFrameHeight(), 0.0f));
	ImGui::SetItemTooltip("%s", tooltip);
	return result;
}


//
//	OtDebuggerUI::renderControlBar
//

void OtDebuggerUI::renderControlBar(OtSubProcess& subprocess) {
	static constexpr float margin = 4.0f;

	if (button(u8"\u25a0", "Stop")) {
		subprocess.kill(SIGINT);
	}

	ImGui::SameLine(0.0f, margin);

	if (button(u8"\u25b6", "Continue")) {
		subprocess.send("continue\n");
	}

	ImGui::SameLine(0.0f, margin);

	if (button(u8"\u271a", "Step Over")) {
		subprocess.send("step\n");
	}

	ImGui::SameLine(0.0f, margin);

	if (button(u8"\u25bc", "Step Into")) {
		subprocess.send("in\n");
	}

	ImGui::SameLine(0.0f, margin);

	if (button(u8"\u25b2", "Step Out")) {
		subprocess.send("out\n");
	}

	ImGui::SameLine();
	std::string title = fmt::format("Debugging [{}]...", currentDebuggable);
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", title.c_str());
	ImGui::Spacing();
}


//
//	OtDebuggerUI::renderSourceCode
//

void OtDebuggerUI::renderSourceCode() {
	// reload source (if required)
	if (currentDebuggable != stackFrame->module) {
		currentDebuggable = stackFrame->module;
		sourceCode.SetReadOnlyEnabled(true);
		sourceCode.SetLanguage(OtObjectTalkLanguage::getDefinition());
		sourceCode.SetText(OtText::load(currentDebuggable));

		float width = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, "#").x;

		sourceCode.SetLineDecorator(width, [this](TextEditor::Decorator& decorator) {
			renderDecorations(decorator);
		});

		currentLine = stackFrame->line;
		sourceCode.ScrollToLine(static_cast<int>(currentLine - 1), TextEditor::Scroll::alignMiddle);

	} else if ( currentLine != stackFrame->line) {
		currentLine = stackFrame->line;
		sourceCode.ScrollToLine(static_cast<int>(currentLine - 1), TextEditor::Scroll::alignMiddle);
	}

	// render source code
	ImGui::PushFont(nullptr, 17.0f);
	sourceCode.Render("SourceCode", ImVec2(sourceCodeWidth, 0.0f), true);
	ImGui::PopFont();
}


//
//	OtDebuggerUI::renderDecorations
//

void OtDebuggerUI::renderDecorations(TextEditor::Decorator& decorator) {
	std::string decoration = (static_cast<size_t>(decorator.line) == stackFrame->line - 1) ? u8" \u25b7" : "  ";
	ImGui::TextUnformatted(decoration.c_str());
}


//
//	OtDebuggerUI::renderVariables
//

void OtDebuggerUI::renderVariables() {
	static constexpr ImGuiTableFlags tableFlags =
		ImGuiTableFlags_BordersV |
		ImGuiTableFlags_BordersOuterH |
		ImGuiTableFlags_Resizable |
		ImGuiTableFlags_RowBg |
		ImGuiTableFlags_NoBordersInBody;

	static constexpr ImGuiTableColumnFlags columnFlags =
		ImGuiTableColumnFlags_NoHide |
		ImGuiTableColumnFlags_WidthStretch;

	if (ImGui::BeginTable("Variables", 3, tableFlags)) {
		ImGui::TableSetupColumn("Name", columnFlags, 3.0f);
		ImGui::TableSetupColumn("Class", columnFlags, 2.0f);
		ImGui::TableSetupColumn("Value", columnFlags, 4.0f);
		ImGui::TableHeadersRow();

		size_t currentFrame = debugState.frames.size() - 1;
		size_t i = 0;

		for (auto& frame : debugState.frames) {
			std::string label = fmt::format("{}: {}", frame.module, frame.line);

			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_LabelSpanAllColumns;

			if (i++ == currentFrame) {
				flags |= ImGuiTreeNodeFlags_DefaultOpen;
			}

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
			auto open = ImGui::TreeNodeEx(label.c_str(), flags);
			ImGui::PopStyleColor();

			if (open) {
				for (auto& variable : frame.variables) {
					renderVariable(variable);
				}

				ImGui::TreePop();
			}
		}

		ImGui::EndTable();
	}
}


//
//	OtDebuggerUI::renderVariable
//

void OtDebuggerUI::renderVariable(OtDebugState::Variable& variable) {
	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	if (variable.members.size()) {
		bool open = ImGui::TreeNodeEx(variable.name.c_str(), ImGuiTreeNodeFlags_SpanAllColumns);
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(variable.type.c_str());
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(variable.value.c_str());

		if (open) {
			for (auto& memberVariable : variable.members) {
				renderVariable(memberVariable);
			}

			ImGui::TreePop();
		}

	} else {
		ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_Leaf |
			ImGuiTreeNodeFlags_Bullet |
			ImGuiTreeNodeFlags_SpanAllColumns |
			ImGuiTreeNodeFlags_NoTreePushOnOpen;

		ImGui::TreeNodeEx(variable.name.c_str(), flags);
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(variable.type.c_str());
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(variable.value.c_str());
	}
}


//
//	OtDebuggerUI::determinePanelHeights
//

void OtDebuggerUI::determinePanelHeights() {
	// get available space in window
	auto available = ImGui::GetContentRegionAvail();

	// determine contents height
	contentsMinHeight = available.y * 0.05f;
	contentsMaxHeight = available.y * 0.9f;

	if (contentsHeight < 0.0f) {
		contentsHeight =  available.y * 0.75f;

	} else {
		contentsHeight = std::clamp(contentsHeight, contentsMinHeight, contentsMaxHeight);
	}
}


//
//	OtDebuggerUI::determinePanelWidths
//

void OtDebuggerUI::determinePanelWidths() {
	// get available space in window
	auto available = ImGui::GetContentRegionAvail();

	// determine source code width
	sourceCodeMinWidth = available.x * 0.05f;
	sourceCodeMaxWidth = available.x * 0.9f;

	if (sourceCodeWidth < 0.0f) {
		sourceCodeWidth =  available.x * 0.5f;

	} else {
		sourceCodeWidth = std::clamp(sourceCodeWidth, sourceCodeMinWidth, sourceCodeMaxWidth);
	}
}
