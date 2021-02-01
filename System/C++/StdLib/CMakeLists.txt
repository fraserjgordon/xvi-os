###
### System::C++::StdLib
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.C++.StdLib LANGUAGES CXX)

set(HEADERS
    algorithm
    any
    array
    atomic
    barrier
    bit
    bitset
    cassert
    cctype
    cerrno
    cfenv
    cfloat
    charconv
    chrono
    cinttypes
    climits
    clocale
    cmath
    codecvt
    compare
    complex
    concepts
    condition_variable
    coroutine
    csetjmp
    csignal
    cstdarg
    cstddef
    cstdint
    cstdio
    cstdlib
    cstring
    ctime
    cuchar
    cwchar
    cwctype
    deque
    exception
    execution
    filesystem
    format
    forward_list
    fstream
    functional
    future
    initializer_list
    iomanip
    ios
    iosfwd
    iostream
    istream
    iterator
    latch
    limits
    list
    locale
    map
    memory
    memory_resource
    mutex
    new
    numbers
    numeric
    optional
    ostream
    queue
    random
    ranges
    ratio
    regex
    scoped_allocator
    semaphore
    set
    shared_mutex
    source_location
    span
    sstream
    stack
    stdexcept
    stop_token
    streambuf
    string
    string_view
    strstream
    syncstream
    system_error
    thread
    tuple
    type_traits
    typeindex
    typeinfo
    unordered_map
    unordered_set
    utility
    valarray
    variant
    vector
    version
)

set(COMPILE_TEST_SOURCES
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/")
list(TRANSFORM COMPILE_TEST_SOURCES PREPEND "Test/System/C++/StdLib/")

xvi_library(System.C++.StdLib.Headers VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)
xvi_library(System.C++.StdLib VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)

target_include_directories(System.C++.StdLib.Headers
    INTERFACE Include/
)

target_sources(System.C++.StdLib.Headers
    INTERFACE ${HEADERS}
)

target_link_libraries(System.C++.StdLib
    INTERFACE System.C++.StdLib.Headers
              System.ABI.C++
              System.C++.Atomic
              System.C++.Chrono
              System.C++.Containers
              System.C++.Format
              System.C++.LanguageSupport
              System.C++.Locale
              System.C++.Runtime
              System.C++.Threads
              System.C++.TypeTraits
)
