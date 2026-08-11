//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "Toastr.h"


//
//	OtNotification
//

class OtNotification : OtSingleton<OtNotification> {
public:
	// add a new notification
	static inline void success(const std::string& message, float dismissTime=3.0f) {
		instance().toastr.Success(message, dismissTime);
	}

	static inline void warning(const std::string& message, float dismissTime=3.0f) {
		instance().toastr.Warning(message, dismissTime);
	}

	static inline void error(const std::string& message, float dismissTime=3.0f) {
		instance().toastr.Error(message, dismissTime);
	}

	static inline void info(const std::string& message, float dismissTime=3.0f) {
		instance().toastr.Info(message, dismissTime);
	}

	// render all active notifications
	static inline void render() {
		auto mainWindowSize = ImGui::GetMainViewport()->Size;
		auto mainWindowPos = ImGui::GetMainViewport()->Pos;

		// auto anchor = ImVec2( // topLeft
		// 	mainWindowPos.x + ImGui::GetStyle().ItemSpacing.x,
		// 	mainWindowPos.y + ImGui::GetStyle().ItemSpacing.y);

		// auto anchor = ImVec2( // topRight
		// 	mainWindowPos.x + mainWindowSize.x - ImGui::GetStyle().ItemSpacing.x,
		// 	mainWindowPos.y + ImGui::GetStyle().ItemSpacing.y);

		// auto anchor = ImVec2( // bottomLeft
		// 	mainWindowPos.x + ImGui::GetStyle().ItemSpacing.x,
		// 	mainWindowPos.y + mainWindowSize.y - ImGui::GetStyle().ItemSpacing.y);

		auto anchor = ImVec2( // bottomRight
			mainWindowPos.x + mainWindowSize.x - ImGui::GetStyle().ItemSpacing.x,
			mainWindowPos.y + mainWindowSize.y - ImGui::GetStyle().ItemSpacing.y);

		instance().toastr.Render(anchor, Toastr::AnchorType::bottomRight);
	}

private:
	// actual toastr notification system
	Toastr toastr;
};
