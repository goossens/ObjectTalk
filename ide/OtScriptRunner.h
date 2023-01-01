//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "OtConcurrentQueue.h"
#include "OtException.h"
#include "OtObject.h"
#include "OtSingleton.h"


//
//	OtScriptRunnerClass
//

class OtScriptRunnerClass;
typedef std::shared_ptr<OtScriptRunnerClass> OtScriptRunner;


class OtScriptRunnerClass : public OtSingleton<OtScriptRunnerClass> {
public:
	// constructor
	OtScriptRunnerClass();

	// run the specified script
	void run(const std::string& filename);

	// update the runner's state by processing events
	void update();

	// get status
	bool isRunning() { return runningScript; }
	bool isRunningServer() { return runningServer; }
	bool isRunningGUI() { return runningGUI; }

private:
	// properties
	std::string filename;

	bool runningScript = false;
	bool runningServer = false;
	bool runningGUI = false;
	bool hasException = false;
	OtException exception;

	std::thread thread;
	std::mutex mutex;
	std::condition_variable cv;

	struct Event {
		enum {
			finishedNormally,
			finishedWithException
		} type;

		OtObject module;
		OtException exception;
	};

	class EventQueue : public OtConcurrentQueue<Event> {
	public:
		void pushFinishedNormallyEvent(OtObject module) {
			Event event;
			event.type = Event::finishedNormally;
			event.module = module;
			push(event);
		}

		void pushFinishedWithExceptionEvent(OtObject module, const OtException& exception) {
			Event event;
			event.type = Event::finishedWithException;
			event.module = module;
			event.exception = exception;
			push(event);
		}
	};

	EventQueue events;
};
