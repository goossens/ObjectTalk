ObjectTalk is supported on Linux. Given the large number
of flavors, the core team only tests the build process
on Ubuntu 22.04 Desktop. In practice, any distribution
should work as it just depends on the availability of
X-Windows, Wayland and Vulkan. Other flavors
of Linux will likely work as well but we'll let the
community provide the instructions.

To build the full version of ObjectTalk on Ubuntu 22.04
Desktop with GUI and IDE, run the following commands:

* Install additional software using **sudo apt-get install build-essential cmake ninja-build git xorg-dev libgl1-mesa-dev x11proto-core-dev libx11-dev libwayland-dev libxkbcommon-x11-dev**
* **cd [to your favorite build directory]**
* If your favorite directory is on a mounted drive, you might have to do a **git config --global --replace-all safe.directory '*'**
* **git clone https://github.com/goossens/ObjectTalk.git**
* **cd ObjectTalk**
* Create a debug version with **make debug** or create a release version with **make release**
* You can run the test suite for the language with **make test** or **make rtest** depending on the build type
* The created binary will be **./build/Linux/Debug/ot** or **./Linux/Release/ot** depending on the build type
