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
#ifndef _WAYTK_STYLES_HPP
#define _WAYTK_STYLES_HPP

#include <waytk/canvas.hpp>
#include <waytk/structs.hpp>

namespace waytk
{
  enum class PseudoClasses
  {
    NONE = 0,
    ACTIVE = 1,
    BACKDROP = 2,
    CHECKED = 4,
    DISABLED = 8,
    FOCUS = 16,
    HOVER = 32,
    SELECTED = 64,
    ADJACENT_TO_TOP = 128,
    ADJACENT_TO_RIGHT = 256,
    ADJACENT_TO_BOTTOM = 512,
    ADJACENT_TO_LEFT = 1024,
    FIRST = 2048,
    LAST = 4096,
    EVEN = 8192,
    ODD = 16384,
    TOP_ACTIVE = 32768, 
    RIGHT_ACTIVE = 65536,
    BOTTOM_ACTIVE = 131072,
    LEFT_ACTIVE = 262144
  };

  inline PseudoClasses operator~(PseudoClasses pseudo_class)
  { return static_cast<PseudoClasses>(static_cast<int>(pseudo_class) ^ 524287); }

  inline PseudoClasses operator&(PseudoClasses pseudo_class1, PseudoClasses pseudo_class2)
  { return static_cast<PseudoClasses>(static_cast<int>(pseudo_class1) & static_cast<int>(pseudo_class2)); }

  inline PseudoClasses operator&=(PseudoClasses &psuedo_class1, PseudoClasses pseudo_class2)
  { psuedo_class1 = psuedo_class1 & pseudo_class2; return psuedo_class1; }

  inline PseudoClasses operator|(PseudoClasses pseudo_class1, PseudoClasses pseudo_class2)
  { return static_cast<PseudoClasses>(static_cast<int>(pseudo_class1) | static_cast<int>(pseudo_class2)); }

  inline PseudoClasses operator|=(PseudoClasses &pseudo_class1, PseudoClasses pseudo_class2)
  { pseudo_class1 = pseudo_class1 | pseudo_class2; return pseudo_class1; }

  inline PseudoClasses operator^(PseudoClasses pseudo_class1, PseudoClasses pseudo_class2)
  { return static_cast<PseudoClasses>(static_cast<int>(pseudo_class1) ^ static_cast<int>(pseudo_class2)); }

  inline PseudoClasses operator^=(PseudoClasses &pseudo_class1, PseudoClasses pseudo_class2)
  { pseudo_class1 = pseudo_class1 ^ pseudo_class2; return pseudo_class1; }

  class Styles
  {
  protected:
    Styles() {}
  public:
    virtual ~Styles();

    virtual Edges<int> margin(PseudoClasses pseudo_classes) const = 0;

    virtual Edges<int> border(PseudoClasses pseudo_classes) const = 0;

    virtual Edges<int> padding(PseudoClasses pseudo_classes) const = 0;

    void draw_background(PseudoClasses pseudo_classes, Canvas *canvas, int x, int y, int width, int height) const
    { draw_background(pseudo_classes, canvas, Rectangle<int>(x, y, width, height)); }

    void draw_background(PseudoClasses pseudo_classes, Canvas *canvas, const Point<int> &point, const Rectangle<int> &size) const
    { draw_background(pseudo_classes, canvas, Rectangle<int>(point.x, point.y, size.width, size.height)); }

    virtual void draw_background(PseudoClasses pseudo_classes, Canvas *canvas, const Rectangle<int> &rect) const = 0;

    virtual Color foreground_color(PseudoClasses pseudo_classes) const = 0;
    
    virtual bool has_adjacency_to(Styles *styles);
  };

  Styles *find_styles(const char *name);
}

#endif
