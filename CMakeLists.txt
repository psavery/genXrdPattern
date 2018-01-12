cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(GenXrdPattern)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# This should find all ObjCryst libraries
find_package(ObjCryst REQUIRED)

include_directories(${ObjCryst_INCLUDE_DIR})

# We want to build a static binary
if(UNIX OR MINGW)
  set(CMAKE_EXE_LINKER_FLAGS "-static")
endif(UNIX OR MINGW)

add_executable(genXrdPattern genXrdPattern.cpp)

# Link twice to prevent errors when libraries depend on each other
target_link_libraries(genXrdPattern ${ObjCryst_LIBRARIES})
target_link_libraries(genXrdPattern ${ObjCryst_LIBRARIES})
