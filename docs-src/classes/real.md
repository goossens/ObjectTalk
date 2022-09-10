Reals in ObjectTalk are double-precision floating point numbers. On most
systems, Reals are implemented using 8 bytes and have a range of
1.7E +/- 308 (15 digits).

**Member Functions**

| Function | Description |
| ------ | ----------- |
| \_\_gt__(real) | Compares real to the object's value and returns a boolean indicating whether the object's value is greater than the integer or not. This member function is called when you use the **greater than** (>) operator. |
| \_\_lt__(real) | Compares real to the object's value and returns a boolean indicating whether the object's value is less than the integer or not. This member function is called when you use the **less than** (<) operator. |
| \_\_ge__(real) | Compares real to the object's value and returns a boolean indicating whether the object's value is greater or equal than the integer or not. This member function is called when you use the **greater or equal than** (>=) operator. |
| \_\_le__(real) | Compares real to the object's value and returns a boolean indicating whether the object's value is less or equal than the integer or not. This member function is called when you use the **less or equal than** (<=) operator. |
| \_\_add__(real) | Add real to object's value and return result. This member function is called when you use the **add** (+) operator. |
| \_\_sub__(real) | Subtract real from object's value and return result. This member function is called when you use the **subtract** (-) operator. |
| \_\_mul__(real) | Multiple real with object's value and return result. This member function is called when you use the **multiply** (*) operator. |
| \_\_div__(real) | Divide object's value by real and return result. This member function is called when you use the **divide** (/) operator. |
| \_\_mod__(real) | Divide object's value by real and return leftover. This member function is called when you use the **modulo** (%) operator. |
| \_\_pow__(real) | Raise object's value to the power of real and return result. This member function is called when you use the **exponent** (\*\*) operator. |
| \_\_neg__() | Perform a unary negation of object's value and return result. This member function is called when you use the **unary negation** (-) operator. |
| abs() | Return absolute value of object's value.
| sign() | Return sign (-1, 0 or 1) of object's value.
| round() | Returns integer that is nearest to object's value, with halfway cases rounded away from zero.
| ceil() | Rounds object's value upward, returning the smallest integral value that is not less than object's value.
| floor() | Rounds object's value downward, returning the largest integral value that is not greater than object's value.
| trunc() | Rounds object's value toward zero, returning the nearest integral value that is not larger in magnitude than object's value.
| min(real) | Return the minimum of real and the object's value.
| max(real) | Return the maximum of real and the object's value.
| clamp(min, max) | Return clamped value between min and max.
| lerp(min, max) | Return the linear interpolation between min and max based on value (as the ratio).
| sin() | Returns the sine of an angle of object's value in radians.
| cos() | Returns the cosine of an angle of object's value in radians.
| tan() | Returns the tangent of an angle of object's value in radians.
| asin() | Returns the principal value of the arc sine of object's value, expressed in radians.
| acos() | Returns the principal value of the arc cosine of object's value, expressed in radians.
| atan() | Returns the principal value of the arc tangent of object's value, expressed in radians.
| atan2() | Returns the principal value of the arc tangent expressed in radians of value/operand using the signs of arguments to determine the correct quadrant.
| radians() | Returns object's value converted from degrees to radians.
| degrees() | Returns object's value converted from radians to degrees.
| sqrt() | Return the square root of object's value.
| pow(exp) | Return the object's value raised to the power exp.
| log() | Return the natural (base-e) logarithm of object's value.
| exp() | Returns the base-e exponential function of object's value.
| log10() | Return the common (base-10) logarithm of object's value.
| random() | Return a non-cryptographic random number between 0.0 and the object's value.
| toFixed(precision) | Format object's value using fixed-point notation.

**Overrides**

The Real class overrides the following member functions from the Object class:

* boolean
* integer
* real
* string
* \_\_eq__
* \_\_ne__
