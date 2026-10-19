| Function | Description |
| ------ | ----------- |
| compose(...) | Create a unions between all provided manifolds and return a single manifold. |
| cube(width, height, depth, center) | Creates a cube or rectangular prism (i.e., a "box") in the first octant. When center is true, the cube is centered on the origin. Argument names are optional if given in the order shown here. |
| cylinder(height, bottomRadius, topRadius, segments, center) | Creates a cylinder or cone centered about the z axis. When center is true, it is also centered vertically along the z axis. |
| extrude(shape, height, divisions, twistDegrees, scaleTop, tolerance) | Constructs a manifold from a set of polygons by extruding them along the Z-axis. Note that high twistDegrees with small nDivisions may cause self-intersection. This is not checked here and it is up to the user to choose the correct parameters.. |
| revolve(shape, segments, revolveDegrees, tolerance) | Constructs a manifold from a shape by revolving this cross-section around its Y-axis and then setting this as the Z-axis of the resulting manifold. If the polygons cross the Y-axis, only the part on the positive X side is used. Geometrically valid input will result in geometrically valid output. |
| sphere(radius, segments) | Creates a sphere at the origin of the coordinate system. |
