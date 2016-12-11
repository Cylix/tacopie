#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

namespace tacopie {

class tacopie_error : public std::runtime_error {
public:
  //! ctor & dtor
  tacopie_error(const std::string& what, const std::string& file, std::size_t line);
  ~tacopie_error(void) = default;

  //! copy ctor & assignment operator
  tacopie_error(const tacopie_error&) = default;
  tacopie_error& operator=(const tacopie_error&) = default;

public:
  //! get location of the error
  const std::string& get_file(void) const;
  std::size_t get_line(void) const;

private:
  //! location of the error
  std::string m_file;
  std::size_t m_line;
};

} //! tacopie

//! macro for convenience
#define __TACOPIE_THROW(level, what)  { \
                                        __TACOPIE_LOG(level, (what)); \
                                        throw tacopie::tacopie_error((what), __FILE__, __LINE__); \
                                      }
