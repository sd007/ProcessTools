cmake_minimum_required(VERSION 3.2)

set(ProjectName SharedMemory)  
project(${ProjectName})

FILE(GLOB_RECURSE writesrc "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/shareMemory/main.cpp")

FILE(GLOB_RECURSE readsrc "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/reader/main.cpp")

include_directories( "src/")

set(EXECUTABLE_OUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_executable(write ${writesrc}) 
add_executable(reade ${readsrc})

