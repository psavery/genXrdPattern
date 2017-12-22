# Copyright (c) 2017 Patrick Avery
# - Find ObjCryst and its libraries
# ObjCryst_INCLUDE_DIR - Where to find ObjCryst header files (directory)
# ObjCryst_LIBRARIES - Where to find ObjCryst libraries (files)
# ObjCryst_FOUND - Set to TRUE if we found everything (libraries and includes)

# Libraries:
#   libcryst.a
#   libCrystVector.a
#   libRefinableObj.a
#   libQuirks.a
#   libcctbx.a
#   libnewmat.a

# If ObjCryst_ROOT is set, it will be searched

set(_toFind_ObjCryst_LIBRARIES
    libnewmat.a
    libcctbx.a
    libCrystVector.a
    libQuirks.a
    libRefinableObj.a
    libcryst.a
)

if(ObjCryst_INCLUDE_DIR AND ObjCryst_LIBRARIES)
  set(ObjCryst_FOUND TRUE)
  return()
endif()

set(_include_search_paths "")
set(_library_search_paths "")

# Add search paths as needed
if(UNIX)
  set(_include_search_paths
      /usr/include
      /usr/local/include
  )
  set(_library_search_paths
      /usr/lib
      /usr/local/lib
  )
endif(UNIX)

find_path(ObjCryst_INCLUDE_DIR ObjCryst/ObjCryst/Atom.h
          HINTS ${ObjCryst_ROOT}
          PATHS ${_include_search_paths})

# Loop through each library
foreach(library ${_toFind_ObjCryst_LIBRARIES})
  find_library(lib ${library}
               HINTS ${ObjCryst_ROOT}/static-libs/lib
                     ${ObjCryst_INCLUDE_DIR}/static-libs/lib
                     ${ObjCryst_ROOT}/*
                     ${ObjCryst_INCLUDE_DIR}/*
                     ${ObjCryst_ROOT}/ObjCryst/*
                     ${ObjCryst_INCLUDE_DIR}/ObjCryst/*
               PATHS ${_include_search_paths})

  if("${lib}" STREQUAL "lib-NOTFOUND")
    message(SEND_ERROR "Could not find ${library}")
    set(ObjCryst_LIBRARIES "")
    break()
  endif()

  set(ObjCryst_LIBRARIES ${ObjCryst_LIBRARIES} ${lib})
  unset(lib CACHE)
endforeach()

if(ObjCryst_INCLUDE_DIR AND ObjCryst_LIBRARIES)
  set(ObjCryst_FOUND TRUE)
endif()

if(ObjCryst_FOUND)
  message(STATUS "Found ObjCryst include dir: ${ObjCryst_INCLUDE_DIR}")
  message(STATUS "Found ObjCryst libraries: ${ObjCryst_LIBRARIES}")
else(ObjCryst_FOUND)
  if(ObjCryst_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find ObjCryst")
  else(ObjCryst_FIND_REQUIRED)
    message(STATUS "Optional package ObjCryst was not found")
  endif(ObjCryst_FIND_REQUIRED)
endif(ObjCryst_FOUND)
