# MIT License
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
