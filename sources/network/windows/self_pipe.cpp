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

#include <tacopie/error.hpp>
#include <tacopie/network/self_pipe.hpp>

#include <Winsock2.h>

#include <fcntl.h>

namespace tacopie {

//!
//! ctor & dtor
//!
self_pipe::self_pipe(void)
: m_fds{__TACOPIE_INVALID_FD, __TACOPIE_INVALID_FD} {
	//! Create a server
	SOCKET server = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//! Bind server to localhost
	struct sockaddr_in inaddr;
	struct sockaddr addr;
	memset(&inaddr, 0, sizeof(inaddr));
	memset(&addr, 0, sizeof(addr));
	inaddr.sin_family = AF_INET;
	inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	inaddr.sin_port = 0;
	bind(server, (struct sockaddr*) &inaddr, sizeof(inaddr));

	//! Wait for connections
	listen(server, 1);

	//! Retrieve server information
	int len = sizeof(inaddr);
	getsockname(server, &addr, &len);
	
	//! connect read fd to the server
	m_fds[0] = ::socket(AF_INET, SOCK_STREAM, 0);
	connect(m_fds[0], &addr, len);

	//! write fds is gonna be the server-side socket
	m_fds[1] = accept(server, 0, 0);

	//! close the server
	closesocket(server);
}

self_pipe::~self_pipe(void) {
  closesocket(m_fds[0]);
  closesocket(m_fds[1]);
}

//!
//! get rd/wr fds
//!
fd_t
self_pipe::get_read_fd(void) const {
  return m_fds[0];
}

fd_t
self_pipe::get_write_fd(void) const {
  return m_fds[1];
}

//!
//! notify
//!
void
self_pipe::notify(void) {
  (void) send(m_fds[1], "a", 1, 0);
}

//!
//! clr buffer
//!
void
self_pipe::clr_buffer(void) {
  char buf[1024];
  (void) recv(m_fds[0], buf, 1024, 0);
}

} //! tacopie
