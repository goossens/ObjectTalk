Manifolds are specializations of Geometries. A manifold mesh is watertight and
contains no holes or missing faces that would cause leaks into the interior of the shape's volume.
For a mesh to be manifold, every edge must have exactly two adjacent faces. Geometries do not have
this constraint.

Manifolds are often used in CAD applications to create 3D models through Constructive Solid Geometry.
You can combined or subtract solid shapes, transform them, extrude them from 2d paths or slice 3D
models back into 2D paths.

Another difference between Manifolds and Geometries is their coordinate system. In ObjectTalk,
a classic coordinate system is used where the positive X-axis points to the right, the positive Y-axis
points up, and the positive Z-axis points out of the screen towards the viewer. This is called a
right-handed Y-up coordinate system. CAD programs and therefore Manifolds typically use a left-handed
Z-up coordinate system where the positive X-axis points to the right, the positive Y-axis points away,
and the positive Z-axis points up.

So when creating a Manifold, it must be done in the left-handed Z-up coordinate system. When converting
to a Mesh or Geometry, this class automatically converts the vertex coordinates to the right-handed
Y-up coordinate system.

**Member Functions**

| Function | Description |
| ------ | ----------- |
| \_\_add__(manifold) | Create the union between two manifolds and return the result. |
| \_\_pow__(manifold) | Create the intersection between two manifolds and return the result. |
| \_\_sub__(manifold) | Create the difference between two manifolds and return the result. |
| clear() | Reset the manifold's mesh. |
| decompose() | Return an array of Manifolds that are topologically disconnected. If everything is connected, the array is length one, containing a copy of the original. |
| getSurfaceArea() | Return the surface area of the manifold. |
| getTriangleCount() | Return the number of triangles in the manifold. |
| getVertexCount() | Return the number of vertices in the manifold. |
| getVolume() | Return the volume of the manifold. |
| hull() | Compute the convex hull of this manifold and return a new Manifold. |
| load(filename) | Load the Manifold from a "GLB", "GLTF", "OBJ"or "STL" file. |
| mirror(x, y, z) | Mirror the Manifold over the plane described by a normal vector and return a copy. |
| project() | Returns a shape representing the projected outline of this Manifold onto the X-Y plane. |
| refine(tolerance) | Smooth Manifold by refining it to a higher triangle count and return result. |
| rotate(x, y, z) | Apply an Euler angle rotation (expressed in degrees) to the manifold and return the result. |
| save(filename) | Save the Manifold to a "GLB", "GLTF", "OBJ"or "STL" file. |
| scale(x, y, z) | Scale this Manifold in space and return the result. |
| simplify(tolerance) | Return a copy of the manifold simplified to the given tolerance. |
| slice(height) | Returns the cross section of this object parallel to the X-Y plane at the specified Z height. |
| split(normal, distance) | Split a Manifold using a plane described by a normal and the distance from the origin. It returns an array of the two manifolds. |
| translate(x, y, z) | Move this Manifold in space and return a new copy. |
