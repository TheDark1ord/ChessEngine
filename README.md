This is my chess engine

## How to compile this project
1. You will need vcpkg, cmake and make
    1. Cmake is used to build make files to actually compile the project
    2. vcpkg is used to donwload all of the dependencies
    3. To actually compile the code you will need any c++ compiler. This project have been tested with clang and Visual studio compilers
2. Dependencies:
    1. SFML - this library is used to display the graphics. Make sure to download SFML 64 bit, version 2.6
    2. NanoSVG - used to convert svg piece textures into a format, that is readable by SFML

> After installing vcpkg you need to provide the path to it in Project\CmakeLists.txt by changing set(CMAKE_TOOLCHAIN_FILE ...) to the path to the file vcpkg.cmake. After that, vcpkg will automatically install all of the necessary packaged while building the project

