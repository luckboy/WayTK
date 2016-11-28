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
#include <limits>
#include "waytk_priv.hpp"

using namespace std;

namespace waytk
{
  Surface::Surface(Widget *widget) :
    _M_root_widget(widget),
    _M_is_modal(false),
    _M_state(SurfaceState::TOPLEVEL),
    _M_point_in_parent(0, 0),
    _M_is_resizable(true),
    _M_is_visible(true),
    _M_size(numeric_limits<int>::max(), numeric_limits<int>::max()),
    _M_on_size_change_callback([](const shared_ptr<Surface> &surface, const Dimension<int> &size) {}),
    _M_on_touch_cancel_callback([](const shared_ptr<Surface> &surface) {}),
    _M_focused_widget(nullptr) {}

  Surface::Surface(const string &title, Widget *widget) :
    _M_title(title),
    _M_root_widget(widget),
    _M_is_modal(false),
    _M_state(SurfaceState::TOPLEVEL),
    _M_point_in_parent(0, 0),
    _M_is_resizable(true),
    _M_is_visible(true),
    _M_size(numeric_limits<int>::max(), numeric_limits<int>::max()),
    _M_on_size_change_callback([](const shared_ptr<Surface> &surface, const Dimension<int> &size) {}),
    _M_on_touch_cancel_callback([](const shared_ptr<Surface> &surface) {}),
    _M_focused_widget(nullptr) {}

  Surface::Surface(const shared_ptr<Surface> &parent, const string &title, Widget *widget, const Point<int> &point) :
    _M_title(title),
    _M_root_widget(widget),
    _M_is_modal(false),
    _M_state(SurfaceState::TRANSIENT),
    _M_parent(parent),
    _M_point_in_parent(point),
    _M_is_resizable(true),
    _M_is_visible(true),
    _M_size(numeric_limits<int>::max(), numeric_limits<int>::max()),
    _M_on_size_change_callback([](const shared_ptr<Surface> &surface, const Dimension<int> &size) {}),
    _M_on_touch_cancel_callback([](const shared_ptr<Surface> &surface) {}),
    _M_focused_widget(nullptr) {}

  Surface::Surface(const shared_ptr<Surface> &parent, Widget *widget, const Point<int> &point) :
    _M_root_widget(widget),
    _M_is_modal(false),
    _M_state(SurfaceState::POPUP),
    _M_parent(parent),
    _M_point_in_parent(point),
    _M_is_resizable(false),
    _M_is_visible(true),
    _M_size(numeric_limits<int>::max(), numeric_limits<int>::max()),
    _M_on_size_change_callback([](const shared_ptr<Surface> &surface, const Dimension<int> &size) {}),
    _M_on_touch_cancel_callback([](const shared_ptr<Surface> &surface) {}),
    _M_focused_widget(nullptr) {}

  Surface::~Surface() {}

  void Surface::set_title(const string &title)
  { normalize_utf8(title, _M_title); }

  bool Surface::is_active() const
  { throw exception(); }

  void Surface::set_modal(bool is_modal)
  {
    if(_M_is_visible && is_added_surface(this)) {
      if(is_modal) {
        if(!_M_is_modal) priv::push_visible_modal_surface(this);
      } else {
        if(_M_is_modal) priv::delete_visible_modal_surface(this);
      }
    }
    _M_is_modal = is_modal;
  }

  void Surface::set_visible(bool is_visible)
  {
    if(_M_is_modal && is_added_surface(this)) {
      if(is_visible) {
        if(!_M_is_visible) priv::push_visible_modal_surface(this);
      } else {
        if(_M_is_visible) priv::delete_visible_modal_surface(this);
      }
    }
    _M_is_visible = is_visible;
  }

  void Surface::on_size_change(const shared_ptr<Surface> &surface, const Dimension<int> &size)
  { throw exception(); }

  void Surface::on_touch_cancel(const shared_ptr<Surface> &surface)
  { throw exception(); }
}
