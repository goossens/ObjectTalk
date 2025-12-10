ObjectTalk provides three primary collection types, known as
[Arrays](classes.html#array),
[Dictionaries](classes.html#dict) and
[Sets](classes.html#set). Arrays are ordered collections of
values. Dictionaries are unordered collections of key-value associations.
Sets are unordered collections of unique values.

### Arrays

An array stores values in an ordered list. The same value can appear in
an array multiple times at different positions.

**Creating an Array**

You can create an array using a literal or the Array class constructor:

	var array1 = [];
	var array2 = [1, 2, 3.14, "test"];
	var array3 = Array();
	var array4 = Array(1, 2, 3.14, "test");

**Accessing and Modifying an Array**

You access and modify an array through its member functions, or by using
the index  (**[]**) and addition  (**+**) operators.

	var array = [1, 2, 3.14, "test"];

	array.append("name");
	array.insert(3, "insert");
	array += 9;

	var size = array.size();

	var entry = array[2];
	array[0] = 0;

	array.erase(2);
	array.eraseMultiple(1, 2);
	array.clear();

You can merge arrays:

	var array1 = [1, 2, 3.14, "test"];
	var array2 = [8, 9];

	var array3 = array1.merge(array2);
	// array3 is now [1, 2, 3.14, "test", 8, 9]

You can use an array as a stack:

	var stack = Array();
	stack.push(5);
	var item = stack.pop();

**Iterating over an Array**

You can iterate over the entire set of values in an array with the
**for-in** loop:

	var array = [1, 2, 3.14, "test"];

	for item in array {
		print(item);
	}

**Sorting an Array**

You can sort an array using the **sort**, **rsort** and **csort**
member functions. **sort** puts the array elements in ascending
order, **rsort** is a reverse sort putting the elements in a
descending order and **csort** is a custom sort that allows you to
provide a function to determine the order.

	var array = [1, 2, 3, 4];

	array.rsort();
	// array is now [4, 3, 2, 1]

	array.sort();
	// array is now [1, 2, 3, 4]

	array.csort(function(a, b) {
		return a > b;
	});

	// array is now [4, 3, 2, 1]

### Dictionaries

A dictionary stores associations between string keys and values in a
collection with no defined ordering. Each value is associated with a
unique key, which acts as an identifier for that value within the
dictionary. Unlike items in an array, items in a dictionary don’t have a
specified order. You use a dictionary when you need to look up values
based on their identifier, in much the same way that a real-world
dictionary is used to look up the definition for a particular word.

**Creating a Dictionary**

You can create an Dictionary using a literal or the Dict class
constructor:

	var dict1 = { "name": "Doe", "age": 29 };
	var dict2 = Dict("name", "Smith", "age", 31);

**Accessing and Modifying a Dictionary**

You access and modify a dictionary through its member functions, or by
using index operator (**[]**).

	var dict = Dict();

	dict["name"] = "John";
	dict["age"] = 39;
	dict.erase("age");

	var size = dict.size();

	dict.clear();

**Iterating over a Dictionary**

You can iterate over a dictionary's keys or values.

	var dict = { "name": "Doe", "age": 29 };

	for key in dict.keys() {
		print(key, ": ", dict[key]);
	}

	for value in dict.values() {
		print(value);
	}

**Dictionary Membership**

Dictionary membership can be determined with the **(not) in** operators
or the contains member functions.

var dict = { "name": "Doe", "age": 29 };

	assert("name" in dict);
	assert("address" not in dict);
	assert(dict.contains("age") == true);

### Sets

A set stores distinct values in a collection with no defined ordering.
You can use a set instead of an array when the order of items isn’t
important, or when you need to ensure that an item only appears once.

**Creating a Set**

You can create a set with the Set class constructor.

	var set1 = Set();
	var set2 = Set(1, 5, 9);

**Accessing and Modifying a Set**

You access and modify a set through its member functions, or by using the
addition (**+**) and subtraction (**-**) operators.

	var set = Set();
	set.insert(1);
	set += 2;
	set.insert(4);
	s -= 1;
	set.erase(2);

	var size = set.size();

	set.clear();

**Set Operations**

You can efficiently perform fundamental set operations, such as combining
two sets together, determining which values two sets have in common, or
determining whether two sets contain all, some, or none of the same values.

	var set = Set(1, 2, 3, 5);
	var set2 = Set(1, 3, 6, 8);

	assert(set.intersection(set2) == Set(1, 3));
	assert(set.difference(set2) == Set(2, 5, 6, 8));
	assert(set.union(set2) == Set(1, 2, 3, 5, 6, 8));
	assert(set.subtract(set2) == Set(2, 5));

**Iterating over a Set**

You can iterate over the entire set of values in an array with the
**for-in** loop:

	var set = Set(1, 2, 3.14, "test");

	for item in set {
		print(item);
	}

**Set Membership**

Set membership can be determined with the **(not) in** operators or the
**contains** member functions.

	var set = Set(1, 2, 3.14, "test");

	assert(1 in set);
	assert("hello" not in set);
	assert(set.contains("test") == true);
