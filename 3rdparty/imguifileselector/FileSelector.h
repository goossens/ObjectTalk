//	FileSelector - A file select dialog for Dear ImGui.
//	Copyright (c) 2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_internal.h"


//
//	FileSelector
//
//	This class implements a file selector dialog window for Dear ImGui. The layout
//	for this dialog is inspired by MacOS but the look and feel is pure Dear ImGui.
//	For now, the visualization is limited to the List View.
//

class FileSelector {
public:
	// singleton implementation
	static inline FileSelector& Instance() {
		static FileSelector singleton;
		return singleton;
	}

	// constructor
	FileSelector();

	// view modes
	enum class View {
		icons,
		list,
		columns
	};

	// sort options
	enum class SortColumn {
		name,
		type,
		date,
		size
	};

	enum class SortOrder {
		ascending,
		descending
	};

	//	access options/state
	inline void SetView(View view) { state.view = view; }
	inline View GetView() { return state.view; }
	inline void SetShowSideBar(bool value) { state.showSideBar = value; }
	inline bool GetShowSideBar() const { return state.showSideBar; }
	inline void SetShowHidden(bool value) { state.showHidden = value; }
	inline bool GetShowHidden() const { return state.showHidden; }
	void SetUserFilter(const std::string& filter);
	std::string GetUserFilter() const;
	void SetExtensionFilter(const std::string& filter);
	std::string GetExtensionFilter() const;
	inline void SetSortColumn(SortColumn value) { state.sortColumn = value; }
	inline SortColumn GetSortColumn() const { return state.sortColumn; }
	inline void SetSortOrder(SortOrder value) { state.sortOrder = value; }
	inline SortOrder GetSortOrder() const { return state.sortOrder; }
	inline void SetSideBarWidthInGlyphs(size_t glyphs) { state.sizeBarWidthInGlyphs = glyphs; }
	inline size_t GetSideBarWidthInGlyphs() const { return state.sizeBarWidthInGlyphs; }
	inline void SetIconScale(float value) { state.iconScale = value; }
	inline float GetIconScale() const { return state.iconScale; }

	inline void SetCurrentPath(const std::filesystem::path& path) { nextPath = path; }
	inline const std::filesystem::path& GetCurrentPath() const { return nextPath.empty() ? state.currentPath : nextPath; }

	// start a selector to open a single file
	// returns true if selector is opened and false if a previous selector is still active
	// accessoryView is a callback function to render custom items above the action buttons
	bool OpenFile(std::function<void()> accessoryView={}, float avHeight=0.0f);

	// start a selector to pick a path to save content to
	// returns true if selector is opened and false if a previous selector is still active
	// accessoryView is a callback function to render custom items above the action buttons
	bool SaveAs(const std::filesystem::path& defaultPath={}, std::function<void()> accessoryView={}, float avHeight=0.0f);

	// start a selector to select one or more files
	// returns true if selector is opened and false if a previous selector is still active
	// accessoryView is a callback function to render custom items above the action buttons
	bool SelectFiles(std::function<void()> accessoryView={}, float avHeight=0.0f);

	// start a selector to select a directory
	// returns true if selector is opened and false if a previous selector is still active
	// accessoryView is a callback function to render custom items above the action buttons
	bool SelectDirectory(std::function<void()> accessoryView={}, float avHeight=0.0f);

	// forcefully close the current selector
	// this doesn't do anything if no selector is open
	//
	// it also doesn't need to be called when a selector completes as that is handled internally
	void Close();

	// check currect selector state
	inline bool IsOpen() const { return mode != Mode::idle; }
	inline bool IsOpenFileOpen() const { return mode == Mode::openFile; }
	inline bool IsSaveAsOpen() const { return mode == Mode::saveAs; }
	inline bool IsSelectFilesOpen() const { return mode == Mode::selectFiles; }
	inline bool IsSelectDirectoryOpen() const { return mode == Mode::selectDirectory; }

	// get selection status
	inline bool WasCancelled() const { return action == Action::cancelled; }
	inline bool SelectedOpenFile() const { return action == Action::selectedOpenFile; }
	inline bool SelectedSaveAs() const { return action == Action::selectedSaveAs; }
	inline bool SelectedFiles() const { return action == Action::selectedFiles; }
	inline bool SelectedDirectory() const { return action == Action::selectedDirectory; }

	// access selected path(s)
	inline const std::filesystem::path& GetSelectedPath() const { return selectedPath; }
	inline const std::vector<std::filesystem::path>& GetSelectedPaths() const { return selectedPaths; }

	// this API only works with an active save as dialog
	// and is therefore only useful in an accessory view callback
	void SetSelectedExtension(const std::filesystem::path& extension);

	// render the file selector widget
	// it is safe to call this every frame as it's a NOOP if no selectors are active
	// returns true if user made selection(s) or cancelled (use selection status above)
	// selected path(s) can be accessed through GetSelectedPath
	bool Render();

	// manage sidebar content
	inline void ClearFavorites() { favorites.entries.clear(); }
	inline void AddFavorite(const std::string& name, const std::filesystem::path& path) { favorites.entries.emplace_back(name, path); }
	inline void AddDefaultFavorites() { addDefaultFavorites(); }

	inline void ClearClouds() { clouds.entries.clear(); }
	inline void AddCloud(const std::string& name, const std::filesystem::path& path) { clouds.entries.emplace_back(name, path); }
	inline void AddDefaultClouds() { addDefaultClouds(); }

	inline void ClearLocations() { locations.entries.clear(); }
	inline void AddLocation(const std::string& name, const std::filesystem::path& path) { locations.entries.emplace_back(name, path); }
	inline void AddDefaultLocations() { addDefaultLocations(); }

	inline void ClearMedia() { media.entries.clear(); }
	inline void AddMedia(const std::string& name, const std::filesystem::path& path) { media.entries.emplace_back(name, path); }
	inline void AddDefaultMedia() { addDefaultMedia(); }

	// data for a custom icon renderer
	struct IconData {
		// top left corner of icon
		ImVec2 pos;

		// scale of icon
		// use this if icons are font based (e.g. ImGui::PushFont(iconFont, scale * ImGui::GetStyle().FontSizeBase))
		float scale;

		// size of icon in pixels = GetFontSize() * scale
		ImVec2 size;

		// drawlist to render to
		ImDrawList* drawList;

		// item information
		bool isDirectory;
		std::string extension;
	};

	// provide a custom icon renderer
	// if the custom renderer returns false, the file selector will render the icon using the build-in mechanism
	inline void SetCustomIconRenderer(std::function<bool(const IconData&)> callback) { iconRenderer = callback; }
	inline void ClearCustomIconRenderer() { iconRenderer = nullptr; }
	inline bool HasCustomIconRenderer() const { return iconRenderer != nullptr; }

	// state access (to make it persistent across program runs; app is responsible to save/restore it)
	struct State {
		std::filesystem::path currentPath;
		std::vector<std::filesystem::path> recentPlaces;
		View view = View::list;
		bool showSideBar = true;
		bool showHidden = false;
		std::vector<std::string> extensionFilter;
		ImGuiTextFilter userFilter;
		SortColumn sortColumn = SortColumn::name;
		SortOrder sortOrder = SortOrder::ascending;
		size_t sizeBarWidthInGlyphs = 25;
		float iconScale = 4.0f;
	};

	void SetState(const State& newState);
	inline const State& GetCurrentState() const { return state; }

	// internationalization support (default language is US English)
	struct Labels {
		std::string open;
		std::string save;
		std::string select;
		std::string create;
		std::string rename;
		std::string ok;
		std::string cancel;
		std::string nameColumn;
		std::string typeColumn;
		std::string dateColumn;
		std::string sizeColumn;
		std::string directory;
		std::string filter;
		std::string favorites;
		std::string clouds;
		std::string locations;
		std::string media;
		std::string saveAs;
		std::string showSideBar;
		std::string showHidden;
		std::string showItemsAs;
		std::string sortItemsBy;
		std::string icons;
		std::string list;
		std::string columns;
		std::string newFolder;
		std::string nameOfNewFolder;
		std::string nameTaken;
		std::string permissions;
		std::string owner;
		std::string group;
		std::string others;
		std::string read;
		std::string write;
		std::string exec;
		std::string all;
		std::string recentPlaces;
		std::string confirmationWindow;
		std::string errorWindow;
		std::string fileExists;
		std::string moveToTrash;
		std::string showInFinder;
		std::string duplicate;
		std::string copy;
		std::string timeFormat;
		std::string bytes;
		std::string kiloBytes;
		std::string megaBytes;
		std::string gigaBytes;
		std::string terraBytes;
		std::string petaBytes;
		std::string exoBytes;
	};

	void SetLabels(struct Labels& newLabels);
	inline const Labels& GetLabels() const { return labels; }

	// support functions to convert between std::filesystem::path and std::string in UTF-8 encoding
	static inline std::string pathToString(const std::filesystem::path& path) {
#if (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || (__cplusplus >= 202002L)
	auto u8Str = path.generic_u8string();
	std::string str(u8Str.begin(), u8Str.end());
	return str;

#else
		return path.u8string();
#endif
	}

	static inline std::filesystem::path stringToPath(const std::string& string) {
#if (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || (__cplusplus >= 202002L)
		return std::filesystem::path(
			reinterpret_cast<const char8_t*>(string.data()),
			reinterpret_cast<const char8_t*>(string.data() + string.size()));

#else
		return std::filesystem::u8path(string);
#endif
	}

#if (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || (__cplusplus >= 202002L)
	using CodePoint = const char8_t*;
#else
	using CodePoint = const char*;
#endif

private:
	// configuration
	Labels labels = {
		"Open",
		"Save",
		"Select",
		"Create",
		"Rename",
		"OK",
		"Cancel",
		"Name",
		"Type",
		"Date",
		"Size",
		"[dir]",
		"filter...",
		"Favorites",
		"Clouds",
		"Locations",
		"Media",
		"Save As:",
		"Show Sidebar",
		"Show Hidden",
		"Show items as:",
		"Sort items by:",
		"Icons",
		"List",
		"Columns",
		"New Folder",
		"Name of new folder:",
		"This name is already taken.",
		"Permissions",
		"Owner",
		"Group",
		"Other",
		"Read",
		"Write",
		"Exec",
		"All",
		"Recent Places",
		"Confirmation...",
		"Error...",
		"File Exists, do you want to overwrite it?",
		"Move to Trash",
		"Show in Finder",
		"Duplicate",
		"copy",
		"%b %d, %Y at %I:%M %p",
		"B ",
		"KB",
		"MB",
		"GB",
		"TB",
		"PB",
		"EB"
	};

	// current dialog mode
	enum class Mode {
		idle,
		openFile,
		saveAs,
		selectFiles,
		selectDirectory
	} mode = Mode::idle;

	// last action taken
	enum class Action {
		none,
		cancelled,
		selectedOpenFile,
		selectedSaveAs,
		selectedFiles,
		selectedDirectory
	}  action = Action::none;

	// current state
	State state;
	std::filesystem::path selectedPath;
	std::vector<std::filesystem::path> selectedPaths;
	bool requestOpen = false;
	std::function<void()> accessoryView;
	float accessoryViewHeight=0.0f;
	std::function<bool(const IconData& data)> iconRenderer;

	// directory traversal history
	std::vector<std::filesystem::path> pathHistory;
	size_t historyIndex = 0;

	// a named path
	struct NamedPath {
		NamedPath() = default;
		NamedPath(std::string name, std::filesystem::path path) : name(name), path(path) {}
		std::string name;
		std::filesystem::path path;
	};

	// parts of the current path in reverse order
	std::vector<NamedPath> pathStack;

	// single directory entry
	struct Entry {
		std::filesystem::path path;
		bool isDirectory;
		std::uintmax_t size;
		std::string extension;
		std::filesystem::file_time_type lastUpdate;
		bool isSelected;
		bool isHidden;

		std::string nameString;
		std::string typeString;
		std::string dateString;
		std::string sizeString;
		std::wstring sortString;

		std::string readableSize(const Labels& labels);
		std::string readableDate(const Labels& labels);
	};

	// current directory listing
	class Listing : public std::vector<Entry> {
	public:
		// load a specified path
		bool load(const std::filesystem::path& path, const State& state, const Labels& labels);
		inline bool reload(const State& state, const Labels& labels) { return load(currentPath, state, labels); }

		// iterate through listing
		void forEach(const State& state, std::function<void(Entry&)> callback);

		// clear all selections
		void clearSelections();

		// (re)sort entries
		void sort(const State& state);

		private:
		// properties
		std::filesystem::path currentPath;
		std::filesystem::file_time_type lastWriteTime;

		// support functions
		bool filter(const State& state, const Entry& entry);
	} listing;

	// known user directory types
	enum class KnownDirectory {
		desktop,
		documents,
		downloads,

		movies,
		music,
		pictures
	};

	// sidebar groups
	struct SideBarGroup {
		std::vector<NamedPath> entries;
		bool expanded = true;

		// add new member based on a name and path
		inline void add(const std::string& name, const std::filesystem::path& path) {
			if (isAccessible(path)) {
				entries.emplace_back(name, path);
			}
		}

		// add new member based on a known directory ID
		inline void add(KnownDirectory directory) {
			std::string name;
			std::filesystem::path path;
			getKnownDirectoryInfo(directory, name, path);
			add(name, path);
		}
	};

	SideBarGroup favorites;
	SideBarGroup clouds;
	SideBarGroup locations;
	SideBarGroup media;

	// confirm overwrite popup properties
	bool openOverWrite = false;

	// new folder popup properties
	bool openNewFolder = false;
	std::string newFolderName;
	std::string newFolderError;

	// handle rename popup properties
	bool openRename = false;
	std::string oldMoveName;
	std::string newMoveName;
	std::string moveError;

	// handle permissions popup properties
	bool openPermissions = false;
	std::filesystem::path permissionsPath;
	std::filesystem::perms oldPermissions;
	std::filesystem::perms newPermissions;
	std::string permissionsError;

	// error popup properties
	bool openError = false;
	std::string errorMessage;

	// work variables
	float frameHeight;
	ImVec2 glyphSize;
	ImVec2 itemSpacing;
	float extraSpacing;
	std::string saveAsString;
	std::string filterString;
	std::filesystem::path nextPath;
	bool refreshListing = false;

	// local functions
	bool openDialog(Mode mode, std::function<void()> accessoryView, float avHeight);
	bool setCurrentPath(const std::filesystem::path path, bool addHistory=true);

	void renderFileDialog();
	void renderSideBar();
	void renderSideBarGroup(const std::string& label, SideBarGroup& group);
	void renderHeader();
	void renderIconView(ImVec2 size);
	void renderListView(ImVec2 size);
	void renderColumnsView(ImVec2 size);
	void renderIcon(const IconData& data);
	void renderEntryContextMenu(Entry& entry);
	void renderActionButtons();

	void renderPopups();
	void renderOverWritePopup();
	void renderNewFolderPopup();
	void renderRenamePopup();
	void renderPermissionsPopup();
	void renderErrorPopup();

	void addDefaultFavorites();
	void addDefaultClouds();
	void addDefaultLocations();
	void addDefaultMedia();

	void handleEntrySelection(Entry& entry);
	bool isOkAvailable();
	void handleOk();

	void spacing();
	ImVec2 rightAlign(const std::string& button1, const std::string& button2="");
	bool grouping(const char* label, bool* expanded);
	static bool inputString(const char* label, std::string* value);
	static bool inputStringWithHint(const char* label, const char* hint, std::string* value);
	static bool inputPath(const char* label, std::string* value);
	void permissionsButton(const std::string& label, std::filesystem::perms mask);
	void permissionCheckBox(const std::string& label, std::filesystem::perms mask);

	static void renderFile(ImDrawList* drawList, ImVec2 pos, float scale, const std::string& extension);
	static void renderFolder(ImDrawList* drawList, ImVec2 pos, float scale);
	static void renderIcon(ImDrawList* drawList, ImVec2 pos, float scale, CodePoint codepoint);

	std::filesystem::path getDuplicatePathName(const std::filesystem::path& path);

	// OS-specific local functions
	static std::filesystem::path getHome();
	static bool isHidden(const std::filesystem::path& path);
	static bool isAccessible(const std::filesystem::path& path);
	static void getKnownDirectoryInfo(KnownDirectory type, std::string& label, std::filesystem::path& path);
	static void forEachKnownLocation(std::function<void(const std::string& name, const std::filesystem::path& path)> callback);
	static bool moveToTrashCan(const std::filesystem::path& path, std::string& errorMessage);
	static void showInFinder(const std::filesystem::path& path);
};
