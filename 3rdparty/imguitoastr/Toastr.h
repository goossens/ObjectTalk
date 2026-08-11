//	TextEditor - A syntax highlighting text editor for ImGui
//	Copyright (c) 2024-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "imgui.h"
#include "imgui_internal.h"


//
//	Toastr
//
//	This class provides a "toast-like" notification system. Users only have to
//	call Success, Warning, Error or Info to add a notification to the stack.
//
//	The user also must call the Render method every frame with the anchor point
//	in viewport space and the desired direction of the stack. Render will update
//	the lifecycle of each notification and will render the stack accordingly.
//

class Toastr {
public:
	// constructor
	Toastr() {
		SetPalette(GetDefaultPalette());
	}

	// add a notifications (by type)
	inline void Success(const std::string_view& message, float dismissTime=4.0f) {
		notifications.emplace_back(ctx, NotificationType::success, message, dismissTime);
	}

	inline void Warning(const std::string_view& message, float dismissTime=4.0f) {
		notifications.emplace_back(ctx, NotificationType::warning, message, dismissTime);
	}

	inline void Error(const std::string_view& message, float dismissTime=4.0f) {
		notifications.emplace_back(ctx, NotificationType::error, message, dismissTime);
	}

	inline void Info(const std::string_view& message, float dismissTime=4.0f) {
		notifications.emplace_back(ctx, NotificationType::info, message, dismissTime);
	}

	// anchor types
	enum class AnchorType : char {
		topLeft,
		topRight,
		bottomLeft,
		bottomRight
	};

	// render notifications as a stack
	inline void Render(ImVec2 anchorPos, AnchorType anchorType) {
		// save anchor information
		ctx.anchorPos = anchorPos;
		ctx.anchorType = anchorType;
		ctx.stackDirection = (anchorType == AnchorType::topLeft || anchorType == AnchorType::topRight) ? 1.0f : -1.0f;

		// update current time
		ctx.currentTime += ImGui::GetIO().DeltaTime;

		// get rendering parameters
		auto& style = ImGui::GetStyle();
		ctx.glyphSize = ImGui::CalcTextSize("#");
		ctx.itemSpacing = style.ItemSpacing;
		ctx.framePadding = style.FramePadding;
		ctx.buttonSize = ctx.glyphSize.y * 2.0f;

		// update state of all notifications
		for (auto& notification : notifications) {
			notification.update(ctx);
		}

		// remove expired notifications
		notifications.erase(std::remove_if(notifications.begin(), notifications.end(), [](const Notification& candidate) {
			return candidate.phase == Notification::Phase::expired;
		}), notifications.end());

		// render remaining notifications
		float offset = 0.0f;

		for (auto& notification : notifications) {
			offset = notification.render(ctx, offset);
		}
	}

	// palette support
	enum class Color : char {
		text,
		successBackground,
		warningBackground,
		errorBackground,
		infoBackground,
		successIcon,
		warningIcon,
		errorIcon,
		infoIcon,
		count
	};

	struct Palette : public std::array<ImU32, static_cast<size_t>(Color::count)> {
		inline ImU32 get(Color color) const { return at(static_cast<size_t>(color)); }
	};

	inline void SetPalette(const Palette& newPalette) { ctx.palette = newPalette; }
	inline const Palette& GetPalette() const { return ctx.palette; }

	static inline const Palette& GetDefaultPalette() {
		const static Palette palette = {{
			IM_COL32(255, 255, 255, 255),	// text
			IM_COL32(  0,  90,  60, 255),	// successBackground
			IM_COL32( 90,  50,   0, 255),	// warningBackground
			IM_COL32( 90,  20,   0, 255),	// errorBackground
			IM_COL32(  0,  40,  90, 255),	// infoBackground
			IM_COL32( 50, 200,  90, 255),	// successIcon
			IM_COL32(255, 160,  10, 255),	// warningIcon
			IM_COL32(210,  50,  50, 255),	// errorIcon
			IM_COL32( 10, 140, 250, 255)	// infoIcon
		}};

		return palette;
	}

private:
	// context = configuration + per frame settings
	struct Context {
		// configuration
		size_t textWidth = 20;
		Palette palette;

		ImU32 successIcon = IM_COL32(0, 150, 100, 255);
		ImU32 warningIcon = IM_COL32(90, 50, 0, 255);
		ImU32 errorIcon = IM_COL32(255, 0, 0, 255);
		ImU32 infoIcon = IM_COL32(0, 40, 90, 255);

		float fadeInDuration = 0.4f;
		float fadeOutDuration = 0.4f;
		float ghostDuration = 0.3f;

		// per frame settings
		float currentTime = 0.0f;
		ImVec2 anchorPos;
		AnchorType anchorType;
		float stackDirection;

		ImVec2 glyphSize;
		ImVec2 itemSpacing;
		ImVec2 framePadding;
		float buttonSize;
	} ctx;

	// notification types
	enum class NotificationType : char {
		success,
		warning,
		error,
		info
	};

	// a single notification
	struct Notification {
		// constructor
		Notification(Context& ctx, NotificationType type, const std::string_view& message, float dismissTime) :type(type), message(message) {
			std::stringstream ss;
			ss << "Notification" << id++;
			name = ss.str();

			fadeInStart = ctx.currentTime;
			waitStart = fadeInStart + ctx.fadeInDuration;
			fadeOutStart = waitStart + dismissTime;
			ghostStart = fadeOutStart + ctx.fadeOutDuration;
			expiredStart = ghostStart + ctx.ghostDuration;
		}

		// update the notification state
		inline void update(const Context& ctx) {
			// update phase
			if (ctx.currentTime >= expiredStart) {
				phase = Phase::expired;

			} else if (ctx.currentTime >= ghostStart) {
				phase = Phase::ghost;

			} else if (ctx.currentTime >= fadeOutStart) {
				phase = Phase::fadeOut;

			} else if (ctx.currentTime >= waitStart) {
				phase = Phase::wait;

			} else {
				phase = Phase::fadeIn;
			}

			// determine transparency
			if (phase == Phase::fadeIn) {
				alpha = (ctx.currentTime - fadeInStart) / ctx.fadeInDuration;

			} else if (phase == Phase::wait) {
				alpha = 1.0f;

			} else if (phase == Phase::fadeOut) {
				alpha = 1.0f - (ctx.currentTime - fadeOutStart) / ctx.fadeOutDuration;

			} else {
				alpha = 0.0f;
			}

			// update ghost height (if required)
			if (phase == Phase::ghost) {
				ghostHeight = (1.0f - (ctx.currentTime - ghostStart) / ctx.ghostDuration) * height;
			}
		}

		// render a notification
		inline float render(const Context& ctx, float offset) {
			if (phase == Notification::Phase::ghost) {
				// ghost windows don't have visible components; they just shrink
				// we just calculate the stack offset for the next notification
				offset += ctx.stackDirection * (ghostHeight + ImGui::GetStyle().ItemSpacing.y);

			} else {
				// determine text size
				auto textSize = (ctx.textWidth == 0)
					? ImGui::CalcTextSize(message.c_str())
					: ImGui::CalcTextSize(message.c_str(), nullptr, false, ctx.textWidth * ctx.glyphSize.x);

				auto textWidth = (ctx.textWidth == 0)
					? textSize.x
					: ctx.textWidth * ctx.glyphSize.x;

				// determine notification window position and pivot point
				auto windowPos = ImVec2(ctx.anchorPos.x, ctx.anchorPos.y + offset);
				ImVec2 windowPivot = getWindowPivot(ctx);

				// determine notification window flags and size
				ImGuiWindowFlags windowFlags =
					ImGuiWindowFlags_NoSavedSettings |
					ImGuiWindowFlags_NoDecoration |
					ImGuiWindowFlags_NoNav |
					ImGuiWindowFlags_NoBringToFrontOnFocus |
					ImGuiWindowFlags_NoFocusOnAppearing;

				if (ctx.textWidth == 0) {
					windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;

				} else {
					ImGui::SetNextWindowSize(ImVec2(textWidth + (ctx.buttonSize + ctx.itemSpacing.x) * 2.0f, 0.0f));
				}

				// start notification window
				ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
 				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
				ImGui::PushStyleColor(ImGuiCol_WindowBg, getBackgroundColor(ctx));
				ImGui::SetNextWindowBgAlpha(alpha);

				ImGui::Begin(name.c_str(), nullptr, windowFlags);
				ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());

				// determine top-right location of window to draw possible close button
				auto right = ImGui::GetCursorScreenPos() + ImVec2(ImGui::GetContentRegionAvail().x, 0.0f);

				// determine vertical offsets
				float iconOffset = std::max((textSize.y - ctx.buttonSize) * 0.5f, 0.0f);
				float textOffset = std::max((ctx.buttonSize - textSize.y) * 0.5f, 0.0f);

				// render icon
				verticalOffset(iconOffset);
				renderIcon(ctx);
				verticalOffset(-iconOffset);
				ImGui::SameLine();

				// render notification message
				verticalOffset(textOffset);
				renderMessage(ctx, textWidth);
				verticalOffset(-textOffset);
				ImGui::SameLine();

				// render button
				verticalOffset(iconOffset);
				renderButton(ctx);
				// see https://github.com/ocornut/imgui/issues/5548 on why we don't restore the vertical offset
				// verticalOffset(-iconOffset);

				// determine height of this window and stack offset for next notification
				height = ImGui::GetWindowHeight();
				offset += ctx.stackDirection * (height + ImGui::GetStyle().ItemSpacing.y);

				// render a button to close the notification (if required)
				// (can't use ImGui::IsWindowHovered as it won't let ImGui::Button catch clicks for some reason)
				if (ImGui::GetCurrentContext()->HoveredWindow == ImGui::GetCurrentContext()->CurrentWindow) {
					auto buttonWidth = ctx.glyphSize.x + ctx.framePadding.x * 2.0f;
					ImGui::SetCursorScreenPos(right - ImVec2(buttonWidth, 0.0f));

					if (ImGui::Button("x")) {
						expiredStart = ctx.currentTime;
					}
				}

				// end the window
				ImGui::End();
				ImGui::PopStyleColor();
				ImGui::PopStyleVar(2);
			}

			// return the offset for the next notification window
			return offset;
		}

		// render icon
		inline void renderIcon(Context ctx) {
			auto radius = ctx.buttonSize * 0.5f;
			auto center = ImGui::GetCursorScreenPos() + ImVec2(radius, radius);
			auto drawList = ImGui::GetWindowDrawList();
			ImGui::Dummy(ImVec2(ctx.buttonSize, ctx.buttonSize));

			drawList->AddCircleFilled(center, radius, getIconColor(ctx));
		}

		// render the notification message
		inline void renderMessage(Context ctx, float width) {
			ImGui::PushTextWrapPos(ImGui::GetCursorScreenPos().x + width);
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ctx.palette.get(Color::text), alpha));
			ImGui::TextUnformatted(message.c_str());
			ImGui::PopStyleColor();
			ImGui::PopTextWrapPos();
		}

		// render close button
		inline void renderButton(Context ctx) {
			auto radius = ctx.buttonSize * 0.5f;
			auto center = ImGui::GetCursorScreenPos() + ImVec2(radius, radius);
			auto drawList = ImGui::GetWindowDrawList();
			ImGui::Dummy(ImVec2(ctx.buttonSize, ctx.buttonSize));

			drawList->AddCircle(center, radius, getIconColor(ctx));
		}

		//	support functions
		inline ImVec2 getWindowPivot(Context ctx) {
			switch (ctx.anchorType) {
				case AnchorType::topLeft: return ImVec2(0.0f, 0.0f); break;
				case AnchorType::topRight: return ImVec2(1.0f, 0.0f); break;
				case AnchorType::bottomLeft: return ImVec2(0.0f, 1.0f); break;
				case AnchorType::bottomRight: return ImVec2(1.0f, 1.0f); break;
			}
		}

		inline ImU32 getBackgroundColor(Context ctx) {
			switch (type) {
				case NotificationType::success: return ctx.palette.get(Color::successBackground); break;
				case NotificationType::warning: return ctx.palette.get(Color::warningBackground); break;
				case NotificationType::error: return ctx.palette.get(Color::errorBackground); break;
				case NotificationType::info: return ctx.palette.get(Color::infoBackground); break;
			}
		}

		inline ImU32 getIconColor(Context ctx) {
			switch (type) {
				case NotificationType::success: return ctx.palette.get(Color::successIcon); break;
				case NotificationType::warning: return ctx.palette.get(Color::warningIcon); break;
				case NotificationType::error: return ctx.palette.get(Color::errorIcon); break;
				case NotificationType::info: return ctx.palette.get(Color::infoIcon); break;
			}
		}

		inline void verticalOffset(float offset) {
			auto pos = ImGui::GetCursorScreenPos();
			pos.y += offset;
			ImGui::SetCursorScreenPos(pos);
		}

		// properties
		enum class Phase : char {
			fadeIn,
			wait,
			fadeOut,
			ghost,
			expired
		};

		static inline size_t id = 1;
		NotificationType type;
		std::string name;
		Phase phase;
		std::string message;
		float alpha;
		float height;
		float ghostHeight;

		float fadeInStart;
		float waitStart;
		float fadeOutStart;
		float ghostStart;
		float expiredStart;
	};

	// list of active notifications
	std::vector<Notification> notifications;
};
