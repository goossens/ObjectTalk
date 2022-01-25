//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "ot/concurrentqueue.h"
#include "ot/exception.h"
#include "ot/scanner.h"


//
//	Event definition
//

struct OtIdeEvent {
	enum {
		writeToOut,
		writeToErr,
		finishedWithoutErrors,
		finishedWithException,
		finishedWithScannerException
	} type;

	std::string module;
	size_t lineNumber;
	size_t start;
	size_t end;
	std::string shortMessage;
	std::string longMessage;
};


//
//	Thread-safe ide event queue
//

class OtIdeEventQueue : public OtConcurrentQueue<OtIdeEvent> {
public:
	void writeToOutEvent(const std::string& text) {
		OtIdeEvent event;
		event.type = OtIdeEvent::writeToOut;
		event.longMessage = text;
		push(event);
	}

	void writeToErrEvent(const std::string& text) {
		OtIdeEvent event;
		event.type = OtIdeEvent::writeToErr;
		event.longMessage = text;
		push(event);
	}

	void pushFinishedWithoutErrorsEvent() {
		OtIdeEvent event;
		event.type = OtIdeEvent::finishedWithoutErrors;
		push(event);
	}

	void pushFinishedWithExceptionEvent(const OtException& exception) {
		OtIdeEvent event;
		event.type = OtIdeEvent::finishedWithException;
		event.longMessage = exception.what();
		push(event);
	}

	void pushFinishedWithScannerExceptionEvent(const OtScannerException& exception) {
		OtIdeEvent event;
		event.type = OtIdeEvent::finishedWithScannerException;
		event.module = exception.getModule();
		event.lineNumber = exception.getLineNumber();
		event.start = exception.getStart();
		event.end = exception.getEnd();
		event.shortMessage = exception.getShortErrorMessage();
		event.longMessage = exception.what();
		push(event);
	}
};
