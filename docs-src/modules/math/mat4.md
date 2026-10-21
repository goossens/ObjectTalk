A Mat4 is a 4x4 floating-point matrix commonly used for 3D graphics transformations like scaling, rotation, translation, and projection. Matrix values are stored in column-major order and can be accessed using the valueCR shortcut where C is a zero-based column number and R is a zero-based row number (e.g. var cell = matrix.value12; or matrix.value30 = 0.1);

Rows and columns can be accessed with the rowR and columnC where R and C are the same zero-based numbers mentioned above. Rows and columns are [Vec4](#vec4) objects (e.g. var row = matrix.row2; or matrix.column3 = Vec4(0.0));

**Member Functions**

| Function | Description |
| ------ | ----------- |
| \_\_init__() | Constructor. When called without parameters, the matrix is set to identity. When called with 1 parameter, it is evaluated as a [Real](language-classes.html#real) and assigned to all cells. When called with 16 parameters, the cells are set to the evaluated Reals in column-major order (value00, value01, value02, value03, value10, etc). |
| \_\_add__(matrix) | Add matrix to object's value and return result. This member function is called when you use the **add** (+) operator. |
| \_\_mul__(operand) | Multiple operand with object's value and return result. Operand can be a Mat4 or a [Vec4](#vec4). This member function is called when you use the **multiply** (*) operator. |
| \_\_sub__(matrix) | Subtract matrix from object's value and return result. This member function is called when you use the **subtract** (-) operator. |
| inverse() | Returns the inverse of the matrix. |
| rotate(vector) | Create a rotation matrix based on provided vector, apply to object's value and return new matrix. |
| scale(vector) | Create a scaling matrix based on provided vector, apply to object's value and return new matrix. |
| translate(vector) | Create a translation matrix based on provided vector, apply to object's value and return new matrix. |
| transpose() | Return a new matrix with rows and columns swapped. |

**Overrides**

The Mat4 class overrides the following member functions from the Object class:

* \_\_eq__
* \_\_ne__
