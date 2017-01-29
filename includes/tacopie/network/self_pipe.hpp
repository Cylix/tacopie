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

#pragma once

#include <tacopie/typedefs.hpp>

namespace tacopie {

class self_pipe {
public:
  //! ctor & dtor
  self_pipe(void);
  ~self_pipe(void);

  //! copy ctor & assignment operator
  self_pipe(const self_pipe&) = delete;
  self_pipe& operator=(const self_pipe&) = delete;

public:
  //! get rd/wr fds
  fd_t get_read_fd(void) const;
  fd_t get_write_fd(void) const;

  //! notify
  void notify(void);

  //! clr buffer
  void clr_buffer(void);

private:
#ifdef _WIN32
  fd_t m_fd;
  struct sockaddr m_addr;
  int m_addr_len;
#else
  fd_t m_fds[2];
#endif /* _WIN32 */
};

} //! tacopie
