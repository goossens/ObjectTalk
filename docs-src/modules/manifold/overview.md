The manifold module provides support for manifold geometries which are specializations of geometries.
A manifold mesh is watertight and contains no holes or missing faces that would cause leaks into
the interior of the shape's volume. For a mesh to be manifold, every edge must have exactly two
adjacent faces. Geometries do not have this constraint.

Manifolds are often used in CAD applications to create 3D models through Constructive Solid Geometry.
You can combine or subtract solid shapes, transform them, extrude them from 2d paths or slice 3D
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
