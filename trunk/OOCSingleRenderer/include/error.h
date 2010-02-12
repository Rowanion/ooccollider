/**
  *  @file error.h
  *
  *  @date Created on: 03.05.2009
  *  @author weltmarktfuehrer@googlemail.com
  */

#ifndef ERROR_H_
#define ERROR_H_

#include <stdexcept>

// Defines THROW and ASSERT macro's that have different behaviour in
// debug and release mode. ASSERT checks a condition and aborts when
// it is false in debug mode, and compiles to nothing in release mode.
// THROW throws an exception, it adds some file and line info to the
// exception in debug mode.

#ifdef NDEBUG
# define THROW(error_type, message) throw error_type(message)
# define ASSERT(expr)
#else
# include <sstream>
# include <iostream>
  inline std::string Mark_String(std::string target, const char* file, int line)
  {
    std::ostringstream new_string;
    new_string << target << " (" << file << " at line " << line << ")";
    return new_string.str();
  }
# define THROW(error_type, message) throw error_type(Mark_String(message, __FILE__, __LINE__))
// In unix we use kill to terminate the application, that makes gdb
// keep it alive which makes debugging easier.
# ifdef _WIN32
#  define DIE() exit(1)
# else
#  include <csignal>
#  define DIE() kill(0, SIGTERM)
# endif
# define ASSERT(expr) \
    if (!(expr)){ \
      std::cout << Mark_String("Assertion " #expr " failed", __FILE__, __LINE__) << std::endl; \
      DIE(); \
    } \
    else
#endif

#endif /* ERROR_H_ */
