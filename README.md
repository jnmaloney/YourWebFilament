# YourWebFilament
How to static link Filament to your application and render to a WebGL canvas.


## Building Filament
You will need to build Filament to be able to link to it.

Use the following commands

    export EMSDK=emsdk    
    CC=/usr/bin/clang CXX=/usr/bin/clang++ CXXFLAGS="-fno-builtin" CPLUS_INCLUDE_PATH=/usr/lib/llvm-7/include/c++/v1:/usr/include/x86_64-linux-gnu/c++/9 LIBRARY_PATH=/usr/lib/llvm-7/lib ./build.sh -j -p webgl release 
    
## Building this project

This project cam be built using the `make` command and emscripten sdk.

## Initialising Filament

Check the javascript code because the GL context needs to be prepared for Filament to initialise properly.
