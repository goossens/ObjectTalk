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

#include "OtHttp.h"
#include "OtHttpRequest.h"
#include "OtHttpResponse.h"


//
//	OtHttpRouter
//

class OtHttpRouterClass;
typedef std::shared_ptr<OtHttpRouterClass> OtHttpRouter;

class OtHttpRouterClass : public OtHttpClass {
	friend class OtHttpNextClass;
	friend class OtHttpMethodHandler;
	friend class OtStaticHandler;

public:
	// add handlers
	OtObject useHandler(OtObject callback);
	OtObject allHandler(const std::string& path, OtObject callback);
	OtObject getHandler(const std::string& path, OtObject callback);
	OtObject putHandler(const std::string& path, OtObject callback);
	OtObject postHandler(const std::string& path, OtObject callback);
	OtObject deleleteHandler(const std::string& path, OtObject callback);
	OtObject staticFiles(const std::string& serverPath, const std::string& filePath);

	// dispatch requests
	OtObject call(OtObject req, OtObject res, OtObject next);

	// set a timer
	OtObject timer(long wait, long repeat, OtObject callback);

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

		// run as a the handler
		virtual void run(OtHttpRequest req, OtHttpResponse res, OtObject next) {}
	};

	// manage handlers
	OtObject addHandler(const std::string& method, const std::string& path, OtObject callback);
	void runHandler(const size_t index, OtHttpRequest req, OtHttpResponse res, OtObject next);

	std::vector<std::shared_ptr<OtHandler>> handlers;
};
