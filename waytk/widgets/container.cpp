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
#include <algorithm>
#include <cmath>
#include <waytk.hpp>

using namespace std;

namespace waytk
{
  Container::~Container() {}

  void Container::initialize(initializer_list<Widget *> widgets)
  {
    _M_widgets.clear();
    for(auto widget : widgets) {
      set_this_as_widget_parent(widget);
      _M_widgets.push_back(unique_ptr<Widget>(widget));
    }
  }
  
  void Container::add_widget(Widget *widget)
  {
    _M_widgets.push_back(unique_ptr<Widget>(widget));
    set_this_as_widget_parent(widget);
  }

  bool Container::delete_widget(Widget *widget)
  {
    auto iter = find_if(_M_widgets.begin(), _M_widgets.end(), [widget](const unique_ptr<Widget> &tmp_widget) {
      return tmp_widget.get() == widget;
    });
    if(iter != _M_widgets.end()) {
      unset_this_as_widget_parent(iter->get());
      _M_widgets.erase(iter);
      return true;
    }
    return false;
  }

  void Container::delete_all_widgets()
  {
    for(auto &widget : _M_widgets) unset_this_as_widget_parent(widget.get());
    _M_widgets.clear();
  }

  bool Container::invoke_fun_for_event(const Point<double> &point, const function<bool (Widget *, const Point<double> &)> &fun)
  {
    Point<int> int_point(round(point.x), round(point.x));
    bool cant_invoke = false;
    for(auto &widget : _M_widgets) {
      Rectangle<int> result;
      if(child_event_bounds().intersect(widget->bounds(), result) && result.contain(int_point)) {
        cant_invoke = widget->invoke_fun_for_event(point, fun);
        break;
      }
    }
    return !cant_invoke ? fun(this, point) : true;
  }
}
