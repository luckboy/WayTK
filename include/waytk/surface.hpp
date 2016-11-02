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
#ifndef _WAYTK_SURFACE_HPP
#define _WAYTK_SURFACE_HPP

#include <functional>
#include <memory>
#include <string>
#include <waytk/structs.hpp>

namespace waytk
{
  class Surface;
  class Widget;

  enum class SurfaceState
  {
    TOPLEVEL,
    MAXIMIZED,
    MINIMIZED,
    TRANSIENT,
    POPUP
  };

  typedef std::function<void (const std::shared_ptr<Surface> &, const Dimension<int> &)> OnSizeChangeListener;
  typedef std::function<void (const std::shared_ptr<Surface> &)> OnTouchCancelListener;

  class Surface
  {
    std::string _M_title;
    std::unique_ptr<Widget> _M_root_widget;
    bool _M_is_modal;
    SurfaceState _M_state;
    std::shared_ptr<Surface> _M_parent;
    Point<int> _M_point_in_parent;
    bool _M_is_resizable;
    bool _M_is_visible;
    Dimension<int> _M_size;
    OnSizeChangeListener _M_on_size_change_listener;
    OnTouchCancelListener _M_on_touch_cancel_listener;
  public:
    Surface(Widget *widget);

    Surface(const std::string &title, Widget *widget);

    Surface(const std::shared_ptr<Surface> &parent, const std::string &title, Widget *widget, const Point<int> &point);

    Surface(const std::shared_ptr<Surface> &parent, Widget *widget, const Point<int> &point);

    virtual ~Surface();

    const std::string &title() const
    { return _M_title; }

    void set_title(const std::string &title)
    { _M_title = title; }
    
    Widget *root_widget() const
    { return _M_root_widget.get(); }

    void set_root_widget(Widget *widget)
    { _M_root_widget = std::unique_ptr<Widget>(widget); }

    const std::shared_ptr<Surface> &perent() const
    { return _M_parent; }

    bool is_modal() const
    { return _M_is_modal; }

    bool set_modal(bool is_modal);

    SurfaceState state() const
    { return _M_state; }

    const std::shared_ptr<Surface> &parent() const
    { return _M_parent; }

    const Point<int> point_in_parent() const
    { return _M_point_in_parent; }

    void set_toplevel()
    {
      _M_state = SurfaceState::TOPLEVEL;
      _M_parent = std::shared_ptr<Surface>();
    }

    void set_maximized()
    {
      _M_state = SurfaceState::MAXIMIZED;
      _M_parent = std::shared_ptr<Surface>();
    }

    void set_minimized()
    {
      _M_state = SurfaceState::MINIMIZED;
      _M_parent = std::shared_ptr<Surface>();
    }

    void set_transient(const std::shared_ptr<Surface> &parent, const Point<int> &point)
    {
      _M_state = SurfaceState::TRANSIENT;
      _M_parent = parent;
      _M_point_in_parent = point;
    }

    void set_popup(const std::shared_ptr<Surface> &parent, const Point<int> &point)
    {
      _M_state = SurfaceState::POPUP;
      _M_parent = parent;
      _M_point_in_parent = point;
    }

    bool is_resizable() const
    { return _M_is_resizable; }

    void set_resizable(bool is_resizable)
    { _M_is_resizable = is_resizable; }

    bool is_visible() const
    { return _M_is_visible; }

    bool set_visible(bool is_visible);

    const Dimension<int> &size() const
    { return _M_size; }

    void set_size(const Dimension<int> &size);

    const OnSizeChangeListener &on_size_change_listener() const
    { return _M_on_size_change_listener; }

    void set_on_size_change_listener(const OnSizeChangeListener &listener)
    { _M_on_size_change_listener = listener; }

    const OnTouchCancelListener &on_touch_cancel_listener() const
    { return _M_on_touch_cancel_listener; }

    void set_on_touch_cancel_listener(const OnTouchCancelListener &listener)
    { _M_on_touch_cancel_listener = listener; }

    virtual void on_size_change(const std::shared_ptr<Surface> &surface, const Dimension<int> &size);

    virtual void on_touch_cancel(const std::shared_ptr<Surface> &surface);
  };
}

#endif
