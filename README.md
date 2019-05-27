# COFFEE
Common OpenGLES Framework For Embedded and Emulated desktop

## Overview

COFFEE is a common platform to test OpenGLES application on embedded SoC EVK and PC desktop.  
For OpenGLES emulation, I use Google ANGLE for supporting Windows/Linux/MacOS.  
Currently, EGL backend for embedded SoC hasn't been implemented yet, and only x11 would be supported.

## Requirement
This framework is to use on embedded Linux BSP, such as Yocto so that it won't depend on external libraries, unique build system.  
Some useful features for OpenGLES are provided by ANGLE. Also, a fundamental matrix and model related functions are provided by COFFEE utils.  

The COFFEE framework uses gcc for Linux and MacOS, and VisualStudio 2017 for Windows.  
The framework also uses cmake build system for both embedded and desktop environment. (Not ninja, which is required by ANGLE)

libEGL and libGLESv2 are required to build on embedded EVK. Please check it with the SDK which SoC vendor would provide.

SoC vendor would support different window systems, such as frame buffer, Wayland, and DRM/KMS.  
You need to check which backends are supported by SDK.  
(E.g. RPi BSP supports x11, but libEGL supports only framebuffer(DISPMANX), it would be overlayed on x11 Window)

## Build samples

### Linux (Embedded/Desktop) and MacOS

Please source `setup.sh` before running cmake.  
`COFFEE_ROOT` environment variable must be set to build.

```
$ . ./setup.sh
$ mkdir build && cd build
$ cmake ..
$ make
```
All assets in each sample programs are copied to build directory.

### Windows

`COFFEE_ROOT` environment variables must be set to run cmake and VS2017.  
I tested CMAKE-GUI 3.14.0-rc2 with targeting x64/VS2017 to generate COFFEE.sln  
VS2017 should build all sample binaries with the solution file.

## Screenshots

## To Do

### High priority
- Add window backend for embedded EVB
  - RPi DISPMANX support
  - Linux Wayland support
  - Linux DRM/KMS support

### Mid priority
- Clean definitions in CMakeLists.txt

### Low priority
- Add material/lighting support to the model viewer
- Add mtl material and texture support 

## License

The source codes from ANGLE project, binaries built with ANGLE, and some sample codes are governed by a BSD-style license that can be found in the LICENSE file under angle directory.

GLES2jni and GLES2jniTex are Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0

A 3-clause BSD license governs other source codes.
