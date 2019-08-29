add_library(__XviStatic INTERFACE)
add_library(__XviShared INTERFACE)
add_library(__XviKernel INTERFACE)

include(Kernel)

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
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_LINK_LIBRARIES
  )

  target_link_directories(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_LINK_DIRECTORIES>
  )

  target_link_options(${TO}
    INTERFACE $<TARGET_PROPERTY:${FROM},INTERFACE_LINK_OPTIONS>
  )
endmacro(__xvi_library_clone_interface)

macro(__xvi_library_variant FROM VARIANT LIBTYPE)
  # Kernel variants replace the leading "System." with "Kernel.".
  if("${VARIANT}" STREQUAL "Kernel")
    string(REGEX REPLACE "^System\\." "Kernel." name ${FROM})
    set(output_name ${name})
  else()
    set(name "${FROM}+${VARIANT}")
    set(output_name ${FROM})
  endif()

  add_library(${name} ${LIBTYPE})

  if(NOT ${LIBTYPE} STREQUAL "INTERFACE")
    set_target_properties(${name} PROPERTIES
        OUTPUT_NAME ${output_name}
    )
  endif()

  set_target_properties(${name} PROPERTIES
    INTERFACE_XVI_VARIANT ${VARIANT}
  )

  if(${LIBTYPE} STREQUAL "INTERFACE")
    __xvi_library_clone_interface(${FROM}+ "${name}")
  else()
    __xvi_library_clone(${FROM}+ "${name}")
    target_link_libraries("${name}"
      PRIVATE "__Xvi${VARIANT}"
    )
  endif()

  if("${VARIANT}" STREQUAL "Kernel")
    add_library("${FROM}+Kernel" ALIAS ${name})
  endif()

  if(NOT ${LIBTYPE} STREQUAL "INTERFACE")
    get_target_property(variants ${FROM}+ XVI_VARIANTS)
    list(APPEND variants ${VARIANT})
    set_target_properties(${FROM}+ PROPERTIES XVI_VARIANTS "${variants}")
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
  add_library(${name} INTERFACE)
  
  foreach(variant IN ITEMS ${ARGN})
    # Ensure the variant name is in titlecase.
    __xvi_titlecase(variant ${variant})

    __xvi_library_variant(${name} ${variant} INTERFACE)
  endforeach()

  target_link_libraries(${name}
    INTERFACE ${name}+$<TARGET_PROPERTY:INTERFACE_XVI_VARIANT>
  )
endmacro(__xvi_interface_library)

macro(__xvi_library name)
  add_library(${name} INTERFACE)
  add_library(${name}+ OBJECT EXCLUDE_FROM_ALL)

  foreach(variant IN ITEMS ${ARGN})
    # Ensure the variant name is in titlecase.
    __xvi_titlecase(variant ${variant})

    if(variant STREQUAL "Static" OR variant STREQUAL "Kernel")
      __xvi_library_variant(${name} ${variant} STATIC)
    else()
      __xvi_library_variant(${name} ${variant} SHARED)
    endif()
  endforeach()

  target_link_libraries(${name}
    INTERFACE ${name}+$<TARGET_PROPERTY:INTERFACE_XVI_VARIANT>
  )
  __xvi_library_clone_interface(${name}+ ${name})
endmacro(__xvi_library)

macro(xvi_library name)
  if(${ARGC} GREATER 1 AND ${ARGV1} STREQUAL "INTERFACE")
    __xvi_interface_library(${name} ${ARGN} STATIC SHARED)
  else()
    __xvi_library(${name} ${ARGN})
  endif()
endmacro(xvi_library)

macro(xvi_target_property NAME)
  get_target_property(variants ${NAME}+ XVI_VARIANTS)
  foreach(i ${variants})
    set_target_properties("${NAME}+${i}" ${ARGN})
  endforeach()
endmacro(xvi_target_property)
