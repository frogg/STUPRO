Kronos
======

STUPRO 2015
-----------

### Satellitengestützte Terrainvisualisierung zur Umweltbeobachtung und Präsentation relevanter Observationen
(Satellite-based terrain visualization for environmental monitoring and presentation of relevant observations)

[![dev-branch build status](https://magnum.travis-ci.com/quappi/STUPRO.svg?token=YqyzbapTc49r8AJqkBGb&branch=develop)](https://magnum.travis-ci.com/quappi/STUPRO/) ![c++](https://img.shields.io/badge/language-C++-blue.svg)

Installation
------------

### Dependencies

#### CMake
To simplify the build process and cross-platform support, we use [CMake](https://cmake.org/) to auto-generate makefiles and project files for various IDEs. Run `apt-get install cmake` to install it. (Note that the CMake version on the Ubuntu 12.04LTS repositories are outdated. Kronos requires CMake at version 2.8.8 or above.)

#### Qt4
We are depending on [Qt4](http://www.qt.io/) for cross-platform support and user interfaces. Install the required libraries and tools by running `apt-get install libqt4-dev qt4-dev-tools`.

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
ParaView MPI functionality requires the openMPI library, run `apt-get install openmpi-common openmpi-bin libopenmpi-dev` on Debian to install the required dependencies.

Alternatively, if you don't want to go through the trouble of building ParaView completely on your own, consider running the `build-paraview.sh` script located in the `scripts` folder from inside the project's root directory to automatically build ParaView inside the binary directory.

#### CppUnit
In order to run unit tests, the [CppUnit](http://sourceforge.net/projects/cppunit/) library is required.
To install it on Debian-based systems (including Ubuntu), run `apt-get install libcppunit-dev`.

### Building Kronos
If all dependencies are installed, building Kronos is as easy as configuring it using `cmake`. Switch into a directory of your choice (we recommend not using the project's root directory, so `cmake` doesn't put auto-generated source files and other CMake-related files into the source folders). To generate the Makefiles for building Kronos, execute `cmake` in one of the following ways:
* Use `cmake [path to kronos]` directly for the non-interactive mode.
* Run `ccmake [path to kronos]` to get a console-based user interface where you can change the build options without the use of command line parameters. When done configuring, press `c` to configure CMake and when finished without errors, press `g` to generate the Makefiles.
* Run `cmake-gui [path to kronos]` to get a nice window-based user interface, similar to the `ccmake` one. Change the flags to your likings, then configure and generate the Makefiles.

When the Makefiles are done, simply run `make` to build Kronos.

To run Kronos, switch to the `src` directory inside your build folder and run `./paraview_stupro`.

### Running unit tests
To run the unit tests, go to your build directory and execute `ctest`. Alternatively, you can switch to the `src` directory in your build folder and run `../test/UnitTester [test-name]` to run specific tests only.

Licenses
--------

Kronos depends on different libraries, released under various licenses:

Dependency | License
-----------|---------
Qt4        | [Lesser General Public License Version 2.1](http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html)
CppUnit    | [Lesser General Public License Version 3](http://www.gnu.org/copyleft/lesser.html)
ParaView   | [ParaView License Version 1.2](https://github.com/Kitware/ParaView/blob/master/License_v1.2.txt)
Eigen      | [Mozilla Public License Version 2.0](https://www.mozilla.org/en-US/MPL/2.0/)
rapidjson  | [MIT License](https://github.com/miloyip/rapidjson/blob/v1.0.2/license.txt)
