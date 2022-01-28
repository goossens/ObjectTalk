* Install Ubuntu 20.04 Desktop

* Install additional software using **sudo apt-get install build-essential cmake git xorg-dev**
* **cd [to your favorite build directory]**
* **git clone https://github.com/goossens/ObjectTalk.git**
* **cd ObjectTalk**
* Create a debug version with **make debug** or create a release version with **make release**
* You can run the test suite for the language with **make test**
* The created binary will be **./debug/bin/ot** or **./release/bin/ot** depending on the build type
* By just running the binary, an Integrated Development Environment (IDE) is launched
* You can directly run an ObjectTalk script using **./debug/bin/ot [script name]**
