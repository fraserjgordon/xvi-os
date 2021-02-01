###
### System
###

cmake_minimum_required(VERSION 3.12)

set(CMAKE_CROSSCOMPILING ON)

include(XVI_Variants)

# Generate a compile commands file to help code editors.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Doxygen is used to extract API information for Sphinx.
find_package(Doxygen)
if (NOT Doxygen_FOUND)
  message(WARNING "Doxygen not found; cannot generate API documentation")
else()
  set(DOXYGEN_INPUT_DIR   "${CMAKE_CURRENT_SOURCE_DIR}")
  set(DOXYGEN_OUTPUT_DIR  "${CMAKE_CURRENT_BINARY_DIR}/Docs/Doxygen")
  set(DOXYGEN_INDEX_FILE  "${DOXYGEN_OUTPUT_DIR}/xml/index.xml")
  
  set(DOXYFILE_IN         "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")
  set(DOXYFILE_OUT        "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

  configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

  file(MAKE_DIRECTORY     ${DOXYGEN_OUTPUT_DIR})

  add_custom_command(
    OUTPUT                ${DOXYGEN_INDEX_FILE}
    COMMAND               ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
    VERBATIM
    MAIN_DEPENDENCY       ${DOXYFILE_OUT} ${DOXYFILE_IN}
    COMMENT               "Generating Doxygen documentation"
  )

  add_custom_target(docs-doxygen
    DEPENDS               ${DOXYGEN_INDEX_FILE}
  )
endif()

# Sphinx is used to generate the project documentation.
find_package(Sphinx)
if (NOT Sphinx_FOUND)
  message(WARNING "Sphinx not found; cannot generate documentation")
else()
  set(SPHINX_SOURCE       "${CMAKE_CURRENT_SOURCE_DIR}/Docs")
  set(SPHINX_BUILD        "${CMAKE_CURRENT_BINARY_DIR}/Docs/Sphinx")

  add_custom_target(docs-sphinx
    COMMAND             ${SPHINX_EXECUTABLE} -j auto -b html ${SPHINX_SOURCE} ${SPHINX_BUILD}
                          -Dbreathe_projects.XVI=${DOXYGEN_OUTPUT_DIR}/xml
    VERBATIM
    WORKING_DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT             "Generating Sphinx documentation"
    DEPENDS             ${DOXYGEN_INDEX_FILE}
  )
endif()

add_subdirectory(ABI)
add_subdirectory(Allocator)
add_subdirectory(Boot)
add_subdirectory(C)
add_subdirectory(C++)
add_subdirectory(Executable)
add_subdirectory(Filesystem)
add_subdirectory(Firmware)
add_subdirectory(HW)
add_subdirectory(IPC)
add_subdirectory(MPA)
add_subdirectory(Text)
#add_subdirectory(Unicode)
add_subdirectory(Utility)


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
