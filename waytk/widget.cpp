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
#include <cstring>
#include <limits>
#include <waytk.hpp>

using namespace std;

namespace waytk
{
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

  void Widget::set_focus(bool has_focus)
  { throw exception(); }

  PseudoClasses Widget::real_pseudo_classes()
  { return _M_pseudo_classes | (surface().lock()->is_active() ? PseudoClasses::NONE : PseudoClasses::BACKDROP); }

  const weak_ptr<Surface> &Widget::surface()
  {
    Widget *root;
    for(root = this; root->_M_parent != nullptr; root = root->_M_parent);
    return root->_M_surface;
  }

  void Widget::add_pointer(const Pointer &pointer)
  { throw exception(); }

  bool Widget::delete_pointer(const Pointer &pointer)
  { throw exception(); }

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
    update_child_points(area_bounds);
  }

  void Widget::update_child_points(const Rectangle<int> &area_bounds) {}

  void Widget::update_size(Canvas *canvas, const Dimension<int> &area_size, const HAlignment *h_align, const VAlignment *v_align)
  {
    PseudoClasses pseudo_classes = real_pseudo_classes();
    Styles *styles = find_styles(name());
    Edges<int> border = styles->border(pseudo_classes);
    Edges<int> padding = styles->padding(pseudo_classes);
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
      _M_bounds.width = min(max(_M_bounds.height, _M_min_height), tmp_area_size.height);
    } else
      _M_bounds.height = tmp_area_size.height;
  }

  void Widget::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { _M_content_size = Dimension<int>(0, 0); }

  void Widget::update_child_sizes(Canvas *canvas, const Dimension<int> &area_size) {}

  bool Widget::can_again_update_width() const
  { return _M_h_align == HAlignment::FILL; }

  bool Widget::can_again_update_height() const
  { return _M_v_align == VAlignment::FILL; }

  void Widget::draw(Canvas *canvas)
  {
    PseudoClasses pseudo_classes = real_pseudo_classes();
    styles()->draw_background(pseudo_classes, canvas, _M_bounds);
    Edges<int> border = styles()->border(pseudo_classes);
    Edges<int> padding = styles()->padding(pseudo_classes);
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
  { throw exception(); }

  bool Widget::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  {
    switch(state) {
      case TouchState::UP:
        add_pointer(pointer);
        break;
      case TouchState::DOWN:
        delete_pointer(pointer);
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
    delete_pointer(pointer);
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

  void Widget::on_scoll(Viewport *viewport)
  { _M_on_scroll_callback(this, viewport); }

  Dimension<int> Widget::block_margin_box_size(const char *name, PseudoClasses pseudo_classes, const Dimension<int> &content_size)
  {
    PseudoClasses tmp_pseudo_classes = real_pseudo_classes();
    tmp_pseudo_classes &= ~(PseudoClasses::ACTIVE | PseudoClasses::CHECKED | PseudoClasses::FOCUS | PseudoClasses::HOVER | PseudoClasses::SELECTED);
    tmp_pseudo_classes |= pseudo_classes;
    Styles *styles = find_styles(name);
    Edges<int> margin = styles->margin(tmp_pseudo_classes);
    Edges<int> border = styles->border(tmp_pseudo_classes);
    Edges<int> padding = styles->padding(tmp_pseudo_classes);
    Dimension<int> size;
    size.width = margin.left + margin.right + border.left + border.right + padding.left + padding.right;
    size.height = margin.top + margin.bottom + border.top + border.bottom + padding.left + padding.bottom;
    return size;
  }

  void Widget::draw_block(const char *name, PseudoClasses pseudo_classes, Canvas *canvas, const Point<int> &margin_box_point, const Dimension<int> &margin_box_size, Rectangle<int> &inner_bounds, Styles *&styles)
  {
    PseudoClasses tmp_pseudo_classes = real_pseudo_classes();
    tmp_pseudo_classes &= ~(PseudoClasses::ACTIVE | PseudoClasses::CHECKED | PseudoClasses::FOCUS | PseudoClasses::HOVER | PseudoClasses::SELECTED);
    tmp_pseudo_classes |= pseudo_classes;
    styles = find_styles(name);
    Edges<int> margin = styles->margin(tmp_pseudo_classes);
    Edges<int> border = styles->border(tmp_pseudo_classes);
    Edges<int> padding = styles->padding(tmp_pseudo_classes);
    Rectangle<int> bounds(margin_box_point.x, margin_box_point.y, margin_box_size.width, margin_box_size.height);
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
    Edges<int> border = styles()->border(real_pseudo_classes());
    Edges<int> padding = styles()->padding(real_pseudo_classes());
    Dimension<int> tmp_size = size;
    if(size.width != numeric_limits<int>::max())
      tmp_size.width -= border.left + border.right + padding.left + padding.right;
    if(size.height != numeric_limits<int>::max())
      tmp_size.height -= border.top + border.bottom + padding.top + padding.bottom;
    tmp_size.width = max(tmp_size.width, 0);
    tmp_size.height = max(tmp_size.height, 0);
    return tmp_size;
  }

  Rectangle<int> Widget::area_bounds_to_inner_area_bounds(const Rectangle<int> &bounds)
  {
    Edges<int> border = styles()->border(real_pseudo_classes());
    Edges<int> padding = styles()->padding(real_pseudo_classes());
    Rectangle<int> tmp_bounds = bounds;
    tmp_bounds.x += border.left + padding.left;
    tmp_bounds.y += border.top + padding.top;
    if(bounds.width != numeric_limits<int>::max())
      tmp_bounds.width -= border.left + border.right + padding.left + padding.right;
    if(bounds.height != numeric_limits<int>::max())
      tmp_bounds.height -= border.top + border.bottom + padding.top + padding.bottom;
    tmp_bounds.width = max(tmp_bounds.width, 0);
    tmp_bounds.height = max(tmp_bounds.height, 0);
    return tmp_bounds;
  }
}
