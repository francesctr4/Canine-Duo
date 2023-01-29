# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6...3.19)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget drmemory_annotations drsyscall drsyscall_static drsymcache drsymcache_static umbra umbra_static drfuzz drfuzz_static)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
if(_IMPORT_PREFIX STREQUAL "/")
  set(_IMPORT_PREFIX "")
endif()

# Create imported target drmemory_annotations
add_library(drmemory_annotations STATIC IMPORTED)

# Create imported target drsyscall
add_library(drsyscall SHARED IMPORTED)

set_target_properties(drsyscall PROPERTIES
  INTERFACE_LINK_LIBRARIES "dynamorio;drmgr;drsyms;drcontainers;drfrontendlib"
)

# Create imported target drsyscall_static
add_library(drsyscall_static STATIC IMPORTED)

set_target_properties(drsyscall_static PROPERTIES
  INTERFACE_LINK_LIBRARIES "dynamorio;drmgr_static;drsyms_static;drcontainers;drfrontendlib"
)

# Create imported target drsymcache
add_library(drsymcache SHARED IMPORTED)

set_target_properties(drsymcache PROPERTIES
  INTERFACE_LINK_LIBRARIES "dynamorio;drmgr"
)

# Create imported target drsymcache_static
add_library(drsymcache_static STATIC IMPORTED)

set_target_properties(drsymcache_static PROPERTIES
  INTERFACE_LINK_LIBRARIES "dynamorio;drmgr_static"
)

# Create imported target umbra
add_library(umbra SHARED IMPORTED)

set_target_properties(umbra PROPERTIES
  INTERFACE_LINK_LIBRARIES "dynamorio;drmgr;drcontainers"
)

# Create imported target umbra_static
add_library(umbra_static STATIC IMPORTED)

set_target_properties(umbra_static PROPERTIES
  INTERFACE_LINK_LIBRARIES "dynamorio;drmgr_static;drcontainers"
)

# Create imported target drfuzz
add_library(drfuzz SHARED IMPORTED)

set_target_properties(drfuzz PROPERTIES
  INTERFACE_LINK_LIBRARIES "dynamorio;drwrap;drmgr;drcontainers;drwrap_static;drmgr_static;drcontainers;drwrap_static;drmgr_static;drcontainers"
)

# Create imported target drfuzz_static
add_library(drfuzz_static STATIC IMPORTED)

set_target_properties(drfuzz_static PROPERTIES
  INTERFACE_LINK_LIBRARIES "dynamorio;drwrap_static;drmgr_static;drcontainers"
)

if(CMAKE_VERSION VERSION_LESS 2.8.12)
  message(FATAL_ERROR "This file relies on consumers using CMake 2.8.12 or greater.")
endif()

# Load information for each installed configuration.
get_filename_component(_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file(GLOB CONFIG_FILES "${_DIR}/DRMFTarget64-*.cmake")
foreach(f ${CONFIG_FILES})
  include(${f})
endforeach()

# Cleanup temporary variables.
set(_IMPORT_PREFIX)

# Loop over all imported files and verify that they actually exist
foreach(target ${_IMPORT_CHECK_TARGETS} )
  foreach(file ${_IMPORT_CHECK_FILES_FOR_${target}} )
    if(NOT EXISTS "${file}" )
      message(FATAL_ERROR "The imported target \"${target}\" references the file
   \"${file}\"
but this file does not exist.  Possible reasons include:
* The file was deleted, renamed, or moved to another location.
* An install or uninstall procedure did not complete successfully.
* The installation package was faulty and contained
   \"${CMAKE_CURRENT_LIST_FILE}\"
but not all the files it references.
")
    endif()
  endforeach()
  unset(_IMPORT_CHECK_FILES_FOR_${target})
endforeach()
unset(_IMPORT_CHECK_TARGETS)

# Make sure the targets which have been exported in some other
# export set exist.
unset(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets)
foreach(_target "dynamorio" "drmgr" "drsyms" "drcontainers" "drfrontendlib" "drmgr_static" "drsyms_static" "drwrap" "drwrap_static" )
  if(NOT TARGET "${_target}" )
    set(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets "${${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets} ${_target}")
  endif()
endforeach()

if(DEFINED ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets)
  if(CMAKE_FIND_PACKAGE_NAME)
    set( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    set( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "The following imported targets are referenced, but are missing: ${${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets}")
  else()
    message(FATAL_ERROR "The following imported targets are referenced, but are missing: ${${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets}")
  endif()
endif()
unset(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets)

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)
