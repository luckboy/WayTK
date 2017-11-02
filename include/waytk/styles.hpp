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
#ifndef _WAYTK_STYLES_HPP
#define _WAYTK_STYLES_HPP

#include <waytk/canvas.hpp>
#include <waytk/structs.hpp>

namespace waytk
{
  ///
  /// An enumeration of pseudo classes.
  ///
  enum class PseudoClasses
  {
    NONE = 0,                   ///< No pseudo classes.
    ACTIVE = 1,                 ///< The widget is clicked.
    BACKDROP = 2,               ///< The widget is on the inactive surface.
    CHECKED = 4,                ///< The wigdet is checked.
    DISABLED = 8,               ///< The widget is disabled.
    FOCUS = 16,                 ///< The widget has focus.
    HOVER = 32,                 ///< The pointer is on the widget.
    SELECTED = 64,              ///< The widget is selected.
    ADJACENT_TO_TOP = 128,      ///< The widget is adjacent to the top widget.
    ADJACENT_TO_RIGHT = 256,    ///< The widget is adjacent to the right widget.
    ADJACENT_TO_BOTTOM = 512,   ///< The widget is adjacent to the bottom widget.
    ADJACENT_TO_LEFT = 1024,    ///< The widget is adjacent to the left widget.
    FIRST = 2048,               ///< The widget at the first row.
    LAST = 4096,                ///< The widget at the last row.
    EVEN = 8192,                ///< The widget at an even row.
    ODD = 16384,                ///< The widget at an odd row.
    TOP_ACTIVE = 32768,         ///< The adjacent top widget to the widget is
                                ///  clicked.
    RIGHT_ACTIVE = 65536,       ///< The adjacent right wigdet to the widget is
                                ///  clicked.
    BOTTOM_ACTIVE = 131072,     ///< The adjacent bottom widget to the widget is
                                ///  clicked.
    LEFT_ACTIVE = 262144        ///< The adjacent left widget to the widget is
                                ///  clicked.
  };

  /// Returns nagation of \p pseudo_classes.
  inline PseudoClasses operator~(PseudoClasses pseudo_classes)
  { return static_cast<PseudoClasses>(static_cast<int>(pseudo_classes) ^ 524287); }

  /// Returns conjuction of \p pseudo_classes1 with \p pseudo_classes2.
  inline PseudoClasses operator&(PseudoClasses pseudo_classes1, PseudoClasses pseudo_classes2)
  { return static_cast<PseudoClasses>(static_cast<int>(pseudo_classes1) & static_cast<int>(pseudo_classes2)); }

  /// Assigns conjuction of \p pseudo_classes1 with \p pseudo_classes2 to
  /// \p pseudo_classes2.
  inline PseudoClasses operator&=(PseudoClasses &psuedo_classes1, PseudoClasses pseudo_classes2)
  { psuedo_classes1 = psuedo_classes1 & pseudo_classes2; return psuedo_classes1; }

  /// Returns disjuction of \p pseudo_classes1 with \p pseudo_classes2.
  inline PseudoClasses operator|(PseudoClasses pseudo_classes1, PseudoClasses pseudo_classes2)
  { return static_cast<PseudoClasses>(static_cast<int>(pseudo_classes1) | static_cast<int>(pseudo_classes2)); }

  /// Assigns disjuction of \p pseudo_classes1 with \p pseudo_classes2 to
  /// \p pseudo_classes2.
  inline PseudoClasses operator|=(PseudoClasses &pseudo_classes1, PseudoClasses pseudo_classes2)
  { pseudo_classes1 = pseudo_classes1 | pseudo_classes2; return pseudo_classes1; }

  /// Returns exclusive disjuction of \p pseudo_classes1 with
  /// \p pseudo_classes2.
  inline PseudoClasses operator^(PseudoClasses pseudo_classes1, PseudoClasses pseudo_classes2)
  { return static_cast<PseudoClasses>(static_cast<int>(pseudo_classes1) ^ static_cast<int>(pseudo_classes2)); }

  /// Assigns exclusive disjuction of \p pseudo_classes1 with \p pseudo_classes2
  /// to \p pseudo_classes2.
  inline PseudoClasses operator^=(PseudoClasses &pseudo_classes1, PseudoClasses pseudo_classes2)
  { pseudo_classes1 = pseudo_classes1 ^ pseudo_classes2; return pseudo_classes1; }

  ///
  /// A class of styles for a widget or a block.
  ///
  /// Styles are used for drawing widgets and blocks in WayTK theme. The styles
  /// contain an information about how to draw a widget or a block for a
  /// specified pseudo classes. Some information are accessible by properties
  /// and this class has methods to drawing a widget background or a block
  /// background.
  ///
  class Styles
  {
  protected:
    /// Default constructor.
    Styles() {}
  public:
    /// Destructor.
    virtual ~Styles();

    /// Returns a margin width for pseudo classes.
    virtual Edges<int> margin(PseudoClasses pseudo_classes) const = 0;

    /// Returns a border width for pseudo classes.
    virtual Edges<int> border(PseudoClasses pseudo_classes) const = 0;

    /// Returns a padding width for pseudo classes.
    virtual Edges<int> padding(PseudoClasses pseudo_classes) const = 0;

    /// Draws background for pseudo classes.
    void draw_background(PseudoClasses pseudo_classes, Canvas *canvas, int x, int y, int width, int height) const
    { draw_background(pseudo_classes, canvas, Rectangle<int>(x, y, width, height)); }

    /// \copydoc draw_background(PseudoClasses pseudo_classes, Canvas *canvas, int x, int y, int width, int height) const
    void draw_background(PseudoClasses pseudo_classes, Canvas *canvas, const Point<int> &point, const Dimension<int> &size) const
    { draw_background(pseudo_classes, canvas, Rectangle<int>(point.x, point.y, size.width, size.height)); }

    /// \copydoc draw_background(PseudoClasses pseudo_classes, Canvas *canvas, int x, int y, int width, int height) const
    virtual void draw_background(PseudoClasses pseudo_classes, Canvas *canvas, const Rectangle<int> &rect) const = 0;

    /// Returns a background color for pseudo classes.
    virtual Color background_color(PseudoClasses pseudo_classes) const = 0;
    
    /// Returns a foreground color for pseudo classes.
    virtual Color foreground_color(PseudoClasses pseudo_classes) const = 0;

    /// Returns \c true if the widget can be adjacent to other widget, otherwise
    /// \c false.
    virtual bool has_adjacency_to() = 0;
  };

  /// Finds styles for a specified name.
  Styles *find_styles(const char *name);
}

#endif
