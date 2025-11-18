ObjectTalk is supported on Sequoia (MacOS 15) and Sonoma (MacOS 14).
There are 4 simple requirements to get your machine ready for ObjectTalk development:

* Install Xcode from the App Store
* Install **homebrew** using the instructions at [https://brew.sh](https://brew.sh)
* Install git using **brew install git** from the command line
* Install CMake using **brew install cmake** from the command line
* Install Ninja using **brew install ninja** from the command line

To build ObjectTalk, do the following from the command line:

* **cd [to your favorite build directory]**
* **git clone https://github.com/goossens/ObjectTalk.git**
* **cd ObjectTalk**
* Create a debug version with **make debug** or create a release version with **make release**
* You can run the test suite for the language with **make test**
* The created binary will be **./debug/bin/ot** or **./release/bin/ot** depending on the build type
* You can also build an Xcode project with **make xcode**: the project will be in the **build/xcode** directory
