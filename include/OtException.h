//	ObjectTalk Scripting Language
//	Copyright 1993-2019 Johan A. Goossens
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
//	OtException
//

class OtException
{
public:
	OtException(std::wstring m) { message = m; }
	virtual const wchar_t* what() const throw() { return message.c_str(); }

private:
	std::wstring message;
};

#define OT_EXCEPT(format, ...) throw OtException(OtFormat("%s: line %d: " format, __FILE__, __LINE__, __VA_ARGS__))
