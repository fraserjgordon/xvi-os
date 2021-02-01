add_library(__Xvi INTERFACE)
add_library(__XviStatic INTERFACE)
add_library(__XviShared INTERFACE)
add_library(__XviKernel INTERFACE)

include(Common)

target_link_libraries(__XviKernel INTERFACE __Xvi)
target_link_libraries(__XviShared INTERFACE __Xvi)
target_link_libraries(__XviStatic INTERFACE __Xvi)

include(Kernel)
include(Shared)
include(Static)

macro(__xvi_library_clone FROM TO)
  # TODO: these should be $<GENEX_EVAL:$<TARGET_PROPERTY:...>>
  #
  target_compile_definitions(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_COMPILE_DEFINITIONS>
    PRIVATE   $<TARGET_PROPERTY:${FROM},COMPILE_DEFINITIONS>
  )

  target_compile_features(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_COMPILE_FEATURES>
    PRIVATE   $<TARGET_PROPERTY:${FROM},COMPILE_FEATURES>
  )

  target_compile_options(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_COMPILE_OPTIONS>
    PRIVATE   $<TARGET_PROPERTY:${FROM},COMPILE_OPTIONS>
  )

  target_include_directories(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_INCLUDE_DIRECTORIES>
    PRIVATE   $<TARGET_PROPERTY:${FROM},INCLUDE_DIRECTORIES>
  )

  target_sources(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_SOURCES>
    PRIVATE   $<TARGET_PROPERTY:${FROM},SOURCES>
  )

  target_link_libraries(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_LINK_LIBRARIES>
    PRIVATE   $<TARGET_PROPERTY:${FROM},LINK_LIBRARIES>
  )

  target_link_directories(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_LINK_DIRECTORIES>
    PRIVATE   $<TARGET_PROPERTY:${FROM},LINK_DIRECTORIES>
  )

  target_link_options(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_LINK_OPTIONS>
    PRIVATE   $<TARGET_PROPERTY:${FROM},LINK_OPTIONS>
  )
endmacro(__xvi_library_clone)

macro(__xvi_library_clone_interface FROM TO)
  # TODO: these should be $<GENEX_EVAL:$<TARGET_PROPERTY:...>>
  #
  target_compile_definitions(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_COMPILE_DEFINITIONS>
  )

  target_compile_features(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_COMPILE_FEATURES>
  )

  target_compile_options(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_COMPILE_OPTIONS>
  )

  target_include_directories(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_INCLUDE_DIRECTORIES>
  )

  target_sources(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_SOURCES>
  )

  target_link_libraries(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_LINK_LIBRARIES>
  )

  target_link_directories(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_LINK_DIRECTORIES>
  )

  target_link_options(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_LINK_OPTIONS>
  )
endmacro(__xvi_library_clone_interface)

function(__xvi_parse_version VER MAJ MIN REV)
  string(REPLACE "." ";" VER_LIST "${VER}")
  list(GET VER_LIST 0 VER_MAJ)
  list(GET VER_LIST 1 VER_MIN)
  list(GET VER_LIST 2 VER_REV)
  set(${MAJ} ${VER_MAJ} PARENT_SCOPE)
  set(${MIN} ${VER_MIN} PARENT_SCOPE)
  set(${REV} ${VER_REV} PARENT_SCOPE)
endfunction(__xvi_parse_version)

macro(__xvi_library_variant FROM VARIANT LIBTYPE VERSION)
  # Extract ABI information from the version (major and minor part).
  __xvi_parse_version(${VERSION} VERSION_MAJOR VERSION_MINOR VERSION_REV)

  # Kernel variants replace the leading "System." with "Kernel.".
  if("${VARIANT}" STREQUAL "Kernel")
    string(REGEX REPLACE "^System\\." "Kernel." name ${FROM})
    set(output_name "${name}-${VERSION_MAJOR}.${VERSION_MINOR}")
  else()
    set(name "${FROM}+${VARIANT}")
    set(output_name "${FROM}-${VERSION_MAJOR}.${VERSION_MINOR}")
  endif()

  if ("${LIBTYPE}" STREQUAL "STATIC")
    set(output_name "${output_name}.${VERSION_REV}")
  endif()

  add_library(${name} ${LIBTYPE})

  #if("${LIBTYPE}" STREQUAL "SHARED")
  #  set_target_properties(${name} PROPERTIES
  #    VERSION   ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REV}
  #    SOVERSION ${VERSION_MAJOR}.${VERSION_MINOR}
  #  )
  #endif()

  if(NOT ${LIBTYPE} STREQUAL "INTERFACE")
    set_target_properties(${name} PROPERTIES
        OUTPUT_NAME ${output_name}
    )
  endif()

  set_target_properties(${name} PROPERTIES
    INTERFACE_XVI_VARIANT ${VARIANT}
    INTERFACE_VERSION     ${VERSION}
  )

  if(${LIBTYPE} STREQUAL "INTERFACE")
    __xvi_library_clone_interface(${FROM} "${name}")
  else()
    __xvi_library_clone("${FROM}+All" "${name}")
    target_link_libraries("${name}"
      PRIVATE "__Xvi${VARIANT}"
    )
    if ("${VARIANT}" STREQUAL "Shared")
      __xvi_library_clone("${FROM}+SharedLibs" "${name}")
      __xvi_library_clone("${FROM}+NonKernel"  "${name}")
    elseif("${VARIANT}" STREQUAL "Kernel")
      __xvi_library_clone("${FROM}+StaticLibs" "${name}")
    else()
      __xvi_library_clone("${FROM}+StaticLibs" "${name}")
      __xvi_library_clone("${FROM}+NonKernel"  "${name}")
    endif()
  endif()

  if("${VARIANT}" STREQUAL "Kernel")
    add_library("${FROM}+Kernel" ALIAS ${name})
  endif()

  if(NOT ${LIBTYPE} STREQUAL "INTERFACE")
    get_target_property(variants ${FROM}+All XVI_VARIANTS)
    list(APPEND variants ${VARIANT})
    set_target_properties(${FROM}+All PROPERTIES XVI_VARIANTS "${variants}")
  endif()
endmacro(__xvi_library_variant)

function(__xvi_titlecase out_var str)
  string(SUBSTRING ${variant} 0 1 first)
  string(SUBSTRING ${variant} 1 -1 rest)
  string(TOUPPER ${first} first)
  string(TOLOWER ${rest} rest)
  set(${out_var} ${first}${rest} PARENT_SCOPE)
endfunction(__xvi_titlecase)

macro(__xvi_interface_library name interface)
  set(variants INTERFACE KERNEL)
  set(options ${variants} NO_STDLIB)
  set(one_value_args VERSION)
  set(multi_value_args)
  cmake_parse_arguments(XVI_LIBRARY "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

  add_library(${name} INTERFACE)

  # Default to depending on the C++ standard library.
  #if(NOT XVI_LIBRARY_NO_STDLIB)
  #  target_link_libraries(${name}
  #    INTERFACE System.C++.StdLib
  #  )
  #endif()
  
  foreach(variant IN ITEMS ${ARGN})
    # Ensure the variant name is in titlecase.
    __xvi_titlecase(variant ${variant})

      # Version is irrelevant as no binaries are generated.
    __xvi_library_variant(${name} ${variant} INTERFACE 1.0.0)
  endforeach()
endmacro(__xvi_interface_library)

macro(__xvi_library name)
  set(variants STATIC SHARED KERNEL)
  set(options ${variants} NO_STDLIB)
  set(one_value_args VERSION)
  set(multi_value_args)
  cmake_parse_arguments(XVI_LIBRARY "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

  if(NOT DEFINED XVI_LIBRARY_VERSION)
    message(WARNING "Version not defined for library ${name}; defaulting to 1.0.0")
    set(XVI_LIBRARY_VERSION 1.0.0)
  endif()

  add_library(${name} INTERFACE)
  add_library(${name}+All OBJECT EXCLUDE_FROM_ALL)
  add_library(${name}+StaticLibs OBJECT EXCLUDE_FROM_ALL)
  add_library(${name}+SharedLibs OBJECT EXCLUDE_FROM_ALL)
  add_library(${name}+NonKernel  OBJECT EXCLUDE_FROM_ALL)

  # Workarounds to let CMake cope with the (empty) object libraries used as aliases.
  target_sources(${name}+StaticLibs PRIVATE ${CMAKE_SOURCE_DIR}/cmake/Dummy.txt)
  target_sources(${name}+SharedLibs PRIVATE ${CMAKE_SOURCE_DIR}/cmake/Dummy.txt)
  target_sources(${name}+NonKernel  PRIVATE ${CMAKE_SOURCE_DIR}/cmake/Dummy.txt)
    
  set_target_properties(${name}+StaticLibs PROPERTIES LINKER_LANGUAGE CXX)
  set_target_properties(${name}+SharedLibs PROPERTIES LINKER_LANGUAGE CXX)
  set_target_properties(${name}+NonKernel  PROPERTIES LINKER_LANGUAGE CXX)

  # Default to depending on the C++ standard library.
  if(NOT XVI_LIBRARY_NO_STDLIB)
    target_link_libraries(${name}+All
      PRIVATE System.C++.StdLib
    )
  else()
    target_compile_definitions(${name}+All
      PRIVATE __XVI_NO_STDLIB
    )
  endif()

  foreach(variant IN ITEMS ${variants})
    if(XVI_LIBRARY_${variant})
      # Ensure the variant name is in titlecase.
      __xvi_titlecase(variant ${variant})

      if(variant STREQUAL "Static" OR variant STREQUAL "Kernel")
        __xvi_library_variant(${name} ${variant} STATIC ${XVI_LIBRARY_VERSION})
      else()
        __xvi_library_variant(${name} ${variant} SHARED ${XVI_LIBRARY_VERSION})
      endif()
    endif()
  endforeach()

  target_link_libraries(${name}
    INTERFACE $<IF:$<STREQUAL:$<TARGET_PROPERTY:INTERFACE_XVI_VARIANT>,>,,${name}+$<TARGET_PROPERTY:INTERFACE_XVI_VARIANT>>
  )

  # If there is no shared variant, make static libs depend on the static variant.
  if ("Static" IN_LIST variants AND NOT "Shared" IN_LIST variants)
    add_library(${name}+Shared INTERFACE)
    target_link_libraries(${name}+Shared INTERFACE ${name}+Static)
  endif()

  __xvi_library_clone_interface(${name}+All ${name})
endmacro(__xvi_library)

macro(xvi_library name)
  set(options INTERFACE STATIC SHARED KERNEL NO_STDLIB)
  set(one_value_args VERSION)
  set(multi_value_args)
  cmake_parse_arguments(XVI_LIBRARY "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})
  if(XVI_LIBRARY_INTERFACE)
    __xvi_interface_library(${name} ${ARGN} STATIC SHARED)
  else()
    __xvi_library(${name} ${ARGN})
  endif()
endmacro(xvi_library)

macro(xvi_executable name)
  set(options BOOTLOADER)
  set(one_value_args VERSION)
  set(multi_value_args)
  cmake_parse_arguments(XVI_EXECUTABLE "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

  add_executable(${name})

  target_link_libraries(${name}
    PRIVATE __Xvi
  )

  if(XVI_EXECUTABLE_BOOTLOADER)
    set_target_properties(${name} PROPERTIES INTERFACE_XVI_VARIANT Kernel)
    target_link_libraries(${name}
      PRIVATE Kernel.C++.StdLib
    )
  else()
  set_target_properties(${name} PROPERTIES INTERFACE_XVI_VARIANT Shared)
    target_link_libraries(${name}
      PRIVATE System.C++.StdLib+Shared
    )
  endif()
endmacro(xvi_executable)

macro(xvi_target_property NAME)
  get_target_property(variants ${NAME}+ XVI_VARIANTS)
  foreach(i ${variants})
    set_target_properties("${NAME}+${i}" ${ARGN})
  endforeach()
endmacro(xvi_target_property)
