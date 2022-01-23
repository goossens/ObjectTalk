* Install Windows (10, 11, Server 2019 or Server 2022)
* Install Visual Studio 17 2022 (make sure you pick the "Desktop development with C++" option)

* Install git from https://git-scm.com/download/win using defaults
* Install cmake from https://cmake.org/download/ (ensure you select  the "add cmake to the system PATH" option)
* Lauch **cmd**
* **cd [to your favorite build directory]**
* **git clone https://github.com/goossens/ObjectTalk.git**
* **cd ObjectTalk**
* **cmake -Bvs -Wno-dev -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug**
* Build ObjectTalk by Hand:
	* **cd vs**
	* **cmake --build .**
	* Run the tests: **ctest -C Debug --output-on-failure**
* Build ObjectTalk in Visual Studio:
	* Open the ObjectTalk solution in the vs folder and go crazy.
