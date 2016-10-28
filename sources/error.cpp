#include <cpp_http_server/error.hpp>

namespace cpp_http_server {

//!
//! ctor & dtor
//!

error::error(const std::string& what, const std::string& file, std::uint32_t line)
: std::runtime_error(what)
, m_file(file)
, m_line(line)
{}

//!
//! get location of the error
//!

const std::string&
error::get_file(void) const {
  return m_file;
}

std::uint32_t
error::get_line(void) const {
  return m_line;
}

} //! cpp_http_server
