//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "collection.h"


//
//	OtArray
//

class OtArrayClass;
typedef std::shared_ptr<OtArrayClass> OtArray;

class OtArrayClass : public OtCollectionClass, public std::vector<OtObject> {
public:
	// convert array to string
	operator std::string();

	// clear array and add all parameters
	OtObject init(OtContext, size_t count, OtObject* parameters);

	// support indexing
	OtObject index(size_t index);

	// start iterator
	OtObject iterate();

	// add two arrays
	OtObject add(OtObject value);

	// does array contains specified object
	OtObject contains(OtObject value);

	// return number of array members
	size_t mySize();

	// find an array entry and return index (-1 if not found)
	long find(OtObject value);

	// clone an array
	OtObject clone();

	// add new member to the end
	OtObject append(OtObject value);

	// insert member at specified location
	OtObject insert(size_t index, OtObject value);

	// remove member at specified location
	OtObject erase(size_t index);

	// erase member from until specified locations
	OtObject eraseMultiple(size_t index1, size_t index2);

	// push object to end of array
	OtObject push(OtObject value);

	// pop object from and of array
	OtObject pop();

	// get type definition
	static OtType getMeta();

	// create a new array
	static OtArray create();
	static OtArray create(size_t count, OtObject* values);
};
