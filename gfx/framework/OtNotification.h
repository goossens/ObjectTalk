//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string_view>

#include "Toastr.h"
#include "imgui.h"


//
//	OtNotification
//

class OtNotification : OtSingleton<OtNotification> {
public:
	// add a new notification
	static inline void success(const std::string_view& message, float dismissTime=4.0f) {
		instance().toastr.Success(message, dismissTime);
	}

	static inline void warning(const std::string_view& message, float dismissTime=4.0f) {
		instance().toastr.Warning(message, dismissTime);
	}

	static inline void error(const std::string_view& message, float dismissTime=4.0f) {
		instance().toastr.Error(message, dismissTime);
	}

	static inline void info(const std::string_view& message, float dismissTime=4.0f) {
		instance().toastr.Info(message, dismissTime);
	}

	// render all active notifications
	static inline void render() {
		auto mainWindowSize = ImGui::GetMainViewport()->Size;
		auto mainWindowPos = ImGui::GetMainViewport()->Pos;
		auto anchor = mainWindowPos + mainWindowSize - ImGui::GetStyle().ItemSpacing;

		instance().toastr.SetTextWidth(30);
		instance().toastr.Render(anchor, Toastr::AnchorType::bottomRight);
	}

private:
	// actual toastr notification system
	Toastr toastr;
};
