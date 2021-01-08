cmake_minimum_required(VERSION 3.3)
project(sthor VERSION 1.0.0 LANGUAGES C)

include(ExternalProject)

set(mtd-utils_BIN ${CMAKE_CURRENT_BINARY_DIR}/mtd-utils)
set(mtd-utils_SOURCE ${mtd-utils_BIN}/src/mtd-utils)
set(mtd-utils_INCLUDE ${mtd-utils_SOURCE}/include)

list(APPEND mtd-utils_CONFIG_ARGS 
    --target=arm-poky-linux-gnueabi 
    --host=arm-poky-linux-gnueabi 
    --build=x86_64-linux 
    --without-zstd 
    --without-ubifs 
    --disable-tests
    --enable-shared
    --prefix=${mtd-utils_BIN}
    )


#if (EXISTS ${mtd-utils_INCLUDE})
ExternalProject_Add(mtd-utils
    URL https://github.com/sigma-star/mtd-utils/archive/v2.1.2.tar.gz
    PREFIX ${mtd-utils_BIN}
    CONFIGURE_COMMAND cd ${mtd-utils_SOURCE} && ./autogen.sh && ./configure ${mtd-utils_CONFIG_ARGS}
    BUILD_COMMAND cd ${mtd-utils_SOURCE} && make
    INSTALL_COMMAND cd ${mtd-utils_SOURCE} && make install
    BUILD_BYPRODUCTS ${mtd-utils_SOURCE}/libmtd.a
    )
#endif()

include(CMakePrintHelpers)

add_executable(sthor
    src/main.c
    )

add_dependencies(sthor mtd-utils)

target_include_directories(sthor PRIVATE ${mtd-utils_INCLUDE})
target_link_libraries(sthor PRIVATE ${mtd-utils_SOURCE}/libmtd.a)

cmake_print_variables(mtd-utils_INCLUDE)
cmake_print_properties(TARGETS sthor PROPERTIES INCLUDE_DIRECTORIES LINK_DIRECTORIES)
