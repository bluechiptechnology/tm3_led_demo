# tm3_led_demo
TM3 LED demo using SquareLine studio and LVGL
-----------------
This LED demo app was produced from source code provided by SquareLine Studio for Linux v 1.3.0

The build scripts build a Linux application based on C code exported from the SquareLine Studio.

  * To build the demo for TM3 Linux run: ./build_app_fb_arm.sh
  * To build the demo for SDL Linux run: ./build_app_sdl.sh

Code structure
---------------
There are 3 layers of code of the app:

1) LVGL library handling the grpahics
2) UI code as exported from the SquareLine studio
3) graphics backend that implements low level drawing routines used by LVGL, and the main app

All 3 of the code layers require 'lv_conf.h' file that configures the features of LVGL libray.
Currently, it is stored in the backend directories 'app_sdl' and 'app_fb' (for TM3 linux).
The build system must provide -I path where the 'lv_conf.h' exists.


**1) LVGL Lib**
------------
The library is located in 'lvgl' directory. To build the LVGL, use cmake like this:
<pre>
cd lvgl
mmkdir -p build && cd build
cmake ..
make
</pre>

This will produce liblvgl.a file in build/lib directory.

The lvgl library source coode was copied from the installation zip file
of the SquareLine Studio. Specifically from zip:/lvgl/lvgl_v8_3_6/lvgl_v8_3_6.zip


**2) UI code**
----------
The code is located in 'led_demo' directory. It is exported from within the SquareLine studio.
To build the UI code, use cmake like this:
<pre>
cd led_demo
mkdir -p build && cd build
cmake ..
make
</pre>

This will produce libui.a file in the build directory.

**3) graphics back-end and main app**
---------------------------------
This code is located in 'app_sdl' subdirectory. It was taken form the 
installation zip file of the SquareLine Studio. Specifically from:
zip:boards/Desktop/sim_sdl_eclipse_v1_0_1/sim_sdl_eclipse.zip

The actual app (for SDL backend) can be compiled like this:
<pre>
cd app_sdl
gcc -o app $CFLAGS -DUSE_SDL  main.c lv_drivers/sdl/sdl.c ../led_demo/build/libui.a ../lvgl/build/lib/libvgl.a -lSDL2
</pre>

where CFLAGS are defined as follows:
<pre>
CFLAGS="\
-I.. \
-I../test_printer \
-I$LV_CONF_INCLUDE_SIMPLE \
-DLV_CONF_INCLUDE_SIMPLE \
-D__UI_LVGL_VERSION_MAJOR__=8 \
-D__UI_LVGL_VERSION_MINOR__=3 \
-D__UI_PROJECT_HOR_RES__=800 \
-D__UI_PROJECT_VER_RES__=480 \
"
</pre>

All the above compilation steps are in the 'build_app_sdl.sh" script.
Note that __UI_PROJECT_HOR_RES__ and __UI_PROJECT_VER_RES__ must
correspond to the physical size of the screen. Also, the UI project itself
must use the same resolution. If that's not the case then
the application will have drawing artefacts (items drawn on wrong positions).
To check the design screen resolution, look into the <project>.sll json 
file (SquareLine Studio project file) which has the screen resolution defined
in 'width' and 'height' parameters.



