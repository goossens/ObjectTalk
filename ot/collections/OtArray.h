//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <vector>

#include "OtCollection.h"


//
//	OtArray
//

class OtArrayClass;
typedef std::shared_ptr<OtArrayClass> OtArray;

class OtArrayClass : public OtCollectionClass {
public:
	// convert array to string
	operator std::string() override;

	// debugging support
	std::string describe() override { return std::to_string(array.size()) + " entries"; }

	// clear array and add all parameters
	OtObject init(size_t count, OtObject* parameters);

	// comparison
	bool operator == (OtObject operand) override;

	// access array members
	OtObject getEntry(size_t index);
	OtObject setEntry(size_t index, OtObject object);

	// support indexing
	OtObject index(size_t index);

	// start iterator
	OtObject iterate();

	// add object to array and return new array
	OtObject add(OtObject object);

	// does array contain a specified object
	bool contains(OtObject object);

	// return number of array members
	size_t size() { return array.size(); }

	// find an array entry and return index (-1 if not found)
	long find(OtObject object);

	// clone an array
	OtObject clone();

	// empty an array
	void clear() { array.clear(); }

	// merge two arrays
	OtObject merge(OtObject object);

	// add new member to the end
	void append(OtObject object);

	// insert object at specified location
	void insert(size_t index, OtObject object);

	// remove member at specified location
	void erase(size_t index);

	// erase member from until specified locations
	void eraseMultiple(size_t index1, size_t index2);

	// sort array in place
	void sort();
	void rsort();
	void csort(OtObject function);

	// push object to end of array
	void push(OtObject object);

	// pop object from and of array
	OtObject pop();

	// fill an array with N copies of provided object
	void fill(size_t count, OtObject object);

	// join array entries into a string with separator
	std::string join(const std::string& separator);

	// get type definition
	static OtType getMeta();

	// create a new array
	static OtArray create();
	static OtArray create(size_t count, OtObject* objects);

	// get access to raw object array
	std::vector<OtObject>& raw() { return array; }

private:
	std::vector<OtObject> array;
};
