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
  ///
  /// A range structure.
  ///
  template<typename _T>
  struct Range
  {
    _T begin, end;

    Range() {}

    Range(_T begin, _T end) : begin(begin), end(end) {}

    bool operator==(const Range<_T> &range) const
    { return begin == range.begin && end == range.end; }

    bool operator!=(const Range<_T> &range) const
    { return !(*this == range); }
  };

  ///
  /// A point structure.
  ///
  template<typename _T>
  struct Point
  {
    _T x, y;

    Point() {}

    Point(_T x, _T y) : x(x), y(x) {}

    bool operator==(const Point<_T> &point) const
    { return x == point.x && y == point.y; }

    bool operator!=(const Point<_T> &point) const
    { return !(*this == point); }
  };

  ///
  /// A dimenstion strucure.
  ///
  template<typename _T>
  struct Dimension
  {
    _T width, height;

    Dimension() {}

    Dimension(_T width, _T height) : width(width), height(height) {}

    bool operator==(const Dimension<_T> &dim) const
    { return width == dim.width && height == dim.height; }

    bool operator!=(const Dimension<_T> &dim) const
    { return !(*this == dim); }
  };

  ///
  /// A rectangle structure.
  ///
  template<typename _T>
  struct Rectangle
  {
    _T x, y, width, height;

    Rectangle() {}
    
    Rectangle(_T x, _T y, _T width, _T height) :
      x(x), y(y), width(width), height(height) {}

    Rectangle(const Point<_T> &point, const Dimension<_T> &size) :
      x(point.x), y(point.y), width(size.width), height(size.height) {}

    bool operator==(const Rectangle<_T> &rect) const
    { return x == rect.x && y == rect.y && width == rect.width && height == rect.height; }

    bool operator!=(const Rectangle<_T> &rect) const
    { return !(*this == rect); }

    Point<_T> point() const
    { return Point<_T>(x, y); }

    Dimension<_T> size() const
    { return Dimension<_T>(width, height); }
  };

  ///
  /// A text position structure.
  ///
  struct TextPosition
  {
    std::size_t line, column;

    TextPosition() {}

    TextPosition(std::size_t line, std::size_t column) :
      line(line), column(column) {}
  };

  ///
  /// A table position structure.
  ///
  struct TablePosition
  {
    std::size_t row, column;

    TablePosition() {}

    TablePosition(std::size_t row, std::size_t column) :
      row(row), column(column) {}
  };

  ///
  /// A tree path structure.
  ///
  struct TreePath
  {
    std::list<std::size_t> nodes;

    TreePath() {}

    explicit TreePath(const std::initializer_list<std::size_t> nodes) :
      nodes(nodes) {}

    explicit TreePath(const std::list<std::size_t> &nodes) :
      nodes(nodes) {}
  };

  ///
  /// A structure that has values of edges.
  ///
  template<typename _T>
  struct Edges
  {
    _T top, right, bottom, left;

    Edges() {}

    Edges(_T top, _T right, _T bottom, _T left) :
      top(top), right(right), bottom(bottom), left(left) {}
  };
  
  ///
  /// A structure that has values of corners.
  ///
  template<typename _T>
  struct Corners
  {
    _T top_left, top_right, bottom_right, bottom_left;

    Corners() {}

    Corners(_T top_left, _T top_right, _T bottom_right, _T bottom_left) : 
      top_left(top_left), top_right(top_right), bottom_right(bottom_right), bottom_left(bottom_left) {}
  };
}

#endif
