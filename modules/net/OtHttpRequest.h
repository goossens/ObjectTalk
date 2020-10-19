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
//	OtHttpRequest
//

class OtHttpRequestClass;
typedef std::shared_ptr<OtHttpRequestClass> OtHttpRequest;


//
//	OtHttpRequestClass
//

class OtHttpRequestClass : public OtInternalClass {
public:
	// get request parts
	const std::wstring getMethod() { return OtTextToWide(req->method); }
	const std::wstring getPath() { return OtTextToWide(req->path); }
	const std::wstring getBody() { return OtTextToWide(req->body); }

	const bool hasHeader(const std::wstring& header) {
		return req->has_header(OtTextToNarrow(header).c_str());
 	}

	const std::wstring getHeader(const std::wstring& header) {
		return OtTextToWide(req->get_header_value(OtTextToNarrow(header).c_str()));
	}

	const bool hasParam(const std::wstring& param) {
		return req->has_param(OtTextToNarrow(param).c_str());
 	}

	const std::wstring getParam(const std::wstring& param) {
		return OtTextToWide(req->get_param_value(OtTextToNarrow(param).c_str()));
	}

	const bool isMultipart() {
		return req->is_multipart_form_data();
 	}

	const bool hasFile(const std::wstring& file) {
		return req->has_file(OtTextToNarrow(file).c_str());
 	}

	const OtObject getFile(const std::wstring& file) {
		httplib::MultipartFormData data = req->get_file_value(OtTextToNarrow(file).c_str());
		OtDict dict = OtDictClass::create();
		dict->operator[] (L"name") = OtStringClass::create(OtTextToWide(data.name));
		dict->operator[] (L"content") = OtStringClass::create(OtTextToWide(data.content));
		dict->operator[] (L"filename") = OtStringClass::create(OtTextToWide(data.filename));
		dict->operator[] (L"type") = OtStringClass::create(OtTextToWide(data.content_type));
		return dict;
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHttpRequestClass>(L"HttpRequest", OtInternalClass::getMeta());

			type->set(L"method", OtFunctionCreate(&OtHttpRequestClass::getMethod));
			type->set(L"path", OtFunctionCreate(&OtHttpRequestClass::getPath));
			type->set(L"body", OtFunctionCreate(&OtHttpRequestClass::getBody));

			type->set(L"hasHeader", OtFunctionCreate(&OtHttpRequestClass::hasHeader));
			type->set(L"getHeader", OtFunctionCreate(&OtHttpRequestClass::getHeader));

			type->set(L"hasParam", OtFunctionCreate(&OtHttpRequestClass::hasParam));
			type->set(L"getParam", OtFunctionCreate(&OtHttpRequestClass::getParam));

			type->set(L"isMultipart", OtFunctionCreate(&OtHttpRequestClass::isMultipart));
			type->set(L"hasFile", OtFunctionCreate(&OtHttpRequestClass::hasFile));
			type->set(L"getFile", OtFunctionCreate(&OtHttpRequestClass::getFile));
		}

		return type;
	}

	// create a new object
	static OtHttpRequest create(const httplib::Request* r) {
		OtHttpRequest request = std::make_shared<OtHttpRequestClass>();
		request->setType(getMeta());
		request->req = r;
		return request;
	}

private:
	const httplib::Request* req;
};
