//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
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


//
//	OtLoop
//

class OtLoopClass;
typedef std::shared_ptr<OtLoopClass> OtLoop;

class OtLoopClass : public OtHttpClass {
public:
	// constructor
	OtLoopClass() {
		uv_loop = uv_default_loop();
	}

	// run loop
	void run() {
		uv_run(uv_loop, UV_RUN_DEFAULT);
	}

	uv_loop_t* getLoop() {
		return uv_loop;
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtLoopClass>("Loop", OtHttpClass::getMeta());

			type->set("run", OtFunctionCreate(&OtLoopClass::run));
		}

		return type;
	}

	// create a new object
	static OtLoop create() {
		OtLoop loop = std::make_shared<OtLoopClass>();
		loop->setType(getMeta());
		return loop;
	}

private:
	uv_loop_t* uv_loop;
};
