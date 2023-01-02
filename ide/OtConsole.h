//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

/*
#include "OtCaptureStdio.h"
*/

#include "OtConcurrentQueue.h"
#include "OtSingleton.h"


//
//	OtConsoleClass
//

class OtConsoleClass;
typedef std::shared_ptr<OtConsoleClass> OtConsole;

class OtConsoleClass : public OtSingleton<OtConsoleClass> {
public:
	// constructor
	OtConsoleClass();

	// update the console
	void update();

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

	// create a new object
	static OtConsole create();

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

/*
	// capture stdin and stderr
	OtCaptureStdout stdout{std::function([this](const char* text, size_t size) {
		write(std::string(text, size));
	})};

	OtCaptureStdout stderr{std::function([this](const char* text, size_t size) {
		writeError(std::string(text, size));
	})};
*/

	// concurrent queue to capture events form other threads
	struct Event {
		enum {
			stdoutEvent,
			stderrEvent
		} type;

		std::string text;
	};

	class EventQueue : public OtConcurrentQueue<Event> {
	public:
		void pushStdoutEvent(const std::string& text) {
			Event event;
			event.type = Event::stdoutEvent;
			event.text = text;
			push(event);
		}

		void pushStderrEvent(const std::string& text) {
			Event event;
			event.type = Event::stderrEvent;
			event.text = text;
			push(event);
		}
	};

	EventQueue events;
};
