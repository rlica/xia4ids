# Xia4IDS changelog tracking

_Note:_  This changelog started after the main update of the Jakku version of the Xia4IDS code (August 2023)

## Jakku version - October 2023

 - _Version of the code:_ Jakku (2023)
 - _Contributors:_            Andres Illana (UCM)

### Notes

We observed that in C++ architectures higher than c++11, the code crashed. Now, after some debugging, the code should work with architecture c++14 and c++17


### Changes

1. Removing "using namespace std;" in some files: /inc/Unpacker.h, ./inc/XiaListModeDataMask.h, /inc/Unpacker.cpp, and ./inc/XiaListModeDataMask.cpp
2. Rewriting part of the xia4ids.cpp code. Moving from C to C++ standards
3. Adding more cards to the Unpacker.cpp config
4. Using only Double_t for the energy (root part) and removing Float_t types for Double_t type
5. Renameing format (int) variables. Adding the word: "_format". Now they are `int corr_format, list_format, gasp_format, root_format, stat_format, rate_format, rate_root_format;`
6. Makefile updated
7. CMake option available
8. We create a folder, "MakeOptions", where the two ways for compiling xia4ids are saved. (Makefile and CMake). The CMakeList includes 2 scripts for assisting the user with the installation.
9. Creating the CHANGELOG file for tracking all the changes.


## Jakku version - November 2023

 - _Version of the code:_ Jakku (2023)
 - _Contributors:_            Andres Illana (UCM)

### Changes

1. Changing the random generator in ./inc/calibrate.hh to TRandom3 standar for better binning
2. Makefile upadted for c++17 standards
