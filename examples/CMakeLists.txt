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
add_executable(tacopie_tcp_server tcp_server.cpp)
target_link_libraries(tacopie_tcp_server tacopie)
IF (LOGGING_ENABLED)
  set_target_properties(tacopie_tcp_server PROPERTIES COMPILE_DEFINITIONS "__TACOPIE_LOGGING_ENABLED=${LOGGING_ENABLED}")
ENDIF (LOGGING_ENABLED)

add_executable(tacopie_tcp_client tcp_client.cpp)
target_link_libraries(tacopie_tcp_client tacopie)
IF (LOGGING_ENABLED)
  set_target_properties(tacopie_tcp_client PROPERTIES COMPILE_DEFINITIONS "__TACOPIE_LOGGING_ENABLED=${LOGGING_ENABLED}")
ENDIF (LOGGING_ENABLED)

add_executable(tacopie_logger logger.cpp)
target_link_libraries(tacopie_logger tacopie)
IF (LOGGING_ENABLED)
  set_target_properties(tacopie_logger PROPERTIES COMPILE_DEFINITIONS "__TACOPIE_LOGGING_ENABLED=${LOGGING_ENABLED}")
ENDIF (LOGGING_ENABLED)
