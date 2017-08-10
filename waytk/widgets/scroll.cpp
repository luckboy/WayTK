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
#include <xkbcommon/xkbcommon-keysyms.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <waytk.hpp>

using namespace std;

namespace waytk
{
  const Block Scroll::H_SCROLL_BAR = Block(1);
  const Block Scroll::LEFT_BUTTON = Block(2);
  const Block Scroll::H_SLIDER = Block(3);
  const Block Scroll::RIGHT_BUTTON = Block(4);
  const Block Scroll::V_SCROLL_BAR = Block(5);
  const Block Scroll::TOP_BUTTON = Block(6);
  const Block Scroll::V_SLIDER = Block(7);
  const Block Scroll::BOTTOM_BUTTON = Block(8);
 
  Scroll::~Scroll() {}

  void Scroll::initialize(Widget *widget)
  {
    set_this_as_widget_parent(widget);
    _M_widget =  unique_ptr<Widget>(widget);
    _M_has_h_scroll_bar = true;
    _M_has_auto_h_scroll_bar = false;
    _M_has_v_scroll_bar = true;
    _M_has_auto_v_scroll_bar = false;
    _M_viewport = unique_ptr<Viewport>(widget->viewport());
    _M_has_enabled_h_scroll_bar = true;
    _M_has_visible_h_scroll_bar = false;
    _M_has_enabled_v_scroll_bar = true;
    _M_has_visible_v_scroll_bar = false;
    _M_left_button_touch_count = 0;
    _M_right_button_touch_count = 0;
    _M_top_button_touch_count = 0;
    _M_bottom_button_touch_count = 0;
    _M_has_h_slider_pointer = false;
    _M_h_slider_pointer = Pointer();
    _M_old_h_slider_pointer_point = Point<int>(0, 0);
    _M_has_v_slider_pointer = false;
    _M_v_slider_pointer = Pointer();
    _M_old_v_slider_pointer_point = Point<int>(0, 0);
    _M_viewport_widget_bounds = Rectangle<int>(0, 0, 0, 0);
    _M_h_scroll_bar_pseudo_classes = PseudoClasses::NONE;
    _M_h_scroll_bar_margin_box_size = Dimension<int>(0, 0);
    _M_h_scroll_bar_bounds = Rectangle<int>(0, 0, 0, 0);
    _M_h_trough_bounds = Rectangle<int>(0, 0, 0, 0);
    _M_center_h_trough_x = 0;
    _M_left_button_pseudo_classes = PseudoClasses::NONE;
    _M_left_button_margin_box_size = Dimension<int>(0, 0);
    _M_left_button_bounds = Rectangle<int>(0, 0, 0, 0);
    _M_h_slider_pseudo_classes = PseudoClasses::NONE;
    _M_h_slider_margin_box_size = Dimension<int>(0, 0);
    _M_min_h_slider_margin_box_width = 0;
    _M_h_slider_bounds = Rectangle<int>(0, 0, 0, 0);
    _M_right_button_pseudo_classes = PseudoClasses::NONE;
    _M_right_button_margin_box_size = Dimension<int>(0, 0);
    _M_right_button_bounds = Rectangle<int>(0, 0, 0, 0);
    _M_v_scroll_bar_pseudo_classes = PseudoClasses::NONE;
    _M_v_scroll_bar_margin_box_size = Dimension<int>(0, 0);
    _M_v_scroll_bar_bounds = Rectangle<int>(0, 0, 0, 0);
    _M_v_trough_bounds = Rectangle<int>(0, 0, 0, 0);
    _M_center_v_trough_y = 0;
    _M_top_button_pseudo_classes = PseudoClasses::NONE;
    _M_top_button_margin_box_size = Dimension<int>(0, 0);
    _M_top_button_bounds = Rectangle<int>(0, 0, 0, 0);
    _M_v_slider_pseudo_classes = PseudoClasses::NONE;
    _M_v_slider_margin_box_size = Dimension<int>(0, 0);
    _M_min_v_slider_margin_box_height = 0;
    _M_v_slider_bounds = Rectangle<int>(0, 0, 0, 0);
    _M_bottom_button_pseudo_classes = PseudoClasses::NONE;
    _M_bottom_button_margin_box_size = Dimension<int>(0, 0);
    _M_bottom_button_bounds = Rectangle<int>(0, 0, 0, 0);
  }

  void Scroll::set_widget(Widget *widget)
  {
    if(_M_widget.get() != nullptr) unset_this_as_widget_parent(_M_widget.get());
    _M_widget = unique_ptr<Widget>(widget);
    set_this_as_widget_parent(_M_widget.get());
  }

  const char *Scroll::name() const
  { return "scroll"; }

  void Scroll::update_child_points()
  {
    Rectangle<int> inner_bounds = bounds_to_inner_bounds(bounds());
    Point<int> viewport_point = inner_bounds.point();
    viewport_point.x += _M_viewport->point().x;
    viewport_point.y += _M_viewport->point().y;
    _M_viewport->update_client_point(viewport_point);
    Edges<int> viewport_widget_margin = _M_viewport->widget_margin();
    _M_viewport_widget_bounds = inner_bounds;
    _M_viewport_widget_bounds.x += viewport_widget_margin.left;
    _M_viewport_widget_bounds.y += viewport_widget_margin.top;
    _M_viewport_widget_bounds.width -= _M_v_scroll_bar_margin_box_size.width;
    _M_viewport_widget_bounds.height -= _M_h_scroll_bar_margin_box_size.height;
    _M_viewport_widget_bounds.width -= viewport_widget_margin.left + viewport_widget_margin.right;
    _M_viewport_widget_bounds.height -= viewport_widget_margin.top + viewport_widget_margin.bottom;
    _M_viewport_widget_bounds.width = max(_M_viewport_widget_bounds.width, 0);
    _M_viewport_widget_bounds.height = max(_M_viewport_widget_bounds.height, 0);
    _M_viewport->update_widget_point(_M_viewport_widget_bounds);
  }

  void Scroll::update_child_sizes(Canvas *canvas, const Dimension<int> &area_size)
  {
    Dimension<int> inner_area_size = area_size_to_inner_area_size(area_size);
    Dimension<int> widget_area_size = inner_area_size;
    widget_area_size.width -= _M_v_scroll_bar_margin_box_size.width;
    widget_area_size.height -= _M_h_scroll_bar_margin_box_size.height;
    widget_area_size.width = max(widget_area_size.width, 0);
    widget_area_size.height = max(widget_area_size.height, 0);
    _M_viewport->update_widget_size(canvas, widget_area_size);
  }

  void Scroll::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  {
    _M_viewport->update_point(canvas);
    _M_viewport->update_client_size(canvas);
    Dimension<int> inner_area_size = area_size_to_inner_area_size(area_size);
    _M_has_visible_h_scroll_bar = (_M_has_h_scroll_bar || _M_has_auto_h_scroll_bar);
    _M_has_visible_v_scroll_bar = (_M_has_v_scroll_bar || _M_has_auto_v_scroll_bar);
    if(inner_area_size.width == numeric_limits<int>::max()) {
      Edges<int> viewport_widget_margin = _M_viewport->widget_margin();
      inner_area_size.width = _M_viewport->max_width();
      inner_area_size.width +=  viewport_widget_margin.left + viewport_widget_margin.right;
      if(_M_has_visible_v_scroll_bar) {
        int content_width;
        content_width = _M_top_button_margin_box_size.width;
        content_width = max(content_width, _M_v_slider_margin_box_size.width);
        content_width = max(content_width, _M_bottom_button_margin_box_size.width);
        inner_area_size.width += block_margin_box_width(v_scroll_bar_name(), _M_v_scroll_bar_pseudo_classes, content_width);
      }
    }
    if(inner_area_size.height == numeric_limits<int>::max()) {
      Edges<int> viewport_widget_margin = _M_viewport->widget_margin();
      inner_area_size.height = _M_viewport->max_height();
      inner_area_size.height +=  viewport_widget_margin.top + viewport_widget_margin.bottom;
      if(_M_has_visible_h_scroll_bar) {
        int content_height;
        content_height = _M_left_button_margin_box_size.width;
        content_height = max(content_height, _M_h_slider_margin_box_size.width);
        content_height = max(content_height, _M_right_button_margin_box_size.width);
        inner_area_size.width += block_margin_box_width(h_scroll_bar_name(), _M_h_scroll_bar_pseudo_classes, content_height);
      }
    }
    update_scroll_bar_margin_box_sizes(canvas, inner_area_size);
    Edges<int> viewport_margin = _M_viewport->margin();
    Dimension<int> viewport_size = inner_area_size;
    viewport_size.width -= _M_viewport->point().x;
    viewport_size.height -= _M_viewport->point().y;
    viewport_size.width -= viewport_margin.left + viewport_margin.right;
    viewport_size.height -= viewport_margin.top + viewport_margin.bottom;
    viewport_size.width -= _M_v_scroll_bar_margin_box_size.width;
    viewport_size.height -= _M_h_scroll_bar_margin_box_size.height;
    viewport_size.width = max(viewport_size.width, 0);
    viewport_size.height = max(viewport_size.height, 0);
    _M_viewport->set_size(viewport_size);
    if(_M_has_auto_h_scroll_bar) {
      _M_has_visible_h_scroll_bar = _M_viewport->width_is_less_than_clien_width();
    }
    if(_M_has_auto_v_scroll_bar) {
      _M_has_visible_v_scroll_bar = _M_viewport->height_is_less_than_clien_height();
    }
    update_scroll_bar_margin_box_sizes(canvas, inner_area_size);
    viewport_size = inner_area_size;
    viewport_size.width -= _M_viewport->point().x;
    viewport_size.height -= _M_viewport->point().y;
    viewport_size.width -= viewport_margin.left + viewport_margin.right;
    viewport_size.height -= viewport_margin.top + viewport_margin.bottom;
    viewport_size.width -= _M_v_scroll_bar_margin_box_size.width;
    viewport_size.height -= _M_h_scroll_bar_margin_box_size.height;
    viewport_size.width = max(viewport_size.width, 0);
    viewport_size.height = max(viewport_size.height, 0);
    _M_viewport->set_size(viewport_size);
    _M_has_enabled_h_scroll_bar = _M_viewport->width_is_less_than_clien_width();
    _M_has_enabled_v_scroll_bar = _M_viewport->height_is_less_than_clien_height();
    if(_M_has_enabled_h_scroll_bar) {
      _M_h_scroll_bar_pseudo_classes &= ~PseudoClasses::DISABLED;
      _M_left_button_pseudo_classes &= ~PseudoClasses::DISABLED;
      _M_h_slider_pseudo_classes &= ~PseudoClasses::DISABLED;
      _M_right_button_pseudo_classes &= ~PseudoClasses::DISABLED;
    } else {
      _M_h_scroll_bar_pseudo_classes |= PseudoClasses::DISABLED;
      _M_left_button_pseudo_classes |= PseudoClasses::DISABLED;
      _M_h_slider_pseudo_classes |= PseudoClasses::DISABLED;
      _M_right_button_pseudo_classes |= PseudoClasses::DISABLED;
    }
    if(_M_has_enabled_v_scroll_bar) {
      _M_v_scroll_bar_pseudo_classes &= ~PseudoClasses::DISABLED;
      _M_top_button_pseudo_classes &= ~PseudoClasses::DISABLED;
      _M_v_slider_pseudo_classes &= ~PseudoClasses::DISABLED;
      _M_bottom_button_pseudo_classes &= ~PseudoClasses::DISABLED;
    } else {
      _M_h_scroll_bar_pseudo_classes |= PseudoClasses::DISABLED;
      _M_top_button_pseudo_classes |= PseudoClasses::DISABLED;
      _M_v_slider_pseudo_classes |= PseudoClasses::DISABLED;
      _M_bottom_button_pseudo_classes |= PseudoClasses::DISABLED;
    }
  }

  void Scroll::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  {
    Edges<int> viewport_margin = _M_viewport->margin();
    Point<int> h_scroll_bar_margin_box_point = inner_bounds.point();
    h_scroll_bar_margin_box_point.x += _M_viewport->point().x - viewport_margin.left;
    h_scroll_bar_margin_box_point.y += inner_bounds.height - _M_h_scroll_bar_margin_box_size.height;
    draw_h_scroll_bar(canvas, h_scroll_bar_margin_box_point);
    Point<int> v_scroll_bar_margin_box_point = inner_bounds.point();
    v_scroll_bar_margin_box_point.x += inner_bounds.width - _M_v_scroll_bar_margin_box_size.width;
    v_scroll_bar_margin_box_point.y += _M_viewport->point().y - viewport_margin.top;
    draw_v_scroll_bar(canvas, v_scroll_bar_margin_box_point);
  }

  void Scroll::draw_children(Canvas *canvas, const Rectangle<int> &inner_bounds)
  {
    canvas->save();
    canvas->rect(_M_viewport_widget_bounds.x, _M_viewport_widget_bounds.y, _M_viewport_widget_bounds.width, _M_viewport_widget_bounds.height);
    canvas->clip();
    canvas->new_path();
    _M_widget->draw(canvas);
    canvas->restore();
  }

  Rectangle<int> Scroll::child_event_bounds() const
  { return _M_viewport_widget_bounds; }
  
  bool Scroll::invoke_fun_for_event(const Point<double> &point, const function<bool (Widget *, const Point<double> &)> &fun)
  {
    Point<int> int_point(round(point.x), round(point.x));
    bool cant_invoke = false;
    Rectangle<int> result;
    if(child_event_bounds().intersect(_M_widget->bounds(), result) && result.contain(int_point))
      cant_invoke = _M_widget->invoke_fun_for_event(point, fun);
    return !cant_invoke ? fun(this, point) : true;
  }

  bool Scroll::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  {
    Point<int> int_point(round(point.x), round(point.y));
    bool is_h_slider_pointer = (_M_has_h_slider_pointer && _M_h_slider_pointer == pointer);
    bool is_v_slider_pointer = (_M_has_v_slider_pointer && _M_v_slider_pointer == pointer);
    if(_M_has_visible_h_scroll_bar && _M_has_enabled_h_scroll_bar && (is_h_slider_pointer || is_v_slider_pointer)) {
      if(_M_left_button_bounds.contain(int_point)) {
        bool had_pointer = block_has_pointer(LEFT_BUTTON, pointer);
        on_touch_for_block(LEFT_BUTTON, pointer, point, state);
        switch(state) {
          case TouchState::DOWN:
            if(!had_pointer) {
              if(_M_left_button_touch_count == 0)
                _M_left_button_pseudo_classes |= PseudoClasses::ACTIVE;
              _M_left_button_touch_count++;
              _M_viewport->h_move_view(-1);
            }
            break;
          case TouchState::REPEATED:
            if(had_pointer) {
              if(_M_left_button_touch_count > 0) _M_viewport->h_move_view(-1);
            }
            break;
          case TouchState::UP:
            if(had_pointer) {
              if(_M_left_button_touch_count > 0) {
                _M_left_button_touch_count--;
                if(_M_left_button_touch_count == 0) {
                  _M_left_button_pseudo_classes &= ~PseudoClasses::ACTIVE;
                  _M_viewport->h_move_view(-1);
                }
              }
            }
            break;
          default:
            break;
        }
        return true;
      } else if(_M_h_slider_bounds.contain(int_point)) {
        on_touch_for_block(H_SLIDER, pointer, point, state);
        if(state == TouchState::DOWN && !_M_has_h_slider_pointer) {
          lock_touch_leaving(pointer);
          _M_h_slider_pseudo_classes |= PseudoClasses::ACTIVE;
          _M_has_h_slider_pointer = true;
          _M_h_slider_pointer = pointer;
          _M_old_h_slider_pointer_point = int_point;
          return true;
        }
      } else if(_M_right_button_bounds.contain(int_point)) {
        bool had_pointer = block_has_pointer(RIGHT_BUTTON, pointer);
        on_touch_for_block(RIGHT_BUTTON, pointer, point, state);
        switch(state) {
          case TouchState::DOWN:
            if(!had_pointer) {
              if(_M_right_button_touch_count == 0)
                _M_right_button_pseudo_classes |= PseudoClasses::ACTIVE;
              _M_right_button_touch_count++;
              _M_viewport->h_move_view(1);
            }
          case TouchState::REPEATED:
            if(had_pointer) {
              if(_M_right_button_touch_count > 0) _M_viewport->h_move_view(1);
            }
            break;
          case TouchState::UP:
            if(had_pointer) {
              if(_M_right_button_touch_count > 0) {
                _M_right_button_touch_count--;
                if(_M_right_button_touch_count == 0) {
                  _M_right_button_pseudo_classes &= ~PseudoClasses::ACTIVE;
                  _M_viewport->h_move_view(1);
                }
              }
            }
            break;
          default:
            break;
        }
        return true;
      } else if(_M_h_scroll_bar_bounds.contain(int_point)) {
        on_touch_for_block(H_SCROLL_BAR, pointer, point, state);
        if(_M_h_trough_bounds.contain(int_point)) {
          int width = max(_M_h_trough_bounds.width - _M_min_h_slider_margin_box_width, 0);
          int x = min(max(int_point.x - _M_center_h_trough_x, 0), width);
          _M_viewport->set_h_scroll_slider_x(x, width);
        }
        return true;
      }
    }
    if(_M_has_visible_v_scroll_bar && _M_has_enabled_v_scroll_bar && (is_h_slider_pointer || is_v_slider_pointer)) {
      if(_M_top_button_bounds.contain(int_point)) {
        bool had_pointer = block_has_pointer(TOP_BUTTON, pointer);
        on_touch_for_block(TOP_BUTTON, pointer, point, state);
        switch(state) {
          case TouchState::DOWN:
            if(!had_pointer) {
              if(_M_top_button_touch_count == 0)
                _M_top_button_pseudo_classes |= PseudoClasses::ACTIVE;
              _M_top_button_touch_count++;
              _M_viewport->v_move_view(1);
            }
            break;
          case TouchState::REPEATED:
            if(had_pointer) {
              if(_M_right_button_touch_count > 0) _M_viewport->v_move_view(-1);
            }
            break;
          case TouchState::UP:
            if(had_pointer) {
              if(_M_top_button_touch_count > 0) {
                _M_top_button_touch_count--;
                if(_M_top_button_touch_count == 0) {
                  _M_top_button_pseudo_classes &= ~PseudoClasses::ACTIVE;
                  _M_viewport->v_move_view(-1);
                }
              }
            }
            break;
          default:
            break;
        }
        return true;
      } else if(_M_v_slider_bounds.contain(int_point)) {
        on_touch_for_block(V_SLIDER, pointer, point, state);
        if(state == TouchState::DOWN && !_M_has_v_slider_pointer) {
          lock_touch_leaving(pointer);
          _M_v_slider_pseudo_classes |= PseudoClasses::ACTIVE;
          _M_has_v_slider_pointer = true;
          _M_v_slider_pointer = pointer;
          _M_old_v_slider_pointer_point = int_point;
          return true;
        }
      } else if(_M_bottom_button_bounds.contain(int_point)) {
        bool had_pointer = block_has_pointer(BOTTOM_BUTTON, pointer);
        on_touch_for_block(BOTTOM_BUTTON, pointer, point, state);
        switch(state) {
          case TouchState::DOWN:
            if(!had_pointer) {
              if(_M_bottom_button_touch_count == 0)
                _M_bottom_button_pseudo_classes |= PseudoClasses::ACTIVE;
              _M_bottom_button_touch_count++;
              _M_viewport->v_move_view(1);
            }
          case TouchState::REPEATED:
            if(had_pointer) {
              if(_M_right_button_touch_count > 0) _M_viewport->v_move_view(1);
            }
            break;
          case TouchState::UP:
            if(had_pointer) {
              if(_M_bottom_button_touch_count > 0) {
                _M_bottom_button_touch_count--;
                if(_M_bottom_button_touch_count == 0) {
                  _M_bottom_button_pseudo_classes &= ~PseudoClasses::ACTIVE;
                  _M_viewport->v_move_view(1);
                }
              }
            }
            break;
          default:
            break;
        }
        return true;
      } else if(_M_v_scroll_bar_bounds.contain(int_point)) {
        on_touch_for_block(V_SCROLL_BAR, pointer, point, state);
        if(_M_v_trough_bounds.contain(int_point)) {
          int height = max(_M_v_trough_bounds.width - _M_min_v_slider_margin_box_height, 0);
          int y = min(max(int_point.y - _M_center_v_trough_y, 0), height);
          _M_viewport->set_v_scroll_slider_y(y, height);
        }
        return true;
      }
    }
    if(_M_has_visible_h_scroll_bar && _M_has_enabled_h_scroll_bar && is_h_slider_pointer) {
      on_touch_for_block(H_SLIDER, pointer, point, state);
      if(state == TouchState::MOTION || state == TouchState::UP) {
        if(state == TouchState::UP) {
          _M_has_h_slider_pointer = false;
          _M_h_slider_pseudo_classes &= ~PseudoClasses::ACTIVE;
          unlock_touch_leaving(pointer);
        }
        if(_M_old_h_slider_pointer_point.x != point.x) {
          int width = max(_M_h_trough_bounds.width - _M_min_h_slider_margin_box_width, 0);
          int x_diff = point.x - _M_old_h_slider_pointer_point.x;
          int x = max(_M_viewport->h_scroll_slider_x(width) + x_diff, 0);
          _M_viewport->set_h_scroll_slider_x(x, width);
        }
      }
      return true;
    }
    if(_M_has_visible_v_scroll_bar && _M_has_enabled_v_scroll_bar && is_v_slider_pointer) {
      on_touch_for_block(V_SLIDER, pointer, point, state);
      if(state == TouchState::MOTION || state == TouchState::UP) {
        if(state == TouchState::UP) {
          _M_has_v_slider_pointer = false;
          _M_v_slider_pseudo_classes &= ~PseudoClasses::ACTIVE;
          unlock_touch_leaving(pointer);
        }
        if(_M_old_v_slider_pointer_point.x != point.x) {
          int height = max(_M_v_trough_bounds.height - _M_min_v_slider_margin_box_height, 0);
          int y_diff = point.y - _M_old_v_slider_pointer_point.y;
          int y = max(_M_viewport->v_scroll_slider_y(height) + y_diff, 0);
          _M_viewport->set_v_scroll_slider_y(y, height);
        }
      }
      return true;
    }
    delete_pointer_from_any_block(pointer);
    this->Widget::on_touch(pointer, point, state);
    if(_M_has_visible_h_scroll_bar) {
      _M_left_button_pseudo_classes &= ~PseudoClasses::ACTIVE;
      _M_h_slider_pseudo_classes &= ~PseudoClasses::ACTIVE;
      _M_bottom_button_pseudo_classes &= ~PseudoClasses::ACTIVE;
    }
    if(_M_has_visible_v_scroll_bar) {
      _M_top_button_pseudo_classes &= ~PseudoClasses::ACTIVE;
      _M_v_slider_pseudo_classes &= ~PseudoClasses::ACTIVE;
      _M_bottom_button_pseudo_classes &= ~PseudoClasses::ACTIVE;
    }
    return false;
  }

  void Scroll::on_touch_leave(const Pointer &pointer)
  {
    this->Widget::on_touch_leave(pointer);
    if(_M_has_visible_h_scroll_bar) {
      _M_left_button_pseudo_classes &= ~PseudoClasses::ACTIVE;
      _M_h_slider_pseudo_classes |= PseudoClasses::ACTIVE;
      _M_bottom_button_pseudo_classes &= ~PseudoClasses::ACTIVE;
    }
    if(_M_has_visible_v_scroll_bar) {
      _M_top_button_pseudo_classes &= ~PseudoClasses::ACTIVE;
      _M_v_slider_pseudo_classes &= ~PseudoClasses::ACTIVE;
      _M_bottom_button_pseudo_classes &= ~PseudoClasses::ACTIVE;
    }
  }

  bool Scroll::on_pointer_axis(Axis axis, double value)
  {
    int x_step = _M_viewport->size().width / 4;
    if(x_step <= 0) x_step = 1;
    int y_step = _M_viewport->size().height / 4;
    if(y_step <= 0) y_step = 1;
    switch(axis) {
      case Axis::HORIZONTAL_SCROLL:
        if(value < 0.0)
          _M_viewport->h_move_view(-x_step);
        else if(value > 0.0)
          _M_viewport->h_move_view(x_step);
        break;
      case Axis::VERTICAL_SCROLL:
        if(value < 0.0)
          _M_viewport->v_move_view(-y_step);
        else if(value > 0.0)
          _M_viewport->v_move_view(y_step);
        break;
    }
    return true;
  }

  bool Scroll::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  {
    this->Widget::on_key(key_sym, modifiers, utf8, state); 
    int x_step = _M_viewport->size().width / 8;
    if(x_step <= 0) x_step = 1;
    int y_step = _M_viewport->size().height / 8;
    if(y_step <= 0) y_step = 1;
    switch(key_sym) {
      case XKB_KEY_Home:
      case XKB_KEY_KP_Home:
        _M_viewport->move_view_to_top(); 
        break;
      case XKB_KEY_Left:
      case XKB_KEY_KP_Left:
        _M_viewport->h_move_view(-x_step);
        return true;
      case XKB_KEY_Up:
      case XKB_KEY_KP_Up:
        _M_viewport->v_move_view(-y_step);
        return true;
      case XKB_KEY_Right:
      case XKB_KEY_KP_Right:
        _M_viewport->h_move_view(x_step);
        return true;
      case XKB_KEY_Down:
      case XKB_KEY_KP_Down:
        _M_viewport->v_move_view(_M_viewport->size().width);
        break;
      case XKB_KEY_Page_Up:
      case XKB_KEY_KP_Page_Up:
        _M_viewport->v_move_view(-_M_viewport->size().height);
        return true;
      case XKB_KEY_Page_Down:
      case XKB_KEY_KP_Page_Down:
        _M_viewport->v_move_view(_M_viewport->size().height);
        return true;
      case XKB_KEY_End:
      case XKB_KEY_KP_End:
        _M_viewport->move_view_to_bottom(); 
        break;
    }
    return false;
  }

  void Scroll::update_scroll_bar_margin_box_sizes(Canvas *canvas, const Dimension<int> &inner_area_size)
  {
    Edges<int> viewport_margin = _M_viewport->margin();
    int h_scroll_bar_inner_width;
    int v_scroll_bar_inner_height;
    if(_M_has_visible_h_scroll_bar) {
      update_left_button_margin_box_size(canvas);
      update_h_slider_margin_box_size(canvas);
      update_right_button_margin_box_size(canvas);
      Dimension<int> content_size;
      content_size.height = _M_left_button_margin_box_size.height;
      content_size.height = max(content_size.height, _M_h_slider_margin_box_size.height);
      content_size.height = max(content_size.height, _M_right_button_margin_box_size.height);
      _M_h_scroll_bar_margin_box_size.width = inner_area_size.width - _M_viewport->point().x + viewport_margin.left;
      _M_h_scroll_bar_margin_box_size.width = max(_M_h_scroll_bar_margin_box_size.width, 0);
      get_block_margin_box_height_and_block_inner_width(h_scroll_bar_name(), _M_h_scroll_bar_pseudo_classes, content_size.height, _M_h_scroll_bar_margin_box_size.width, _M_h_scroll_bar_margin_box_size.height, h_scroll_bar_inner_width);
    } else
      _M_h_scroll_bar_margin_box_size = Dimension<int>(0, 0);
    if(_M_has_visible_v_scroll_bar) {
      update_top_button_margin_box_size(canvas);
      update_v_slider_margin_box_size(canvas);
      update_bottom_button_margin_box_size(canvas);
      Dimension<int> content_size;
      content_size.width = _M_top_button_margin_box_size.width;
      content_size.width = max(content_size.width, _M_v_slider_margin_box_size.width);
      content_size.width = max(content_size.width, _M_bottom_button_margin_box_size.width);
      _M_v_scroll_bar_margin_box_size.height = inner_area_size.height - _M_viewport->point().y + viewport_margin.top;
      _M_v_scroll_bar_margin_box_size.height = max(_M_v_scroll_bar_margin_box_size.height, 0);
      get_block_margin_box_width_and_block_inner_height(v_scroll_bar_name(), _M_v_scroll_bar_pseudo_classes, content_size.width, _M_v_scroll_bar_margin_box_size.height, _M_v_scroll_bar_margin_box_size.width, v_scroll_bar_inner_height);
    } else
      _M_v_scroll_bar_margin_box_size = Dimension<int>(0, 0);
    if(_M_has_visible_h_scroll_bar) {
      if(_M_h_scroll_bar_margin_box_size.width != numeric_limits<int>::max()) {
        _M_h_scroll_bar_margin_box_size.width -= _M_v_scroll_bar_margin_box_size.width;
        _M_h_scroll_bar_margin_box_size.width = max(_M_h_scroll_bar_margin_box_size.width, 0);
      }
    }
    if(_M_has_visible_v_scroll_bar) {
      if(_M_v_scroll_bar_margin_box_size.height != numeric_limits<int>::max()) {
        _M_v_scroll_bar_margin_box_size.height -= _M_h_scroll_bar_margin_box_size.height;
        _M_v_scroll_bar_margin_box_size.height = max(_M_h_scroll_bar_margin_box_size.height, 0);
      }
    }
    if(_M_has_visible_h_scroll_bar)
      update_h_slider_margin_box_width(canvas, h_scroll_bar_inner_width);
    if(_M_has_visible_v_scroll_bar)
      update_v_slider_margin_box_height(canvas, v_scroll_bar_inner_height);
  }

  const char *Scroll::h_scroll_bar_name() const
  { return "scroll.h_scroll_bar"; }

  void Scroll::draw_h_scroll_bar(Canvas *canvas, const Point<int> &margin_box_point)
  {
    if(_M_has_visible_h_scroll_bar) {
      Styles *h_slider_styles;
      Rectangle<int> inner_bounds;
      draw_block(h_scroll_bar_name(), _M_h_scroll_bar_pseudo_classes, canvas, margin_box_point, _M_h_scroll_bar_margin_box_size, inner_bounds, _M_h_scroll_bar_bounds);
      Point<int> left_button_margin_box_point = inner_bounds.point();
      left_button_margin_box_point.y += (inner_bounds.height - _M_left_button_margin_box_size.height) / 2;
      draw_left_button(canvas, left_button_margin_box_point);
      int width = inner_bounds.width;
      width -= _M_left_button_margin_box_size.width;
      width -= _M_right_button_margin_box_size.width;
      width -= _M_min_h_slider_margin_box_width;
      width = max(width, 0);
      Point<int> h_slider_margin_box_point = inner_bounds.point();
      h_slider_margin_box_point.x += _M_left_button_margin_box_size.width;
      h_slider_margin_box_point.x += _M_viewport->h_scroll_slider_x(width);
      h_slider_margin_box_point.y += (inner_bounds.height - _M_h_slider_margin_box_size.height) / 2;
      draw_h_slider(canvas, h_slider_margin_box_point, h_slider_styles);
      Point<int> right_button_margin_box_point = inner_bounds.point();
      right_button_margin_box_point.x += (inner_bounds.width - _M_right_button_margin_box_size.width);
      right_button_margin_box_point.y += (inner_bounds.height - _M_right_button_margin_box_size.height) / 2;
      draw_right_button(canvas, right_button_margin_box_point);
      _M_h_trough_bounds = inner_bounds;
      _M_h_trough_bounds.x += _M_left_button_margin_box_size.width;
      _M_h_trough_bounds.width -= _M_right_button_margin_box_size.width;
      _M_h_trough_bounds.width = max(_M_h_trough_bounds.width, 0);
      Edges<int> h_slider_margin = h_slider_styles->margin(_M_h_slider_pseudo_classes);
      _M_center_h_trough_x = _M_h_trough_bounds.x;
      _M_center_h_trough_x += h_slider_margin.left;
      _M_center_h_trough_x += _M_h_slider_bounds.width / 2;
    }
  }

  const char *Scroll::left_button_name() const
  { return "scroll.h_scroll_bar.left_button"; }

  void Scroll::update_left_button_margin_box_size(Canvas *canvas)
  { _M_left_button_margin_box_size = block_margin_box_size(left_button_name(), _M_left_button_pseudo_classes, Dimension<int>(0, 0)); }

  void Scroll::draw_left_button(Canvas *canvas, const Point<int> &margin_box_point)
  {
    Rectangle<int> inner_bounds;
    draw_block(left_button_name(), _M_left_button_pseudo_classes, canvas, margin_box_point, _M_left_button_margin_box_size, inner_bounds, _M_left_button_bounds);
  }

  const char *Scroll::h_slider_name() const
  { return "scroll.h_scroll_bar.slider"; }

  void Scroll::update_h_slider_margin_box_size(Canvas *canvas)
  { _M_h_slider_margin_box_size = block_margin_box_size(h_slider_name(), _M_h_scroll_bar_pseudo_classes, Dimension<int>(0, 0)); }

  void Scroll::update_h_slider_margin_box_width(Canvas *canvas, int inner_width)
  {
    _M_min_h_slider_margin_box_width = _M_h_slider_margin_box_size.width;
    int width = inner_width;
    width -= _M_left_button_margin_box_size.width;
    width -= _M_right_button_margin_box_size.width;
    width -= _M_min_h_slider_margin_box_width;
    width = max(width, 1);
    _M_h_slider_margin_box_size.width += _M_viewport->h_scroll_slider_width(width);
  }

  void Scroll::draw_h_slider(Canvas *canvas, const Point<int> &margin_box_point, Styles *&styles)
  {
    Rectangle<int> inner_bounds;
    draw_block(h_slider_name(), _M_h_slider_pseudo_classes, canvas, margin_box_point, _M_h_slider_margin_box_size, inner_bounds, _M_h_slider_bounds, styles);
  }

  const char *Scroll::right_button_name() const
  { return "scroll.h_scroll_bar.right_button"; }

  void Scroll::update_right_button_margin_box_size(Canvas *canvas)
  { _M_right_button_margin_box_size = block_margin_box_size(right_button_name(), _M_right_button_pseudo_classes, Dimension<int>(0, 0)); }

  void Scroll::draw_right_button(Canvas *canvas, const Point<int> &margin_box_point)
  {
    Rectangle<int> inner_bounds;
    draw_block(right_button_name(), _M_right_button_pseudo_classes, canvas, margin_box_point, _M_right_button_margin_box_size, inner_bounds, _M_right_button_bounds);
  }

  const char *Scroll::v_scroll_bar_name() const
  { return "scroll.v_scroll_bar"; }

  void Scroll::draw_v_scroll_bar(Canvas *canvas, const Point<int> &margin_box_point)
  {
    if(_M_has_visible_h_scroll_bar) {
      Styles *v_slider_styles;
      Rectangle<int> inner_bounds;
      draw_block(v_scroll_bar_name(), _M_v_scroll_bar_pseudo_classes, canvas, margin_box_point, _M_v_scroll_bar_margin_box_size, inner_bounds, _M_h_scroll_bar_bounds);
      Point<int> top_button_margin_box_point = inner_bounds.point();
      top_button_margin_box_point.x += (inner_bounds.width - _M_top_button_margin_box_size.width) / 2;
      draw_top_button(canvas, top_button_margin_box_point);
      int height = inner_bounds.height;
      height -= _M_top_button_margin_box_size.height;
      height -= _M_bottom_button_margin_box_size.height;
      height -= _M_min_v_slider_margin_box_height;
      height = max(height, 0);
      Point<int> v_slider_margin_box_point = inner_bounds.point();
      v_slider_margin_box_point.x += (inner_bounds.width - _M_v_slider_margin_box_size.width) / 2;
      v_slider_margin_box_point.y += _M_top_button_margin_box_size.height;
      v_slider_margin_box_point.y += _M_viewport->v_scroll_slider_y(height);
      draw_v_slider(canvas, v_slider_margin_box_point, v_slider_styles);
      Point<int> bottom_button_margin_box_point = inner_bounds.point();
      bottom_button_margin_box_point.x += (inner_bounds.width - _M_bottom_button_margin_box_size.width) / 2;
      bottom_button_margin_box_point.x += (inner_bounds.width - _M_bottom_button_margin_box_size.width);
      draw_bottom_button(canvas, bottom_button_margin_box_point);
      _M_v_trough_bounds = inner_bounds;
      _M_v_trough_bounds.y += _M_top_button_margin_box_size.height;
      _M_v_trough_bounds.height -= _M_bottom_button_margin_box_size.height;
      _M_v_trough_bounds.height = max(_M_v_trough_bounds.height, 0);
      Edges<int> v_slider_margin = v_slider_styles->margin(_M_v_slider_pseudo_classes);
      _M_center_v_trough_y = _M_h_trough_bounds.x;
      _M_center_v_trough_y += v_slider_margin.top;
      _M_center_v_trough_y += _M_h_slider_bounds.height / 2;
    }
  }

  const char *Scroll::top_button_name() const
  { return "scroll.v_scroll_bar.top_button"; }

  void Scroll::update_top_button_margin_box_size(Canvas *canvas)
  { _M_top_button_margin_box_size = block_margin_box_size(top_button_name(), _M_top_button_pseudo_classes, Dimension<int>(0, 0)); }

  void Scroll::draw_top_button(Canvas *canvas, const Point<int> &margin_box_point)
  {
    Rectangle<int> inner_bounds;
    draw_block(top_button_name(), _M_top_button_pseudo_classes, canvas, margin_box_point, _M_top_button_margin_box_size, inner_bounds, _M_top_button_bounds);
  }

  const char *Scroll::v_slider_name() const
  { return "scroll.v_scroll_bar.slider"; }

  void Scroll::update_v_slider_margin_box_size(Canvas *canvas)
  { _M_h_slider_margin_box_size = block_margin_box_size(h_slider_name(), _M_h_scroll_bar_pseudo_classes, Dimension<int>(0, 0)); }

  void Scroll::update_v_slider_margin_box_height(Canvas *canvas, int inner_height)
  {
    _M_min_v_slider_margin_box_height = _M_h_slider_margin_box_size.height;
    int height = inner_height;
    height -= _M_top_button_margin_box_size.height;
    height -= _M_bottom_button_margin_box_size.height;
    height -= _M_min_v_slider_margin_box_height;
    height = max(height, 1);
    _M_v_slider_margin_box_size.height += _M_viewport->v_scroll_slider_height(height);
  }

  void Scroll::draw_v_slider(Canvas *canvas, const Point<int> &margin_box_point, Styles *&styles)
  {
    Rectangle<int> inner_bounds;
    draw_block(v_slider_name(), _M_v_slider_pseudo_classes, canvas, margin_box_point, _M_v_slider_margin_box_size, inner_bounds, _M_v_slider_bounds, styles);
  }

  const char *Scroll::bottom_button_name() const
  { return "scroll.v_scroll_bar.bottom_button"; }

  void Scroll::update_bottom_button_margin_box_size(Canvas *canvas)
  { _M_bottom_button_margin_box_size = block_margin_box_size(bottom_button_name(), _M_bottom_button_pseudo_classes, Dimension<int>(0, 0)); }

  void Scroll::draw_bottom_button(Canvas *canvas, const Point<int> &margin_box_point)
  {
    Rectangle<int> inner_bounds;
    draw_block(bottom_button_name(), _M_bottom_button_pseudo_classes, canvas, margin_box_point, _M_bottom_button_margin_box_size, inner_bounds, _M_bottom_button_bounds);
  }

  int Scroll::block_margin_box_width(const char *name, PseudoClasses pseudo_classes, int content_width)
  {
    PseudoClasses tmp_pseudo_classes = this->pseudo_classes();
    tmp_pseudo_classes &= ~(PseudoClasses::ACTIVE | PseudoClasses::CHECKED | PseudoClasses::FOCUS | PseudoClasses::HOVER | PseudoClasses::SELECTED);
    tmp_pseudo_classes |= pseudo_classes;
    Styles *styles = find_styles(name);
    Edges<int> margin = styles->margin(tmp_pseudo_classes);
    Edges<int> border = styles->border(tmp_pseudo_classes);
    Edges<int> padding = styles->padding(tmp_pseudo_classes);
    return content_width + margin.left + margin.right + border.left + border.right + padding.left + padding.right;
  }

  void Scroll::get_block_margin_box_width_and_block_inner_height(const char *name, PseudoClasses pseudo_classes, int content_width, int margin_box_height, int &margin_box_width, int &inner_height)
  {
    PseudoClasses tmp_pseudo_classes = this->pseudo_classes();
    tmp_pseudo_classes &= ~(PseudoClasses::ACTIVE | PseudoClasses::CHECKED | PseudoClasses::FOCUS | PseudoClasses::HOVER | PseudoClasses::SELECTED);
    tmp_pseudo_classes |= pseudo_classes;
    Styles *styles = find_styles(name);
    Edges<int> margin = styles->margin(tmp_pseudo_classes);
    Edges<int> border = styles->border(tmp_pseudo_classes);
    Edges<int> padding = styles->padding(tmp_pseudo_classes);
    margin_box_width = content_width;
    margin_box_width += margin.left + margin.right + border.left + border.right + padding.left + padding.right;
    inner_height = margin_box_height;
    inner_height -= margin.top + margin.bottom + border.top + border.bottom + padding.top + padding.bottom;
    inner_height = max(inner_height, 0);
  }

  int Scroll::block_margin_box_height(const char *name, PseudoClasses pseudo_classes, int content_height)
  {
    PseudoClasses tmp_pseudo_classes = this->pseudo_classes();
    tmp_pseudo_classes &= ~(PseudoClasses::ACTIVE | PseudoClasses::CHECKED | PseudoClasses::FOCUS | PseudoClasses::HOVER | PseudoClasses::SELECTED);
    tmp_pseudo_classes |= pseudo_classes;
    Styles *styles = find_styles(name);
    Edges<int> margin = styles->margin(tmp_pseudo_classes);
    Edges<int> border = styles->border(tmp_pseudo_classes);
    Edges<int> padding = styles->padding(tmp_pseudo_classes);
    return content_height + margin.top + margin.bottom + border.top + border.bottom + padding.top + padding.bottom;
  }

  void Scroll::get_block_margin_box_height_and_block_inner_width(const char *name, PseudoClasses pseudo_classes, int content_height, int margin_box_width, int &margin_box_height, int &inner_width)
  {
    PseudoClasses tmp_pseudo_classes = this->pseudo_classes();
    tmp_pseudo_classes &= ~(PseudoClasses::ACTIVE | PseudoClasses::CHECKED | PseudoClasses::FOCUS | PseudoClasses::HOVER | PseudoClasses::SELECTED);
    tmp_pseudo_classes |= pseudo_classes;
    Styles *styles = find_styles(name);
    Edges<int> margin = styles->margin(tmp_pseudo_classes);
    Edges<int> border = styles->border(tmp_pseudo_classes);
    Edges<int> padding = styles->padding(tmp_pseudo_classes);
    margin_box_height = content_height;
    margin_box_height += margin.top + margin.bottom + border.top + border.bottom + padding.top + padding.bottom;
    inner_width = margin_box_width;
    inner_width -= margin.left + margin.right + border.left + border.right + padding.left + padding.right;
    inner_width = max(inner_width, 0);
  }
}
