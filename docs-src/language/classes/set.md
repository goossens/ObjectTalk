Sets store distinct values in a collection without
defined order. You can use a set instead of an array when the order of
items isn’t important, or when you need to ensure that an item only
appears once. Sets can be created using the Set class constructor.
The Set class also has many member functions to manipulate the content of
a set or to perform set operations (e.g. union, difference).

	var set = Set(1, 2, 3, 5);
	var set2 = Set(1, 3, 6, 8);

	assert(set.intersection(set2) == Set(1, 3));
	assert(set.difference(set2) == Set(2, 5, 6, 8));
	assert(set.union(set2) == Set(1, 2, 3, 5, 6, 8));
	assert(set.subtract(set2) == Set(2, 5));

**Member Functions**

| Function | Description |
| ------ | ----------- |
| \_\_init__(...) | Construct a Set. This member function is called when you use the **execution** [()] operator on the Set class. Every value passed to this member function becomes a member of the new set. You typically don't call this member function directly. |
| \_\_iter__() | Return a [SetIterator](SetIterator.md) object to iterate through objects in a set. This member function is called when you use the **for in** statement. You typically don't call this member function directly. |
| \_\_add__(object) | Add object to the set and return a new set. This member function is called when you use the **addition** (+) operator. If the object is another set, it will be added as a single member creating a set hierarchy. If you want to perform mathematical function on two sets use the intersection, difference, union and subtract functions. |
| \_\_sub__(object) | Remove object from the set and return a new set. This member function is called when you use the **subtraction** (-) operator. |
| \_\_contains__(object) | Determine if object is a member of the set. This member function is called when you use the **(not) in** operator. You typically don't call this member function directly. |
| size() | Return number of objects in the set. |
| contains(object) | Determine if object is a member of the set. |
| clone() | Return a clone of the set. |
| merge() | Merge two sets and return a new one. |
| clear() | Clear (empty) the set. |
| insert(object) | Insert object into the set. |
| erase(object) | Remove object from the set. |
| intersection(set) | Return a new set with only the values common to both sets. |
| difference(set) | Return a new set with values in either set, but not both. |
| union(set) | Return a new set with all of the values in both sets. |
| subtract(set) | Return a new set with values not in the specified set. |

**Overrides**

The Set class overrides the following member functions from the Object class:

* string
