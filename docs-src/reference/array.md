Arrays are sequences of objects that can be constructed using square
brackets ([]) or through the Array class constructor. The Array class has
many member functions to manipulate the content of an array.

    var array1 = [ 1, 2, "test", 7 + 4, a ];
    var array2 = Array(3.14, 7, 34, 1);

	array1.append("new value");
	var thirdValue = array1[3];

	var array3 = [3.14, [3, 5, 4], 34, 1];

**Member Functions**

| Function | Description |
| ------ | ----------- |
| \_\_init__(...) | Construct an array. This member function is called when you use the **execution** [()] operator on the Array class. Every value passed to this function becomes a member of the new array. You typically don't call this member function directly. |
| \_\_index__(integer) | Return an [ArrayReference](ArrayReference.md) object to address individual objects in an array. This member function is called when you use the **index** ([]) operator. You typically don't call this member function directly. |
| \_\_iter__() | Return an [ArrayIterator](ArrayIterator.md) object to iterate through objects in an array. This member function is called when you use the **for in** statement. You typically don't call this member function directly. |
| \_\_add__(object) | Append object to the array and return a new array. This member function is called when you use the **addition** (+) operator. If the object is another array, it will be added as a single member creating an array hierarchy. If you want to join two arrays, use the join member function. |
| \_\_contains__(object) | Determine if object is member of array. |
| size() | Return number of objects in array. |
| find(object) | Find object in array and return its position. If object is not found, -1 is returned. |
| contains(object) | Determine if object is member of array. This member function is called when you use the **(not) in** operator. You typically don't call this member function directly. |
| clone() | Return a clone of the array. |
| merge(array) | Append array to object's value and return a new array. |
| clear() | Clear (empty) the array. |
| append(object) | Append object to array. |
| insert(index, object) | Insert object at specified location. |
| erase(index) | Remove object at specified location. |
| eraseMultiple(from, to) | Remove all objects between specified locations. |
| push(object) | Push object to array. This is the same as append but makes it more logical if you use an array as a stack. |
| pop() | Pop object from the end of the array allowing it to be used as a stack. |

**Overrides**

The Array class overrides the following member functions from the Object class:

* string
