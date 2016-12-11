###
# compilation options
###
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


###
# includes
###
include_directories(${PROJECT_SOURCE_DIR}/includes
                    ${TACOPIE_INCLUDES})


###
# executable
###
add_executable(tcp_server tcp_server.cpp)
target_link_libraries(tcp_server tacopie)
IF (LOGGING_ENABLED)
set_target_properties(tcp_server PROPERTIES COMPILE_DEFINITIONS "__TACOPIE_LOGGING_ENABLED=${LOGGING_ENABLED}")
ENDIF (LOGGING_ENABLED)

add_executable(tcp_client tcp_client.cpp)
target_link_libraries(tcp_client tacopie)
IF (LOGGING_ENABLED)
set_target_properties(tcp_client PROPERTIES COMPILE_DEFINITIONS "__TACOPIE_LOGGING_ENABLED=${LOGGING_ENABLED}")
ENDIF (LOGGING_ENABLED)

add_executable(logger logger.cpp)
target_link_libraries(logger tacopie)
IF (LOGGING_ENABLED)
set_target_properties(logger PROPERTIES COMPILE_DEFINITIONS "__TACOPIE_LOGGING_ENABLED=${LOGGING_ENABLED}")
ENDIF (LOGGING_ENABLED)
