/*
 * Copyright (c) 2016 Łukasz Szpakowski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef _WAYTK_EXCEPTIONS_HPP
#define _WAYTK_EXCEPTIONS_HPP

#include <exception>

namespace waytk
{
  ///
  /// An exception class that is base for all WayTK exceptions.
  ///
  /// WayTK exceptions are thrown by some WayTK functions and methods of WayTK
  /// classes.
  ///
  class Exception : public std::exception
  {
    const char *_M_msg;
  public:
    /// Constructor.
    Exception(const char *msg) throw() :
      _M_msg(msg) {}

    /// Destructor.
    virtual ~Exception() throw();

    /// Returns the exception message.
    virtual const char *what() const throw();
  };

  ///
  /// A runtime exception that is thrown when a runtime error occurs.
  ///
  class RuntimeException : public Exception
  {
  public:
    /// Constructor.
    RuntimeException(const char *msg) throw() :
      Exception(msg) {}

    /// Destructor.
    virtual ~RuntimeException() throw();
  };

  ///
  /// An I/O exception that is thrown when an I/O error occurs.
  ///
  class IOException : public Exception
  {
  public:
    /// Constructor.
    IOException(const char *msg) throw() :
      Exception(msg) {}

    /// Destructor.
    virtual ~IOException() throw();
  };

  ///
  /// A file format exception that is thrown if a file format isn't supported.
  ///
  class FileFormatException : public Exception
  {
  public:
    /// Constructor
    FileFormatException(const char *msg) throw() :
      Exception(msg) {}

    /// Destructor.
    virtual ~FileFormatException() throw();
  };

  ///
  /// A canvas exception that is thrown by canvas methods.
  ///
  class CanvasException : public Exception
  {
  public:
    /// Constructor.
    CanvasException(const char *msg) throw() :
      Exception(msg) {}

    /// Destructor.
    virtual ~CanvasException() throw();
  };
}

#endif
