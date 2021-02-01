###
### System::C++::Containers
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.C++.Containers LANGUAGES CXX)

set(HEADERS
    Array.hh
    Deque.hh
    ForwardList.hh
    List.hh
    Map.hh
    Queue.hh
    Set.hh
    Stack.hh
    UnorderedMap.hh
    UnorderedSet.hh
    Vector.hh

    Private/Config.hh
    Private/Namespace.hh
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/C++/Containers/")


xvi_library(System.C++.Containers VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)

target_include_directories(System.C++.Containers
    INTERFACE   Include/
)

target_sources(System.C++.Containers
    INTERFACE   ${HEADERS}
)

target_link_libraries(System.C++.Containers
    INTERFACE   System.C++.LanguageSupport.Headers
                System.C++.Utility.Headers
)
