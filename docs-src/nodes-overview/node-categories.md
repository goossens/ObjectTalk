Nodes are grouped in categories that make it easier to find them
and recognize them in a graph. Each category has a unique color as
shown on the right below.

![screenshot](img/node-categories.png#right)

- **Input Nodes** - These nodes enable users to input values or
select assets (e.g. images, textures, geometries). If a nodes graph
is used in another object (e.g. a 3D scene), input fields enable
users to interact with the graph's functionality without having the
understand the full logic. This in essence makes a nodes graph
a macro somewhere else.
- **Field Nodes** - These nodes
- **Math Nodes** - These nodes perform mathematical operations
on primitive (integer, real) and composite (vector) values.
- **Probe Nodes** - These nodes provide debugging support by
visualizing various data types.
- **Save Nodes** - These nodes enable the user to save various
data types to specified files.
- **Virtualize Nodes** - These nodes save data types to the internal
virtual (memory-based) file system. This allows assets to be created
dynamically with nodes and then be used in for instance 3D scenes.
- **Image Nodes** - These nodes create, process, filter or transform
images. In ObjectTalk, images are pictures that live on the CPU.
- **Texture Nodes** - These nodes create, process, filter or transform
textures. In ObjectTalk, textures are pictures that live on the GPU.
In general, texture nodes are faster than image nodes as all processing
is GPU accelerated. Nodes are available to convert between images
and textures.
- **Shape Nodes** - These nodes generate, process or convert 2D
vector shapes that are created with paths.
- **Geometry Nodes** - These nodes generate, manipulate and convert
geometries which consist of one or more 3D meshes. In ObjectTalk,
a geometry is a 3D object made up of meshes which are a collection of
interconnected triangles (faces). These triangles are formed by connecting
vertices (points) with straight edges. The mesh defines the surface of the
3D object, but not its interior volume. 
- **Manifold Nodes** - These nodes operate on manifolds. In ObjectTalk,
manifolds are specializations of geometries. A manifold mesh is watertight
and contains no holes or missing faces that would cause leaks into the
interior of the shape's volume. For a mesh to be manifold, every edge
must have exactly two adjacent faces. Geometries do not have this constraint.
Manifolds are often used in CAD applications to create 3D models through
Constructive Solid Geometry. You can combined or subtract solid shapes,
transform them, extrude them from 2D shapes or slice 3D models back into 2D
shapes.
- **Map Nodes** - These nodes work on geospatial data (either real or
fictitious).
