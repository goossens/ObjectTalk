//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <unordered_map>

#include "OtCollection.h"
#include "OtIdentifier.h"


//
//	OtDict
//

class OtDictClass;
using OtDict = OtObjectPointer<OtDictClass>;

class OtDictClass : public OtCollectionClass {
public:
	// constructors
	OtDictClass() = default;
	OtDictClass(size_t count, OtObject* objects);

	// convert dictionary to string
	operator std::string() override;

	// debugging support
	inline std::string describe() override { return std::to_string(dict.size()) + " entries"; }

	// initializer
	void init(size_t count, OtObject* parameters);

	// comparison
	bool operator==(OtObject operand) override;

	// access dict members
	OtObject set(OtID id, OtObject value) override;
	OtObject get(OtID id) override;

	OtObject setEntry(const std::string& index, OtObject object);
	OtObject getEntry(const std::string& index);

	// support indexing
	OtObject index(const std::string& index);

	// add two dictionaries
	OtObject add(OtObject value);

	// does dictionary contains specified object
	inline bool contains(const std::string& name) { return dict.find(name) != dict.end(); }

	// get dictionary size
	inline size_t size() { return dict.size(); }

	// return dictionary clone
	OtObject clone();

	// merge two dictionaries
	OtObject merge(OtObject object);

	// remove all dictionary entries
	inline void clear() { dict.clear(); }

	// remove dictionary entry
	void eraseEntry(const std::string& name);

	// get array of dictionary names
	OtObject keys();

	// get array of dictionary values
	OtObject values();

	// get type definition
	static OtType getMeta();

	// get access to raw object dictionary
	std::unordered_map<std::string, OtObject>& raw() { return dict; }

private:
	std::unordered_map<std::string, OtObject> dict;
};
