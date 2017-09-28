/*
 * Copyright (c) 2016-2017 Łukasz Szpakowski
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

#include <cstddef>

namespace waytk
{
  namespace priv
  {
    const std::size_t MAX_NORMALIZED_UTF8_CHAR_LENGTH = 4;
    const std::size_t MAX_UNNORMALIZED_UTF8_CHAR_LENGTH = 6;

    template<typename _InputIter, typename _OutputIter>
    inline _OutputIter normalize_utf8_char(_InputIter begin, _InputIter end, _OutputIter result, std::size_t &input_char_length, std::size_t &output_char_length)
    {
      _InputIter iter = begin;
      input_char_length = 0;
      while(true) {
        if(iter == end) {
          output_char_length = 0;
          break;
        }
        bool is_char = true;
        unsigned c;
        std::size_t count;
        if((*iter & 0x80) == 0) {
          c = *iter  & 0x7f;
          count = 0;
        } else if((*iter & 0xe0) == 0xc0) {
          c = *iter & 0x1f;
          count = 1;
        } else if((*iter & 0xf0) == 0xe0) {
          c = *iter & 0x0f;
          count = 2;
        } else if((*iter & 0xf8) == 0xf0) {
          c = *iter & 0x07;
          count = 3;
        } else if((*iter & 0xfc) == 0xf8) {
          c = *iter & 0x03;
          count = 4;
        } else if((*iter & 0xfe) == 0xfc) {
          c = *iter & 0x01;
          count = 5;
        } else {
          is_char = false;
        }
        iter++;
        input_char_length++;
        if(is_char) {
          for(std::size_t i = 0; i < count; i++) {
            if(iter == end) {
              is_char = false;
              break;
            }
            if((*iter & 0xc0) == 0x80) {
              c = (c << 6) | (*iter & 0x3f);
              iter++;
              input_char_length++;
            } else {
              is_char = false;
              break;
            }
          }
        }
        if(is_char) {
          _OutputIter output_iter = result;
          if(c <= 0x7f) {
            *output_iter = c;
            output_iter++;
            output_char_length = 1;
            break;
          } else if(c >= 0x80 && c <= 0x7ff) {
            *output_iter = (c >> 6) | 0xc0;
            output_iter++;
            *output_iter = (c & 0x3f) | 0x80;
            output_iter++;
            output_char_length = 2;
            break;
          } else if(c >= 0x800 && c <= 0xffff) {
            *output_iter = (c >> 12) | 0xe0;
            output_iter++;
            *output_iter = ((c >> 6) & 0x3f) | 0x80;
            output_iter++;
            *output_iter = (c & 0x3f) | 0x80;
            output_iter++;
            output_char_length = 3;
            break;
          } else if(c >= 0x10000 && c <= 0x10ffff) {
            *output_iter = (c >> 18) | 0xf0;
            output_iter++;
            *output_iter = ((c >> 12) & 0x3f) | 0x80;
            output_iter++;
            *output_iter = ((c >> 6) & 0x3f) | 0x80;
            output_iter++;
            *output_iter = (c & 0x3f) | 0x80;
            output_iter++;
            output_char_length = 4;
            break;
          }
        }
      }
      return result;
    }

    template<typename _InputIter, typename _OutputIter>
    inline _OutputIter normalize_utf8_string(_InputIter begin, _InputIter end, _OutputIter result)
    {
      _InputIter iter = begin;
      _OutputIter out_iter = result;
      while(iter != end) {
        char buf[MAX_NORMALIZED_UTF8_CHAR_LENGTH];
        std::size_t in_char_length, out_char_length;
        normalize_utf8_char(begin, end, buf, in_char_length, out_char_length);
        for(std::size_t i = 0; i < out_char_length; i++, out_iter++) {
          *out_iter = *buf;
        }
        for(std::size_t i = 0; i < in_char_length; i++, iter++);
      }
    }

    template<typename _Iter>
    std::size_t current_utf8_char_length(_Iter iter, _Iter end)
    {
      std::size_t length = 0;
      if(iter == end) return 0;
      unsigned c;
      std::size_t count = 0;
      if((*iter & 0x80) == 0) {
        c = *iter  & 0x7f;
        count = 0;
      } else if((*iter & 0xe0) == 0xc0) {
        c = *iter  & 0x1f;
        count = 1;
      } else if((*iter & 0xf0) == 0xe0) {
        c = *iter  & 0x0f;
        count = 2;
      } else if((*iter & 0xf8) == 0xf0) {
        c = *iter  & 0x0f;
        count = 3;
      } else {
        c = *iter  & 0xff;
        count = 0;
      }
      iter++;
      length++;
      if(count == 1 && c < 2) return length;
      for(std::size_t i = 0; i < count; i++, iter++, length++) {
        if(iter == end) break;
        if((*iter & 0xc0) != 0x80) break;
        if(i == 0) {
          c = (c << 6) | (*iter & 0x3f);
          if(count == 2 && c < 0x20) break;
          if(count == 3 && (c < 0x10 || c > 0x10f)) break;
        }
      }
      return length;
    }

    template<typename _Iter>
    std::size_t previous_utf8_char_length(_Iter end, _Iter begin)
    {
      _Iter iter = end;
      std::size_t length = 0;
      if(iter == begin) return 0;
      unsigned c = 0;
      std::size_t i;
      for(i = 0; iter != begin; i++) {
        iter--;
        length++;
        if((*iter & 0xc0) != 0x80) break;
        c = *iter & 0x3f;
        if(i >= 3) break;
      }
      bool is_char = false;
      switch(i) {
        case 0:
          if((*iter & 0x80) == 0) {
            is_char = true;
            break;
          }
        case 1:
          if((*iter & 0xe0) == 0xc0) {
            if((*iter & 0x1f) >= 2) {
              is_char = true;
              break;
            } else
              break;
          }
        case 2:
          if((*iter & 0xf0) == 0xe0) {
            c = ((*iter & 0x0f) << 6) | c;
            if(c >= 0x20) {
              is_char = true;
              break;
            } else
              break;
          }
        case 3:
          if((*iter & 0xf8) == 0xf0) {
            c = ((*iter & 0x0f) << 6) | c;
            if(c >= 0x10 && c <= 0x10f) {
              is_char = true;
              break;
            } else
              break;
          }
      }
      return is_char ? length : 1;
    }

    template<typename _Iter>
    _Iter first_utf8_char_byte_iter(_Iter iter, _Iter begin)
    {
      while(iter != begin) {
        if((*iter & 0xc0) != 0x80) break;
        iter--;
      }
      return iter;
    }
  }
}

#endif
