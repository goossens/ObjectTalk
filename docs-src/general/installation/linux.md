ObjectTalk is supported on Linux. Given the large number of flavors, the core team only tests the build process
on Ubuntu 24.04 Desktop. In practice, any distribution should work as it just depends on the availability of
X-Windows, Wayland and Vulkan. Other flavors of Linux will likely work as well but we'll let the
community provide the instructions.

To build the full version of ObjectTalk on Ubuntu 24.04 Desktop with GUI and IDE, run the following commands:

* Install additional software using **sudo apt-get install build-essential git make pkg-config cmake ninja-build libasound2-dev libpulse-dev libaudio-dev libfribidi-dev libjack-dev libsndio-dev libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libpipewire-0.3-dev libwayland-dev libdecor-0-dev liburing-dev**

* **cd [to your favorite build directory]**
* If your favorite directory is on a mounted drive, you might have to do a **git config --global --replace-all safe.directory '*'**
* **git clone https://github.com/goossens/ObjectTalk.git**
* **cd ObjectTalk**
* Create a debug version with **make debug** or create a release version with **make release**
* You can run the test suite for the language with **make test** or **make rtest** depending on the build type
* The created binary will be **${HOME}/build/ObjectTalk/Debug/ot** or **${HOME}/build/ObjectTalk/Release/ot** depending on the build type
