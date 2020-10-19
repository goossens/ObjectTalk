//	ObjectTalk Scripting Language
//	Copyright 1993-2020 Johan A. Goossens
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.


#include <httplib.h>


//
//	OtHTTP
//

class OtHTTPClass;
typedef std::shared_ptr<OtHTTPClass> OtHTTP;


//
//	OtHTTPClass
//

class OtHTTPClass : public OtNetClass {
public:
	// add handlers
	void get(const std::wstring& path, OtObject callback) {
		server.Get(OtTextToNarrow(path).c_str(), [this, callback](const httplib::Request& req, httplib::Response &res) {
			OtHttpRequest request = OtHttpRequestClass::create(&req);
			res.set_content("Hello World!\n", "text/plain");
		});
	}

	// listen for requests
	void listen(const std::wstring& address, long port) {
		server.listen(OtTextToNarrow(address).c_str(), port);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHTTPClass>(L"HTTP", OtHTTPClass::getMeta());

			type->set(L"get", OtFunctionCreate(&OtHTTPClass::get));
			type->set(L"listen", OtFunctionCreate(&OtHTTPClass::listen));
		}

		return type;
	}

	// create a new object
	static OtHTTP create() {
		OtHTTP http = std::make_shared<OtHTTPClass>();
		http->setType(getMeta());
		return http;
	}

private:
	httplib::Server server;
};
