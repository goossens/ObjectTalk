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

#include "OtNodesAsset.h"

#include "OtEditor.h"
#include "OtEditorTask.h"
#include "OtNodesWidget.h"
#include "OtTaskManager.h"


//
//	OtNodesEditor
//

class OtNodesEditor : public OtEditor {
public:
	// constructor
	OtNodesEditor();

	// file handling functions
	void newFile(const std::string& path) override;
	void openFile(const std::string& path) override;
	void saveFile() override;
	void saveAsFile(const std::string& path) override;
	inline std::string getFileExtension() override { return ".otn"; }
	inline std::string getFilePath() override { return asset.getPath(); }

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
	// the nodes being edited
	OtAsset<OtNodesAsset> asset;

	// the editor's UI
	std::unique_ptr<OtNodesWidget> widget;

	// to handle do/undo/redo
	OtTaskManager taskManager;
	std::shared_ptr<OtEditorTask> nextTask = nullptr;

	// to handle cut/copy/paste
	std::string clipboard;
};
