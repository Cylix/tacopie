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
set(PROJECT tacopie)
project(${PROJECT} CXX)


###
# compilation options
###
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -Wextra -O3")


###
# variables
###
set(DEPS_FOLDER ${PROJECT_SOURCE_DIR}/deps/build)
IF (NOT GTEST_INCLUDES)
  set(GTEST_INCLUDES ${DEPS_FOLDER}/gtest/include)
ENDIF (NOT GTEST_INCLUDES)
IF (NOT GTEST_LIBS)
  set(GTEST_LIBS ${DEPS_FOLDER}/gtest/lib)
ENDIF (NOT GTEST_LIBS)
set(TACOPIE_INCLUDES ${PROJECT_SOURCE_DIR}/includes)


###
# includes
###
include_directories(${TACOPIE_INCLUDES})


###
# link
###
link_directories(${GTEST_LIBS})


###
# sources
###
set(SRC_DIRS "sources" "sources/network" "sources/utils" "includes/tacopie" "includes/tacopie/network" "includes/tacopie/utils")
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
add_library(${PROJECT} STATIC ${SOURCES})
target_link_libraries(${PROJECT} pthread)

# __TACOPIE_LOGGING_ENABLED
IF (LOGGING_ENABLED)
  set_target_properties(${PROJECT} PROPERTIES COMPILE_DEFINITIONS "__TACOPIE_LOGGING_ENABLED=${LOGGING_ENABLED}")
ENDIF (LOGGING_ENABLED)

# __TACOPIE_CONNECTION_QUEUE_SIZE
IF (CONNECTION_QUEUE_SIZE)
  set_target_properties(${PROJECT} PROPERTIES COMPILE_DEFINITIONS "__TACOPIE_CONNECTION_QUEUE_SIZE=${CONNECTION_QUEUE_SIZE}")
ENDIF (CONNECTION_QUEUE_SIZE)

#__TACOPIE_IO_SERVICE_NB_WORKERS
IF (IO_SERVICE_NB_WORKERS)
  set_target_properties(${PROJECT} PROPERTIES COMPILE_DEFINITIONS "__TACOPIE_IO_SERVICE_NB_WORKERS=${IO_SERVICE_NB_WORKERS}")
ENDIF(IO_SERVICE_NB_WORKERS)


###
# install
###
install (DIRECTORY ${CMAKE_BINARY_DIR}/lib/ DESTINATION lib USE_SOURCE_PERMISSIONS)
install (DIRECTORY ${CMAKE_BINARY_DIR}/bin/ DESTINATION bin USE_SOURCE_PERMISSIONS)
install (DIRECTORY ${TACOPIE_INCLUDES}/ DESTINATION include USE_SOURCE_PERMISSIONS)


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
