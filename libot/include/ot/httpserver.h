//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>
#include <vector>

#include "ot/httprouter.h"
#include "ot/httprequest.h"
#include "ot/httpresponse.h"
#include "ot/httpsession.h"
#include "ot/libuv.h"


//
//	OtHttpServerClass
//

class OtHttpServerClass;
typedef std::shared_ptr<OtHttpServerClass> OtHttpServer;

class OtHttpServerClass : public OtHttpRouterClass {
public:
	// handle connection requests
	void onConnect();

	// listen for requests on specified IP address and port
	OtObject listen(const std::string& ip, long port);

	// run server
	void run();

	// set a timer
	OtObject timer(long wait, long repeat, OtObject callback);

	// stop server
	void stop();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtHttpServer create();

private:
	uv_tcp_t uv_server;
	uv_timer_t uv_watchdog;
	uv_timer_t uv_shutdown;

	std::vector<OtHttpSession> sessions;
};
