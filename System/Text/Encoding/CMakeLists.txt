###
### System::Text::Encoding
###

cmake_minimum_required(VERSION 3.12)

project(Syste.Text.Encoding LANGUAGES CXX)

set(HEADERS
    Codec.hh
    Decoder.hh
    EncodedString.hh
    Encoder.hh
    EncodingFactory.hh
    EncodingID.hh
    EncodingInfo.hh
    Iterator.hh
)

set(UTF8_SOURCES
    UTF-8/UTF-8.hh
    UTF-8/UTF-8.cc
)

set(UTF16_SOURCES
    UTF-16/UTF-16.hh
    UTF-16/UTF-16.cc
)

set(UTF32_SOURCES
    UTF-32/UTF-32.hh
    UTF-32/UTF-32.cc
)

set(ASCII_SOURCES
    ASCII/ASCII.hh
    ASCII/ASCII.cc
)

set(ISO_8859_1_SOURCES
    ISO-8859-1/ISO-8859-1.hh
    ISO-8859-1/ISO-8859-1.cc
)

set(SOURCES
    CodecImpl.hh
    DecoderImpl.hh
    EncoderImpl.hh
    EncodingFactoryImpl.hh
    InfoTable.hh
    IteratorImpl.hh

    Codec.cc
    EncodingFactory.cc
    InfoTable.cc
    Iterator.cc
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/Text/Encoding/")
list(TRANSFORM SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Text/Encoding/")

list(TRANSFORM UTF8_SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Text/Encoding/")
list(TRANSFORM UTF16_SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Text/Encoding/")
list(TRANSFORM UTF32_SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Text/Encoding/")
list(TRANSFORM ASCII_SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Text/Encoding/")
list(TRANSFORM ISO_8859_1_SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Text/Encoding/")

# Main library.
set(SYSTEM_TEXT_ENCODING_VERSION_MAJOR      1)
set(SYSTEM_TEXT_ENCODING_VERSION_MINOR      0)
set(SYSTEM_TEXT_ENCODING_VERSION_REVISION   0)
set(V ${SYSTEM_TEXT_ENCODING_VERSION_MAJOR}.${SYSTEM_TEXT_ENCODING_VERSION_MINOR}.${SYSTEM_TEXT_ENCODING_VERSION_REVISION})
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Include/System/Text/Encoding/Private/Config.hh.in" "${CMAKE_CURRENT_BINARY_DIR}/Include/System/Text/Encoding/Private/Config.hh" @ONLY)
xvi_library(System.Text.Encoding        VERSION ${V} STATIC SHARED KERNEL)

# Unicode UTF modules.
xvi_library(System.Text.Encoding.UTF8   VERSION 1.0.0 STATIC SHARED KERNEL)
xvi_library(System.Text.Encoding.UTF16  VERSION 1.0.0 STATIC SHARED KERNEL)
xvi_library(System.Text.Encoding.UTF32  VERSION 1.0.0 STATIC SHARED KERNEL)

# Other Unicode modules.

# Legacy 7-/8-bit encodings.
xvi_library(System.Text.Encoding.ASCII          VERSION 1.0.0 STATIC SHARED KERNEL)
xvi_library(System.Text.Encoding.ISO_8859_1     VERSION 1.0.0 STATIC SHARED KERNEL)


macro(encoding_module name)
    target_sources(System.Text.Encoding.${name}+All PRIVATE ${ARGN} ${HEADERS})
    target_include_directories(System.Text.Encoding.${name}+All PUBLIC Include/ PRIVATE Source/ "${CMAKE_CURRENT_BINARY_DIR}/Include/")
    target_link_libraries(System.Text.Encoding+All PRIVATE System.Text.Encoding.${name})
endmacro(encoding_module)


encoding_module(UTF8 ${UTF8_SOURCES})
encoding_module(UTF16 ${UTF16_SOURCES})
encoding_module(UTF32 ${UTF32_SOURCES})
encoding_module(ASCII ${ASCII_SOURCES})
encoding_module(ISO_8859_1 ${ISO_8859_1_SOURCES})


target_sources(System.Text.Encoding+All
    PRIVATE ${SOURCES}
)

target_include_directories(System.Text.Encoding+All
    PUBLIC      Include/
    PRIVATE     Source/
                "${CMAKE_CURRENT_BINARY_DIR}/Include/"
)
