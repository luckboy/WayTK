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
#include <waytk.hpp>

using namespace std;

namespace waytk
{
  MenuItem::~MenuItem() {}

  const char *MenuItem::name() const
  { return "menu_item"; }

  void MenuItem::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  {
    Point<int> content_point(inner_bounds.x, inner_bounds.y);
    content_point.y += (inner_bounds.height - content_size().height) / 2;
    int x_offset = 0;
    if(icon() != Icon()) {
      Point<int> icon_margin_box_point = content_point;
      icon_margin_box_point.y += (content_size().height - icon_margin_box_size().height) / 2;
      draw_icon(canvas, icon_margin_box_point);
      x_offset = icon_margin_box_size().width;
    }
    if(!label().empty()) {
      Point<int> label_margin_box_point = content_point;
      label_margin_box_point.x += x_offset;
      label_margin_box_point.y += (content_size().height - label_margin_box_size().height) / 2;
      draw_label(canvas, label_margin_box_point);
    }
  }

  const char *MenuItem::icon_name() const
  { return "menu_item.icon"; }

  IconSize MenuItem::icon_size() const
  { return IconSize::SMALL; }

  const char *MenuItem::label_name() const
  { return "menu_item.label"; }
}
