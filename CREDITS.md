ObjectTalk uses a lot of Open Source products without which it would have
been a lot harder to implement the project.

![Screenshots](docs/img/screenshots.png)

## Software

* [libuv](https://libuv.org) - multi-platform support library with a focus on asynchronous I/O released under the [MIT license](https://opensource.org/licenses/MIT). Its documentation is released uder the [CC BY 4.0 license](https://creativecommons.org/licenses/by/4.0/)
* [{fmt}](https://github.com/fmtlib/fmt) - open-source formatting library released under the [MIT license](https://opensource.org/licenses/MIT).
* [JSON for Modern C++](https://github.com/nlohmann/json) - JSON creation/parsing library released under the [MIT license](https://opensource.org/licenses/MIT).
* [argparse](https://github.com/p-ranav/argparse) - command line argument parser released under the [MIT license](https://opensource.org/licenses/MIT).
* [llhttp](https://llhttp.org) - HTTP protocol parser released under the [MIT license](https://opensource.org/licenses/MIT).
* [multipart-parser](https://github.com/francoiscolas/multipart-parser) - HTTP multipart body parser released under the [MIT license](https://opensource.org/licenses/MIT).
* [isocline](https://github.com/daanx/isocline) - cross-platform library that can be used as an alternative to the GNU readline library released under the [MIT license](https://opensource.org/licenses/MIT).

In case User Interface (UI) modules are included, we also use:

* [GLFW](https://www.glfw.org) - multi-platform library for desktop application development released under the [zlib license](https://opensource.org/licenses/Zlib).
* [bgfx](https://github.com/bkaradzic/bgfx) - cross-platform, graphics API agnostic rendering library. It's dependencies [bimg](https://github.com/bkaradzic/bimg) and [bx](https://github.com/bkaradzic/bx) are also used. All are released under the [BSD 2-Clause "Simplified" License](https://opensource.org/licenses/BSD-2-Clause)
* [Dear ImGui](https://github.com/ocornut/imgui) - graphical user interface library released under the [MIT license](https://opensource.org/licenses/MIT).
* [ImGuiFileDialog](https://github.com/aiekick/ImGuiFileDialog) - file selection dialog for Dear ImGui released under the [MIT license](https://opensource.org/licenses/MIT).
* [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) - Gizmo to visually manipulate 4x4 matrices released under the [MIT license](https://opensource.org/licenses/MIT).
* [glm](https://github.com/g-truc/glm) - mathematics library for graphics software released under the [MIT license](https://opensource.org/licenses/MIT).
* [libtess2](https://github.com/memononen/libtess2) - library to convert (tesssallate) polygons into triangles by [Mikko Mononen](https://github.com/memononen) released under the [SGI Free Sofware License B, version 2](https://directory.fsf.org/wiki/License:SGI-B-2.0)
* [PlutoVG](https://github.com/sammycage/plutovg) - standalone 2D vector graphics library in C released under the [MIT license](https://opensource.org/licenses/MIT).
* [LunaSVG](https://github.com/sammycage/lunasvg) - SVG rendering library designed to be lightweight and portable, offering efficient rendering and manipulation released under the [MIT license](https://opensource.org/licenses/MIT).
* [EnTT](https://github.com/skypjack/entt) - Entity Component System (ECS) by [Michele Caini](https://github.com/skypjack) released under the [MIT license](https://opensource.org/licenses/MIT).
* [BS::thread_pool](https://github.com/bshoshany/thread-pool) - A fast, lightweight, and easy-to-use C++17 thread pool library released under the [MIT license](https://opensource.org/licenses/MIT).
* [Jolt Physics](https://github.com/jrouwe/JoltPhysics) - A multi core friendly 3D rigid body physics and collision detection library released under the [MIT license](https://opensource.org/licenses/MIT).
* [Box2D](https://box2d.org) - 2D physics engine released under the [MIT license](https://opensource.org/licenses/MIT).
* [tweeny](https://github.com/mobius3/tweeny) - animation library released under the [MIT license](https://opensource.org/licenses/MIT).
* [assimp](https://github.com/assimp/assimp) - to import 3D models released under a [custom license](https://github.com/assimp/assimp/blob/master/LICENSE) based on the [3-clause BSD-License license](https://opensource.org/licenses/BSD-3-Clause).

## Algorithms

* Some of the shaders use code snippets from the [LYGIA Shader Library](https://github.com/patriciogonzalezvivo/lygia) released under the [Prosperity Public License](https://prosperitylicense.com/versions/3.0.0).
* The sky shader is based on a [GitHub project](https://github.com/shff/opengl_sky) released under the [MIT License](https://opensource.org/licenses/MIT).
* The Image Based Lighting (IBL) shaders are based on an article by [Bruno Opsenica](https://bruop.github.io/ibl/).
* The Fast Approximate Anti-Aliasing (FXAA) post-processing shader was originally developped by Timothy Lottes at Nvidia. It was released under a public domain license. A later version is released under a [3-clause BSD license](https://opensource.org/license/bsd-3-clause).
* Some shaders and shader ideas are based on [JungleIN](https://github.com/BKcore/JungleIN), an OpenGL jungle viewer written as part of a scholar project, released under the [MIT license](https://opensource.org/licenses/MIT).

## Data

The [BitStream Vera Mono Font](https://en.wikipedia.org/wiki/Bitstream_Vera) is extracted from
[here](https://download.gnome.org/sources/ttf-bitstream-vera/1.10/) and it was released onder a
[generous license](https://raw.githubusercontent.com/goossens/ObjectTalk/master/gfx/framework/OtBitstreamVeraMono.lic)
similar to the [SIL Open Font License](https://openfontlicense.org/documents/OFL.txt).

The Simple Font is extracted from the [Hershey Fonts](https://github.com/kamalmostafa/hershey-fonts)
released under the [GNU General Public License, version 2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html).

## Tools

* [CMake](https://cmake.org) - cross-platform family of tools designed to build, test and package software released under the [OSI-approved BSD 3-clause License](https://gitlab.kitware.com/cmake/cmake/raw/master/Copyright.txt).
* [re2c](https://github.com/skvadrik/re2c) - a free and open-source lexer generator for C/C++, Go and Rust released under a public domain license.
* [Gunther Rademacher](https://github.com/GuntherRademacher) - a ["RailRoad Diagram Generator"](https://github.com/GuntherRademacher/rr) which is used in the documentation to visualize the ObjectTalk grammar. The tool is released under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0)

## Documentation

A another special thanks goes to the team that wrote the [Swift](https://swift.org) computer language
[documentation](https://swift.org/documentation/). It is awesome and I've respectfully reused a lot of it to document ObjectTalk.

## Tutorials

The following tutorials were used in the development of ObjectTalk:

* [Crafting Interpreters](http://craftinginterpreters.com) by [Robert Nystrom](http://journal.stuffwithstuff.com) release under the [MIT License](https://opensource.org/licenses/MIT) and the [CC BY-NC-ND 4.0 license](https://creativecommons.org/licenses/by-nc-nd/4.0/).
Please see his [github repository](https://github.com/munificent/craftinginterpreters/blob/master/LICENSE) for an explanation on
what applies where.
* [Learn OpenGL](https://learnopengl.com) by [Joey de Vries](http://joeydevries.com/#home) released under the [Creative Commons Attribution-NonCommercial 4.0 International License](https://creativecommons.org/licenses/by-nc/4.0/legalcode).
* [OGLdev](https://ogldev.org/index.html) by [Etay Meiri](https://ogldev.org/contact.html) release under the [GNU General Public License Version 3](https://www.gnu.org/licenses/gpl-3.0.txt). You can find his videos on [YouTube](https://www.youtube.com/channel/UC7Z1FTCdSln_qFKK95AWplw).
* [Hazel](https://github.com/TheCherno/Hazel) by [Yan Chernikov (The Cherno)](https://www.youtube.com/@TheCherno/about) released under the [Apache License version 2.0](http://www.apache.org/licenses/LICENSE-2.0). You can find his videos on [YouTube](https://www.youtube.com/@TheCherno).

## Assets

Some examples use a number of assetts (e.g. images and models) that were reused from other projects.
A full list with links and licenses can be found in the appropriate asset folders.
