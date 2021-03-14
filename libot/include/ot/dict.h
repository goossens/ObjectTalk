//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <unordered_map>

#include "collection.h"


//
//	OtDict
//

class OtDictClass;
typedef std::shared_ptr<OtDictClass> OtDict;

class OtDictClass : public OtCollectionClass, public std::unordered_map<std::string, OtObject> {
public:
	// convert dictionary to string
	operator std::string();

	// initializer
	OtObject init(OtContext, size_t count, OtObject* parameters);

	// support indexing
	OtObject index(const std::string& index);

	// get dictionary size
	size_t mySize();

	// return dictionary clone
	OtObject clone();

	// remove dictionary entry
	OtObject eraseEntry(const std::string& name);

	// get array of dictionary names
	OtObject keys();

	// get array of dictionary values
	OtObject values();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtDict create();
	static OtDict create(size_t count, OtObject* values);
};
