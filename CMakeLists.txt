
cmake_minimum_required (VERSION 2.8)
if (NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build configure." FORCE)
endif ()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin/ CACHE STRING "" FORCE)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin/ CACHE STRING "" FORCE)

project (video_VH)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include"
                    "${CMAKE_CURRENT_SOURCE_DIR}/src")


#testbed for extract key frames
find_package (OpenCV REQUIRED)
include_directories (${OpenCV_INCLUDE_DIRS})

set (target "extractKF")
file(GLOB file_incs "src/*.h" "src/*.hpp" "include/*.h" "include/*.hpp")
file(GLOB file_srcs "src/*.c" "src/*.cpp")
add_executable(${target} ${file_incs} ${file_srcs} ./testbed/extractKF.cpp )
target_link_libraries(${target} ${OpenCV_LIBS})
