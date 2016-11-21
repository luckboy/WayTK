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
  /// A structure template of range.
  ///
  template<typename _T>
  struct Range
  {
    _T begin, end;

    /// Default constructor.
    Range() {}

    /// Constructor
    Range(_T begin, _T end) : begin(begin), end(end) {}

    /// Returns \c true if the range is equal to \p range, otherwise \c false.
    bool operator==(const Range<_T> &range) const
    { return begin == range.begin && end == range.end; }

    /// Returns \c true if the range isn't equal to \p range, otherwise
    /// \c false.
    bool operator!=(const Range<_T> &range) const
    { return !(*this == range); }
  };

  ///
  /// A structure template of point.
  ///
  template<typename _T>
  struct Point
  {
    _T x;               ///< The X coordinate of the point.
    _T y;               ///< The Y coordinate of the point.

    /// Default constructor.
    Point() {}

    /// Constructor.
    Point(_T x, _T y) : x(x), y(x) {}

    /// Returns \c true if the point is equal to \p point, otherwise \c false.
    bool operator==(const Point<_T> &point) const
    { return x == point.x && y == point.y; }

    /// Returns \c true if the point isn't equal to \p point, otherwise
    /// \c false.
    bool operator!=(const Point<_T> &point) const
    { return !(*this == point); }
  };

  ///
  /// A structure template of dimension.
  ///
  template<typename _T>
  struct Dimension
  {
    _T width;           ///< The width.
    _T height;          ///< The height.

    /// Default constructor.
    Dimension() {}

    /// Constructor.
    Dimension(_T width, _T height) : width(width), height(height) {}

    /// Returns \c true if the dimension is equal to \p dim, otherwise \c false.
    bool operator==(const Dimension<_T> &dim) const
    { return width == dim.width && height == dim.height; }

    /// Returns \c true if the dimension isn't equal to \p dim, otherwise
    /// \c false.
    bool operator!=(const Dimension<_T> &dim) const
    { return !(*this == dim); }
  };

  ///
  /// A structure template of rectangle.
  ///
  template<typename _T>
  struct Rectangle
  {
    _T x;               ///< The X coordinate of the top left point of the
                        ///  rectangle.
    _T y;               ///< The Y coordinate of the top left point of the
                        ///  rectangle.
    _T width;           ///< The rectangle width.
    _T height;          ///< The rectangle height.

    /// Default constructor.
    Rectangle() {}

    /// Constructor.
    Rectangle(_T x, _T y, _T width, _T height) :
      x(x), y(y), width(width), height(height) {}

    /// Constructor.
    Rectangle(const Point<_T> &point, const Dimension<_T> &size) :
      x(point.x), y(point.y), width(size.width), height(size.height) {}

    /// Returns \c true if the rectangle is equal to \p rect, otherwise
    /// \c false.
    bool operator==(const Rectangle<_T> &rect) const
    { return x == rect.x && y == rect.y && width == rect.width && height == rect.height; }

    /// Returns \c true if the rectangle isn't equal to \p rect, otherwise
    /// \c false.
    bool operator!=(const Rectangle<_T> &rect) const
    { return !(*this == rect); }

    /// Returns the top left point of the rectangle
    Point<_T> point() const
    { return Point<_T>(x, y); }

    /// Returns the rectangle size.
    Dimension<_T> size() const
    { return Dimension<_T>(width, height); }
  };

  ///
  /// A structure of text position.
  ///
  struct TextPosition
  {
    std::size_t line;   ///< The line.
    std::size_t column; ///< The column.

    /// Default constructor.
    TextPosition() {}

    /// Constructor.
    TextPosition(std::size_t line, std::size_t column) :
      line(line), column(column) {}

    /// Returns \c true if the text position is equal to \p pos, otherwise
    /// \c false.
    bool operator==(const TextPosition &pos) const
    { return line == pos.line && column == pos.column; }

    /// Returns \c true if the text position isn't equal to \p pos, otherwise
    /// \c false.
    bool operator!=(const TextPosition &pos) const
    { return !(*this == pos); }

    /// Returns \c true if the text position is less than \p pos, otherwise
    /// \c false.
    bool operator<(const TextPosition &pos) const
    { return line == pos.line ? column < pos.column : line < pos.line; }

    /// Returns \c true if the text position is greater than or equal to \p pos,
    /// otherwise \c false.
    bool operator>=(const TextPosition &pos) const
    { return !(*this < pos); }

    /// Returns \c true if the text position is greater than \p pos, otherwise
    /// \c false.
    bool operator>(const TextPosition &pos) const
    { return pos < *this; }

    /// Returns \c true if the text position is less than or equal to \p pos,
    /// otherwise \c false.
    bool operator<=(const TextPosition &pos) const
    { return !(pos < *this); }
  };

  ///
  /// A structure of table position.
  ///
  struct TablePosition
  {
    std::size_t row, column;

    /// Default constructor.
    TablePosition() {}

    /// Constructor.
    TablePosition(std::size_t row, std::size_t column) :
      row(row), column(column) {}

    /// Returns \c true if the text position is equal to \p pos, otherwise
    /// \c false.
    bool operator==(const TablePosition &pos) const
    { return row == pos.row && column == pos.column; }

    /// Returns \c true if the text position isn't equal to \p pos, otherwise
    /// \c false.
    bool operator!=(const TablePosition &pos) const
    { return !(*this == pos); }
  };

  ///
  /// A structure of tree path.
  ///
  struct TreePath
  {
    std::list<std::size_t> nodes; ///< The nodes of the tree path.

    /// Default constructor.
    TreePath() {}

    /// Constructor.
    explicit TreePath(const std::initializer_list<std::size_t> nodes) :
      nodes(nodes) {}

    /// Constructor.
    explicit TreePath(const std::list<std::size_t> &nodes) :
      nodes(nodes) {}

    /// Returns \c true if the tree path is equal to \p pos, otherwise \c false.
    bool operator==(const TreePath &path) const;

    /// Returns \c true if the tree path isn't equal to \p pos, otherwise
    /// \c false.
    bool operator!=(const TreePath &path) const
    { return !(*this == path); }
  };

  ///
  /// A structure template that has values of edges.
  ///
  template<typename _T>
  struct Edges
  {
    _T top;             ///< The value of top edge.
    _T right;           ///< The value of right edge.
    _T bottom;          ///< The value of bottom edge.
    _T left;            ///< The value of left edge.

    /// Default constructor.
    Edges() {}

    /// Constructor.
    Edges(_T top, _T right, _T bottom, _T left) :
      top(top), right(right), bottom(bottom), left(left) {}
  };
  
  ///
  /// A structure template that has values of corners.
  ///
  template<typename _T>
  struct Corners
  {
    _T top_left;        ///< The value of top left corner.
    _T top_right;       ///< The value of top right corner.
    _T bottom_right;    ///< the value of bottom right corner.
    _T bottom_left;     ///< The value of bottom left corner.

    /// Default constructor.
    Corners() {}

    /// Constructor.
    Corners(_T top_left, _T top_right, _T bottom_right, _T bottom_left) : 
      top_left(top_left), top_right(top_right), bottom_right(bottom_right), bottom_left(bottom_left) {}
  };
}

#endif
