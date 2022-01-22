//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/callback.h"
#include "ot/exception.h"
#include "ot/function.h"
#include "ot/vm.h"
#include "ot/worker.h"


//
//	OtWorkerClass::~OtWorkerClass
//

OtWorkerClass::~OtWorkerClass() {
	// send a "done" message to our thread and wait until it ends (if required)
	if (running) {
		requests.push(OtWorkerRequest(nullptr, nullptr, true));
		worker.join();
	}
}


//
//	OtWorkerClass::init
//

void OtWorkerClass::init(OtObject object) {
	// sanity check
	OtCallbackValidate(object, 1);
	handler = object;

	// start a thread
	running = true;

	worker = std::thread([this]() {
		try {
			// run until we get a "done" request
			bool done = false;

			while (!done) {
				requests.wait();
				inRequest = true;
				OtWorkerRequest request = requests.pop();

				if (request.done) {
					done = true;

				} else {
					// execute request in module
					auto response = OtVM::instance()->callMemberFunction(handler, "__call__", request.request);

					// return result
					responses.push(OtWorkerResponse(request.request, response, request.callback));
				}

				inRequest = false;
			}

			running = false;

		} catch (const OtException& e) {
			// handle all failures
			std::wcerr << "Error: " << e.what() << std::endl;
			exit(EXIT_FAILURE);
		}
	});
}


//
//	OtWorkerClass::run
//

void OtWorkerClass::run(OtObject request, OtObject callback) {
	// sanity check
	OtCallbackValidate(callback, 2);

	// add request to queue
	requests.push(OtWorkerRequest(request, callback));
}


//
//	OtWorkerClass::update
//

void OtWorkerClass::update() {
	// process responses
	if (!responses.empty()) {
		// get response and execute callback
		auto response = responses.pop();
		OtVM::instance()->callMemberFunction(response.callback, "__call__", response.request, response.response);
	}
}


//
//	OtWorkerClass::done
//

bool OtWorkerClass::done() {
	return requests.size() == 0 && !inRequest && responses.size() == 0;
}


//
//	OtWorkerClass::getMeta
//

OtType OtWorkerClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtWorkerClass>("Worker", OtSystemClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtWorkerClass::init));
		type->set("run", OtFunctionClass::create(&OtWorkerClass::run));
		type->set("update", OtFunctionClass::create(&OtWorkerClass::update));
		type->set("done", OtFunctionClass::create(&OtWorkerClass::done));
	}

	return type;
}


//
//	OtWorkerClass::create
//

OtWorker OtWorkerClass::create() {
	OtWorker worker = std::make_shared<OtWorkerClass>();
	worker->setType(getMeta());
	return worker;
}
