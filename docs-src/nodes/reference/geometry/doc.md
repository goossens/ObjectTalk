#### Geometry Generator

![screenshot](img/geometry-generator.png#right)

The Geometry Generator node outputs a geometry based on the specified properties/

**Properties**

* Type - Specifies the type a standard geometry selected from a list: Grid, Plan, Circle,
Cube, Cylinder, Sphere, Torus, Tetrahedron, Octahedron, Dodecahedron, Icosahedron and
Spherical cube.
* The rest of the properties depend on the selected geometry type.

**Outputs**

* Geometry - The specified geometry.

#### Geometry Transform

![screenshot](img/geometry-transform.png#right)

The Geometry Transform node transforms a provided geometry based on scaling, rotating and translating
operations (which are executed in that order). If you need a different order, you need to use multiple
Geometry Nodes in a sequence.

**Inputs**

* Input - The target geometry.
* Translate - a 3D translation vector.
* Rotate - a 3D vector expressing the euler rotation angles in degrees.
* Scale - a 3D scaling vector.

**Outputs**

* Output - The transformed geometry.

#### Instances on Faces

![screenshot](img/instances-on-faces.png#right)

The Instances on Faces node generates a set of specified instances based on the faces of an input geometry.

**Inputs**

* Geometry -
* Seed -
* Instance Count -
* Selection -
* Rotate to Normals -
* Rotation -
* Scale -

**Outputs**

* Instances -

#### Instances on Vertices

![screenshot](img/instances-on-vertices.png#right)

**Inputs**

**Outputs**

#### Instances Transform

![screenshot](img/instances-transform.png#right)

**Inputs**

**Outputs**

#### Terrain Builder

![screenshot](img/terrain-builder.png#right)

**Inputs**

**Outputs**
