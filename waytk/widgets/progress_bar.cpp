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
  ProgressBar::~ProgressBar() {}

  void ProgressBar::initialize(int max_value)
  {
    _M_max_value = max_value;
    _M_value = 0;
  }

  const char *ProgressBar::name() const
  { return "progress_bar"; }

  void ProgressBar::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { set_content_size(block_margin_box_size(progress_name(), PseudoClasses::NONE, Dimension<int>(0, 0))); }

  void ProgressBar::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  {
    Point<int> progress_margin_box_point = inner_bounds.point();
    Dimension<int> progress_margin_box_size;
    if(_M_max_value != 0)
      progress_margin_box_size.width = (static_cast<int64_t>(_M_value) * inner_bounds.width) / _M_max_value;
    else
      progress_margin_box_size.width = inner_bounds.width;
    progress_margin_box_size.height = inner_bounds.height;
    draw_progress(canvas, progress_margin_box_point, progress_margin_box_size);
  }

  const char *ProgressBar::progress_name() const
  { return "progress_bar.progress"; }

  void ProgressBar::draw_progress(Canvas *canvas, const Point<int> &margin_box_point, const Dimension<int> &margin_box_size)
  { draw_block(progress_name(), PseudoClasses::NONE, canvas, margin_box_point, margin_box_size); }
}
