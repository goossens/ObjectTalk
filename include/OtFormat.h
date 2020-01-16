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
//	OtFormat
//

template <typename... Args>
std::wstring OtFormat(const std::wstring &format, Args... args)
{
	unsigned required = std::swprintf(nullptr, 0, format.c_str(), args...) + 1;
	wchar_t bytes[required];
	std::swprintf(bytes, required, format.c_str(), args...);
	return std::wstring(bytes);
}
