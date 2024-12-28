This is my chess engine

## How to compile this project

1. You will need [vcpkg](https://github.com/microsoft/vcpkg), [cmake](https://cmake.org/) and [make](https://gnuwin32.sourceforge.net/packages/make.htm)
    1. Cmake is used to build make files to actually compile the project
    2. vcpkg is used to donwload all of the dependencies
    3. To actually compile the code you will need any c++ compiler. This project have been tested with clang and Visual studio compilers
> After installing vcpkg you need to provide the path to it in src\CmakeLists.txt by changing "set(CMAKE_TOOLCHAIN_FILE ...)" to the path to the file 'vcpkg.cmake'. After that, vcpkg will automatically install all of the necessary packages on project startup.

2. Dependencies:
    1. SFML - this library is used to display the graphics. Make sure to download SFML 64 bit, version 2.6
    2. NanoSVG - used to convert svg piece textures into a format, that is readable by SFML

<br/><br/>

## Как скомпилировать этот проект

1. Понадобится [vcpkg](https://github.com/microsoft/vcpkg) [cmake](https://cmake.org/)
    1. сmake нужен для созжания make файлов, которые используются для компиляции проекта\
    2. vcpkg используется для скачивания зависимостей
    3. Для того, чтобы скомпилировать код, требуется любой компилятор c++. Этот проект тестировался с clang++ и компилятором Visual Studio.

>После установки vcpkg нужно сказать Cmake где он находится. Создайте файл src/CmakeLists.txt из src/CmakeLists.txt.template и измените строчку CMAKE_TOOLCHAIN_FILE чтобы она указывала на файл vcpkg.cmake в папке установки vcpkg. После чего vcpkg автоматически установит все зависимости при сборке проекта.

2. Зависимости:
    1. SFML -- графическая библиотека.
    2. NanoSVG -- библиотека для конвертации svg файлов в растровый формат, который может прочитать SFML.

3. Для компилации проекта можно испоьзовать VS, VSCode или консоль, команды для сборки проекта в консоли
```
mkdir build
cd build
cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_BUILD_TYPE=RELEASE ../
cmake --build .
```
После компиляции проект будет лежать в папке _app/_
