//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <unordered_map>

#include "collection.h"


//
//	OtDict
//

class OtDictClass;
typedef std::shared_ptr<OtDictClass> OtDict;

class OtDictClass : public OtCollectionClass {
public:
	// convert dictionary to string
	operator std::string();

	// debugging support
	std::string describe() { return std::to_string(dict.size()) + " entries"; }

	// initializer
	OtObject init(size_t count, OtObject* parameters);

	// access dict members
	OtObject getEntry(const std::string& index);
	OtObject setEntry(const std::string& index, OtObject object);

	// support indexing
	OtObject index(const std::string& index);

	// add two dictionaries
	OtObject add(OtObject value);

	// does dictionary contains specified object
	bool contains(const std::string& name) { return dict.find(name) != dict.end(); }

	// get dictionary size
	size_t size() { return dict.size(); }

	// return dictionary clone
	OtObject clone();

	// remove all dictionary entries
	void clear() { dict.clear(); }

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

private:
	std::unordered_map<std::string, OtObject> dict;
};
