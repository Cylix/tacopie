#include <tacopie/error.hpp>

namespace tacopie {

//!
//! ctor & dtor
//!

tacopie_error::tacopie_error(const std::string& what, const std::string& file, std::size_t line)
: std::runtime_error(what)
, m_file(file)
, m_line(line) {}

//!
//! get location of the error
//!

const std::string&
tacopie_error::get_file(void) const {
  return m_file;
}

std::size_t
tacopie_error::get_line(void) const {
  return m_line;
}

} //! tacopie
