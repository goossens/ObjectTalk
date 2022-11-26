ObjectTalk is supported on Windows 10, 11, Server 2019
or Server 2022). There are 3 simple requirements
to get your machine ready for ObjectTalk development:

* Install Visual Studio 17 2022 (make sure you pick the "Desktop development with C++" option): Visual Studio 16 2019 also works
* Install git from [https://git-scm.com/download/win](https://git-scm.com/download/win) using defaults
* Install cmake from [https://cmake.org/download/](https://cmake.org/download/) (ensure you select  the "add cmake to the system PATH" option)

To build ObjectTalk, do the following from the command line:

* **cd [to your favorite build directory]**
* **git clone https://github.com/goossens/ObjectTalk.git**
* If your favorite directory is on a mounted drive, you might have to do a **git config --global --replace-all safe.directory "*"**
* **cd ObjectTalk**
* **cmake -Bvs -G "Visual Studio 17 2022" -A x64**
* Build ObjectTalk by Hand:
	* **cd vs**
	* **cmake --build .**
	* Run the tests: **ctest -C Debug --output-on-failure**
* Build ObjectTalk in Visual Studio:
	* Open the **ObjectTalk.sln** solution in the **vs** folder and go crazy.
