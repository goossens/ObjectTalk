My older 3D graphics projects were always based on OpenGL as at the
time, it felt like a good cross-platform solution and a standard that
most operating system providers would implement. Fast forwarding
a few years and we now have a number of graphics APIs like DirectX,
Vulkan and Metal with programmable shaders and all of them are different.
So in the end, I decided that my graphics engine mentioned above will
provide an abstraction over all of these APIs (in fact, the graphics
engine uses DirectX on Windows, Vulkan on Linux and Metal on MacOS)
and focus the 3D scene capability at a higher level.

This is where the Entity Component System (ECS) comes in as an architectural
pattern that has already been used in games for many years. ObjectTalk's
IDE contains an ECS editor that allows you to define entities (objects in
the real world) and associate a variety of components with them that offer
functionality.

The 3D scene ECS also has nodes and script components to make the whole
thing more dynamic. In the examples folder, you'll find automatically
spinning cubes based on timers and an earth rotating based on mouse movements.
Both of these a driven by script components. The jungle and island examples
use nodes to procedurally create terrain and asset instances.

To round it all out, modern engine features like geometry/model importing,
support for animation, deferred rendering, Physically Based Rendering (PBR),
particle systems, Image Based Lighting (IBL), Cascaded Shadow Maps (CSMs) and
post processing with Fog, Bloom, God Rays, Fast Approximate Anti-Aliasing (FXAA),
Exposure/Contrast correction and Tone Mapping are also available.

<div align="center">

![Scenes](img/scenes.png)

</div>
