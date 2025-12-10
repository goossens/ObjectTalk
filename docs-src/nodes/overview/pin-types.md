The input and output pins of a node can only carry one specific
data type. Each of these types is color coded and the IDE ensures
links are only created using compatible types. Below are all the
data types used by pins:

![screenshot](img/pin-types.png#right)

- **Boolean Pins** - Booleans can only hold two values: true and false.
- **Integer Pins** - Integers represent negative and positive whole numbers.
- **Float Pins** - Floats are double-precision floating point numbers.
- **String Pins** - Strings are sequences of UTF-8 encoded characters.
- **VectorX Pins** - Vectors (2D, 3D or 4D) represent a quantity with both magnitude
and direction expressed as coordinates along the dimensional axis.
- **Color Pins** - Colors are represented by their RGBA values.
- **Font Pins** - Fonts are a set of printed letters, numbers, and other symbols
of the same style.
- **Image Pins** - Images are a visual representation of something (a picture)
that lives on the CPU.
- **Texture Pins** - Textures are a visual representation of something (a picture)
that lives on the GPU.
- **Shape Pins** - Shapes define the outline or shape of an object using one or more
paths.
- **Geometry Pins** - Geometries are three dimensional objects consisting of triangular
faces that connect vertices.
- **Manifold Pins** - Manifolds are specializations of geometries. A manifold mesh
is watertight and contains no holes or missing faces that would cause leaks into the
interior of the shape's volume. For a mesh to be manifold, every edge
must have exactly two adjacent faces. Geometries do not have this constraint.
- **Map Pins** - Maps describe geospatial geometries (either real or
fictitious) including properties of the faces (e.g. biome type).
- **Instances Pins** -Instances describe the transforms applied to a numbers
of 3D geometry instances.
