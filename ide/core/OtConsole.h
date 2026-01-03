//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <vector>

#include "OtLog.h"


//
//	OtConsole
//

class OtConsole {
public:
	// constructor
	OtConsole();

	// render the console
	void render();

	// clear the console
	void clear();

	// copy console context to clipboard
	void copy();

	// scroll to the bottom
	void scrollToBottom() { scrollBottom = true; }

	// write to console
	void write(const std::string& text);
	void writeError(const std::string& text);
	void writeHelp(const std::string& text);
	void writeInput(const std::string& text);
	void writeSuccess(const std::string& text);
	void writeLog(OtLog::Type type, const std::string& text);

private:
	// screen text
	enum class Type {
		standardOut,
		standardError,
		help,
		input,
		success,

		debug = 10,
		info,
		warning,
		error,
		fatal
	};

	struct Line {
		Line(Type tp, std::string txt) : type(tp), text(txt) {}
		Type type;
		std::string text;
	};

	std::vector<Line> lines;
	std::string buffer;

	// write colored line of text
	void writeColored(Type type, const std::string& text);

	// properties
	bool scrollBottom = false;
};
