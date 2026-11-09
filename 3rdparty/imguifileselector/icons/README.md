# File Selector Icons

When the file selector is used in icon mode, the user can either use the built-in icons
or provide a rendering callback to render whatever it wants. All build-in icons
are derived from [Pictogrammers](https://pictogrammers.com/) and are stored in the
local src folder.

This folder contains a script that converts the icons in SVG format to a truetype font
and then bakes this font into a C++ module together with supporting code. This C++
module resides at the top-level of this repository under the names
[FileSelectorIcons.cpp](FileSelectorIcons.cpp) and [FileSelectorIcons.h](FileSelectorIcons.h).
Since these two files are include in the repository, the build process in this folder
only has to be rerun when icons or the support code are added, removed or changed.

To run the build process in this folder, the host process must have the following
executables in the path: fantasticon and python.
