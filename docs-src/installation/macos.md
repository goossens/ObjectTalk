* Install MacOS Monterey or Big Sur
* Install Xcode
* Install **homebrew** using the instructions at https://brew.sh
* Install Cmake using **brew install cmake**
* **cd [to your favorite build directory]**
* **git clone https://github.com/goossens/ObjectTalk.git**
* **cd ObjectTalk**
* Create a debug version with **make debug** or create a release version with **make release**
* You can run the test suite for the language with **make test**
* The created binary will be **./debug/bin/ot** or **./release/bin/ot** depending on the build type
* By just running the binary, an Integrated Development Environment (IDE) is launched
* You can directly run an ObjectTalk script using **./debug/bin/ot [script name]**
* You can also build an Xcode project with **make xcode**: the project will be in the xcode directory
