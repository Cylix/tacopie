###
# compilation options
###
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


###
# includes
###
include_directories(${PROJECT_SOURCE_DIR}/includes
                    ${CPP_HTTP_SERVER_INCLUDES})


###
# executable
###
add_executable(http_server http_server.cpp)
target_link_libraries(http_server cpp_http_server)
