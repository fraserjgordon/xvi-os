###
### System::C::LanguageSupport
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.C.LanguageSupport LANGUAGES C)

set(HEADERS
    ErrorCodes.h
    IntTypes.h
    StdArg.h
    StdBool.h
    StdDef.h
    StdInt.h

    ../../../stdarg.h
    ../../../stdbool.h
    ../../../stddef.h
    ../../../stdint.h
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/C/LanguageSupport/")

xvi_library(System.C.LanguageSupport VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)

target_include_directories(System.C.LanguageSupport
    INTERFACE Include/
)

target_sources(System.C.LanguageSupport
    INTERFACE ${HEADERS}
)
