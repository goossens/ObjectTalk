The Manifold class stores manifold information and provides a wide variety of member functions
to manipulate manifolds.

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
