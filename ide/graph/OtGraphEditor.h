//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <vector>

#include "imgui.h"

#include "OtGraphAsset.h"

#include "OtEditor.h"
#include "OtEditorTask.h"
#include "OtGraphWidget.h"
#include "OtTaskManager.h"


//
//	OtGraphEditor
//

class OtGraphEditor : public OtEditor {
public:
	// constructor
	OtGraphEditor();

	// file handling functions
	void newFile(const std::string& path) override;
	void openFile(const std::string& path) override;
	void saveFile() override;
	void saveAsFile(const std::string& path) override;
	inline std::string getFileExtension() override { return ".otg"; }
	inline std::string getFilePath() override { return asset->getPath(); }

	// get editor status
	inline bool isReady() override { return asset.isReady(); }
	bool isDirty() override;

	// render the editor
	void renderMenu() override;
	void renderEditor() override;

	// clipboard operations
	void cutSelectedNodes();
	void copySelectedNodes();
	void pasteSelectedNodes();
	void deleteSelectedNodes();
	void duplicateSelectedNodes();

private:
	// the graph being edited
	OtAsset<OtGraphAsset> asset;

	// the editor's UI
	std::unique_ptr<OtGraphWidget> widget;

	// to handle do/undo/redo
	OtTaskManager taskManager;
	std::shared_ptr<OtEditorTask> nextTask = nullptr;

	// to handle cut/copy/paste
	std::string clipboard;
};
