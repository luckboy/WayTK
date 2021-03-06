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
#include <cmath>
#include <waytk.hpp>

using namespace std;

namespace waytk
{
  Label::~Label() {}

  void Label::initialize(const string &text)
  { normalize_utf8(text, _M_text); }

  void Label::set_text(const string &text)
  { normalize_utf8(text, _M_text); }

  const char *Label::name() const
  { return "label"; }

  void Label::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  {
    FontMetrics font_metrics;
    TextMetrics text_metrics;
    canvas->get_font_matrics(font_metrics);
    canvas->get_text_matrics(_M_text, text_metrics);
    Dimension<int> content_size;
    content_size.width = ceil(text_metrics.x_advance);
    content_size.height = ceil(font_metrics.height);
    set_content_size(content_size);
  }

  void Label::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  {
    FontMetrics font_metrics;
    canvas->get_font_matrics(font_metrics);
    canvas->move_to(inner_bounds.x, inner_bounds.y + (inner_bounds.height - content_size().height) / 2 + font_metrics.ascent);
    canvas->set_color(styles()->foreground_color(pseudo_classes()));
    canvas->show_text(_M_text);
  }
}
