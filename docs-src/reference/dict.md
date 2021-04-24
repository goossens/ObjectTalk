Dictionaries contain indexed key/value pairs that are constructed using
curly brackets ({}) or through the Dict class constructor. The Dict class
also has many member functions to manipulate the content of a dictionary.

	var dict1 = [ "First Name": "John", "Last Name": "Doe", "Age": 34 };
	var dict2 = Dict("Name", "John Doe", "Address", "Unknown");

	dict1["Last Update"] = "1 Apr 2000";
	var name = dict2["Name"];

**Member Functions**

| Function | Description |
| ------ | ----------- |
| \_\_init__(...) | Construct a dictionary. This member function is called when you use the **execution** [()] operator on the Dict class. Every two parameters will become a dictionary entry. The first is the index and the second is the value. You typically don't call this member function directly. |
| \_\_index__(string) | Return a [DictReference](DictReference.md) object to address individual objects in a dictionary. This member function is called when you use the **index** ([]) operator. You typically don't call this member function directly. |
| \_\_add__(dict) | Concatenates objects from the dictionary to this object's value and return a new dictionary. This member function is called when you use the **addition** (+) operator. In case there are duplicates, the entries in the operand will win. You typically don't call this member function directly. |
| \_\_contains__(string) | Determine if named object is member of dictionary. This member function is called when you use the **(not) in** operator. You typically don't call this member function directly. |
| size() | Return number of objects in the dictionary. |
| contains(key) | See if we have named object in dictionary. |
| clone() | Return a clone of the dictionary. |
| clear() | Clear (empty) the dictionary. |
| erase(string) | Remove the named object from the dictionary. |
| keys() | Return an array with all the dictionary's keys. |
| values() | Return an array with all the dictionary's values. |

**Overrides**

The Dict class overrides the following member functions from the Object class:

* string
