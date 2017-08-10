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
#include <cstring>
#include <limits>
#include <utility>
#include "widget_viewport.hpp"

using namespace std;

namespace waytk
{ 
  namespace priv
  {
    //
    // A WidgetViewport class.
    //

    WidgetViewport::~WidgetViewport() {}

    Edges<int> WidgetViewport::margin() const
    { return Edges<int>(0, 0, 0, 0); }

    Point<int> WidgetViewport::point() const
    { return _M_bounds.point(); }

    Dimension<int> WidgetViewport::size() const
    { return _M_bounds.size(); }

    void WidgetViewport::set_size(const Dimension<int> &size)
    {
      _M_bounds.width = size.width;
      _M_bounds.height = size.height;
    }

    void WidgetViewport::update_point(Canvas *canvas)
    { _M_bounds.x = _M_bounds.y = 0; }

    void WidgetViewport::move_view_to_top()
    { _M_view_point.y = 0; }

    void WidgetViewport::move_view_to_bottom()
    { _M_view_point.y = max(client_size().height - _M_bounds.height, 0); }

    void WidgetViewport::h_move_view(int x)
    {
      _M_view_point.x += x;
      _M_view_point.x = min(max(_M_view_point.x, 0), max(client_size().width - _M_bounds.width, 0));
    }

    void WidgetViewport::v_move_view(int y)
    {
      _M_view_point.y += y;
      _M_view_point.y = min(max(_M_view_point.y, 0), max(client_size().height - _M_bounds.height, 0));
    }

    int WidgetViewport::h_scroll_slider_x(int width) const
    { return static_cast<int64_t>(_M_view_point.x) * width / _M_bounds.width; }

    void WidgetViewport::set_h_scroll_slider_x(int x, int width)
    {
      _M_view_point.x = static_cast<int64_t>(x) * _M_bounds.width / width;
      _M_view_point.x = min(max(_M_view_point.x, 0), max(client_size().width - _M_bounds.width, 0));
    }

    void WidgetViewport::add_onto_h_scroll_slider_x(int x, int width)
    {
      _M_view_point.x += static_cast<int64_t>(x) * _M_bounds.width / width;
      _M_view_point.x = min(max(_M_view_point.x, 0), max(client_size().width - _M_bounds.width, 0));
    }

    int WidgetViewport::h_scroll_slider_width(int width) const
    {
      int64_t x1 = client_size().width - _M_bounds.width;
      int64_t x2 = client_size().width;
      return x2 * width / _M_bounds.width - x1 * width / _M_bounds.width;
    }

    int WidgetViewport::v_scroll_slider_y(int height) const
    { return static_cast<int64_t>(_M_view_point.y) * height / _M_bounds.height; }

    void WidgetViewport::set_v_scroll_slider_y(int y, int height)
    {
      _M_view_point.y = static_cast<int64_t>(y) * _M_bounds.height / height;
      _M_view_point.y = min(max(_M_view_point.y, 0), max(client_size().height - _M_bounds.height, 0));
    }

    void WidgetViewport::add_onto_v_scroll_slider_y(int y, int height)
    {
      _M_view_point.y += static_cast<int64_t>(y) * _M_bounds.height / height;
      _M_view_point.y = min(max(_M_view_point.y, 0), max(client_size().height - _M_bounds.height, 0));
    }

    int WidgetViewport::v_scroll_slider_height(int height) const
    {
      int64_t y1 = client_size().height - _M_bounds.height;
      int64_t y2 = client_size().height;
      return y2 * height / _M_bounds.height - y1 * height / _M_bounds.height;
    }
    
    bool WidgetViewport::width_is_less_than_clien_width() const
    { return _M_bounds.width < client_size().width; }

    bool WidgetViewport::height_is_less_than_clien_height() const
    { return _M_bounds.height < client_size().height; }

    int WidgetViewport::max_width() const
    { return client_size().width; }

    int WidgetViewport::max_height() const
    { return client_size().height; }

    void WidgetViewport::update_client_point(const Point<int> &viewport_point)
    {
      Edges<int> margin = _M_widget->margin();
      Rectangle<int> area_bounds;
      area_bounds.x = viewport_point.x - _M_view_point.x;
      area_bounds.y = viewport_point.y - _M_view_point.y;
      area_bounds.width = numeric_limits<int>::max();
      area_bounds.height = numeric_limits<int>::max();
      area_bounds.x -= margin.left;
      area_bounds.y -= margin.top;
      _M_widget->update_point(area_bounds);
    }

    void WidgetViewport::update_client_size(Canvas *canvas)
    {
      Dimension<int> area_size(numeric_limits<int>::max(), numeric_limits<int>::max());
      _M_widget->update_size(canvas, area_size);
    }

    Dimension<int> WidgetViewport::client_size() const
    { return _M_widget->bounds().size(); }

    void WidgetViewport::update_widget_point(const Rectangle<int> &area_bounds) {}

    void WidgetViewport::update_widget_size(Canvas *canvas, const Dimension<int> &area_size) {}
  }

  //
  // An Icon class.
  //

  shared_ptr<CanvasImage> Icon::image(IconSize size) const
  { throw exception(); }

  //
  // A Viewport class.
  //

  Viewport::~Viewport() {}

  //
  // A Widget class.
  //

  Widget::Widget() :
    _M_is_enabled(true),
    _M_has_focus(false),
    _M_pseudo_classes(PseudoClasses::NONE),
    _M_is_visible(true),
    _M_h_align(HAlignment::CENTER),
    _M_v_align(VAlignment::CENTER),
    _M_max_width(numeric_limits<int>::max()),
    _M_max_height(numeric_limits<int>::max()),
    _M_min_width(0),
    _M_min_height(0),
    _M_weight(0),
    _M_bounds(0, 0, 0, 0),
    _M_parent(nullptr),
    _M_style_name(nullptr),
    _M_styles(nullptr),
    _M_content_size(0, 0),
    _M_on_touch_callback([](Widget *widget, const Pointer &pointer, const Point<double> &point, TouchState state) {}),
    _M_on_pointer_motion_callback([](Widget *widget, const Point<double> &point) {}),
    _M_on_pointer_leave_callback([](Widget *widget) {}),
    _M_on_pointer_axis_callback([](Widget *widget, Axis axis, double value) {}),
    _M_on_key_callback([](Widget *widget, uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state) {}),
    _M_on_scroll_callback([](Widget *widgwet, Viewport *viewport) {}) {}

  Widget::~Widget() {}
  
  void Widget::set_enabled(bool is_enabled)
  {
    _M_is_enabled = is_enabled;
    if(_M_is_enabled)
      _M_pseudo_classes &= ~PseudoClasses::DISABLED;
    else
      _M_pseudo_classes |= PseudoClasses::DISABLED;
  }

  bool Widget::set_focus(bool has_focus)
  {
    shared_ptr<Surface> tmp_surface = surface().lock();
    if(tmp_surface.get() == nullptr) return false;
    if(has_focus) {
      if(tmp_surface->_M_focused_widget != nullptr) {
        tmp_surface->_M_focused_widget->_M_has_focus = false;
        tmp_surface->_M_focused_widget->_M_pseudo_classes &= ~PseudoClasses::FOCUS;
      }
      tmp_surface->_M_focused_widget = this;
    } else {
      if(tmp_surface->_M_focused_widget == this)
        tmp_surface->_M_focused_widget = nullptr;
    }
    _M_has_focus = has_focus;
    if(_M_has_focus)
      _M_pseudo_classes |= PseudoClasses::FOCUS;
    else
      _M_pseudo_classes &= ~PseudoClasses::FOCUS;
    return true;
  }

  const weak_ptr<Surface> &Widget::surface()
  {
    Widget *root;
    for(root = this; root->_M_parent != nullptr; root = root->_M_parent);
    return root->_M_surface;
  }

  bool Widget::block_has_pointer(Block block, const Pointer &pointer)
  {
    shared_ptr<Surface> tmp_surface = surface().lock();
    if(tmp_surface.get() == nullptr) return false;
    auto iter = tmp_surface->_M_touch_infos.find(pointer);
    if(iter != tmp_surface->_M_touch_infos.end())
      return iter->second.widget == this && iter->second.block == block;
    else
      return false;
  }

  void Widget::add_pointer(Block block, const Pointer &pointer)
  {
    shared_ptr<Surface> tmp_surface = surface().lock();
    if(tmp_surface.get() == nullptr)
      throw new RuntimeException("widget hasn't surface");
    Surface::TouchInfo info(this, block);
    tmp_surface->_M_touch_infos[pointer] = info;
  }

  bool Widget::delete_pointer(Block block, const Pointer &pointer)
  {
    shared_ptr<Surface> tmp_surface = surface().lock();
    if(tmp_surface.get() == nullptr)
      throw new RuntimeException("widget hasn't surface");
    auto iter = tmp_surface->_M_touch_infos.find(pointer);
    if(iter != tmp_surface->_M_touch_infos.end() && iter->second.widget == this && iter->second.block == block) {
      tmp_surface->_M_touch_infos.erase(iter);
      return true;
    } else
      return false;
  }

  bool Widget::delete_pointer_from_any_block(const Pointer &pointer)
  {
    shared_ptr<Surface> tmp_surface = surface().lock();
    if(tmp_surface.get() == nullptr)
      throw new RuntimeException("widget hasn't surface");
    auto iter = tmp_surface->_M_touch_infos.find(pointer);
    if(iter != tmp_surface->_M_touch_infos.end() && iter->second.widget == this) {
      tmp_surface->_M_touch_infos.erase(iter);
      return true;
    } else
      return false;
  }

  bool Widget::lock_touch_leaving(const Pointer &pointer)
  {
    shared_ptr<Surface> tmp_surface = surface().lock();
    if(tmp_surface.get() == nullptr)
      throw new RuntimeException("widget hasn't surface");
    auto pair = tmp_surface->_M_touch_leaving_lock_widgets.insert(make_pair(pointer, this));
    if(pair.second) {
      pair.first->second = this;
      return true;
    } else
      return false;
  }

  bool Widget::unlock_touch_leaving(const Pointer &pointer)
  {
    shared_ptr<Surface> tmp_surface = surface().lock();
    if(tmp_surface.get() == nullptr)
      throw new RuntimeException("widget hasn't surface");
    auto iter = tmp_surface->_M_touch_leaving_lock_widgets.find(pointer);
    if(iter != tmp_surface->_M_touch_leaving_lock_widgets.end() && iter->second == this) {
      tmp_surface->_M_touch_leaving_lock_widgets.erase(iter);
      return true;
    } else
      return false;
  }

  Styles *Widget::styles()
  {
    if(_M_style_name != name() && strcmp(_M_style_name, name()) != 0) {
      _M_style_name = name();
      _M_styles = find_styles(_M_style_name);
    }
    return _M_styles;
  }
  
  const char *Widget::name() const
  { return "widget"; }

  void Widget::update_pseudo_classes()
  {
    if(surface().lock()->is_active())
      _M_pseudo_classes |= PseudoClasses::BACKDROP;
    else
      _M_pseudo_classes &= ~PseudoClasses::BACKDROP;
  }

  void Widget::update_point(const Rectangle<int> &area_bounds, const HAlignment *h_align, const VAlignment *v_align)
  {
    if(h_align == nullptr) h_align = &_M_h_align;
    if(v_align == nullptr) v_align = &_M_v_align;
    switch(*h_align) {
      case HAlignment::LEFT:
        _M_bounds.x = area_bounds.x;
        break;
      case HAlignment::CENTER:
        if(area_bounds.width != numeric_limits<int>::max())
          _M_bounds.x = area_bounds.x + (area_bounds.width - _M_bounds.width) / 2;
        else
          _M_bounds.x = area_bounds.x;
        break;
      case HAlignment::RIGHT:
        if(area_bounds.width != numeric_limits<int>::max())
          _M_bounds.x = area_bounds.x + (area_bounds.width - _M_bounds.width);
        else
          _M_bounds.x = area_bounds.x;
        break;
      case HAlignment::FILL:
        _M_bounds.x = area_bounds.x;
        break;
    }
    switch(*v_align) {
      case VAlignment::TOP:
        _M_bounds.y = area_bounds.y;
        break;
      case VAlignment::CENTER:
        if(area_bounds.height != numeric_limits<int>::max())
          _M_bounds.y = area_bounds.y + (area_bounds.height - _M_bounds.height) / 2;
        else
          _M_bounds.y = area_bounds.y;
        break;
      case VAlignment::BOTTOM:
        if(area_bounds.height != numeric_limits<int>::max())
          _M_bounds.y = area_bounds.y + (area_bounds.height - _M_bounds.height);
        else
          _M_bounds.y = area_bounds.y;
        break;
      case VAlignment::FILL:
        _M_bounds.y = area_bounds.y;
        break;
    }
    update_child_points();
  }

  void Widget::update_child_points() {}

  void Widget::update_size(Canvas *canvas, const Dimension<int> &area_size, const HAlignment *h_align, const VAlignment *v_align)
  {
    Styles *styles = find_styles(name());
    Edges<int> border = styles->border(_M_pseudo_classes);
    Edges<int> padding = styles->padding(_M_pseudo_classes);
    Dimension<int> tmp_area_size = area_size;
    if(h_align == nullptr) h_align = &_M_h_align;
    if(v_align == nullptr) v_align = &_M_v_align;
    tmp_area_size.width = min(tmp_area_size.width, _M_max_width);
    tmp_area_size.height = min(tmp_area_size.height, _M_max_height);
    update_child_sizes(canvas, tmp_area_size);
    update_content_size(canvas, tmp_area_size);
    if(*h_align != HAlignment::FILL || area_size.width == numeric_limits<int>::max()) {
      _M_bounds.width = _M_content_size.width + border.left + border.right + padding.left + padding.right;
      _M_bounds.width = min(max(_M_bounds.width, _M_min_width), tmp_area_size.width);
    } else
      _M_bounds.width = tmp_area_size.width;
    if(*v_align != VAlignment::FILL || area_size.height == numeric_limits<int>::max()) {
      _M_bounds.height = _M_content_size.height + border.top + border.bottom + padding.top + padding.bottom;
      _M_bounds.height = min(max(_M_bounds.height, _M_min_height), tmp_area_size.height);
    } else
      _M_bounds.height = tmp_area_size.height;
  }

  void Widget::update_child_sizes(Canvas *canvas, const Dimension<int> &area_size) {}

  void Widget::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { _M_content_size = Dimension<int>(0, 0); }

  bool Widget::can_again_update_width() const
  { return _M_h_align == HAlignment::FILL; }

  bool Widget::can_again_update_height() const
  { return _M_v_align == VAlignment::FILL; }

  void Widget::draw(Canvas *canvas)
  {
    styles()->draw_background(_M_pseudo_classes, canvas, _M_bounds);
    Edges<int> border = styles()->border(_M_pseudo_classes);
    Edges<int> padding = styles()->padding(_M_pseudo_classes);
    Rectangle<int> inner_bounds = _M_bounds;
    inner_bounds.x += border.left + padding.left;
    inner_bounds.y += border.top + padding.top;
    inner_bounds.width -= border.left + border.right + padding.left + padding.right;
    inner_bounds.height -= border.top + border.bottom + padding.top + padding.bottom;
    inner_bounds.width = max(inner_bounds.width, 0);
    inner_bounds.height = max(inner_bounds.height, 0);
    draw_content(canvas, inner_bounds);
    draw_children(canvas, inner_bounds);
  }

  void Widget::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds) {}

  void Widget::draw_children(Canvas *canvas, const Rectangle<int> &inner_bounds) {}

  Viewport *Widget::viewport()
  { return new priv::WidgetViewport(this); }

  Rectangle<int> Widget::child_event_bounds() const
  { return _M_bounds; }

  bool Widget::invoke_fun_for_event(const Point<double> &point, const function<bool (Widget *, const Point<double> &)> &fun)
  { return fun(this, point); }

  bool Widget::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  { return on_touch_for_block(Block(), pointer, point, state); }

  bool Widget::on_touch_for_block(Block block, const Pointer &pointer, const Point<double> &point, TouchState state)
  {
    switch(state) {
      case TouchState::UP:
        add_pointer(block, pointer);
        break;
      case TouchState::DOWN:
        delete_pointer(block, pointer);
        break;
      default:
        break;
    }
    if(pointer.is_touch()) _M_pseudo_classes |= PseudoClasses::HOVER;
    _M_on_touch_callback(this, pointer, point, state);
    return false;
  }

  void Widget::on_touch_leave(const Pointer &pointer)
  {
    _M_pseudo_classes &= ~PseudoClasses::HOVER;
    delete_pointer_from_any_block(pointer);
    _M_on_touch_leave_callback(this, pointer);
  }

  bool Widget::on_pointer_motion(const Point<double> &point)
  {
    _M_pseudo_classes |= PseudoClasses::HOVER;
    _M_on_pointer_motion_callback(this, point);
    return false;
  }

  void Widget::on_pointer_leave()
  {
    _M_pseudo_classes &= ~PseudoClasses::HOVER;
    _M_on_pointer_leave_callback(this);
  }

  bool Widget::on_pointer_axis(Axis axis, double value)
  { _M_on_pointer_axis_callback(this, axis, value); return false; }

  bool Widget::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  { _M_on_key_callback(this, key_sym, modifiers, utf8, state); return false; }

  void Widget::on_scroll(Viewport *viewport)
  { _M_on_scroll_callback(this, viewport); }

  Dimension<int> Widget::block_margin_box_size(const char *name, PseudoClasses pseudo_classes, const Dimension<int> &content_size)
  {
    PseudoClasses tmp_pseudo_classes = _M_pseudo_classes;
    tmp_pseudo_classes &= ~(PseudoClasses::ACTIVE | PseudoClasses::CHECKED | PseudoClasses::FOCUS | PseudoClasses::HOVER | PseudoClasses::SELECTED);
    tmp_pseudo_classes |= pseudo_classes;
    Styles *styles = find_styles(name);
    Edges<int> margin = styles->margin(tmp_pseudo_classes);
    Edges<int> border = styles->border(tmp_pseudo_classes);
    Edges<int> padding = styles->padding(tmp_pseudo_classes);
    Dimension<int> size = content_size;
    size.width += margin.left + margin.right + border.left + border.right + padding.left + padding.right;
    size.height += margin.top + margin.bottom + border.top + border.bottom + padding.left + padding.bottom;
    return size;
  }

  void Widget::draw_block(const char *name, PseudoClasses pseudo_classes, Canvas *canvas, const Point<int> &margin_box_point, const Dimension<int> &margin_box_size, Rectangle<int> &inner_bounds, Rectangle<int> &bounds, Styles *&styles)
  {
    PseudoClasses tmp_pseudo_classes = _M_pseudo_classes;
    tmp_pseudo_classes &= ~(PseudoClasses::ACTIVE | PseudoClasses::CHECKED | PseudoClasses::FOCUS | PseudoClasses::HOVER | PseudoClasses::SELECTED);
    tmp_pseudo_classes |= pseudo_classes;
    styles = find_styles(name);
    Edges<int> margin = styles->margin(tmp_pseudo_classes);
    Edges<int> border = styles->border(tmp_pseudo_classes);
    Edges<int> padding = styles->padding(tmp_pseudo_classes);
    bounds = Rectangle<int>(margin_box_point, margin_box_size);
    bounds.x += margin.left;
    bounds.y += margin.top;
    bounds.width -= margin.right;
    bounds.height -= margin.bottom;
    bounds.width = max(bounds.width, 0);
    bounds.height = max(bounds.height, 0);
    styles->draw_background(tmp_pseudo_classes, canvas, bounds);
    inner_bounds = bounds;
    inner_bounds.x += border.left + padding.left;
    inner_bounds.y += border.top + padding.top;
    inner_bounds.width -= border.left + border.right + padding.left + padding.right;
    inner_bounds.height -= border.top + border.bottom + padding.top + padding.bottom;
    inner_bounds.width = max(inner_bounds.width, 0);
    inner_bounds.height = max(inner_bounds.height, 0);
  }

  Dimension<int> Widget::area_size_to_inner_area_size(const Dimension<int> &size)
  {
    Edges<int> border = styles()->border(_M_pseudo_classes);
    Edges<int> padding = styles()->padding(_M_pseudo_classes);
    Dimension<int> tmp_size = size;
    if(size.width != numeric_limits<int>::max())
      tmp_size.width -= border.left + border.right + padding.left + padding.right;
    if(size.height != numeric_limits<int>::max())
      tmp_size.height -= border.top + border.bottom + padding.top + padding.bottom;
    tmp_size.width = max(tmp_size.width, 0);
    tmp_size.height = max(tmp_size.height, 0);
    return tmp_size;
  }

  Rectangle<int> Widget::bounds_to_inner_bounds(const Rectangle<int> &bounds)
  {
    Edges<int> border = styles()->border(_M_pseudo_classes);
    Edges<int> padding = styles()->padding(_M_pseudo_classes);
    Rectangle<int> tmp_bounds = bounds;
    tmp_bounds.x += border.left + padding.left;
    tmp_bounds.y += border.top + padding.top;
    tmp_bounds.width -= border.left + border.right + padding.left + padding.right;
    tmp_bounds.height -= border.top + border.bottom + padding.top + padding.bottom;
    tmp_bounds.width = max(tmp_bounds.width, 0);
    tmp_bounds.height = max(tmp_bounds.height, 0);
    return tmp_bounds;
  }
}
