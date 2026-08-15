//	Toastr - A notification system for Dear ImGui.
//	Copyright (c) 2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>
#include <string>
#include <string_view>
#include <vector>

#include "imgui.h"


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
	Toastr();

	// add a notifications (by type)
	inline void Success(const std::string_view& message, float displayTime=4.0f) {
		notifications.emplace_back(ctx, NotificationType::success, message, displayTime);
	}

	inline void Warning(const std::string_view& message, float displayTime=4.0f) {
		notifications.emplace_back(ctx, NotificationType::warning, message, displayTime);
	}

	inline void Error(const std::string_view& message, float displayTime=4.0f) {
		notifications.emplace_back(ctx, NotificationType::error, message, displayTime);
	}

	inline void Info(const std::string_view& message, float displayTime=4.0f) {
		notifications.emplace_back(ctx, NotificationType::info, message, displayTime);
	}

	// anchor types
	enum class AnchorType : char {
		topLeft,
		topRight,
		bottomLeft,
		bottomRight
	};

	// render notifications as a stack
	void Render(ImVec2 anchorPos, AnchorType anchorType);

	// access options
	inline void SetWindowRounding(float rounding) { ctx.windowRounding = rounding; }
	inline float GetWindowRounding() const { return ctx.windowRounding; }
	inline void SetWindowBorder(bool border) { ctx.windowBorder = border; }
	inline bool HasWindowBorder() const { return ctx.windowBorder; }
	inline void SetTextWidth(size_t glyphs) { ctx.textWidth = glyphs; }
	inline size_t GetTextWidth() const { return ctx.textWidth; }
	inline void SetFadeInDuration(float duration) { ctx.fadeInDuration = duration; }
	inline float GetFadeInDuration() const { return ctx.fadeInDuration; }
	inline void SetFadeOuDuration(float duration) { ctx.fadeOutDuration = duration; }
	inline float GetFadeOutDuration() const { return ctx.fadeOutDuration; }
	inline void SetGhostDuration(float duration) { ctx.ghostDuration = duration; }
	inline float GetGhostDuration() const { return ctx.ghostDuration; }

	// palette support
	enum class Color : char {
		successBackground,
		warningBackground,
		errorBackground,
		infoBackground,
		messageColor,
		successIcon,
		warningIcon,
		errorIcon,
		infoIcon,
		iconColor,
		count
	};

	struct Palette : public std::array<ImU32, static_cast<size_t>(Color::count)> {
		inline ImU32 get(Color color) const { return at(static_cast<size_t>(color)); }
		inline void set(Color color, ImU32 code) { at(static_cast<size_t>(color)) = code; }
	};

	inline void SetPalette(const Palette& newPalette) { ctx.palette = newPalette; }
	inline const Palette& GetPalette() const { return ctx.palette; }

	static const Palette& GetDarkPalette();
	static const Palette& GetSaturatedPalette();
	static const Palette& GetPastelPalette();

	// render a sample at the current position (only used in example application)
	inline void RenderSuccessSample() { renderSample(NotificationType::success); }
	inline void RenderWarningSample() { renderSample(NotificationType::warning); }
	inline void RenderErrorSample() { renderSample(NotificationType::error); }
	inline void RenderInfoSample() { renderSample(NotificationType::info); }

private:
	// context = configuration + per frame settings
	struct Context {
		// configuration
		float windowRounding = 6.0f;
		bool windowBorder = false;
		size_t textWidth = 0;
		Palette palette;

		float fadeInDuration = 0.4f;
		float fadeOutDuration = 0.4f;
		float ghostDuration = 0.3f;

		// per frame settings
		float currentTime = 0.0f;
		ImVec2 anchorPos;
		AnchorType anchorType;
		float stackDirection;

		ImFont* font;
		ImVec2 glyphSize;
		float highDpiScale;
		ImVec2 itemSpacing;
		ImVec2 framePadding;
		ImVec2 windowPadding;
		float buttonSize;
	} ctx;

	// notification types
	enum class NotificationType : char {
		success,
		warning,
		error,
		info
	};

	// render a sample notification
	void renderSample(NotificationType type);

	// a single notification
	class Notification {
	public:
		// constructor
		Notification(Context& ctx, NotificationType type, const std::string_view& message, float displayTime);

		// update the notification state
		void update(const Context& ctx);

		// render a notification
		float render(const Context& ctx, float offset);

	private:
		friend class Toastr;

		// render functions
		void renderIcon(Context ctx);
		void renderMessage(Context ctx, float width);
		void renderButton(Context ctx, ImVec2 right);

		//	support functions
		ImVec2 getTextSize(Context ctx);
		ImVec2 getWindowPivot(Context ctx);
		ImU32 getBackgroundColor(Context ctx);
		ImU32 getIconBackgroundColor(Context ctx);

		// properties
		enum class Phase : char {
			fadeIn,
			display,
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
		float displayStart;
		float fadeOutStart;
		float ghostStart;
		float expiredStart;
	};

	// list of active notifications
	std::vector<Notification> notifications;
};
