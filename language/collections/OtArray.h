//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
using OtArray = OtObjectPointer<OtArrayClass>;

class OtArrayClass : public OtCollectionClass {
public:
	// conversion operators
	inline operator bool() override { return false; }
	inline operator int() override { return 0; }
	inline operator int64_t() override { return 0; }
	inline operator size_t() override { return 0; }
	inline operator float() override { return 0.0f; }
	inline operator double() override { return 0.0; }
	operator std::string() override;

	// debugging support
	inline std::string describe() override { return std::to_string(array.size()) + " entries"; }

	// clear array and add all parameters
	void init(size_t count, OtObject* parameters);

	// comparison
	bool operator==(OtObject operand) override;

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
	inline size_t size() { return array.size(); }

	// find an array entry and return index (-1 if not found)
	int64_t find(OtObject object);

	// clone an array
	OtObject clone();

	// empty an array
	void clear() { array.clear(); }

	// merge two arrays
	OtObject merge(OtObject object);

	// add new member to the end
	OtObject append(OtObject object);

	// insert object at specified location
	OtObject insert(size_t index, OtObject object);

	// remove member at specified location
	OtObject erase(size_t index);

	// erase member from until specified locations
	OtObject eraseMultiple(size_t index1, size_t index2);

	// sort array in place
	OtObject sort();
	OtObject rsort();
	OtObject csort(OtObject function);

	// push object to end of array
	OtObject push(OtObject object);

	// pop object from and of array
	OtObject pop();

	// fill an array with N copies of provided object
	OtObject fill(size_t count, OtObject object);

	// join array entries into a string with separator
	std::string join(const std::string& separator);

	// get type definition
	static OtType getMeta();

	// get access to raw object array
	std::vector<OtObject>& raw() { return array; }

protected:
	// constructors
	friend class OtObjectPointer<OtArrayClass>;
	OtArrayClass() = default;
	OtArrayClass(size_t count, OtObject* objects);

private:
	// data
	std::vector<OtObject> array;
};
