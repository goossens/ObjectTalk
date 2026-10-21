A Vec4 is a 4-component ([Real](language-classes.html#real) values) vector class. Components can be accessed with the x, y, z and w shortcuts like var y = vector.y; or vector.w = 1.0;


**Member Functions**

| Function | Description |
| ------ | ----------- |
| \_\_init__() | Constructor. When called without parameters, all components are set to 0.0. When called with 1 parameter, it is evaluated as a [Real](language-classes.html#real) and assigned to all components. When called with 4 parameters, the components are set to the evaluated Reals. |
| \_\_add__(vector) | Add vector to object's value and return result. This member function is called when you use the **add** (+) operator. |
| \_\_div__(vector) | Divide object's value by vector and return result. This member function is called when you use the **divide** (/) operator. |
| \_\_mod__(vector) | Divide object's value by vector and return leftover. This member function is called when you use the **modulo** (%) operator. |
| \_\_mul__(vector) | Multiple vector with object's value and return result. This member function is called when you use the **multiply** (*) operator. |
| \_\_sub__(vector) | Subtract vector from object's value and return result. This member function is called when you use the **subtract** (-) operator. |
| abs() | Returns a new vector with each component being the absolute value. |
| ceil() | Returns a new vector with each component being the nearest integer that is greater than or equal. |
| clamp(min, max) | Returns a new vector with each component constrained to the range min to max. |
| distance(point) | Returns the distance between the 2 vectors (interpreted as points). |
| dot(vector) | Returns the dot product of this vector and the provided vector. |
| floor() | Returns a new vector with each component being the nearest integer that is less than or equal. |
| fract() | Returns a new vector with each component being the fractional part of the original calculated as x - floor(x). |
| homogenize() | Returns a new vector converting homogeneous coordinates to 3D Cartesian coordinates by dividing its x, y, and z components by its w component (also known as perspective division). |
| length() | Returns the length of the vector. |
| max(vector) | Returns a new vector with each component being the maximum of the two parameters. |
| min(vector) | Returns a new vector with each component being the minimum of the two parameters. |
| mix(vector, interpolate) | Returns a new vector with each component being linearly interpolated between this object and the provided vector using interpolate to weight between them. |
| normalize() | Returns a new vector with the same direction but with length 1.  |
| round() | Returns a new vector with each component being the nearest integer. |
| sign() | Returns a new vector with -1.0 if component is less than 0.0, 0.0 if component is equal to 0.0, and +1.0 if component is greater than 0.0. |
| smoothstep() | Like step but it performs smooth Hermite interpolation between 0 and 1. |
| step(edge) | Returns a new vector with each component being 0.0 if [i] < edge[i], and 1.0 otherwise. |
| trunc() |  Return a new vector with the value equal to the nearest integer to x whose absolute value is not larger than the absolute value of each component. |

**Overrides**

The Vec4 class overrides the following member functions from the Object class:

* \_\_eq__
* \_\_ne__
