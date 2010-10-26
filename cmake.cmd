rmdir /S /Q cmake
rmdir /S /Q build
mkdir cmake
mkdir build
cd cmake
cmake.exe -G "Visual Studio 9 2008" ../
rem cmake.exe -G "Visual Studio 9 2008 Win64" ../
pause