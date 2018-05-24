//	Another Web-Enabled Service-Oriented Multitouch Environment (AWESOME)
//	Copyright (C) 2010-2018 Johan A. Goossens
//
//	AWESOME is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	AWESOME is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with AWESOME. If not, see <http:#www.gnu.org/licenses/>.


#pragma once


//
//  OtCollection
//

class OtCollection : public OtObject
{
public:
	OtCollection() {}
};


//
//  OtArray
//

class OtArray : public OtCollection, public std::vector<OtValue>
{
public:
	OtArray() {}

	operator std::string()
	{
		// convert array to string
		std::string result("[");
		bool first = true;

		for (auto const& entry : *this)
		{
			if (first)
				first = false;

			else
				result += ",";

			result += entry->repr();
		}

		result += "]";
		return result;
	}

	OtValue init(size_t count, OtValue* parameters)
	{
		// clear array and add all calling parameters
		clear();

		for (size_t c = 0; c < count; c++)
			push_back(parameters[c]);

		return getSharedPtr();
	}

	OtValue index(size_t index) { return OtArrayReferenceCreate(getSharedPtr(), index); }

	size_t mySize()
	{
		return size();
	}

	OtValue add(OtValue value)
	{
		std::shared_ptr<OtArray> result = OtTypeCast<OtArray>(clone());
		result->append(value);
		return result;
	}

	OtValue clone()
	{
		std::shared_ptr<OtArray> result = OtTypeCast<OtArray>(OtArrayCreate());
		for (auto& it : *this) result->push_back(it);
		return result;
	}

	OtValue append(OtValue value)
	{
		push_back(value);
		return getSharedPtr();
	}

	OtValue insert(size_t index, OtValue value)
	{
		std::vector<OtValue>::insert(begin() + index, value);
		return getSharedPtr();
	}

	OtValue erase(size_t index)
	{
		std::vector<OtValue>::erase(begin() + index);
		return getSharedPtr();
	}

	OtValue eraseMultiple(size_t index1, size_t index2)
	{
		std::vector<OtValue>::erase(begin() + index1, begin() + index2);
		return getSharedPtr();
	}

	OtValue push(OtValue value)
	{
		push_back(value); return value;
		return getSharedPtr();
	}

	OtValue pop()
	{
		OtValue value = back();
		pop_back();
		return value;
	}
};


//
//  OtDict
//

class OtDict : public OtCollection, public std::map<std::string, OtValue>
{
public:
	OtDict() {}

	operator std::string()
	{
		// convert dictionary to string
		std::string result("{");
		bool first = true;

		for (auto const& entry : *this)
		{
			if (first)
				first = false;

			else
				result += ",";

			result += entry.first + ":" + entry.second->repr();
		}

		result += "}";
		return result;
	}

	OtValue init(size_t count, OtValue* parameters)
	{
		// clear array and add all calling parameters
		clear();

		for (size_t c = 0; c < count; c += 2)
			insert(std::make_pair((std::string) *parameters[c], parameters[c + 1]));

		return getSharedPtr();
	}

	OtValue index(const std::string& index) { return OtDictReferenceCreate(getSharedPtr(), index); }

	size_t mySize()
	{
		return size();
	}

	OtValue clone()
	{
		std::shared_ptr<OtDict> result = OtTypeCast<OtDict>(OtDictCreate());
		for (auto& it : *this) result->insert(std::make_pair(it.first, it.second));
		return result;
	}

	OtValue eraseEntry(const std::string& name)
	{
		OtValue value = operator[] (name);
		erase(name);
		return value;
	}

	OtValue keys()
	{
		// create array of all keys in context
		std::shared_ptr<OtArray> array = OtTypeCast<OtArray>(OtArrayCreate());

		for (auto const& entry : *this)
			array->push_back(OtValueCreate(entry.first));

		return array;
	}

	OtValue values()
	{
		// create array of all values in context
		std::shared_ptr<OtArray> array = OtTypeCast<OtArray>(OtArrayCreate());

		for (auto const& entry : *this)
			array->push_back(entry.second);

		return array;
	}
};
