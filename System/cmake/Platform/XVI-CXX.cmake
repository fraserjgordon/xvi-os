include("${CMAKE_CURRENT_LIST_DIR}/XVI-C.cmake")

# Currently, support for the experimental C++20 standard is needed. The version of CMake that this file supports doesn't
# know about that standard so the flag needs to be hard-coded.
set(CMAKE_CXX_FLAGS "--std=c++2a ${CMAKE_CXX_FLAGS}")

# Support for the concepts feature is needed
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts")
