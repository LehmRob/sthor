cmake_minimum_required(VERSION 3.3)
project(sthor VERSION 1.0.0 LANGUAGES C)

list(APPEND mtd-utils_CONFIG_ARGS 
    --target=arm-poky-linux-gnueabi 
    --host=arm-poky-linux-gnueabi 
    --build=x86_64-linux 
    --without-zstd 
    --without-ubifs 
    --disable-tests
    )

include(ExternalProject)
ExternalProject_Add(mtd-utils
    GIT_REPOSITORY git://git.infradead.org/mtd-utils.git
    GIT_TAG 7b986779342021bda87c04da3bf729718736d8ab # TAG 2.1.2
	INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/mtd-utils
    CONFIGURE_COMMAND cd <SOURCE_DIR> && ./autogen.sh 
    COMMAND <SOURCE_DIR>/configure ${mtd-utils_CONFIG_ARGS}
    )

#include(FetchContent) 
#FetchContent_Declare(mtd-utils
    #GIT_REPOSITORY git://git.infradead.org/mtd-utils.git
    #GIT_TAG 7b986779342021bda87c04da3bf729718736d8ab # TAG 2.1.2
    #)

#FetchContent_GetProperties(mtd-utils)

#if (NOT mtd-utils_POPULATED)
    #FetchContent_Populate(mtd-utils)

    #add_library(mtd-libmtd 
        #${mtd-utils_SOURCE_DIR}/lib/libmtd.c
        #${mtd-utils_SOURCE_DIR}/lib/libfec.c
        #${mtd-utils_SOURCE_DIR}/lib/common.c
        #${mtd-utils_SOURCE_DIR}/lib/libcrc32.c
        #${mtd-utils_SOURCE_DIR}/lib/libmtd_legacy.c
        #${mtd-utils_SOURCE_DIR}/lib/libmtd_int.h
        #)
    #target_include_directories(mtd-libmtd PRIVATE ${mtd-utils_SOURCE_DIR}/include)
#endif()
