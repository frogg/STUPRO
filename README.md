Kronos
======

STUPRO 2015
-----------

### Satellitengestützte Terrainvisualisierung zur Umweltbeobachtung und Präsentation relevanter Observationen
(Satellite-based terrain visualization for environmental monitoring and presentation of relevant observations)



![alt text](https://raw.githubusercontent.com/quappi/STUPRO/develop/screenshots/kronos_banner.png?token=AE9oyX3JdFRkCNEjB7H8wxjN53hdspUxks5XHfKEwA%3D%3D "Kronos Banner")

[![dev-branch build status](https://magnum.travis-ci.com/quappi/STUPRO.svg?token=YqyzbapTc49r8AJqkBGb&branch=develop)](https://magnum.travis-ci.com/quappi/STUPRO/) ![c++](https://img.shields.io/badge/language-C++-blue.svg)

Installation
------------

### Dependencies

#### CMake
To simplify the build process and cross-platform support, we use [CMake](https://cmake.org/) to auto-generate makefiles and project files for various IDEs. Run `apt-get install cmake` to install it. (Note that the CMake version on the Ubuntu 12.04LTS repositories are outdated. Kronos requires CMake at version 2.8.8 or above.)
For Windows, just follow [this (CMake 3.4.0 installer)](https://cmake.org/files/v3.4/cmake-3.4.0-win32-x86.exe) or [this (CMake 3.4.0 zip)](https://cmake.org/files/v3.4/cmake-3.4.0-win32-x86.zip) link and just install or unpack it.

#### Qt4
We are depending on [Qt4](http://www.qt.io/) (recommended: Qt 4.8.7) for cross-platform support and user interfaces. Install the required libraries and tools by running `apt-get install libqt4-dev qt4-dev-tools`. You'll need to compile it yourself for Windows-builds.

#### ParaView
The whole Kronos project is based on [ParaView](https://github.com/Kitware/ParaView) and therefore needs a working build of it to function. Since Kronos uses somewhat specialized functionality of ParaView (for example MPI), it is necessary to compile ParaView with the correct flags set. For in-depth build instructions of ParaView, refer to [the official ParaView wiki page](http://www.paraview.org/Wiki/ParaView:Build_And_Install). Make sure to set the following flags during the configuration step:
```
Module_vtkGeovisCore=ON
Module_vtkIOGeoJSON=ON
Module_vtkViewsGeovis=ON
PARAVIEW_USE_MPI=ON
```
In order to save yourself some build time, consider also setting the following flags:
```
CMAKE_BUILD_TYPE=Release
BUILD_TESTING=OFF
BUILD_DOCUMENTATION=OFF
```
ParaView MPI functionality requires the openMPI library, run `apt-get install openmpi-common openmpi-bin libopenmpi-dev` on Debian to install the required dependencies. On Windows, however, it's recommended you use [Microsoft MPI](https://www.microsoft.com/en-us/download/details.aspx?id=49926), as OpenMPI is no longer officially supported on Windows.

Alternatively, if you don't want to go through the trouble of building ParaView completely on your own, consider running the `build-paraview.sh` script located in the `scripts` folder from inside the project's root directory to automatically build ParaView inside the binary directory.

#### GTest
For running unit tests we're using [GTest](https://github.com/google/googletest/). GTest is already contained in the lib folder as a submodule, but you'll have to build it yourself since GTest is not on the apt repositories. A script responsible for building the GTest libraries is already present at `scripts/build-gtest.sh`. Make sure to run `git submodule init` and `git submodule update` (or execute `./scripts/pre-build.sh`) before building GTest to make sure the submodule is installed and up to date. When building GTest yourself, you may use the CMake flags `BUILD_SHARED_LIBS=ON`, `BUILD_GTEST=ON` and `BUILD_GMOCK=OFF` to prevent GMock from building and build GTest as a shared library.

### Building Kronos
If all dependencies are installed, building Kronos is as easy as configuring it using `cmake`. Switch into a directory of your choice (we recommend not using the project's root directory, so `cmake` doesn't put auto-generated source files and other CMake-related files into the source folders). To generate the Makefiles for building Kronos, execute `cmake` in one of the following ways:
* Use `cmake [path to Kronos]` directly for the non-interactive mode.
* Run `ccmake [path to Kronos]` to get a console-based user interface where you can change the build options without the use of command line parameters. When done configuring, press `c` to configure CMake and when finished without errors, press `g` to generate the Makefiles.
* Run `cmake-gui [path to Kronos]` to get a nice window-based user interface, similar to the `ccmake` one. Change the flags to your likings, then configure and generate the Makefiles.

When the Makefiles are done, simply run `make` to build Kronos.

To run Kronos, switch to the `src` directory inside your build folder and run `./kronos`.

#### Windows specific
When building on Windows, you should use the cmake-gui client. Load the folders and upon clicking on "Configure", choose the same compiler-version you used for ParaView. Most likely, it will be the `Visual Studio 12 2013 Win64`-compiler. Once you're finished generating the solution, you should execute the PowerShell-script (`scripts/windows-add-freetype-dependencies.ps1`) to add the missing FreeType-dependencies.
After that, you can open the `kronos.sln` and let Visual Studio do the work.

### Running unit tests
To run the unit tests, go to your build directory and execute `ctest`. Alternatively, you can switch to the `src` directory in your build folder and run `../test/UnitTester --gtest_filter=[test-name].*` to run tests of a specific group only, refer to [the GTest documentation on running specific tests](https://github.com/google/googletest/blob/master/googletest/docs/AdvancedGuide.md#running-a-subset-of-the-tests) for more information.

Licenses
--------

Kronos depends on different libraries, released under various licenses:

Dependency | License
-----------|---------
Qt4        | [Lesser General Public License Version 2.1](http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html)
GTest      | [MIT License](https://github.com/google/googletest/blob/master/googletest/LICENSE)
ParaView   | [ParaView License Version 1.2](https://github.com/Kitware/ParaView/blob/master/License_v1.2.txt)
Eigen      | [Mozilla Public License Version 2.0](https://www.mozilla.org/en-US/MPL/2.0/)
rapidjson  | [MIT License](https://github.com/miloyip/rapidjson/blob/v1.0.2/license.txt)
