//	FileSelector - A file select dialog for Dear ImGui.
//	Copyright (c) 2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#if _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <locale>
#include <regex>
#include <unordered_map>

#include "FileSelector.h"


//
//	FileSelector::FileSelector
//

FileSelector::FileSelector() {
	// set current path to current working directory
	nextPath = std::filesystem::current_path();

	// add default sidebar links
	addDefaultFavorites();
	addDefaultClouds();
	addDefaultLocations();
	addDefaultMedia();
}


//
//	FileSelector::SetUserFilter
//

void FileSelector::SetUserFilter(const std::string& filter) {
	std::copy_n(
		filter.c_str(),
		std::min(filter.size() + 1, sizeof(state.userFilter.InputBuf)),
		state.userFilter.InputBuf);

	state.userFilter.Build();
}


//
//	FileSelector::GetUserFilter
//

std::string FileSelector::GetUserFilter() const {
	return std::string(state.userFilter.InputBuf);
}


//
//	FileSelector::SetExtensionFilter
//

void FileSelector::SetExtensionFilter(const std::string& filter) {
	// set new filter based on file extensions (empty filter means no filtering)
	state.extensionFilter.clear();
	std::stringstream ss(filter);
	std::string extension;

	while (std::getline(ss, extension, ',')) {
		state.extensionFilter.emplace_back(extension);
	}
}


//
//	FileSelector::GetExtensionFilter
//

std::string FileSelector::GetExtensionFilter() const {
	std::string filter;
	bool first = true;

	for (auto& extension : state.extensionFilter) {
		if (first) {
			first = false;

		} else {
			filter.append(",");
		}

		filter.append(extension);
	}

	return filter;
}


//
//	FileSelector::OpenFile
//

bool FileSelector::OpenFile(std::function<void()> av, float avHeight) {
	return openDialog(Mode::openFile, av, avHeight);
}


//
//	FileSelector::SaveAs
//

bool FileSelector::SaveAs(const std::filesystem::path& defaultPath, std::function<void()> av, float avHeight) {
	if (mode == Mode::idle) {
		if (!defaultPath.empty()) {
			if (std::filesystem::is_directory(defaultPath)) {
				nextPath = defaultPath;
				saveAsString.clear();

			} else {
				nextPath = defaultPath.parent_path();
				saveAsString = pathToString(defaultPath.filename());
			}

		} else {
			saveAsString.clear();
		}
	}

	return openDialog(Mode::saveAs, av, avHeight);
}


//
//	FileSelector::SelectFiles
//

bool FileSelector::SelectFiles(std::function<void()> av, float avHeight) {
	return openDialog(Mode::selectFiles, av, avHeight);
}


//
//	FileSelector::SelectDirectory
//

bool FileSelector::SelectDirectory(std::function<void()> av, float avHeight) {
	return openDialog(Mode::selectDirectory, av, avHeight);
}


//
//	FileSelector::Close
//

void FileSelector::Close() {
	if (mode != Mode::idle) {
		mode = Mode::idle;
		action = Action::cancelled;
	}
}


//
//	FileSelector::openDialog
//

bool FileSelector::openDialog(Mode openMode, std::function<void()> av, float avHeight) {
	// don't open multiple instances
	if (mode == Mode::idle) {
		mode = openMode;
		accessoryView = av;
		accessoryViewHeight = avHeight;
		selectedPath.clear();
		selectedPaths.clear();
		refreshListing = true;
		requestOpen = true;
		return true;

	} else {
		return false;
	}
}


//
//	FileSelector::SetSelectedExtension
//

void FileSelector::SetSelectedExtension(const std::filesystem::path& extension) {
	if (mode == Mode::saveAs && saveAsString.size()) {
		selectedPath.replace_extension(extension);
		saveAsString = pathToString(stringToPath(saveAsString).replace_extension(extension));
	}
}


//
//	FileSelector::Render
//

bool FileSelector::Render() {
	// don't do anything if a file selector isn't open
	if (mode == Mode::idle) {
		return false;
	}

	// ask popup to be opened (if required)
	if (requestOpen) {
		ImGui::OpenPopup("ImGuiFileSelector");
		requestOpen = false;
	}

	// render the selector popup
	auto viewPort = ImGui::GetMainViewport();
	const ImVec2 center = viewPort->GetCenter();
	const ImVec2 maxSize = viewPort->Size;
	const ImVec2 minSize = maxSize * 0.7f;
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSizeConstraints(minSize, maxSize);

	const ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;

	if (ImGui::BeginPopupModal("###ImGuiFileSelector", nullptr, windowFlags)) {
		// render dialog window and possible popups
		action = Action::none;
		renderFileDialog();
		renderPopups();

		if (ImGui::Shortcut(ImGuiKey_Escape)) {
			selectedPath.clear();
			selectedPaths.clear();
			action = Action::cancelled;
		}

		// see if user made selection
		if (action != Action::none && action != Action::cancelled) {
			// remove recent places entry to avoid duplication (if required)
			const auto i = std::find(state.recentPlaces.begin(), state.recentPlaces.end(), state.currentPath);

			if (i != state.recentPlaces.end()) {
				state.recentPlaces.erase(i);
			}

			// limit list (if required)
			if (state.recentPlaces.size() > 7) {
				state.recentPlaces.resize(7);
			}

			// add current path to recent places
			state.recentPlaces.emplace(state.recentPlaces.begin(), state.currentPath);
		}

		if (action != Action::none) {
			// close selector popup
			ImGui::CloseCurrentPopup();
			mode = Mode::idle;
		}

		ImGui::EndPopup();
	}

	return action != Action::none;
}


//
//	FileSelector::SetState
//

void FileSelector::SetState(const State& newState) {
	if (std::filesystem::exists(newState.currentPath)) {
		state.currentPath = newState.currentPath;
	}

	state.recentPlaces.clear();

	for (auto& path : newState.recentPlaces) {
		if (std::filesystem::exists(path)) {
			state.recentPlaces.emplace_back(path);
		}
	}

	state.view = newState.view;
	state.showSideBar = newState.showSideBar;
	state.showHidden = newState.showHidden;
	state.extensionFilter = newState.extensionFilter;
	state.userFilter = newState.userFilter;
	state.sortColumn = newState.sortColumn;
	state.sortOrder = newState.sortOrder;
	state.sizeBarWidthInGlyphs = newState.sizeBarWidthInGlyphs;
}


//
//	FileSelector::SetLabels
//

void FileSelector::SetLabels(Labels& newLabels) {
	labels = newLabels;
	refreshListing = true;
}


//
//	FileSelector::setCurrentPath
//

bool FileSelector::setCurrentPath(const std::filesystem::path path, bool addHistory) {
	// convert path to an absolute, unique path with no relative elements (. or ..) or symbolic links
	std::error_code ec;
	const auto canonicalPath = std::filesystem::canonical(path, ec);

	// sanity checks
	if (ec) {
		return false;
	}

	if (!isAccessible(canonicalPath)) {
		return false;
	}

	// ignore request if we already are looking at this place
	if (path == state.currentPath) {
		return false;
	}

	// try to reload the directory listing and check for errors
	if (!listing.load(path, state, labels)) {
		return false;
	}

	// save new path
	state.currentPath = canonicalPath;

	// reset selections
	selectedPath.clear();
	selectedPaths.clear();

	// build a stack of path parts (in reverse order)
	pathStack.clear();
	std::filesystem::path partialPath;

	for (auto i = state.currentPath.begin(); i != state.currentPath.end(); i++) {
		partialPath /= *i;
		pathStack.emplace_back(pathToString(*i), partialPath);
	}

#ifdef _WIN32
	// handle root name/directory/path madness in Windows (thank you DOS :-)
	pathStack[1].name = pathToString(pathStack[1].path);
	pathStack.erase(pathStack.begin());
#endif

	std::reverse(pathStack.begin(), pathStack.end());

	// add to history (if required)
	if (addHistory) {
		pathHistory.resize(historyIndex);
		pathHistory.emplace_back(canonicalPath);
		historyIndex++;
	}

	return true;
}


//
//	FileSelector::renderFileDialog
//

void FileSelector::renderFileDialog() {
	// update path (if required)
	if (!nextPath.empty()) {
		setCurrentPath(nextPath, true);
		nextPath.clear();
		refreshListing = false;
	}

	// refresh directory lists (if required)
	if (refreshListing) {
		listing.reload(state, labels);
		refreshListing = false;
	}

	// get reusable values
	frameHeight = ImGui::GetFrameHeight();
	glyphSize = ImGui::CalcTextSize("#");
	itemSpacing = ImGui::GetStyle().ItemSpacing;
	extraSpacing = frameHeight * 0.4f;

	// generate sidebar (if configured)
	if (state.showSideBar) {
		const auto availableSpace = ImGui::GetContentRegionAvail();

		const ImGuiChildFlags flags =
			ImGuiChildFlags_Borders |
			ImGuiChildFlags_ResizeX;

		if (ImGui::BeginChild("sideBar",ImVec2(glyphSize.x * state.sizeBarWidthInGlyphs, availableSpace.y), flags)) {
			renderSideBar();
		}

		ImGui::EndChild();
		ImGui::SameLine();
	}

	// render main area
	if (ImGui::BeginChild("mainArea", ImGui::GetContentRegionAvail())) {
		renderHeader();
		const auto availableSpace = ImGui::GetContentRegionAvail();
		const auto accessorHeight = accessoryView ? 1.0f + accessoryViewHeight + extraSpacing * 2.0f + itemSpacing.y * 2.0f : 0.0f;
		const auto actionButtonHeight = frameHeight * 1.5f + itemSpacing.y * 2.0f;
		const auto size = ImVec2(availableSpace.x, availableSpace.y - actionButtonHeight - accessorHeight);

		switch (state.view) {
			case View::icons: renderIconView(size); break;
			case View::list: renderListView(size); break;
			case View::columns: renderColumnsView(size); break;
		}

		if (accessoryView) {
			spacing();
			accessoryView();
			spacing();
			ImGui::Separator();
		}

		renderActionButtons();
	}

	ImGui::EndChild();
}


//
//	FileSelector::renderSideBar
//

void FileSelector::renderSideBar() {
	// render sidebar group (empty groups are skipped)
	renderSideBarGroup(labels.favorites.c_str(), favorites);
	renderSideBarGroup(labels.clouds.c_str(), clouds);
	renderSideBarGroup(labels.locations.c_str(), locations);
	renderSideBarGroup(labels.media.c_str(), media);
}


//
//	FileSelector::renderSideBarGroup
//

void FileSelector::renderSideBarGroup(const std::string& label, SideBarGroup& group) {
	// skip empty groups
	if (group.entries.size()) {
		// render group label and expansion toggle
		grouping(label.c_str(), &(group.expanded));

		// ensure group content is visible
		if (group.expanded) {
			ImGui::Indent();

			// render all group entries
			for (auto& entry : group.entries) {
				ImGui::PushID(&entry);

				if (ImGui::Selectable(entry.name.c_str())) {
					// select path if group entry is selected
					nextPath = entry.path;
				}

				ImGui::PopID();
			}

			ImGui::Unindent();
		}

		ImGui::Spacing();
	}
}


//
//	FileSelector::renderHeader
//

void FileSelector::renderHeader() {
	// render header part of common area
	const auto width = ImGui::GetContentRegionAvail().x;
	spacing();

	// render filename input if we are in "SaveAs" mode
	if (mode == Mode::saveAs) {
		const auto saveAsPos = ImGui::GetCursorScreenPos();
		const auto saveAsWidth = glyphSize.x * 30.0f;
		const auto totalWidth = ImGui::CalcTextSize(labels.saveAs.c_str()).x + itemSpacing.x + saveAsWidth;
		ImGui::SetCursorScreenPos(ImVec2(saveAsPos.x + (width - totalWidth) * 0.5f, saveAsPos.y));
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(labels.saveAs.c_str());
		ImGui::SameLine();
		ImGui::SetNextItemWidth(saveAsWidth);

		if (ImGui::IsWindowAppearing()) {
			ImGui::SetKeyboardFocusHere();
		}

		inputPath("###saveas", &saveAsString);
		spacing();
	}

	// render history navigation buttons
	auto disabled = historyIndex <= 1;
	if (disabled) { ImGui::BeginDisabled(); }

	if (ImGui::ArrowButton("previous", ImGuiDir_Left)) {
		historyIndex--;
		setCurrentPath(pathHistory[historyIndex - 1], false);
	}

	if (disabled) { ImGui::EndDisabled(); }

	ImGui::SameLine();
	disabled = historyIndex == pathHistory.size();
	if (disabled) { ImGui::BeginDisabled(); }

	if (ImGui::ArrowButton("next", ImGuiDir_Right)) {
		setCurrentPath(pathHistory[historyIndex++], false);
	}

	if (disabled) { ImGui::EndDisabled(); }
	ImGui::SameLine();

	// render view selector
	ImGui::SetNextItemWidth(glyphSize.x * 6.0f);

	if (ImGui::BeginCombo("##view", nullptr)) {
		ImGui::MenuItem(labels.showSideBar.c_str(), nullptr, &state.showSideBar);
		ImGui::MenuItem(labels.showHidden.c_str(), nullptr, &state.showHidden);
		ImGui::Separator();
		ImGui::TextDisabled("%s", labels.showItemsAs.c_str());
		bool selected;
		selected = state.view == View::icons; if (ImGui::MenuItem(labels.icons.c_str(), nullptr, selected)) { state.view = View::icons; }
		selected = state.view == View::list; if (ImGui::MenuItem(labels.list.c_str(), nullptr, selected)) { state.view = View::list; }
		selected = state.view == View::columns; if (ImGui::MenuItem(labels.columns.c_str(), nullptr, selected)) { state.view = View::columns; }
		ImGui::EndCombo();
	}

	if (ImGui::BeginComboPreview()) {
		const auto drawList = ImGui::GetWindowDrawList();
		const auto pos = ImGui::GetCursorScreenPos();
		const auto color = ImGui::GetColorU32(ImGuiCol_TextDisabled);

		switch (state.view) {
			case View::icons: {
				const auto rectangle = [drawList, color](ImVec2 p, ImVec2 s) {
					drawList->AddRect(p, p + s, color);
				};

				const auto gap = 1.0f;
				const auto side = (glyphSize.y - gap) * 0.5f;
				const auto size = ImVec2(side, side);
				rectangle(pos, size);
				rectangle(pos + ImVec2(side + gap, 0.0f), size);
				rectangle(pos + ImVec2(0.0f, side + gap), size);
				rectangle(pos + ImVec2(side + gap, side + gap), size);
				break;
			}

			case View::list: {
				const auto line = [drawList, color](ImVec2 p, float w) {
					drawList->AddCircle(p + ImVec2(1.0f, 0.0f), 1.0f, color);
					drawList->AddLineH(p.x + 3.0f, p.x + w, p.y, color);
				};

				const auto iconWidth = glyphSize.y;
				line(pos + ImVec2(0.0f, 2.0f), iconWidth);
				line(pos + ImVec2(0.0f, glyphSize.y * 0.5f), iconWidth);
				line(pos + ImVec2(0.0f, glyphSize.y - 2.0f), iconWidth);
				break;
			}

			case View::columns: {
				const auto rectangle = [drawList, color](ImVec2 p, ImVec2 s) {
					drawList->AddRect(p, p + s, color);
				};

				const auto columnWidth = glyphSize.y / 3.0f;
				const auto size = ImVec2(columnWidth, glyphSize.y);
				rectangle(pos, size);
				rectangle(pos + ImVec2(columnWidth, 0.0f), size);
				rectangle(pos + ImVec2(columnWidth * 2.0f, 0.0f), size);
				break;
			}
		}

		ImGui::EndComboPreview();
	}

	// render path selector and recent places
	ImGui::SameLine();
	const auto pathSelectorWidth = glyphSize.x * 25.0f;
	const auto filterWidth = glyphSize.x * 20.0f;
	const auto selectorFilterGap = (ImGui::GetContentRegionAvail().x - pathSelectorWidth - filterWidth) * 0.5f;
	ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(selectorFilterGap, 0.0f));

	const float itemHeight = ImGui::GetTextLineHeightWithSpacing();
	const float popupHeight = itemHeight * 12 + ImGui::GetStyle().FramePadding.y * 4.0f;
	ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, popupHeight));
	ImGui::SetNextItemWidth(pathSelectorWidth);

	if (ImGui::BeginCombo("###pathSelector", pathStack[0].name.c_str())) {
		for (auto i = pathStack.begin() + 1; i < pathStack.end(); i++) {
			ImGui::PushID(&(*i));

			if (ImGui::Selectable(i->name.c_str())) {
				nextPath = i->path;
			}

			ImGui::PopID();
		}

		ImGui::Separator();
		ImGui::TextDisabled("%s", labels.recentPlaces.c_str());

		for (auto i = state.recentPlaces.begin(); i < state.recentPlaces.end(); i++) {
			ImGui::PushID(&(*i));
			const auto name = pathToString(i->filename());

			if (ImGui::Selectable(reinterpret_cast<const char*>(name.c_str()))) {
				nextPath = *i;
			}

			ImGui::PopID();
		}

		ImGui::EndCombo();
	}

	// render user filter input field
	ImGui::SameLine();
	ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(selectorFilterGap, 0.0f));
	ImGui::SetNextItemWidth(filterWidth);

	if (inputStringWithHint("###filter", labels.filter.c_str(), &filterString)) {
		SetUserFilter(filterString);
	}

	spacing();
}


//
//	FileSelector::renderIconView
//

void FileSelector::renderIconView(ImVec2 size) {
	ImGui::BeginChild("icons", size, ImGuiChildFlags_Borders);

	auto drawList = ImGui::GetWindowDrawList();
	auto font = ImGui::GetFont();
	const auto color = ImGui::GetColorU32(ImGuiCol_Text);

	const auto fontSize = ImGui::GetStyle().FontSizeBase - 2.0f;
	const auto iconMargin = glyphSize.x * 4.0f;
	const auto iconOffset = ImVec2(iconMargin, itemSpacing.y);
	const auto iconSize = glyphSize.y * state.iconScale;
	const auto right = ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x;

	listing.forEach(state, [&](Entry& entry) {
		const auto pos = ImGui::GetCursorScreenPos();
		const auto label = entry.nameString.c_str();
		const auto labelSize = font->CalcTextSizeA(fontSize, FLT_MAX, iconSize + iconMargin * 2.0f, label);
		const auto entrySize = ImVec2(iconSize + iconMargin * 2.0f, itemSpacing.y + iconSize + labelSize.y);

		if (ImGui::InvisibleButton(("##" + entry.nameString).c_str(), entrySize)) {
			handleEntrySelection(entry);
		}

		if (ImGui::IsItemHovered()) {
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				handleEntrySelection(entry);
			}

			if (ImGui::IsItemActive()) {
				drawList->AddRectFilled(pos, pos + entrySize, ImGui::GetColorU32(ImGuiCol_HeaderActive));

			} else {
				drawList->AddRectFilled(pos, pos + entrySize, ImGui::GetColorU32(ImGuiCol_HeaderHovered));
			}

		} else if (entry.isSelected) {
			drawList->AddRectFilled(pos, pos + entrySize, ImGui::GetColorU32(ImGuiCol_Header));
		}

		// render popup context menu for right click (ctrl-click on MacOS)
		if (ImGui::BeginPopupContextItem()) {
			renderEntryContextMenu(entry);
			ImGui::EndPopup();
		}

		// render the icon
		IconData iconData{
			pos + iconOffset,
			state.iconScale,
			ImVec2(iconSize, iconSize),
			drawList,
			entry.isDirectory,
			entry.extension
		};

		renderIcon(iconData);

		// render the label
		const auto labelPos = ImVec2(pos.x + (entrySize.x - labelSize.x) / 2.0f, pos.y + itemSpacing.y + iconSize);
		drawList->AddText(font, fontSize, labelPos, color, label, 0, entrySize.x);

		if (pos.x + entrySize.x * 2.0f < right) {
			ImGui::SameLine();
		}
	});

	if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_NoOpenOverItems)) {
		auto sortMenu = [this](const std::string& label, SortColumn column) {
			bool isSelected = state.sortColumn == column;

			if (ImGui::MenuItem(label.c_str(), nullptr, isSelected)) {
				if (isSelected) {
					state.sortOrder = (state.sortOrder == SortOrder::ascending)
						? SortOrder::descending
						: SortOrder::ascending;

				} else {
					state.sortColumn = column;
					state.sortOrder = SortOrder::ascending;
				}

				listing.sort(state);
			}
		};

		if (ImGui::MenuItem(labels.showInFinder.c_str())) {
			showInFinder(state.currentPath.parent_path());
		}

		ImGui::Separator();

		ImGui::TextDisabled("%s", labels.sortItemsBy.c_str());
		sortMenu(labels.nameColumn, SortColumn::name);
		sortMenu(labels.typeColumn, SortColumn::type);
		sortMenu(labels.dateColumn, SortColumn::date);
		sortMenu(labels.sizeColumn, SortColumn::size);
		ImGui::EndPopup();
	}

	ImGui::EndChild();
}


//
//	FileSelector::renderListView
//

void FileSelector::renderListView(ImVec2 size) {
	// build table of current directory entries
	const ImGuiTableFlags tableFlags =
		ImGuiTableFlags_ScrollY |
		ImGuiTableFlags_RowBg |
		ImGuiTableFlags_BordersOuterH |
		ImGuiTableFlags_Sortable;

	if (ImGui::BeginTable("listView", 4, tableFlags, size)) {
		ImGui::TableSetupColumn(labels.nameColumn.c_str(), ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn(labels.typeColumn.c_str(), ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize);
		ImGui::TableSetupColumn(labels.dateColumn.c_str(), ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize);
		ImGui::TableSetupColumn(labels.sizeColumn.c_str(), ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize);
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableHeadersRow();

		// handle sort requests
		if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs()) {
			if (sortSpecs->SpecsDirty) {
				sortSpecs->SpecsDirty = false;
				state.sortColumn = static_cast<SortColumn>(sortSpecs->Specs->ColumnIndex);
				state.sortOrder	= sortSpecs->Specs->SortDirection == ImGuiSortDirection_Ascending ? SortOrder::ascending : SortOrder::descending;
				listing.sort(state);
			}
		}

		// render each entry
		listing.forEach(state, [this](Entry& entry) {
			ImGui::TableNextRow();

			// show filename
			ImGui::TableSetColumnIndex(0);

			// render the icon
			const auto iconSize = ImVec2(glyphSize.y, glyphSize.y);

			IconData iconData{
				ImGui::GetCursorScreenPos(),
				1.0f,
				iconSize,
				ImGui::GetWindowDrawList(),
				entry.isDirectory,
				entry.extension
			};

			renderIcon(iconData);
			ImGui::Dummy(iconSize);
			ImGui::SameLine();

			ImGuiSelectableFlags selectableFlags =
				ImGuiSelectableFlags_SpanAllColumns |
				ImGuiSelectableFlags_AllowDoubleClick;

			if (ImGui::Selectable(entry.nameString.c_str(), entry.isSelected, selectableFlags)) {
				handleEntrySelection(entry);
			}

			// render popup context menu for right click (ctrl-click on MacOS)
			if (ImGui::BeginPopupContextItem()) {
				renderEntryContextMenu(entry);
				ImGui::EndPopup();
			}

			// show type
			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(entry.typeString.c_str());

			// show date
			ImGui::TableSetColumnIndex(2);
			ImGui::TextUnformatted(entry.dateString.c_str());

			// show size
			ImGui::TableSetColumnIndex(3);
			ImGui::TextUnformatted(entry.sizeString.c_str());
		});

		ImGui::EndTable();
	}
}


//
//	FileSelector::renderColumnsView
//

void FileSelector::renderColumnsView(ImVec2 size) {
	ImGui::BeginChild("icons", size, ImGuiChildFlags_Borders);
	ImGui::TextUnformatted("Not yet implemented");
	ImGui::EndChild();
}


//
//	FileSelector::renderIcon
//

void FileSelector::renderIcon(const IconData& data) {
	bool useBuiltInIcons = true;

	if (iconRenderer) {
		if (iconRenderer(data)) {
			useBuiltInIcons = false;
		}
	}

	if (useBuiltInIcons) {
		if (data.isDirectory) {
			renderFolder(data.drawList, data.pos, data.scale);

		} else {
			renderFile(data.drawList, data.pos, data.scale, data.extension);
		}
	}
}


//
//	FileSelector::renderEntryContextMenu
//

void FileSelector::renderEntryContextMenu(Entry& entry) {
	if (ImGui::MenuItem(labels.rename.c_str())) {
		openRename = true;
		oldMoveName = entry.nameString;
	}

	if (ImGui::MenuItem(labels.moveToTrash.c_str())) {
		if (moveToTrashCan(entry.path, errorMessage)) {
			if (entry.isSelected) {
				unSelectEntry(entry);
			}

			refreshListing = true;

		} else {
			openError = true;
		}
	}

	if (ImGui::MenuItem(labels.duplicate.c_str())) {
		std::error_code ec;

		std::filesystem::copy(
			entry.path,
			getDuplicatePathName(entry.path),
			std::filesystem::copy_options::recursive,
			ec);

		if (ec) {
			errorMessage = ec.message();
			openError = true;

		} else {
			nextPath = state.currentPath;
		}
	}

	if (ImGui::MenuItem(labels.permissions.c_str())) {
		oldPermissions = std::filesystem::status(entry.path).permissions();
		newPermissions = oldPermissions;
		permissionsPath = entry.path;
		openPermissions = true;
	}

	if (ImGui::MenuItem(labels.showInFinder.c_str())) {
		showInFinder(entry.path.parent_path());
	}
}


//
//	FileSelector::renderActionButtons
//

void FileSelector::renderActionButtons() {
	// a little vertical spacing
	spacing();

	// add ability to create a new folder
	if (ImGui::Button(labels.newFolder.c_str())) {
		openNewFolder = true;
	}

	ImGui::SameLine();

	// select label for "OK" button
	const std::string& okLabel = (mode == Mode::openFile) ? labels.open : (mode == Mode::saveAs) ? labels.save : labels.select;

	// right align buttons
	const auto size = rightAlign(labels.cancel, okLabel);

	// handle cancel button and shortcut
	if (ImGui::Button(labels.cancel.c_str(), size)) {
		selectedPath.clear();
		selectedPaths.clear();
		action = Action::cancelled;
	}

	// handle OK button (disable when nothing is selected)
	ImGui::SameLine();
	const auto okAvailable = isOkAvailable();

	if (!okAvailable) {
		ImGui::BeginDisabled();
	}

	if (ImGui::Button(okLabel.c_str(), size) || (okAvailable && ImGui::Shortcut(ImGuiKey_Enter, ImGuiInputFlags_RouteOverActive))) {
		handleOk();
	}

	if (!okAvailable) {
		ImGui::EndDisabled();
	}
}


//
//	FileSelector::renderPopups
//

void FileSelector::renderPopups() {
	// render possible popup dialogs (these function do nothing unless triggered)
	renderOverWritePopup();
	renderNewFolderPopup();
	renderRenamePopup();
	renderPermissionsPopup();
	renderErrorPopup();
}


//
//	FileSelector::renderOverWritePopup
//

void FileSelector::renderOverWritePopup() {
	// open dialog (if required)
	if (openOverWrite) {
		ImGui::OpenPopup(labels.confirmationWindow.c_str());
		openOverWrite = false;
	}

	// render dialog (if required)
	if (ImGui::BeginPopupModal(labels.confirmationWindow.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		spacing();
		ImGui::TextUnformatted(labels.fileExists.c_str());

		spacing();
		const auto size = rightAlign(labels.cancel, labels.ok);

		if (ImGui::Button(labels.cancel.c_str(), size)) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button(labels.ok.c_str(), size)) {
			action = Action::selectedSaveAs;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}


//
//	FileSelector::renderNewFolderPopup
//

void FileSelector::renderNewFolderPopup() {
	// open dialog (if required)
	bool appearing = false;

	if (openNewFolder) {
		ImGui::OpenPopup(labels.newFolder.c_str());
		openNewFolder = false;
		newFolderName.clear();
		newFolderError.clear();
		appearing = true;
	}

	// render dialog (if required)
	if (ImGui::BeginPopupModal(labels.newFolder.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::TextUnformatted(labels.nameOfNewFolder.c_str());

		spacing();

		if (appearing) {
			ImGui::SetKeyboardFocusHere();
		}

		inputPath("###newfolder", &newFolderName);
		bool emptyName = newFolderName.empty();
		bool invalidName = std::filesystem::exists(state.currentPath / newFolderName);

		if (!emptyName && invalidName) {
			ImGui::TextDisabled("%s", labels.nameTaken.c_str());
		}

		if (newFolderError.size()) {
			ImGui::TextDisabled("%s", newFolderError.c_str());
		}

		spacing();
		const auto size = rightAlign(labels.cancel, labels.create);

		if (ImGui::Button(labels.cancel.c_str(), size) || ImGui::Shortcut(ImGuiKey_Escape)) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (emptyName || invalidName) {
			ImGui::BeginDisabled();
		}

		if (ImGui::Button(labels.create.c_str(), size) || ImGui::Shortcut(ImGuiKey_Enter, ImGuiInputFlags_RouteOverActive)) {
			std::error_code ec;
			std::filesystem::create_directory(state.currentPath / newFolderName, ec);

			if (ec) {
				newFolderError = ec.message();

			} else {
				refreshListing = true;
				ImGui::CloseCurrentPopup();
			}
		}

		if (emptyName || invalidName) {
			ImGui::EndDisabled();
		}

		ImGui::EndPopup();
	}
}


//
//	FileSelector::renderRenamePopup
//

void FileSelector::renderRenamePopup() {
	// open dialog (if required)
	bool appearing = false;

	if (openRename) {
		ImGui::OpenPopup(labels.rename.c_str());
		openRename = false;
		newMoveName = oldMoveName;
		moveError.clear();
		appearing = true;
	}

	// render dialog (if required)
	if (ImGui::BeginPopupModal(labels.rename.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		spacing();

		if (appearing) {
			ImGui::SetKeyboardFocusHere();
		}

		inputPath("###newname", &newMoveName);
		bool emptyName = newMoveName.empty();
		bool invalidName = std::filesystem::exists(state.currentPath / newMoveName);

		if (!emptyName && invalidName) {
			ImGui::TextDisabled("%s", labels.nameTaken.c_str());
		}

		if (moveError.size()) {
			ImGui::TextDisabled("%s", moveError.c_str());
		}

		spacing();
		const auto size = rightAlign(labels.cancel, labels.rename);

		if (ImGui::Button(labels.cancel.c_str(), size) || ImGui::Shortcut(ImGuiKey_Escape)) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (emptyName || invalidName) {
			ImGui::BeginDisabled();
		}

		if (ImGui::Button(labels.rename.c_str(), size) || ImGui::Shortcut(ImGuiKey_Enter, ImGuiInputFlags_RouteOverActive)) {
			std::error_code ec;
			std::filesystem::rename(state.currentPath / oldMoveName, state.currentPath / newMoveName, ec);

			if (ec) {
				moveError = ec.message();

			} else {
				refreshListing = true;
				ImGui::CloseCurrentPopup();
			}
		}

		if (emptyName || invalidName) {
			ImGui::EndDisabled();
		}

		ImGui::EndPopup();
	}
}


//
//	FileSelector::renderPermissionsPopup
//

void FileSelector::renderPermissionsPopup() {
	// open dialog (if required)
	if (openPermissions) {
		ImGui::OpenPopup(labels.permissions.c_str());
		permissionsError.clear();
		openPermissions = false;
	}

	// render dialog (if required)
	if (ImGui::BeginPopupModal(labels.permissions.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		spacing();

		static constexpr std::filesystem::perms readAll =
			std::filesystem::perms::owner_read |
			std::filesystem::perms::group_read |
			std::filesystem::perms::others_read;

		static constexpr std::filesystem::perms writeAll =
			std::filesystem::perms::owner_write |
			std::filesystem::perms::group_write |
			std::filesystem::perms::others_write;

		static constexpr std::filesystem::perms execAll =
			std::filesystem::perms::owner_exec |
			std::filesystem::perms::group_exec |
			std::filesystem::perms::others_exec;

		if (ImGui::BeginTable("permissions", 4)) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); permissionsButton(labels.all, std::filesystem::perms::all);
			ImGui::TableNextColumn(); permissionsButton(labels.read, readAll);
			ImGui::TableNextColumn(); permissionsButton(labels.write, writeAll);
			ImGui::TableNextColumn(); permissionsButton(labels.exec, execAll);

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); permissionsButton(labels.owner, std::filesystem::perms::owner_all);
			ImGui::TableNextColumn(); permissionCheckBox("###ownerRead", std::filesystem::perms::owner_read);
			ImGui::TableNextColumn(); permissionCheckBox("###ownerWrite", std::filesystem::perms::owner_write);
			ImGui::TableNextColumn(); permissionCheckBox("###ownerExec", std::filesystem::perms::owner_exec);

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); permissionsButton(labels.group, std::filesystem::perms::group_all);
			ImGui::TableNextColumn(); permissionCheckBox("###groupRead", std::filesystem::perms::group_read);
			ImGui::TableNextColumn(); permissionCheckBox("###groupWrite", std::filesystem::perms::group_write);
			ImGui::TableNextColumn(); permissionCheckBox("###groupExec", std::filesystem::perms::group_exec);

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); permissionsButton(labels.others, std::filesystem::perms::others_all);
			ImGui::TableNextColumn(); permissionCheckBox("###othersRead", std::filesystem::perms::others_read);
			ImGui::TableNextColumn(); permissionCheckBox("###othersWrite", std::filesystem::perms::others_write);
			ImGui::TableNextColumn(); permissionCheckBox("###othersExec", std::filesystem::perms::others_exec);

			ImGui::EndTable();
		}

		if (permissionsError.size()) {
			ImGui::TextDisabled("%s", permissionsError.c_str());
		}

		spacing();
		const auto size = rightAlign(labels.cancel, labels.ok);

		if (ImGui::Button(labels.cancel.c_str(), size) || ImGui::Shortcut(ImGuiKey_Escape)) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (newPermissions == oldPermissions) {
			ImGui::BeginDisabled();
		}

		if (ImGui::Button(labels.ok.c_str(), size) || ImGui::Shortcut(ImGuiKey_Enter, ImGuiInputFlags_RouteOverActive)) {
			std::error_code ec;
			std::filesystem::permissions(permissionsPath, newPermissions, ec);

			if (ec) {
				permissionsError = ec.message();

			} else {
				refreshListing = true;
				ImGui::CloseCurrentPopup();
			}
		}

		if (newPermissions == oldPermissions) {
			ImGui::EndDisabled();
		}

		ImGui::EndPopup();
	}
}


//
//	FileSelector::renderErrorPopup
//

void FileSelector::renderErrorPopup() {
	// open dialog (if required)
	if (openError) {
		ImGui::OpenPopup(labels.errorWindow.c_str());
		openError = false;
	}

	// render dialog (if required)
	if (ImGui::BeginPopupModal(labels.errorWindow.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		spacing();
		ImGui::TextUnformatted(errorMessage.c_str());

		spacing();
		const auto size = rightAlign(labels.ok);

		if (ImGui::Button(labels.ok.c_str(), size)) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}


//
//	FileSelector::addDefaultFavorites
//

void FileSelector::addDefaultFavorites() {
	const auto home = getHome();

	if (!home.empty()) {
		// these only get added when they exist
		favorites.add("Home", home);
		favorites.add(KnownDirectory::desktop);
		favorites.add(KnownDirectory::documents);
		favorites.add(KnownDirectory::downloads);
	}
}


//
//	FileSelector::addDefaultClouds
//

void FileSelector::addDefaultClouds() {
	const auto home = getHome();

	if (!home.empty()) {
		clouds.add("iCloud Drive", home / "Library" / "Mobile Documents" / "com~apple~CloudDocs");
		clouds.add("OneDrive", home / "OneDrive");
	}
}


//
//	FileSelector::addDefaultLocations
//

void FileSelector::addDefaultLocations() {
	forEachKnownLocation([this](const std::string &name, const std::filesystem::path &path) {
		 locations.add(name, path);
	});
}


//
//	FileSelector::addDefaultMedia
//

void FileSelector::addDefaultMedia() {
	const auto home = getHome();

	if (!home.empty()) {
		media.add(KnownDirectory::movies);
		media.add(KnownDirectory::music);
		media.add(KnownDirectory::pictures);
	}
}


//
//	FileSelector::handleEntrySelection
//

void FileSelector::handleEntrySelection(Entry& entry) {
	// handle clicking on an entry in a directory
	if (entry.isDirectory) {
		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			nextPath = entry.path;

		} else if (mode == Mode::selectDirectory) {
			toggleEntrySelection(entry);
		}

	} else {
		switch (mode) {
			case Mode::openFile: {
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					selectedPath = entry.path;
					action = Action::selectedOpenFile;

				} else {
					toggleEntrySelection(entry);
				}

				break;
			}

			case Mode::saveAs: {
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					selectedPath = entry.path;

					if (std::filesystem::exists(selectedPath)) {
						openOverWrite = true;

					} else {
						action = Action::selectedSaveAs;
					}

				} else {
					saveAsString = entry.nameString;
				}

				break;
			}

			case Mode::selectFiles: {
				toggleEntrySelection(entry);
				break;
			}

			default:
				break;
		}
	}
}


//
//	FileSelector::isOkAvailable
//

bool FileSelector::isOkAvailable() {
	// determine state of "OK" button in file selector
	switch (mode) {
		case Mode::openFile: return !selectedPath.empty(); break;
		case Mode::saveAs: return saveAsString.size(); break;
		case Mode::selectFiles: return selectedPaths.size(); break;
		case Mode::selectDirectory: return true; break;
		default: break;
	}

	return false;
}


//
//	FileSelector::handleOk
//

void FileSelector::handleOk() {
	// determine what to do with OK based on selector mode
	switch (mode) {
		case Mode::openFile:
			action = Action::selectedOpenFile;
			break;

		case Mode::saveAs:
			selectedPath = state.currentPath / saveAsString;

			if (std::filesystem::exists(selectedPath)) {
				openOverWrite = true;

			} else {
				action = Action::selectedSaveAs;
			}

			break;

		case Mode::selectFiles:
			action = Action::selectedFiles;
			break;

		case Mode::selectDirectory:
			if (selectedPath.empty()) {
				selectedPath = state.currentPath;
			}

			action = Action::selectedDirectory;
			break;

		default:
			break;
	}
}


//
//	FileSelector::spacing
//

void FileSelector::spacing() {
	// add a little bit of vertical spacing for a prettier layout
	const auto pos = ImGui::GetCursorScreenPos();
	ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + extraSpacing));
}


//
//	FileSelector::rightAlign
//

ImVec2 FileSelector::rightAlign(const std::string& button1, const std::string& button2) {
	// reposition current screen cursor to right align botton(s)
	if (button2.empty()) {
		// right align a single button
		const auto pos = ImGui::GetCursorScreenPos();
		const auto availableSpace = ImGui::GetContentRegionAvail();
		const auto size = ImVec2(ImGui::CalcTextSize(button1.c_str()).x + glyphSize.x * 2.0f, 0.0f);
		ImGui::SetCursorScreenPos(ImVec2(pos.x + availableSpace.x - size.x, pos.y));
		return size;

	} else {
		// right align two buttons
		const auto pos = ImGui::GetCursorScreenPos();
		const auto availableSpace = ImGui::GetContentRegionAvail();

		const auto size = ImVec2(
			std::max(
				ImGui::CalcTextSize(button1.c_str()).x,
				ImGui::CalcTextSize(button2.c_str()).x) + glyphSize.x * 2.0f,
			0.0f);

		ImGui::SetCursorScreenPos(ImVec2(pos.x + availableSpace.x - size.x * 2.0f - itemSpacing.x, pos.y));
		return size;
	}
}


//
//	FileSelector::grouping
//

bool FileSelector::grouping(const char* label, bool* expanded) {
	// determine position and space
	const auto pos = ImGui::GetCursorScreenPos();
	auto size = ImGui::GetContentRegionAvail();
	size.y = glyphSize.y;

	// run button action
	const bool changed = ImGui::InvisibleButton(label, size);

	if (changed) {
		*expanded = !*expanded;
	}

	// render label and state
	auto drawList = ImGui::GetWindowDrawList();
	const auto color = ImGui::GetColorU32(ImGuiCol_TextDisabled);
	drawList->AddText(pos, color, label);

	if (ImGui::IsItemHovered()) {
		const auto right = pos + ImVec2(size.x - glyphSize.x, 0.0f);
		ImVec2 p1 = ImVec2(right + ImVec2(0.0f, glyphSize.y * 0.3f));
		ImVec2 p2 = right + (*expanded ? ImVec2(glyphSize.x * 0.5f, glyphSize.y * 0.7f) : ImVec2(glyphSize.x, glyphSize.y * 0.5f));
		ImVec2 p3 = right + (*expanded ? ImVec2(glyphSize.x, glyphSize.y * 0.3f) : ImVec2(0.0f, glyphSize.y * 0.7f));
		drawList->AddLine(p1, p2, color);
		drawList->AddLine(p2, p3, color);
	}

	// run result
	return changed;
}


//
//	FileSelector::inputString
//

bool FileSelector::inputString(const char* label, std::string* value) {
	// text input field with data from std::string
	ImGuiInputTextFlags flags =
		ImGuiInputTextFlags_NoUndoRedo |
		ImGuiInputTextFlags_CallbackResize;

	return ImGui::InputText(label, value->data(), value->capacity() + 1, flags, [](ImGuiInputTextCallbackData* data) {
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			std::string* value = static_cast<std::string*>(data->UserData);
			value->resize(data->BufTextLen);
			data->Buf = (char*) value->c_str();
		}

		return 0;
	}, value);
}


//
//	FileSelector::inputStringWithHint
//

bool FileSelector::inputStringWithHint(const char* label, const char* hint, std::string* value) {
	// text input field with hint with data from std::string
	ImGuiInputTextFlags flags =
		ImGuiInputTextFlags_NoUndoRedo |
		ImGuiInputTextFlags_CallbackResize;

	return ImGui::InputTextWithHint(label, hint, value->data(), value->capacity() + 1, flags, [](ImGuiInputTextCallbackData* data) {
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			std::string* value = static_cast<std::string*>(data->UserData);
			value->resize(data->BufTextLen);
			data->Buf = (char*) value->c_str();
		}

		return 0;
	}, value);
}


//
//	FileSelector::inputPath
//

bool FileSelector::inputPath(const char* label, std::string* value) {
	// text input field with data from std::string while applying naming rules for path name parts
	ImGuiInputTextFlags flags =
		ImGuiInputTextFlags_NoUndoRedo |
		ImGuiInputTextFlags_CallbackResize |
		ImGuiInputTextFlags_CallbackCharFilter;

	return ImGui::InputText(label, value->data(), value->capacity() + 1, flags, [](ImGuiInputTextCallbackData* data) {
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			std::string* value = static_cast<std::string*>(data->UserData);
			value->resize(data->BufTextLen);
			data->Buf = (char*) value->c_str();

		} else if (data->EventFlag == ImGuiInputTextFlags_CallbackCharFilter) {
			// MacOS illegal: /:
			// Linux illegal: /
			// Windows illegal: <>:\"/\\|?*
			// web undesirable: %&#+={}
			static const std::string illegalChars = "<>:\"/\\|?*%&#+={}";

			if (illegalChars.find(static_cast<char>(data->EventChar)) != std::string::npos) {
				return 1;
			}
		}

		return 0;
	}, value);
}


//
//	FileSelector::permissionsButton
//

void FileSelector::permissionsButton(const std::string& label, std::filesystem::perms mask) {
	// toggle entries (based on mask) for subset of permissions
	if (ImGui::Button(label.c_str())) {
		if ((newPermissions & mask) == mask) {
			newPermissions &= ~mask;

		} else {
			newPermissions |= mask;
		}
	}
}


//
//	FileSelector::permissionCheckBox
//

void FileSelector::permissionCheckBox(const std::string& label, std::filesystem::perms mask) {
	// toggle a single permission based on mask
	bool value = (newPermissions & mask) != std::filesystem::perms::none;

	if (ImGui::Checkbox(label.c_str(), &value)) {
		if (value) {
			newPermissions |= mask;

		} else {
			newPermissions &= ~mask;
		}
	}
}


//
//	FileSelector::Listing::load
//

bool FileSelector::Listing::load(const std::filesystem::path& path, const State& state, const Labels& labels) {
	// we load to a temporary list first so we can detect errors
	std::vector<Entry> entries;
	bool success = true;

	// get system locale
	const std::locale locale("");

	// get the facets for wide characters (wstring)
	auto& ctypeFacet = std::use_facet<std::ctype<wchar_t>>(locale);
	auto& collateFacet = std::use_facet<std::collate<wchar_t>>(locale);

	try {
		for (const auto& node : std::filesystem::directory_iterator(path)) {
			if (node.is_regular_file() || (node.is_directory() && isAccessible(node.path()))) {
				// create a new entry
				auto& entry = entries.emplace_back();

				// get entry metadata and set state
				entry.path = node.path();
				entry.isDirectory = node.is_directory();
				entry.size = entry.isDirectory ? 0 : node.file_size();
				entry.extension = entry.isDirectory ? "" : pathToString(entry.path.extension());
				entry.lastUpdate = node.last_write_time();
				entry.isHidden = isHidden(entry.path);
				entry.isSelected = false;

				// precalculate strings for faster rendering
				entry.nameString = pathToString(entry.path.filename());
				entry.typeString = entry.isDirectory ? labels.directory : entry.extension;
				entry.sizeString = entry.isDirectory ? "    ---" : entry.readableSize(labels);
				entry.dateString = entry.readableDate(labels);

				// precalculate sort string
				auto sortString = entry.path.filename().generic_wstring();
				ctypeFacet.tolower(sortString.data(), sortString.data() + sortString.size());
				entry.sortString = collateFacet.transform(sortString.data(), sortString.data() + sortString.size());
			}
		}

		clear();

		for (auto& entry : entries) {
			emplace_back(entry);
		}

		sort(state);
		currentPath = path;
		lastWriteTime = std::filesystem::last_write_time(path);

	} catch (const std::filesystem::filesystem_error&) {
		success = false;
	}

	return success;
}


//
//	FileSelector::Listing::forEach
//

void FileSelector::Listing::forEach(const State& state, std::function<void(Entry&)> callback) {
	// iterate through all entries in listing by calling function for each
	for (auto& entry : *this) {
		if (filter(state, entry)) {
			callback(entry);
		}
	}
}


//
//	FileSelector::Listing::clearSelections
//

void FileSelector::Listing::clearSelections() {
	// clear all entry selections
	for (auto& entry : *this) {
		entry.isSelected = false;
	}
}


//
//	FileSelector::Listing::sort
//

void FileSelector::Listing::sort(const State& state) {
	// sort current entries
	std::sort(begin(), end(), [state](const Entry& left, const Entry& right) {

		if (state.sortColumn == SortColumn::name) {
			return (state.sortOrder == SortOrder::ascending)
				? left.sortString < right.sortString
				: left.sortString > right.sortString;

		} else if (state.sortColumn == SortColumn::type) {
			return (state.sortOrder == SortOrder::ascending)
				? left.typeString < right.typeString
				: left.typeString > right.typeString;

		} else if (state.sortColumn == SortColumn::date) {
			return (state.sortOrder == SortOrder::ascending)
				? left.lastUpdate < right.lastUpdate
				: left.lastUpdate > right.lastUpdate;

		} else if (state.sortColumn == SortColumn::size) {
			return (state.sortOrder == SortOrder::ascending)
				? left.size < right.size
				: left.size > right.size;
		}

		return false;
	});
}


//
//	FileSelector::Listing::filter
//

bool FileSelector::Listing::filter(const State& state, const Entry& entry) {
	// filter entry by visibility
	if (!state.showHidden && entry.isHidden) {
		return false;
	}

	// filter entry by extension
	if (!entry.isDirectory && state.extensionFilter.size()) {
		if (std::find(state.extensionFilter.begin(), state.extensionFilter.end(), entry.extension) == state.extensionFilter.end()) {
			return false;
		}
	}

	// filter entry by user request
	if (state.userFilter.IsActive()) {
		if (!state.userFilter.PassFilter(entry.nameString.c_str())) {
			return false;
		}
	}

	return true;
}


//
//	FileSelector::Entry::readableSize
//

std::string FileSelector::Entry::readableSize(const Labels& labels) {
	// turn size into user readable string
	size_t i = 0;
	double mantissa = static_cast<double>(size);

	while (mantissa >= 1024.0) {
		mantissa /= 1024;
		i++;
	}

	mantissa = std::ceil(mantissa * 10.0) / 10.0;
	std::stringstream ss;
	ss << std::fixed << std::setw(5) << std::setprecision(1) << std::setfill(' ') << mantissa;

	switch (i) {
		case 0: ss << labels.bytes; break;
		case 1: ss << labels.kiloBytes; break;
		case 2: ss << labels.megaBytes; break;
		case 3: ss << labels.gigaBytes; break;
		case 4: ss << labels.terraBytes; break;
		case 5: ss << labels.petaBytes; break;
		case 6: ss << labels.exoBytes; break;
		default: ss << "??"; break;
	}

	return ss.str();
}


//
//	FileSelector::Entry::readableDate
//

std::string FileSelector::Entry::readableDate(const Labels& labels) {
	// turn date into user readable string
	const auto wallNow = std::chrono::system_clock::now();
	const auto fileNow = std::filesystem::file_time_type::clock::now();

	const auto duration = lastUpdate - fileNow;
	const auto sysTime = wallNow + std::chrono::duration_cast<std::chrono::system_clock::duration>(duration);

	const auto tt = std::chrono::system_clock::to_time_t(sysTime);
	std::tm localTime;

#ifdef _WIN32
	localtime_s(&localTime, &tt);

#else
	localtime_r(&tt, &localTime);
#endif

	std::stringstream ss;
	ss << std::put_time(&localTime, labels.timeFormat.c_str());
	return ss.str();
}


//
//	FileSelector::selectEntry
//

void FileSelector::selectEntry(Entry& entry) {
	if (!entry.isSelected) {
		if (mode == Mode::selectFiles) {
			selectedPaths.emplace_back(entry.path);

		} else {
			listing.clearSelections();
			selectedPath = entry.path;
		}

		entry.isSelected = true;
	}
}


//
//	FileSelector::unSelectEntry
//

void FileSelector::unSelectEntry(Entry& entry) {
	if (entry.isSelected) {
		if (mode == Mode::selectFiles) {
			selectedPaths.erase(
				std::remove(
					selectedPaths.begin(),
					selectedPaths.end(),
					entry.path),
				selectedPaths.end());

		} else {
			selectedPath.clear();
		}

		entry.isSelected = false;
	}
}


//
//	FileSelector::toggleEntrySelection
//

void FileSelector::toggleEntrySelection(Entry& entry) {
	if (entry.isSelected) {
		unSelectEntry(entry);

	} else {
		selectEntry(entry);
	}
}


//
//	FileSelector::getDuplicatePathName
//

std::filesystem::path FileSelector::getDuplicatePathName(const std::filesystem::path& path) {
	// get path parts
	const auto parent = path.parent_path();
	const auto extension = path.extension().string();
	auto stem = path.stem().string();

	// adjust stem if it's already a copy
	std::regex regex(" " + labels.copy + "( [0-9]+)?$");
	std::smatch match;

	if (std::regex_search(stem, match, regex)) {
		stem = stem.substr(0, stem.size() - match.length());
	}

	// try the first macOS duplicate variant: "filename copy.ext"
	auto candidate = parent / (stem + " " + labels.copy + extension);
	auto done = false;
	auto counter = 2;

	if (!std::filesystem::exists(candidate)) {
		done = true;
	}

	// if "filename copy.ext" also exists, start incrementing: "filename copy 2.ext", etc.
	while (!done) {
		candidate = parent / (stem + " " + labels.copy + " " + std::to_string(counter) + extension);

		if (std::filesystem::exists(candidate)) {
			counter++;

		} else {
			done = true;
		}
	}

	// return path name for duplicate
	return candidate;
}
