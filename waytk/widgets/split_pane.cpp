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
#include <cmath>
#include <waytk.hpp>

using namespace std;

namespace waytk
{
  SplitPane::~SplitPane() {}

  void SplitPane::initialize(Orientation orientation, Widget *first_widget, Widget *second_widget)
  {
    _M_orientation = orientation;
    _M_first_widget = unique_ptr<Widget>(first_widget);
    _M_second_widget = unique_ptr<Widget>(second_widget);
  }

  void SplitPane::set_first_widget(Widget *widget)
  {
    _M_first_widget = unique_ptr<Widget>(widget);
    set_this_as_widget_parent(_M_first_widget.get());
  }

  void SplitPane::set_second_widget(Widget *widget)
  {
    _M_second_widget = unique_ptr<Widget>(widget);
    set_this_as_widget_parent(_M_second_widget.get());
  }

  const char *SplitPane::name() const
  { return "split_pane"; }

  void SplitPane::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { throw exception(); }

  void SplitPane::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  { throw exception(); }
  
  bool SplitPane::invoke_fun_for_event(const Point<double> &point, const std::function<bool (Widget *, const Point<double> &)> &fun)
  {
    Point<int> int_point(round(point.x), round(point.x));
    bool cant_invoke = false;
    Rectangle<int> result;
    if(child_event_bounds().intersect(_M_first_widget->bounds(), result) && result.contain(int_point))
      cant_invoke = _M_first_widget->invoke_fun_for_event(point, fun);
    else if(child_event_bounds().intersect(_M_second_widget->bounds(), result) && result.contain(int_point))
      cant_invoke = _M_second_widget->invoke_fun_for_event(point, fun);
    return !cant_invoke ? fun(this, point) : true;
  }

}
