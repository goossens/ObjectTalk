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
#include <string>

#include "OtNodes.h"

#include "OtEditor.h"
#include "OtEditorTask.h"
#include "OtNodesWidget.h"
#include "OtTaskManager.h"


//
//	OtNodesEditor
//

class OtNodesEditor : public OtEditor {
public:
	// file handling functions
	void newFile(const std::string& path) override;
	void openFile(const std::string& path) override;
	void saveFile() override;
	void saveAsFile(const std::string& path) override;
	inline std::string getFileExtension() override { return ".otn"; }
	inline std::string getFilePath() override { return path; }

	// get editor status
	inline bool isDirty() override { return taskManager.isDirty(); }

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
	// the path to the nodes file
	std::string path;

	// the nodes being edited
	OtNodes nodes;

	// the editor's UI
	OtNodesWidget widget;

	// to handle do/undo/redo
	OtTaskManager taskManager;
	std::shared_ptr<OtEditorTask> nextTask = nullptr;

	// to handle cut/copy/paste
	std::string clipboard;
};
