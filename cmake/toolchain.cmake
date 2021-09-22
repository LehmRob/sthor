# CMake system name must be something like "Linux".
# This is important for cross-compiling.
set( CMAKE_SYSTEM_NAME Linux )
set( CMAKE_SYSTEM_PROCESSOR arm )

if(NOT DEFINED SDKPATH)
    # define some default sdk path
    set(SDKPATH /opt/poky/2.1)
endif()

set( SDKSYSROOT ${SDKPATH}/sysroots/cortexa9hf-neon-poky-linux-gnueabi)
set( TOOLCHAIN_PATH ${SDKPATH}/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi )
set( CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/arm-poky-linux-gnueabi-gcc )
set( CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/arm-poky-linux-gnueabi-g++ )
set( CMAKE_ASM_COMPILER ${TOOLCHAIN_PATH}/arm-poky-linux-gnueabi-gcc )
set( CMAKE_AR ${TOOLCHAIN_PATH}/arm-poky-linux-gnueabi-ar CACHE FILEPATH "Archiver" )
set( CMAKE_RANLIB ${TOOLCHAIN_PATH}/arm-poky-linux-gnueabi-ranlib CACHE FILEPATH "Ranlib" )
set( CMAKE_C_FLAGS " -march=armv7-a -mfpu=neon  -mfloat-abi=hard -mcpu=cortex-a9 --sysroot=${SDKSYSROOT} -O2 -pipe -g -feliminate-unused-debug-types " CACHE STRING "CFLAGS" )
set( CMAKE_CXX_FLAGS " -march=armv7-a -mfpu=neon  -mfloat-abi=hard -mcpu=cortex-a9 --sysroot=${SDKSYSROOT} -O2 -pipe -g -feliminate-unused-debug-types -fvisibility-inlines-hidden" CACHE STRING "CXXFLAGS" )
set( CMAKE_ASM_FLAGS " -march=armv7-a -mfpu=neon  -mfloat-abi=hard -mcpu=cortex-a9 --sysroot=${SDKSYSROOT} -O2 -pipe -g -feliminate-unused-debug-types " CACHE STRING "ASM FLAGS" )
set( CMAKE_C_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "Additional CFLAGS for release" )
set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "Additional CXXFLAGS for release" )
set( CMAKE_ASM_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "Additional ASM FLAGS for release" )
set( CMAKE_C_LINK_FLAGS " -march=armv7-a -mfpu=neon  -mfloat-abi=hard -mcpu=cortex-a9  --sysroot=${SDKSYSROOT} -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed" CACHE STRING "LDFLAGS" )
set( CMAKE_CXX_LINK_FLAGS " -march=armv7-a -mfpu=neon  -mfloat-abi=hard -mcpu=cortex-a9 --sysroot=${SDKSYSROOT} -O2 -pipe -g -feliminate-unused-debug-types -fvisibility-inlines-hidden -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed" CACHE STRING "LDFLAGS" )

# only search in the paths provided so cmake doesnt pick
# up libraries and tools from the native build machine
set( CMAKE_FIND_ROOT_PATH ${SDKSYSROOT} )
set( CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )

# We need to set the rpath to the correct directory as cmake does not provide any
# directory as rpath by default
set( CMAKE_INSTALL_RPATH  )

# add for non /usr/lib libdir, e.g. /usr/lib64
set( CMAKE_LIBRARY_PATH /usr/lib /lib)
