cmake_policy(SET CMP0048 NEW)
cmake_minimum_required(VERSION 3.12)
project(sthor VERSION 1.0.0)

add_compile_options(-Wall -Wextra -pedantic -Werror)
set(C_STANDARD C11)

include(ExternalProject)

set(mtd-utils_BIN ${CMAKE_CURRENT_BINARY_DIR}/mtd-utils)
set(mtd-utils_SOURCE ${mtd-utils_BIN}/src/mtd-utils)
set(mtd-utils_INCLUDE ${mtd-utils_SOURCE}/include)

list(APPEND mtd-utils_CONFIG_ARGS 
    --host=arm-poky-linux-gnueabi
    --target=arm-poky-linux-gnueabi
    --without-jffs
    --without-ubifs
    --disable-tests
    --disable-shared
    --prefix=${mtd-utils_BIN}
    )
list(APPEND mtd-utils_COMPILER_FLAGS
    "CC=${CMAKE_C_COMPILER}"
    "CFLAGS=${CMAKE_C_FLAGS}"
    "CXX=${CMAKE_CXX_COMPILER}"
    "AR=${CMAKE_AR}"
    "RANLIB=${CMAKE_RANLIB}"
    )

ExternalProject_Add(mtd-utils
    URL https://github.com/sigma-star/mtd-utils/archive/v1.5.2.tar.gz
    PREFIX ${mtd-utils_BIN}
    #CONFIGURE_COMMAND cd ${mtd-utils_SOURCE} && ./autogen.sh && ./configure ${mtd-utils_CONFIG_ARGS}
    CONFIGURE_COMMAND cd ${mtd-utils_SOURCE}
    BUILD_COMMAND cd ${mtd-utils_SOURCE} && make ${mtd-utils_COMPILER_FLAGS}
    INSTALL_COMMAND cd ${mtd-utils_SOURCE}
    BUILD_BYPRODUCTS ${mtd-utils_SOURCE}/lib/libmtd.a
    )

include(CMakePrintHelpers)

add_executable(${PROJECT_NAME} 
    main.c
    flash.c
    )
add_dependencies(${PROJECT_NAME} mtd-utils)

target_include_directories(${PROJECT_NAME} PRIVATE ${mtd-utils_INCLUDE})
target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${mtd-utils_SOURCE}/lib/libmtd.a 
    ${mtd-utils_SOURCE}/ubi-utils/libubi.a
    )
