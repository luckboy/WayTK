/*
 * Copyright (c) 2017 Łukasz Szpakowski
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
#include <numeric>
#include <waytk.hpp>
#include "styles.hpp"

using namespace std;

namespace waytk
{
  namespace priv
  {
    namespace
    {
      template<typename _T>
      inline _T find_style_attr(const list<pair<PseudoClasses, _T>> &values, PseudoClasses pseudo_classes, _T value)
      {
        return accumulate(values.begin(), values.end(), value, [pseudo_classes](_T value, pair<PseudoClasses, _T> tmp_pair) {
          return (pseudo_classes & tmp_pair.first) == tmp_pair.first ? tmp_pair.second : value;
        });
      }
    }
  
    Edges<int> ImplStyles::margin(PseudoClasses pseudo_classes) const
    {
      Edges<int> tmp_margin;
      tmp_margin.top = find_style_attr(_M_margins.top, pseudo_classes, 0);
      tmp_margin.right = find_style_attr(_M_margins.right, pseudo_classes, 0);
      tmp_margin.bottom = find_style_attr(_M_margins.bottom, pseudo_classes, 0);
      tmp_margin.left = find_style_attr(_M_margins.left, pseudo_classes, 0);
      return tmp_margin;
    }

    Edges<int> ImplStyles::border(PseudoClasses pseudo_classes) const
    {
      Edges<int> tmp_border;
      tmp_border.top = find_style_attr(_M_borders.top, pseudo_classes, 0);
      tmp_border.right = find_style_attr(_M_borders.right, pseudo_classes, 0);
      tmp_border.bottom = find_style_attr(_M_borders.bottom, pseudo_classes, 0);
      tmp_border.left = find_style_attr(_M_borders.left, pseudo_classes, 0);
      return tmp_border;
    }

    Edges<int> ImplStyles::padding(PseudoClasses pseudo_classes) const
    {
      Edges<int> tmp_padding;
      tmp_padding.top = find_style_attr(_M_paddings.top, pseudo_classes, 0);
      tmp_padding.right = find_style_attr(_M_paddings.right, pseudo_classes, 0);
      tmp_padding.bottom = find_style_attr(_M_paddings.bottom, pseudo_classes, 0);
      tmp_padding.left = find_style_attr(_M_paddings.left, pseudo_classes, 0);
      return tmp_padding;
    }

    void ImplStyles::draw_background(PseudoClasses pseudo_classes, Canvas *canvas, const Rectangle<int> &rect) const
    { throw exception(); }

    Color ImplStyles::background_color(PseudoClasses pseudo_classes) const
    { return find_style_attr(_M_background_colors, pseudo_classes, Color(0xffffffff)); }
    
    Color ImplStyles::foreground_color(PseudoClasses pseudo_classes) const
    { return find_style_attr(_M_foreground_colors, pseudo_classes, Color(0xff000000)); }

    bool ImplStyles::has_adjacency_to()
    { throw exception(); }
  }
}
