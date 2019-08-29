###
### System
###

cmake_minimum_required(VERSION 3.12)

set(CMAKE_CROSSCOMPILING ON)

include(XVI_Variants)

# Generate a compile commands file to help code editors.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(ABI)
add_subdirectory(Allocator)
add_subdirectory(C)
add_subdirectory(C++)
add_subdirectory(HW)
#add_subdirectory(Filesystem)
#add_subdirectory(Unicode)


##### Testing infrastructure #####

if (NOT DEFINED XVI_GTEST)
  set(XVI_GTEST OFF)
endif()

if (XVI_GTEST)
    set(BUILD_SHARED_LIBS_ORIG ${BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS ON)
    add_subdirectory(/usr/src/googletest
        ${CMAKE_CURRENT_BINARY_DIR}/googletest
    )
    set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_ORIG})

    include(CTest)

    enable_testing()
endif()
