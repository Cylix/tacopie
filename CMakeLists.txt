###
# config
###
cmake_minimum_required(VERSION 2.8.7)
set(CMAKE_MACOSX_RPATH 1)


###
# verbose make
###
# set(CMAKE_VERBOSE_MAKEFILE TRUE)


###
# project
###
set(PROJECT cpp_http_server)
project(${PROJECT} CXX)


###
# compilation options
###
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -Wextra -O3")


###
# variables
###
set(DEPS_FOLDER ${PROJECT_SOURCE_DIR}/deps/build)
set(GTEST_INCLUDES ${DEPS_FOLDER}/gtest/include)
set(GTEST_LIBS ${DEPS_FOLDER}/gtest/lib)
set(CPP_HTTP_SERVER_INCLUDES ${PROJECT_SOURCE_DIR}/includes)


###
# includes
###
include_directories(${CPP_HTTP_SERVER_INCLUDES})


###
# link
###
link_directories(${GTEST_LIBS})


###
# sources
###
set(SRC_DIRS "sources" "sources/network" "sources/utils")
foreach(dir ${SRC_DIRS})
  # get directory sources
  file(GLOB s_${dir} "${dir}/*.cpp")
  # set sources
  set(SOURCES ${SOURCES} ${s_${dir}})
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
add_library(${PROJECT} STATIC ${SOURCES})
target_link_libraries(${PROJECT} pthread)

# __CPP_HTTP_SERVER_LOGGING_ENABLED
IF (LOGGING_ENABLED)
set_target_properties(${PROJECT} PROPERTIES COMPILE_DEFINITIONS "__CPP_HTTP_SERVER_LOGGING_ENABLED=${LOGGING_ENABLED}")
ENDIF (LOGGING_ENABLED)


###
# install
###
install (DIRECTORY ${CMAKE_BINARY_DIR}/lib/ DESTINATION lib USE_SOURCE_PERMISSIONS)
install (DIRECTORY ${CMAKE_BINARY_DIR}/bin/ DESTINATION bin USE_SOURCE_PERMISSIONS)
install (DIRECTORY ${CPP_HTTP_SERVER_INCLUDES}/ DESTINATION include USE_SOURCE_PERMISSIONS)


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
ENDIF(BUILD_TESTS)
