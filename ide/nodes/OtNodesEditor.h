//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

#include "OtAlignNodesTask.h"
#include "OtEditor.h"
#include "OtEditorTask.h"
#include "OtNodesWidget.h"
#include "OtTaskManager.h"


//
//	OtNodesEditor
//

class OtNodesEditor : public OtEditor {
public:
	// get editor status
	inline bool isDirty() override { return taskManager.isDirty(); }
	inline std::string getExtension() override { return extension; }
	static constexpr const char* extension = ".otn";

	// render the editor
	void renderMenu(bool canRun) override;
	void renderEditor() override;

	// clipboard operations
	void cutSelectedNodes();
	void copySelectedNodes();
	void pasteSelectedNodes();
	void deleteSelectedNodes();
	void duplicateSelectedNodes();

	// alignment operations
	void alignSelectedNodes(OtAlignNodesTask::Alignment alignment);

private:
	// file access functions
	void clear() override;
	void load() override;
	void save() override;

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
