//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <vector>

#include "collection.h"


//
//	OtArray
//

class OtArrayClass;
typedef std::shared_ptr<OtArrayClass> OtArray;

class OtArrayClass : public OtCollectionClass {
public:
	// convert array to string
	operator std::string();

	// debugging support
	std::string describe() { return std::to_string(array.size()) + " entries"; }

	// clear array and add all parameters
	OtObject init(size_t count, OtObject* parameters);

	// comparison
	bool operator ==(OtObject operand);

	// access array members
	OtObject getEntry(size_t index);
	OtObject setEntry(size_t index, OtObject object);

	// support indexing
	OtObject index(size_t index);

	// start iterator
	OtObject iterate();

	// add object to array
	OtObject add(OtObject object);

	// does array contains specified object
	OtObject contains(OtObject object);

	// return number of array members
	size_t size() { return array.size(); }

	// find an array entry and return index (-1 if not found)
	long find(OtObject object);

	// clone an array
	OtObject clone();

	// empty an array
	void clear() { array.clear(); }

	// join two arrays
	OtObject join(OtObject object);

	// add new member to the end
	OtObject append(OtObject object);

	// insert object at specified location
	OtObject insert(size_t index, OtObject object);

	// remove member at specified location
	OtObject erase(size_t index);

	// erase member from until specified locations
	OtObject eraseMultiple(size_t index1, size_t index2);

	// push object to end of array
	OtObject push(OtObject object);

	// pop object from and of array
	OtObject pop();

	// get type definition
	static OtType getMeta();

	// create a new array
	static OtArray create();
	static OtArray create(size_t count, OtObject* values);

private:
	std::vector<OtObject> array;
};
