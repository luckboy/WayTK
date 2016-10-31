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
#ifndef _UTIL_HPP
#define _UTIL_HPP

#include <exception>
#include <waytk.hpp>

namespace waytk
{
  namespace priv
  {
    const std::size_t MAX_NORMALIZED_UTF8_CHAR_LENGTH = 4;
    const std::size_t MAX_UNNORMALIZED_UTF8_CHAR_LENGTH = 6;
    
    template<typename _InputIter, typename _OutputIter>
    inline _OutputIter normalize_utf8_char(_InputIter begin, _InputIter end, _OutputIter result, std::size_t &char_length)
    { throw std::exception(); }

    template<typename _InputIter, typename _OutputIter>
    inline _OutputIter normalize_utf8_string(_InputIter begin, _InputIter end, _OutputIter result)
    { throw std::exception(); }

    template<typename _Iter>
    std::size_t current_utf8_char_length(_Iter iter, _Iter end)
    { throw std::exception(); }

    template<typename _Iter>
    std::size_t previous_utf8_char_length(_Iter end, _Iter begin)
    { throw std::exception(); }

    template<typename _Iter>
    _Iter first_utf8_char_byte_iter(_Iter iter, _Iter begin)
    { throw std::exception(); }
  }
}

#endif
