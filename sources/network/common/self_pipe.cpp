// MIT License
//
// Copyright (c) 2016-2017 Simon Ninon <simon.ninon@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include <tacopie/utils/typedefs.hpp>
#include <tacopie/network/self_pipe.hpp>
#include <tacopie/utils/error.hpp>
#include <tacopie/network/socket_utils.hpp>

#ifdef H_OS_WINDOWS
#include <Winsock2.h>
#include <iostream>
#else
#include <unistd.h>
#endif
#include <fcntl.h>

namespace tacopie {

		//!
		//! ctor & dtor
		//!
		self_pipe::self_pipe(void)
		 {
#ifdef H_OS_WINDOWS
			//! Create a server
			m_fd = ::socket(AF_INET, SOCK_DGRAM, 0);
			if (m_fd == __TACOPIE_INVALID_FD) { __TACOPIE_THROW(error, "fail socket()"); }

			socket_utils::socket_set_nobio(m_fd);
			
			//! Bind server to localhost
			struct sockaddr_in inaddr;
			memset(&inaddr, 0, sizeof(inaddr));
			inaddr.sin_family = AF_INET;
			inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
			inaddr.sin_port = 0;
			struct sockaddr*  addr = (struct sockaddr*)&inaddr;
			if (bind(m_fd, addr, sizeof(*addr)) == SOCKET_ERROR) { __TACOPIE_THROW(error, "fail bind()"); }

			//! Retrieve server information
			m_addr_len = sizeof(m_addr);
			memset(&m_addr, 0, sizeof(m_addr));
			if (getsockname(m_fd, &m_addr, &m_addr_len) == SOCKET_ERROR) { __TACOPIE_THROW(error, "fail getsockname()"); }

			//! connect read fd to the server
			if (connect(m_fd, &m_addr, m_addr_len) == SOCKET_ERROR) { __TACOPIE_THROW(error, "fail connect()"); }
#else
			m_fds[0] =  __TACOPIE_INVALID_FD, m_fds[1] =  __TACOPIE_INVALID_FD ;
            if (pipe(m_fds) == -1) { __TACOPIE_THROW(error, "pipe() failure"); }
#endif
		}

		self_pipe::~self_pipe(void) {
#ifdef H_OS_WINDOWS
			if (m_fd != __TACOPIE_INVALID_FD) {
				closesocket(m_fd);
			}
#else
			if (m_fds[0] != __TACOPIE_INVALID_FD) {
				close(m_fds[0]);
			}

			if (m_fds[1] != __TACOPIE_INVALID_FD) {
				close(m_fds[1]);
			}
#endif

	}

		//!
		//! get rd/wr fds
		//!
		fd_t
			self_pipe::get_read_fd(void) const {
#ifdef H_OS_WINDOWS
			return m_fd;
#else
			return m_fds[0];
#endif
		}

		fd_t
			self_pipe::get_write_fd(void) const {
#ifdef H_OS_WINDOWS
			return m_fd;
#else
			return m_fds[1];
#endif
		}

		//!
		//! notify
		//!
		void
			self_pipe::notify(void) {
#ifdef H_OS_WINDOWS
			(void)sendto(m_fd, "a", 1, 0, &m_addr, m_addr_len);
#else
			(void)write(m_fds[1], "a", 1);
#endif
		}

		//!
		//! clr buffer
		//!
		void
			self_pipe::clr_buffer(void) {
			char buf[1024] = {0};
#ifdef H_OS_WINDOWS
			
			(void)recvfrom(m_fd, buf, 1024, 0, &m_addr, &m_addr_len);
#else

			(void)read(m_fds[0], buf, 1024);
#endif
		}

} // namespace tacopie

