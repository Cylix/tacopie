# The MIT License (MIT)
#
# Copyright (c) 2016-2017 Simon Ninon <simon.ninon@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

###
# config
###
cmake_minimum_required(VERSION 2.8.7)
set(CMAKE_MACOSX_RPATH 1)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)


###
# verbose make
###
# set(CMAKE_VERBOSE_MAKEFILE TRUE)


###
# project
###
set(PROJECT tacopie)
project(${PROJECT} CXX)


###
# compilation options
###
IF (WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /O2 /bigobj")

  # was causing conflics with gtest build
  string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

  IF ("${MSVC_RUNTIME_LIBRARY_CONFIG}" STREQUAL "")
    set(MSVC_RUNTIME_LIBRARY_CONFIG "/MT")
  ENDIF()

  foreach (flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
    IF ("${MSVC_RUNTIME_LIBRARY_CONFIG}" STREQUAL "/MT")
      string(REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    ELSEIF ("${MSVC_RUNTIME_LIBRARY_CONFIG}" STREQUAL "/MD")
      string(REPLACE "/MT" "/MD" ${flag_var} "${${flag_var}}")
    ELSE ()
      string(REPLACE "/MD" "${MSVC_RUNTIME_LIBRARY_CONFIG}" ${flag_var} "${${flag_var}}")
      string(REPLACE "/MT" "${MSVC_RUNTIME_LIBRARY_CONFIG}" ${flag_var} "${${flag_var}}")
    ENDIF()
  endforeach()

  add_definitions(-D_UNICODE)
  add_definitions(-DUNICODE)
  add_definitions(-DWIN32_LEAN_AND_MEAN)
ELSE ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -Wextra -O3")
ENDIF (WIN32)


###
# variables
###
set(DEPS_INCLUDES ${PROJECT_SOURCE_DIR}/deps/include)
set(DEPS_LIBRARIES ${PROJECT_SOURCE_DIR}/deps/lib)
set(TACOPIE_INCLUDES ${PROJECT_SOURCE_DIR}/includes)


###
# includes
###
include_directories(${TACOPIE_INCLUDES})

###
# sources
###
set(SRC_DIRS "sources" "sources/network" "sources/network/common" "sources/utils" "includes/tacopie" "includes/tacopie/network" "includes/tacopie/utils")

IF (WIN32)
  set(SRC_DIRS ${SRC_DIRS} "sources/network/windows")
ELSE ()
  set(SRC_DIRS ${SRC_DIRS} "sources/network/unix")
ENDIF (WIN32)

foreach(dir ${SRC_DIRS})
  # get directory sources and headers
  file(GLOB s_${dir} "${dir}/*.cpp")
  file(GLOB h_${dir} "${dir}/*.hpp")
  file(GLOB i_${dir} "${dir}/*.ipp")

  # set sources
  set(SOURCES ${SOURCES} ${s_${dir}} ${h_${dir}} ${i_${dir}})
endforeach()


###
# outputs
###
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


###
# executable
###
add_library(${PROJECT} ${SOURCES})
set_property(TARGET ${PROJECT} PROPERTY POSITION_INDEPENDENT_CODE ON)

IF (WIN32)
   set_target_properties(${PROJECT}
                         PROPERTIES COMPILE_PDB_NAME ${PROJECT}
                         COMPILE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
ENDIF (WIN32)

IF (WIN32)
  target_link_libraries(${PROJECT} ws2_32)
ELSE ()
  target_link_libraries(${PROJECT} pthread)
ENDIF (WIN32)

# __TACOPIE_LOGGING_ENABLED
IF (LOGGING_ENABLED)
  set_property(TARGET ${PROJECT} APPEND_STRING PROPERTY COMPILE_DEFINITIONS " __TACOPIE_LOGGING_ENABLED=${LOGGING_ENABLED}")
ENDIF (LOGGING_ENABLED)

# __TACOPIE_CONNECTION_QUEUE_SIZE
IF (CONNECTION_QUEUE_SIZE)
  set_property(TARGET ${PROJECT} APPEND_STRING PROPERTY COMPILE_DEFINITIONS " __TACOPIE_CONNECTION_QUEUE_SIZE=${CONNECTION_QUEUE_SIZE}")
ENDIF (CONNECTION_QUEUE_SIZE)

#__TACOPIE_IO_SERVICE_NB_WORKERS
IF (IO_SERVICE_NB_WORKERS)
  set_property(TARGET ${PROJECT} APPEND_STRING PROPERTY COMPILE_DEFINITIONS " __TACOPIE_IO_SERVICE_NB_WORKERS=${IO_SERVICE_NB_WORKERS}")
ENDIF(IO_SERVICE_NB_WORKERS)

#__TACOPIE_TIMEOUT
IF (SELECT_TIMEOUT)
  set_property(TARGET ${PROJECT} APPEND_STRING PROPERTY COMPILE_DEFINITIONS " __TACOPIE_TIMEOUT=${SELECT_TIMEOUT}")
ENDIF(SELECT_TIMEOUT)


###
# install
###
# ensure lib and bin directories exist
install(CODE "FILE(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})")
install(CODE "FILE(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})")
# install tacopie
install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/ DESTINATION lib USE_SOURCE_PERMISSIONS)
install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ DESTINATION bin USE_SOURCE_PERMISSIONS)
install(DIRECTORY ${TACOPIE_INCLUDES}/ DESTINATION include USE_SOURCE_PERMISSIONS)


###
# examples
###
IF (BUILD_EXAMPLES)
  add_subdirectory(examples)
ENDIF(BUILD_EXAMPLES)


###
# tests
###
IF (BUILD_TESTS)
  add_subdirectory(tests)
  ExternalProject_Add("googletest"
                      GIT_REPOSITORY "https://github.com/google/googletest.git"
                      CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/deps")
ENDIF(BUILD_TESTS)
