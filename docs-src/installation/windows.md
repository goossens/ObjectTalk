**Windows 10 or Server 2019**

* Install Windows
* Install Visual Studio 16 2019, arch: x64 (make sure you pick the "Desktop development with C++" option)

** Windows 11 or Server 2022**

* Install Windows
* Install Visual Studio 17 2022, arch: x64 (make sure you pick the "Desktop development with C++" option)

**Common for all Windows versions**

* Install git from https://git-scm.com/download/win using defaults
* Install cmake from https://cmake.org/download/ (ensure you select  the "add cmake to the system PATH" option)
* Lauch **cmd**
* **cd** <to your favorite directory>
* **git clone https://github.com/goossens/ObjectTalk.git**
* **cd ObjectTalk**
* Configure your Visual Studio project
	* For Windows 10 or Server 2019, **cmake -Bvs -Wno-dev -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Debug**
	* For Windows 11 or Server 202, **cmake -Bvs -Wno-dev -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug**

**Build ObjectTalk by Hand**

* **cd vs**
* **cmake --build .**
* Run the tests: **ctest -C Debug --output-on-failure**

**Build ObjectTalk in Visual Studio**

* Open the ObjectTalk solution in the vs folder and go crazy.
