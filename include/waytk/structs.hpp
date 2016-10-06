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
#ifndef _WAYTK_STRUCTS_HPP
#define _WAYTK_STRUCTS_HPP

#include <cstddef>
#include <initializer_list>
#include <list>

namespace waytk
{
  template<typename _T>
  struct Range
  {
    _T begin, end;

    Range(_T begin, _T end) : begin(begin), end(end) {}

    bool operator==(const Range<_T> &range) const
    { return begin == range.begin && end == range.end; }

    bool operator!=(const Range<_T> &range) const
    { return !(*this == range); }
  };

  template<typename _T>
  struct Point
  {
    _T x, y;

    Point(_T x, _T y) : x(x), y(x) {}

    bool operator==(const Point<_T> &point) const
    { return x == point.x && y == point.y; }

    bool operator!=(const Point<_T> &point) const
    { return !(*this == point); }
  };

  template<typename _T>
  struct Dimension
  {
    _T width, height;

    Dimension(_T width, _T height) : width(width), height(height) {}

    bool operator==(const Dimension<_T> &dim) const
    { return width == dim.width && height == dim.height; }

    bool operator!=(const Dimension<_T> &dim) const
    { return !(*this == dim); }
  };

  template<typename _T>
  struct Rectangle
  {
    _T x, y, width, height;

    Rectangle(_T x, _T y, _T width, _T height) :
      x(x), y(y), width(width), height(height) {}

    bool operator==(const Rectangle<_T> &rect) const
    { return x == rect.x && y == rect.y && width == rect.width && height == rect.height; }

    bool operator!=(const Rectangle<_T> &rect) const
    { return !(*this == rect); }
  };

  struct TextPosition
  {
    std::size_t line, column;

    TextPosition(std::size_t line, std::size_t column) :
      line(line), column(column) {}
  };

  struct TablePosition
  {
    std::size_t row, column;

    TablePosition(std::size_t row, std::size_t column) :
      row(row), column(column) {}
  };

  struct TreePath
  {
    std::list<std::size_t> nodes;

    TreePath() {}

    TreePath(const std::initializer_list<std::size_t> nodes) :
      nodes(nodes) {}

    TreePath(const std::list<std::size_t> &nodes) :
      nodes(nodes) {}
  };
}

#endif
