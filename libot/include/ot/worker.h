//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <thread>

#include "concurrentqueue.h"
#include "system.h"
#include "vm.h"


//
//	OtWorker
//

class OtWorkerClass;
typedef std::shared_ptr<OtWorkerClass> OtWorker;

class OtWorkerClass : public OtSystemClass {
public:
	// destructor
	~OtWorkerClass();

	// initializer
	void init(OtObject handler);

	// pass a job to a worker
	void run(OtObject request, OtObject callback);

	// see if results are available
	void update();

	// see if all requests are processed and results returned
	bool done();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtWorker create();

private:
	// the worker code
	OtObject handler;

	// worker request/response information
	class OtWorkerRequest {
	public:
		OtWorkerRequest(OtObject req, OtObject cb, bool d=false) : request(req), callback(cb), done(d) {};
		OtObject request;
		OtObject callback;
		bool done;
	};

	class OtWorkerResponse {
	public:
		OtWorkerResponse(OtObject req, OtObject res, OtObject cb) : request(req), response(res), callback(cb) {};
		OtObject request;
		OtObject response;
		OtObject callback;
	};

	// managing our worker thread
	bool running = false;
	bool inRequest = false;
	std::thread worker;
	OtConcurrentQueue<OtWorkerRequest> requests;
	OtConcurrentQueue<OtWorkerResponse> responses;
};
