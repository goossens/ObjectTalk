//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "ot/context.h"

#include "http.h"
#include "request.h"
#include "response.h"


//
//	OtHttpRouter
//

class OtHttpRouterClass;
typedef std::shared_ptr<OtHttpRouterClass> OtHttpRouter;

class OtHttpRouterClass : public OtHttpClass {
	friend class OtHttpNextClass;
	friend class OtMethodHandler;
	friend class OtStaticHandler;

public:
	// add handlers
	OtObject useHandler(OtContext context, size_t count, OtObject* parameters);
	OtObject allHandler(OtContext context, size_t count, OtObject* parameters);
	OtObject getHandler(OtContext context, size_t count, OtObject* parameters);
	OtObject putHandler(OtContext context, size_t count, OtObject* parameters);
	OtObject postHandler(OtContext context, size_t count, OtObject* parameters);
	OtObject deleleteHandler(OtContext context, size_t count, OtObject* parameters);
	OtObject staticFiles(OtContext context, size_t count, OtObject* parameters);
	OtObject call(OtContext context, size_t count, OtObject* p);
	void clearHandlers();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtHttpRouter create();

private:
	class OtHandler {
	public:
		// constructor
		OtHandler() = default;

		// destructor
		virtual ~OtHandler() {}

		// run the handler
		virtual void run(OtHttpRequest req, OtHttpResponse res, OtObject next) {}
	};

	// manage handlers
	OtObject addHandler(OtContext context, const std::string& method, size_t count, OtObject* parameters);
	void runHandler(const size_t index, OtHttpRequest req, OtHttpResponse res, OtObject next);

	std::vector<std::shared_ptr<OtHandler>> handlers;
};
