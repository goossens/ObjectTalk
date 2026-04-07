//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <chrono>
#include <string>
#include <vector>

#include "fmt/format.h"
#include "imgui.h"
#include "imgui_internal.h"

#include "OtFontAwesome.h"
#include "OtSingleton.h"
#include "OtUi.h"


//
//	OtNotification
//

class OtNotification : OtSingleton<OtNotification> {
public:
	// notifications types
	enum class Type {
		success,
		warning,
		error,
		info
	};

	// add a new notification
	static inline void add(Type type, const std::string& message, int dismissTime=3000) {
		instance().notifications.push_back({type, message, dismissTime});
	}

	// render all active notifications
	static inline void render() {
		// update state of all notifications
		auto& notificationList = instance().notifications;

		for (auto& notification : notificationList) {
			notification.update();
		}

		// remove expired notifications
		notificationList.erase(std::remove_if(notificationList.begin(), notificationList.end(), [](Notification& candidate) {
			return candidate.phase == Notification::Phase::expired;
		}), notificationList.end());

		// render remaining notifications
		float offset = 0.0f;

		for (auto& notification : notificationList) {
			offset = notification.render(offset);
		}
	}

private:
	// a single notification
	struct Notification {
		// constructor
		Notification(Type type, const std::string& message, int dismissTime) : type(type), message(message) {
			name = fmt::format("Notification{}", id++);

			switch (type) {
				case Type::success:
					icon = OtFontAwesome::circleCheck;
					title = "Success";
					titleColor = {0.0f, 255.0f, 0.0f, 255.0f};
					break;

				case Type::warning:
					icon = OtFontAwesome::triangleExclamation;
					title = "Warning";
					titleColor = {255.0f, 255.0f, 0.0f, 255.0f};
					break;

				case Type::error:
					icon = OtFontAwesome::circleExclamation;
					title = "Error";
					titleColor = {255.0f, 0.0f, 0.0f, 255.0f};
					break;

				case Type::info:
					icon = OtFontAwesome::circleInfo;
					title = "Info";
					titleColor = {0.0f, 157.0f, 255.0f, 255.0f};
					break;
			}

			fadeInStart = std::chrono::system_clock::now();
			waitStart = fadeInStart + fadeInDuration;
			fadeOutStart = waitStart + std::chrono::milliseconds(dismissTime);
			ghostStart = fadeOutStart + fadeOutDuration;
			expiredStart = ghostStart + ghostDuration;
		}

		// update the notification state
		inline void update() {
			// get current time
			auto now = std::chrono::system_clock::now();

			// update phase
			if (now >= expiredStart) {
				phase = Phase::expired;

			} else if (now >= ghostStart) {
				phase = Phase::ghost;

			} else if (now >= fadeOutStart) {
				phase = Phase::fadeOut;

			} else if (now >= waitStart) {
				phase = Phase::wait;

			} else {
				phase = Phase::fadeIn;
			}

			// determine transparency
			using dmilliseconds = std::chrono::duration<float, std::milli>;

			if (phase == Phase::fadeIn) {
				alpha = std::chrono::duration_cast<dmilliseconds>(now - fadeInStart) / fadeInDuration;

			} else if (phase == Phase::wait) {
				alpha = 1.0f;

			} else if (phase == Phase::fadeOut) {
				alpha = 1.0f - (std::chrono::duration_cast<dmilliseconds>(now - fadeOutStart) / fadeOutDuration);

			} else {
				alpha = 0.0f;
			}

			titleColor.w = alpha;

			// update ghost height (if required)
			if (phase == Phase::ghost) {
				ghostHeight = (1.0f - (std::chrono::duration_cast<dmilliseconds>(now - ghostStart) / ghostDuration)) * height;
			}
		}

		// render a notification
		inline float render(float offset) {
			if (phase == Notification::Phase::ghost) {
				offset += ghostHeight + ImGui::GetStyle().ItemSpacing.y;

			} else {
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
				ImGui::SetNextWindowBgAlpha(alpha);

				auto mainWindowSize = ImGui::GetMainViewport()->Size;
				auto mainWindowPos = ImGui::GetMainViewport()->Pos;

				ImGui::SetNextWindowPos(
					ImVec2(
						mainWindowPos.x + mainWindowSize.x - ImGui::GetStyle().ItemSpacing.x,
						mainWindowPos.y + mainWindowSize.y - ImGui::GetStyle().ItemSpacing.y - offset),
					ImGuiCond_Always,
					ImVec2(1.0f, 1.0f));

				ImGuiWindowFlags flags =
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoDecoration |
					ImGuiWindowFlags_NoNav |
					ImGuiWindowFlags_NoBringToFrontOnFocus |
					ImGuiWindowFlags_NoFocusOnAppearing;

				ImGui::Begin(name.c_str(), nullptr, flags);
				ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());

				ImGui::PushStyleColor(ImGuiCol_Text, titleColor);
				ImGui::AlignTextToFramePadding();
				ImGui::PushFont(OtUi::getAwesomeFont(), 0.0f);
				ImGui::TextUnformatted(reinterpret_cast<const char*>(icon));
				ImGui::PopFont();

				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("%s", title.c_str());
				ImGui::PopStyleColor();

				auto buttonWidth = ImGui::CalcTextSize("x").x + ImGui::GetStyle().FramePadding.x * 2.0f;
				ImGui::SameLine(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - buttonWidth);

				if (ImGui::Button("x")) {
					expiredStart = std::chrono::system_clock::now();
				}

				ImGui::PushTextWrapPos(mainWindowSize.x / 4.0f);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("%s", message.c_str());
				ImGui::PopTextWrapPos();

				height = ImGui::GetWindowHeight();
				offset += height + ImGui::GetStyle().ItemSpacing.y;

				ImGui::End();
				ImGui::PopStyleVar();
			}

			return offset;
		}

		// properties
		static constexpr std::chrono::milliseconds fadeInDuration{400};
		static constexpr std::chrono::milliseconds fadeOutDuration{400};
		static constexpr std::chrono::milliseconds ghostDuration{300};

		enum class Phase {
			fadeIn,
			wait,
			fadeOut,
			ghost,
			expired
		};

		inline static size_t id = 1;
		OtFontAwesome::Type icon;
		std::string name;
		Type type;
		Phase phase;
		std::string title;
		std::string message;
		ImVec4 titleColor;
		float alpha;
		float height;
		float ghostHeight;

		std::chrono::system_clock::time_point fadeInStart;
		std::chrono::system_clock::time_point waitStart;
		std::chrono::system_clock::time_point fadeOutStart;
		std::chrono::system_clock::time_point ghostStart;
		std::chrono::system_clock::time_point expiredStart;
	};

	// all active notifications
	std::vector<Notification> notifications;
};
