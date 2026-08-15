//	Toastr - A notification system for Dear ImGui.
//	Copyright (c) 2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <sstream>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui.h"
#include "imgui_internal.h"

#include "Toastr.h"


//
//	Toastr::Toastr
//

Toastr::Toastr() {
	SetPalette(GetDarkPalette());
}


//
//	Toastr::Render
//

void Toastr::Render(ImVec2 anchorPos, AnchorType anchorType) {
	// save anchor information
	ctx.anchorPos = anchorPos;
	ctx.anchorType = anchorType;
	ctx.stackDirection = (anchorType == AnchorType::topLeft || anchorType == AnchorType::topRight) ? 1.0f : -1.0f;

	// update current time
	ctx.currentTime += ImGui::GetIO().DeltaTime;

	// get rendering parameters
	auto& style = ImGui::GetStyle();
	ctx.font = ImGui::GetFont();
	ctx.glyphSize = ImGui::CalcTextSize("#");
	ctx.highDpiScale = style.FontScaleDpi;
	ctx.itemSpacing = style.ItemSpacing;
	ctx.framePadding = style.FramePadding;
	ctx.windowPadding = style.WindowPadding;
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


//
//	Toastr::Notification::Notification
//

Toastr::Notification::Notification(Context& ctx, NotificationType type, const std::string_view& message, float displayTime)
	: type(type), message(message) {

	std::stringstream ss;
	ss << "Notification" << id++;
	name = ss.str();

	fadeInStart = ctx.currentTime;
	displayStart = fadeInStart + ctx.fadeInDuration;
	fadeOutStart = displayStart + displayTime;
	ghostStart = fadeOutStart + ctx.fadeOutDuration;
	expiredStart = ghostStart + ctx.ghostDuration;
}


//
//	Toastr::Notification::update
//

void Toastr::Notification::update(const Context& ctx) {
	// update phase
	if (ctx.currentTime >= expiredStart) {
		phase = Phase::expired;

	} else if (ctx.currentTime >= ghostStart) {
		phase = Phase::ghost;

	} else if (ctx.currentTime >= fadeOutStart) {
		phase = Phase::fadeOut;

	} else if (ctx.currentTime >= displayStart) {
		phase = Phase::display;

	} else {
		phase = Phase::fadeIn;
	}

	// determine transparency
	if (phase == Phase::fadeIn) {
		alpha = (ctx.currentTime - fadeInStart) / ctx.fadeInDuration;

	} else if (phase == Phase::display) {
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


//
//	Toastr::Notification::render
//

float Toastr::Notification::render(const Context& ctx, float offset) {
	if (phase == Notification::Phase::ghost) {
		// ghost windows don't have visible components; they just shrink
		// we just calculate the stack offset for the next notification
		offset += ctx.stackDirection * (ghostHeight + ImGui::GetStyle().ItemSpacing.y);

	} else {
		// determine text size
		auto textSize = getTextSize(ctx);

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

		if (ctx.textWidth) {
			auto contentSize = ctx.buttonSize + ctx.itemSpacing.x + textSize.x;
			ImGui::SetNextWindowSize(ImVec2(contentSize + ctx.windowPadding.x * 2.0f, 0.0f));
		}

		// start notification window
		ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, ctx.windowBorder ? ctx.highDpiScale : 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, ctx.windowRounding);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, getBackgroundColor(ctx));
		ImGui::SetNextWindowBgAlpha(alpha);

		ImGui::Begin(name.c_str(), nullptr, windowFlags);
		ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());

		// determine top-right location of window
		auto topLeft = ImGui::GetCursorScreenPos();
		auto bottomRight = topLeft + ImVec2(ImGui::GetContentRegionAvail().x, 0.0f);

		// determine vertical offsets
		float iconOffset = std::max((textSize.y - ctx.buttonSize) * 0.5f, 0.0f);
		float textOffset = std::max((ctx.buttonSize - textSize.y) * 0.5f, 0.0f);

		// render icon
		ImGui::SetCursorScreenPos(ImVec2(topLeft.x, topLeft.y + iconOffset));
		renderIcon(ctx);
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, topLeft.y));
		ImGui::SameLine();

		// render notification message
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, topLeft.y + textOffset));
		renderMessage(ctx, textSize.x);

		// render button
		renderButton(ctx, bottomRight);

		// determine height of this window and stack offset for next notification
		height = ImGui::GetWindowHeight();
		offset += ctx.stackDirection * (height + ctx.itemSpacing.y);

		// end the window
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(3);
	}

	// return the offset for the next notification window
	return offset;
}


//
//	Toastr::renderSample
//

void Toastr::renderSample(NotificationType type) {
	// update context
	ctx.font = ImGui::GetFont();
	ctx.glyphSize = ImGui::CalcTextSize("#");
	auto& style = ImGui::GetStyle();
	ctx.highDpiScale = style.FontScaleDpi;
	ctx.itemSpacing = style.ItemSpacing;
	ctx.framePadding = style.FramePadding;
	ctx.windowPadding = style.WindowPadding;
	ctx.buttonSize = ctx.glyphSize.y * 2.0f;

	// determine message
	std::string message;

	switch (type) {
		case NotificationType::success: message = "Success"; break;
		case NotificationType::warning: message = "Warning"; break;
		case NotificationType::error: message = "Error"; break;
		case NotificationType::info: message = "Info"; break;
	}

	// create dummy notification
	Notification notification{ctx, type, message, 1.0f};
	notification.phase = Notification::Phase::display;
	notification.alpha = 1.0f;

	// render dummy notification
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ctx.windowRounding);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, notification.getBackgroundColor(ctx));

	ImGuiChildFlags flags =
		ctx.windowBorder ? ImGuiChildFlags_Borders : ImGuiChildFlags_None |
		ImGuiChildFlags_AlwaysUseWindowPadding;

	size_t messageGlyphs = std::clamp(ctx.textWidth, static_cast<size_t>(11), static_cast<size_t>(30));
	auto messageWidth = ctx.glyphSize.x * messageGlyphs;
	auto contentHeight = ctx.buttonSize + ctx.windowPadding.x * 2.0f;
	auto contentWidth = contentHeight + ctx.itemSpacing.x + messageWidth ;
	ImGui::BeginChild(message.c_str(), ImVec2(contentWidth, contentHeight), flags);

	notification.renderIcon(ctx);
	ImGui::SameLine();
	ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0.0f, ctx.glyphSize.y * 0.5f));
	notification.renderMessage(ctx, messageWidth);

	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}


//
//	Toastr::Notification::renderIcon
//

void Toastr::Notification::renderIcon(Context ctx) {
	auto radius = ctx.buttonSize * 0.5f;
	auto center = ImGui::GetCursorScreenPos() + ImVec2(radius, radius);
	auto drawList = ImGui::GetWindowDrawList();
	ImGui::Dummy(ImVec2(ctx.buttonSize, ctx.buttonSize));

	drawList->AddCircleFilled(center, radius, ImGui::GetColorU32(getIconBackgroundColor(ctx), alpha));
	auto color = ImGui::GetColorU32(ctx.palette.get(Color::iconColor), alpha);
	auto width = ctx.highDpiScale * 2.0f;

	switch (type) {
		case NotificationType::success: {
			auto left = center - ImVec2(0.55f * radius, 0.0f);
			auto bottom = center + ImVec2(-0.1f * radius, 0.5f * radius);
			auto right = center + ImVec2(0.45f * radius, -0.4f * radius);

			drawList->AddLine(left, bottom, color, width);
			drawList->AddLine(bottom, right, color, width);
			break;
		}

		case NotificationType::warning: {
			auto left = center.x - 0.25f * width;
			auto triangleTop = ImVec2(center.x, center.y - 0.6f * radius);
			auto triangleLeft = ImVec2(center.x - 0.55f * radius, center.y + 0.5f * radius);
			auto triangleRight = ImVec2(center.x + 0.55f * radius, center.y + 0.5f * radius);
			auto startLine = ImVec2(left, center.y - 0.25f * radius);
			auto endLine = ImVec2(left, center.y + 0.1f * radius);
			auto startDot = ImVec2(left, center.y + 0.3f * radius);
			auto endDot = startDot - ImVec2(0.0f, width);

			drawList->AddTriangle(triangleLeft, triangleTop, triangleRight, color, width);
			drawList->AddLine(startLine, endLine, color, width);
			drawList->AddLine(startDot, endDot, color, width);
			break;
		}

		case NotificationType::error: {
			auto offset = radius * 0.4f;
			auto startLine = center - ImVec2(offset, offset);
			auto endLine = center + ImVec2(offset, offset);

			drawList->AddCircle(center, radius * 0.6f, color, 0, width);
			drawList->AddLine(startLine, endLine, color, width);
			break;
		}

		case NotificationType::info: {
			auto left = center.x - 0.25f * width;
			auto startDot = ImVec2(left, center.y - 0.35f * radius);
			auto endDot = startDot + ImVec2(0.0f, width);
			auto startI = ImVec2(left, center.y - 0.15f * radius);
			auto endI = ImVec2(left, center.y + 0.3f * radius);

			drawList->AddCircle(center, radius * 0.6f, color, 0, width);
			drawList->AddLine(startDot, endDot, color, width);
			drawList->AddLine(startI, endI, color, width);
			break;
		}
	}
}


//
//	Toastr::Notification::renderMessage
//

void Toastr::Notification::renderMessage(Context ctx, float width) {
	ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + width);
	ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ctx.palette.get(Color::messageColor), alpha));
	ImGui::TextUnformatted(message.c_str());
	ImGui::PopStyleColor();
	ImGui::PopTextWrapPos();
}


//
//	Toastr::Notification::renderButton
//

void Toastr::Notification::renderButton(Context ctx, ImVec2 right) {
	// render a button to close the notification (if required)
	auto buttonWidth = ctx.glyphSize.x + ctx.framePadding.x * 2.0f;
	ImGui::SetCursorScreenPos(right - ImVec2(buttonWidth, 0.0f));

	// (can't use ImGui::IsWindowHovered as it won't let ImGui::Button catch clicks for some reason)
	if (ImGui::GetCurrentContext()->HoveredWindow == ImGui::GetCurrentContext()->CurrentWindow) {
		if (ImGui::Button("x")) {
			expiredStart = ctx.currentTime;
		}

	} else {
		ImGui::Dummy(ImVec2(buttonWidth, buttonWidth));
	}
}


//
//	Toastr::Notification::getTextSize
//

ImVec2 Toastr::Notification::getTextSize(Context ctx) {
	if (ctx.textWidth == 0) {
		return ImGui::CalcTextSize(message.c_str());

	} else {
		auto availableSpace = ctx.textWidth * ctx.glyphSize.x;
		auto textSize = ctx.font->CalcTextSizeA(ImGui::GetStyle().FontSizeBase, availableSpace, availableSpace, message.c_str());
		textSize.x = availableSpace;
		return textSize;
	}
}


//
//	Toastr::Notification::getWindowPivot
//

ImVec2 Toastr::Notification::getWindowPivot(Context ctx) {
	switch (ctx.anchorType) {
		case AnchorType::topLeft: return ImVec2(0.0f, 0.0f); break;
		case AnchorType::topRight: return ImVec2(1.0f, 0.0f); break;
		case AnchorType::bottomLeft: return ImVec2(0.0f, 1.0f); break;
		case AnchorType::bottomRight: return ImVec2(1.0f, 1.0f); break;
	}

	// to keep MSVC happy
	return ImVec2();
}


//
//	Toastr::Notification::getBackgroundColor
//

ImU32 Toastr::Notification::getBackgroundColor(Context ctx) {
	switch (type) {
		case NotificationType::success: return ctx.palette.get(Color::successBackground); break;
		case NotificationType::warning: return ctx.palette.get(Color::warningBackground); break;
		case NotificationType::error: return ctx.palette.get(Color::errorBackground); break;
		case NotificationType::info: return ctx.palette.get(Color::infoBackground); break;
	}

	// to keep MSVC happy
	return 0;
}


//
//	Toastr::Notification::getIconBackgroundColor
//

ImU32 Toastr::Notification::getIconBackgroundColor(Context ctx) {
	switch (type) {
		case NotificationType::success: return ctx.palette.get(Color::successIcon); break;
		case NotificationType::warning: return ctx.palette.get(Color::warningIcon); break;
		case NotificationType::error: return ctx.palette.get(Color::errorIcon); break;
		case NotificationType::info: return ctx.palette.get(Color::infoIcon); break;
	}

	// to keep MSVC happy
	return 0;
}


//
//	Toastr::GetDarkPalette
//

const Toastr::Palette& Toastr::GetDarkPalette() {
	const static Palette palette = {{
		IM_COL32( 30,  60,  20, 255),	// successBackground
		IM_COL32( 60,  50,  10, 255),	// warningBackground
		IM_COL32( 80,  20,  20, 255),	// errorBackground
		IM_COL32( 20,  40,  80, 255),	// infoBackground
		IM_COL32(255, 255, 255, 255),	// messageColor
		IM_COL32( 50, 200,  90, 255),	// successIcon
		IM_COL32(255, 160,  10, 255),	// warningIcon
		IM_COL32(210,  50,  50, 255),	// errorIcon
		IM_COL32( 10, 140, 250, 255),	// infoIcon
		IM_COL32(255, 255, 255, 255)	// iconColor
	}};

	return palette;
}


//
//	Toastr::GetSaturatedPalette
//

const Toastr::Palette& Toastr::GetSaturatedPalette() {
	const static Palette palette = {{
		IM_COL32( 40, 170,  70, 255),	// successBackground
		IM_COL32(230, 140,  10, 255),	// warningBackground
		IM_COL32(190,  40,  60, 255),	// errorBackground
		IM_COL32(  0, 140, 240, 255),	// infoBackground
		IM_COL32(255, 255, 255, 255),	// messageColor
		IM_COL32( 50, 200,  90, 255),	// successIcon
		IM_COL32(250, 190,  10, 255),	// warningIcon
		IM_COL32(255,  50,  50, 255),	// errorIcon
		IM_COL32( 10, 180, 255, 255),	// infoIcon
		IM_COL32(255, 255, 255, 255)	// iconColor
	}};

	return palette;
}


//
//	Toastr::GetPastelPalette
//

const Toastr::Palette& Toastr::GetPastelPalette() {
	const static Palette palette = {{
		IM_COL32(200, 220, 200, 255),	// successBackground
		IM_COL32(240, 230, 190, 255),	// warningBackground
		IM_COL32(230, 200, 200, 255),	// errorBackground
		IM_COL32(190, 220, 230, 255),	// infoBackground
		IM_COL32(110, 110, 110, 255),	// messageColor
		IM_COL32( 50, 200,  90, 255),	// successIcon
		IM_COL32(255, 160,  10, 255),	// warningIcon
		IM_COL32(210,  50,  50, 255),	// errorIcon
		IM_COL32( 10, 140, 250, 255),	// infoIcon
		IM_COL32(255, 255, 255, 255)	// iconColor
	}};

	return palette;
}
