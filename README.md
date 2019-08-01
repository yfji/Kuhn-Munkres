# Kuhn Munkres algorithm for weighted bipartite graph matching
This is a C++ implement of the Kuhn Munkres (KM) algorithm. Besides, we provide a Python interface named KM.pyd compiled using VS2017 and Python3.6.

## Usage
* C++
* Python

## Compile with Visual Studio
* Build a DLL project in VS
* Copy the `km.h` and `km.cpp` into your source folder
* Add the Python include and library path to the peoject. The Python36.lib is used.
* Modify the project property. Set the target file name to `KM` and the target extension to `pyd`
* Build, then you get `xx.lib` and `KM.pyd`