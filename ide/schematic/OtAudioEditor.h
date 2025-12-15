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

#include "OtAudio.h"

#include "OtAlignCircuitsTask.h"
#include "OtEditor.h"
#include "OtEditorTask.h"
#include "OtAudioWidget.h"
#include "OtTaskManager.h"


//
//	OtAudioEditor
//

class OtAudioEditor : public OtEditor {
public:
	// get editor status
	inline bool isDirty() override { return taskManager.isDirty(); }
	inline std::string getExtension() override { return extension; }
	static constexpr const char* extension = ".ota";

	// render the editor
	void renderMenus() override;
	void handleShortcuts() override;
	void renderEditor() override;

	// clipboard operations
	void cutSelectedCircuits();
	void copySelectedCircuits();
	void pasteSelectedCircuits();
	void deleteSelectedCircuits();
	void duplicateSelectedCircuits();

	// alignment operations
	void alignSelectedCircuits(OtAlignCircuitsTask::Alignment alignment);

private:
	// file access functions
	void clear() override;
	void load() override;
	void save() override;

	// the audio being edited
	OtAudio audio;

	// the editor's UI
	OtAudioWidget widget;

	// to handle do/undo/redo
	OtTaskManager taskManager;
	std::shared_ptr<OtEditorTask> nextTask = nullptr;

	// to handle cut/copy/paste
	static std::string clipboard;
};
