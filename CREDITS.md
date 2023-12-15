ObjectTalk uses a lot of Open Source products without which it would have
been a lot harder to implement the language and it's libraries.

## Software

* [CMake](https://cmake.org) - cross-platform family of tools designed to build, test and package software released under the [OSI-approved BSD 3-clause License](https://gitlab.kitware.com/cmake/cmake/raw/master/Copyright.txt).
* [libuv](https://libuv.org) - multi-platform support library with a focus on asynchronous I/O released under the [MIT license](https://opensource.org/licenses/MIT). Its documentation is released uder the [CC BY 4.0 license](https://creativecommons.org/licenses/by/4.0/)
* [argparse](https://github.com/p-ranav/argparse) - command line argument parser released under the [MIT license](https://opensource.org/licenses/MIT).
* [llhttp](https://llhttp.org) - HTTP protocol parser released under the [MIT license](https://opensource.org/licenses/MIT).
* [multipart-parser](https://github.com/francoiscolas/multipart-parser) - HTTP multipart body parser released under the [MIT license](https://opensource.org/licenses/MIT).

In case the GUI modules are included, we also use:

* [GLFW](https://www.glfw.org) - multi-platform library for desktop application development released under the [zlib license](https://opensource.org/licenses/Zlib).
* [bgfx](https://github.com/bkaradzic/bgfx) - cross-platform, graphics API agnostic rendering library. It's dependencies [bimg](https://github.com/bkaradzic/bimg) and [bx](https://github.com/bkaradzic/bx) are also included. All are released under the [BSD 2-Clause "Simplified" License](https://opensource.org/licenses/BSD-2-Clause)
* [Dear ImGui](https://github.com/ocornut/imgui) - graphical user interface library released under the [MIT license](https://opensource.org/licenses/MIT).
* [ImGuiFileDialog](https://github.com/aiekick/ImGuiFileDialog) - file selection dialog for Dear ImGui released under the [MIT license](https://opensource.org/licenses/MIT).
* [ImGuiColorTextEdit](https://github.com/BalazsJako/ImGuiColorTextEdit) - syntax highlighting text editor for Dear ImGui released under the [MIT license](https://opensource.org/licenses/MIT). As this looks like an abandoned library, a private copy is included in the 3rdparty folder. Improvements made in the [santaclose fork](https://github.com/santaclose/ImGuiColorTextEdit/tree/master) are also included.
* [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) - Gizmo to visually manipulate 4x4 matrices released under the [MIT license](https://opensource.org/licenses/MIT).
* [glm](https://github.com/g-truc/glm) - mathematics library for graphics software released under the [MIT license](https://opensource.org/licenses/MIT).
* [libtess2](https://github.com/memononen/libtess2) - library to convert (tesssallate) polygons into triangles by [Mikko Mononen](https://github.com/memononen) released under the [SGI Free Sofware License B, version 2](https://directory.fsf.org/wiki/License:SGI-B-2.0)
* [EnTT](https://github.com/skypjack/entt) - Entity Component System (ECS) released under the [MIT license](https://opensource.org/licenses/MIT).
* [JSON for Modern C++](https://github.com/nlohmann/json) - JSON creation/parsing library released under the [MIT license](https://opensource.org/licenses/MIT).
* [Box2D](https://box2d.org) - 2D physics engine released under the [MIT license](https://opensource.org/licenses/MIT).
* [tweeny](https://github.com/mobius3/tweeny) - animation library released under the [MIT license](https://opensource.org/licenses/MIT).
* [assimp](https://github.com/assimp/assimp) - to import 3D models released under a [custom license](https://github.com/assimp/assimp/blob/master/LICENSE) based on the [3-clause BSD-License license](https://opensource.org/licenses/BSD-3-Clause).

## Algorithms

* The sky shader is based on a [GitHub project](https://github.com/shff/opengl_sky) released under the [MIT License](https://opensource.org/licenses/MIT).

## Data

The Simple Font is extracted from the [Hershey Fonts](https://github.com/kamalmostafa/hershey-fonts)
released under the [GNU General Public License, version 2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html).

## Tools

A special thanks also goes to [Gunther Rademacher](https://github.com/GuntherRademacher) for his
["RailRoad Diagram Generator"](https://www.bottlecaps.de/rr/ui)
which is used in the documentation to visualize the ObjectTalk grammar. The tool is release under the
[Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0)

## Documentation

A another special thanks goes to the team that wrote the [Swift](https://swift.org) computer language
[documentation](https://swift.org/documentation/). It is awesome and I've respectfully reused a lot of it to document ObjectTalk.

## Tutorials

The following tutorials were used in the development of ObjectTalk:

* [Crafting Interpreters](https://www.craftinginterpreters.com) by [Robert Nystrom](http://journal.stuffwithstuff.com) release under the [MIT License](https://opensource.org/licenses/MIT) and the [CC BY-NC-ND 4.0 license](https://creativecommons.org/licenses/by-nc-nd/4.0/).
Please see his [github repository](https://github.com/munificent/craftinginterpreters/blob/master/LICENSE) for an explanation on
what applies where.
* [Learn OpenGL](https://learnopengl.com) by [Joey de Vries](http://joeydevries.com/#home) released under the [Creative Commons Attribution-NonCommercial 4.0 International License](https://creativecommons.org/licenses/by-nc/4.0/legalcode).
* [OGLdev](https://ogldev.org/index.html) by [Etay Meiri](https://ogldev.org/contact.html) release under the [GNU General Public License Version 3](https://www.gnu.org/licenses/gpl-3.0.txt). You can find his videos on [YouTube](https://www.youtube.com/channel/UC7Z1FTCdSln_qFKK95AWplw).
* [Hazel](https://github.com/TheCherno/Hazel) by [Yan Chernikov (The Cherno)](https://www.youtube.com/@TheCherno/about) released under the [Apache License version 2.0](http://www.apache.org/licenses/LICENSE-2.0). You can find his videos on [YouTube](https://www.youtube.com/@TheCherno).

## Assets

Some examples use a number of assetts (e.g. images and models) that were reused from other projects.
A full list with links and licenses can be found in the appropriate asset folders.
