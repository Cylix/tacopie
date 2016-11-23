###
# project
###
set(PROJECT tacopie_tests)
project(${PROJECT} CXX)


###
# compilation options
###
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


###
# includes
###
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/includes
                    ${TACOPIE_INCLUDES}
                    ${GTEST_INCLUDES})


###
# sources
###
set(SOURCES "")
set(DIRS "sources" "sources/spec" "sources/spec/**")
foreach(dir ${DIRS})
  # get directory sources
  file(GLOB s_${dir} "${dir}/*.cpp")
  # set sources
  set(SOURCES ${SOURCES} ${s_${dir}})
endforeach()


###
# executable
###
add_executable(${PROJECT} ${SOURCES})
target_link_libraries(${PROJECT} tacopie gtest pthread)
