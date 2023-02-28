//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstddef>
#include <cstring>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "OtException.h"
#include "OtSingleton.h"


//
//	OtSelector
//

class OtSelector : public OtSingleton<OtSelector> {
public:
	// get a new selector
	size_t get(const std::string& text) {
		// calculate selector
		size_t selector = std::hash<std::string>{}(text);

		// ensure we have no collisions
		if (selectors.count(selector) && selectors[selector] != text) {
			OtExcept("Internal error: selector collision between [%s] and [%s]", selectors[selector].c_str(), text.c_str());
		}

		// remember selector and text
		selectors[selector] = text;

		// return selector
		return selector;
	}

	size_t get(const char* text) {
		// calculate selector
		auto sv = std::string_view(text, std::strlen(text));
		size_t selector = std::hash<std::string_view>{}(sv);

		// ensure we have no collisions
		if (selectors.count(selector) && selectors[selector] != sv) {
			OtExcept("Internal error: selector collision between [%s] and [%s]", selectors[selector].c_str(), text);
		}

		// remember selector and text
		selectors[selector] = text;

		// return selector
		return selector;
	}

	// get the string associated with the selector
	const std::string& get(size_t selector) {
		return selectors[selector];
	}

	// see if selector exists
	bool has(size_t selector) {
		return selectors.count(selector) != 0;
	}

	// static shortcuts
	static size_t create(const std::string& text) {
		return instance()->get(text);
	}

	static size_t create(const char* text) {
		return instance()->get(text);
	}
	static std::string name(size_t selector) {
		return instance()->get(selector);
	}

private:
	// list of selectors already created
	std::unordered_map<size_t, std::string> selectors;
};
