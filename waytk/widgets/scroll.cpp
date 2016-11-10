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
  Scroll::~Scroll() {}

  void Scroll::initialize(Widget *widget)
  { set_this_as_widget_parent(widget); }

  void Scroll::set_widget(Widget *widget)
  {
    if(_M_widget.get() != nullptr) unset_this_as_widget_parent(_M_widget.get());
    _M_widget = unique_ptr<Widget>(widget);
    set_this_as_widget_parent(_M_widget.get());
  }

  const char *Scroll::name() const
  { return "scroll"; }

  void Scroll::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { throw exception(); }

  void Scroll::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds, Styles *style)
  { throw exception(); }

  bool Scroll::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  { throw exception(); }

  void Scroll::on_touch_leave(const Pointer &pointer)
  { throw exception(); }

  bool Scroll::on_pointer_axis(Axis axis, double value)
  { throw exception(); }
}
