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
IF (LOGGING_ENABLED)
set_target_properties(http_server PROPERTIES COMPILE_DEFINITIONS "__CPP_HTTP_SERVER_LOGGING_ENABLED=${LOGGING_ENABLED}")
ENDIF (LOGGING_ENABLED)

add_executable(logger logger.cpp)
target_link_libraries(logger cpp_http_server)
IF (LOGGING_ENABLED)
set_target_properties(logger PROPERTIES COMPILE_DEFINITIONS "__CPP_HTTP_SERVER_LOGGING_ENABLED=${LOGGING_ENABLED}")
ENDIF (LOGGING_ENABLED)
