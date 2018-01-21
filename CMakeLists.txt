cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(GenXrdPattern)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# This should find all ObjCryst libraries
find_package(ObjCryst REQUIRED)

include_directories(${ObjCryst_INCLUDE_DIR})

# We want to build a static binary
if((UNIX OR MINGW) AND NOT APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

if(APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mmacosx-version-min=10.9")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.9")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmacosx-version-min=10.9")
endif(APPLE)

add_executable(genXrdPattern genXrdPattern.cpp)

# Link twice to prevent errors when libraries depend on each other
target_link_libraries(genXrdPattern ${ObjCryst_LIBRARIES})
target_link_libraries(genXrdPattern ${ObjCryst_LIBRARIES})
