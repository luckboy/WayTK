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
#ifndef _WAYTK_CALLBACK_HPP
#define _WAYTK_CALLBACK_HPP

#include <functional>

namespace waytk
{
  ///
  /// A class template of callback.
  ///
  /// A callback is an object that stores a listener. The callback listener can
  /// be invoked by an invocation of the callback. The invocation of the
  /// callback guaratees that the listener can't be invoked by the callback if
  /// the listener already is invoked by the callback.
  ///
  template<typename... _Ts>
  class Callback
  {
    std::function<void (_Ts...)> _M_listener;
    bool _M_can_invoke_listener;
  public:
    /// Default constructor.
    Callback() :
      _M_can_invoke_listener(true) {}

    /// Constructor.
    explicit Callback(const std::function<void (_Ts...)> &listener) :
      _M_listener(listener), _M_can_invoke_listener(true) {}

    /// Invokes the listener if this listener already isn't invoked by the
    /// callback.
    void operator()(_Ts... args)
    {
      if(_M_can_invoke_listener) {
        _M_can_invoke_listener = false;
        try {
          _M_listener(args...);
        } catch(...) {
          _M_can_invoke_listener = true;
          throw;
        }
        _M_can_invoke_listener = true;
      }
    }

    /// Returns the listener.
    const std::function<void (_Ts...)> &listener() const
    { return _M_listener; }

    /// Sets the listener.
    void set_listener(const std::function<void (_Ts...)> &listener)
    { _M_listener = listener; }
  };
}

#endif
