//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>
#include <vector>

#include "OtHttpRouter.h"
#include "OtHttpRequest.h"
#include "OtHttpResponse.h"
#include "OtHttpSession.h"
#include "OtLibuv.h"


//
//	OtHttpServerClass
//

class OtHttpServerClass;
using OtHttpServer = OtObjectPointer<OtHttpServerClass>;

class OtHttpServerClass : public OtHttpClass {
public:
	// constructor/destructor
	OtHttpServerClass();
	~OtHttpServerClass();

	// initialize server
	void init(OtObject object);

	// handle connection requests
	void onConnect();

	// listen for requests on specified IP address and port
	OtObject listen(const std::string& ip, int port);

	// cleaup connections
	void cleanup();

	// get type definition
	static OtType getMeta();

private:
	uv_tcp_t uv_server;
	uv_timer_t uv_watchdog;

	OtHttpRouter router;
	std::vector<OtHttpSession> sessions;
};
