//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <vector>


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

	// scroll to the bottom
	void scrollToBottom() { scrollBottom = true; }

	// write to console
	void write(const std::string& text);
	void writeHelp(const std::string& text);
	void writeInput(const std::string& text);
	void writeError(const std::string& text);

private:
	// screen text
	enum LineType {
		Normal,
		Help,
		Input,
		Error
	};

	struct Line {
		Line(LineType tp, std::string txt) : type(tp), text(txt) {}
		LineType type;
		std::string text;
	};

	std::vector<Line> lines;
	std::string buffer;

	// write colored line of text
	void writeColored(LineType type, const std::string& text);

	// properties
	bool scrollBottom = false;
};
