//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <set>

#include "collection.h"


//
//	OtSet
//

class OtSetClass;
typedef std::shared_ptr<OtSetClass> OtSet;

class OtSetClass : public OtCollectionClass {
	friend class OtSetIteratorClass;

public:
	// convert set to string
	operator std::string();

	// debugging support
	std::string describe() { return std::to_string(set.size()) + " entries"; }

	// clear array and add all parameters
	OtObject init(size_t count, OtObject* parameters);

	// comparison
	bool operator == (OtObject object);

	// start iterator
	OtObject iterate();

	OtObject add(OtObject object);
	OtObject subtract(OtObject object);

	// return number of set members
	size_t size() { return set.size(); }

	// see if object is in set
	bool contains(OtObject object);

	// clone a set
	OtObject clone();

	// merge two sets
	OtObject merge(OtObject object);

	// empty a set
	void clear() { set.clear(); }

	// insert a new member in the set
	void insert(OtObject object);

	// remove object from set
	void erase(OtObject object);

	// classic set operations
	OtObject intersectWith(OtObject object);
	OtObject diffFrom(OtObject object);
	OtObject unionWith(OtObject object);
	OtObject subtractFrom(OtObject object);

	// get type definition
	static OtType getMeta();

	// create a new set
	static OtSet create();
	static OtSet create(size_t count, OtObject* objects);

private:
	std::set<OtObject> set;
};
