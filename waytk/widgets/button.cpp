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
#include <cstring>
#include <waytk.hpp>

using namespace std;

namespace waytk
{
  Button::~Button() {}

  void Button::initialize(const Icon &icon, const string &label, const OnClickListener &listener, bool is_button_h_align)
  {
    if(is_button_h_align) set_h_align(HAlignment::CENTER);
    _M_icon = icon;
    normalize_utf8(label, _M_label);
    _M_on_click_callback.set_listener(listener);
    _M_touch_count = 0;
    _M_icon_margin_box_size = Dimension<int>(0, 0);
    _M_label_margin_box_size = Dimension<int>(0, 0);
  }

  void Button::set_label(const string &label)
  { normalize_utf8(label, _M_label); }

  const char *Button::name() const
  { return "button"; }

  void Button::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  {
    update_icon_margin_box_size(canvas);
    update_label_margin_box_size(canvas);
    Dimension<int> content_size;
    content_size.width = _M_icon_margin_box_size.width;
    content_size.height = _M_icon_margin_box_size.height;
    content_size.width +=_M_label_margin_box_size.width;
    content_size.height = max(content_size.height, _M_label_margin_box_size.height);
    set_content_size(content_size);
  }

  void Button::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  {
    Point<int> content_point = inner_bounds.point();
    content_point.x += (inner_bounds.width - content_size().width) / 2;
    content_point.y += (inner_bounds.height - content_size().height) / 2;
    int x_offset = 0;
    if(_M_icon != Icon()) {
      Point<int> icon_margin_box_point = content_point;
      icon_margin_box_point.y += (content_size().height - _M_icon_margin_box_size.height) / 2;
      draw_icon(canvas, icon_margin_box_point);
      x_offset = _M_icon_margin_box_size.width;
    }
    if(!_M_label.empty()) {
      Point<int> label_margin_box_point = content_point;
      label_margin_box_point.x += x_offset;
      label_margin_box_point.y += (content_size().height - _M_label_margin_box_size.height) / 2;
      draw_label(canvas, label_margin_box_point);
    }
  }

  bool Button::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  {
    bool had_pointer = has_pointer(pointer);
    this->Widget::on_touch(pointer, point, state);
    switch(state) {
      case TouchState::DOWN:
        if(!had_pointer) {
          if(_M_touch_count == 0)
            set_pseudo_classes(pseudo_classes() | PseudoClasses::ACTIVE);
          _M_touch_count++;
        }
        break;
      case TouchState::UP:
        if(had_pointer) {
          if(_M_touch_count > 0) {
            _M_touch_count--;
            if(_M_touch_count == 0) {
              set_pseudo_classes(pseudo_classes() & ~PseudoClasses::ACTIVE);
              on_click();
            }
          }
        }
        break;
      default:
        break;
    }
    return true;
  }

  void Button::on_touch_leave(const Pointer &pointer)
  {
    bool had_pointer = has_pointer(pointer);
    this->Widget::on_touch_leave(pointer);
    if(had_pointer) {
      if(_M_touch_count > 0) {
        _M_touch_count--;
        if(_M_touch_count == 0)
          set_pseudo_classes(pseudo_classes() & ~PseudoClasses::ACTIVE);
      }
    }
  }

  bool Button::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  {
    this->Widget::on_key(key_sym, modifiers, utf8, state);
    if((strcmp(utf8, "\r") == 0 || strcmp(utf8, " ") == 0)) {
      switch(state) {
        case KeyState::PRESSED:
          if(_M_touch_count == 0)
            set_pseudo_classes(pseudo_classes() | PseudoClasses::ACTIVE);
          break;
        case KeyState::REPEATED:
          on_click();
          break;
        case KeyState::RELEASED:
          if(_M_touch_count == 0) {
            set_pseudo_classes(pseudo_classes() & ~PseudoClasses::ACTIVE);
            on_click();
          }
          break;
      }
      return true;
    } else
      return false;
  }

  void Button::on_click()
  { _M_on_click_callback(this); }

  const char *Button::icon_name() const
  { return "button.icon"; }

  IconSize Button::icon_size() const
  { return IconSize::MEDIUM; }

  void Button::update_icon_margin_box_size(Canvas *canvas)
  {
    if(_M_icon != Icon()) {
      _M_icon_margin_box_size = block_margin_box_size(icon_name(), PseudoClasses::NONE, _M_icon.image(icon_size())->size());
    } else {
      _M_icon_margin_box_size.width = 0;
      _M_icon_margin_box_size.height = block_margin_box_size(icon_name(), PseudoClasses::NONE, icon_size_to_dimension(icon_size())).height;
    }
  }

  void Button::draw_icon(Canvas *canvas, const Point<int> &margin_box_point)
  {
    if(_M_icon != Icon()) {
      Rectangle<int> inner_bounds;
      draw_block(icon_name(), PseudoClasses::NONE, canvas, margin_box_point, _M_icon_margin_box_size, inner_bounds);
      canvas->save();
      canvas->rect(inner_bounds.x, inner_bounds.y, inner_bounds.width, inner_bounds.height);
      canvas->set_image(_M_icon.image(icon_size()).get(), inner_bounds.x, inner_bounds.y);
      canvas->restore();
    }
  }

  const char *Button::label_name() const
  { return "button.label"; }

  void Button::update_label_margin_box_size(Canvas *canvas)
  {
    if(!_M_label.empty()) {
      FontMetrics font_metrics;
      TextMetrics text_metrics;
      canvas->get_font_matrics(font_metrics);
      canvas->get_text_matrics(_M_label, text_metrics);
      Dimension<int> label_content_size(ceil(text_metrics.x_advance), ceil(font_metrics.height));
      _M_label_margin_box_size = block_margin_box_size(label_name(), PseudoClasses::NONE, label_content_size);
    } else
      _M_label_margin_box_size = Dimension<int>(0, 0);
  }

  void Button::draw_label(Canvas *canvas, const Point<int> &margin_box_point)
  {
    if(!_M_label.empty()) {
      Rectangle<int> inner_bounds;
      Styles *label_styles;
      draw_block(label_name(), PseudoClasses::NONE, canvas, margin_box_point, _M_label_margin_box_size, inner_bounds, label_styles);
      FontMetrics font_metrics;
      canvas->get_font_matrics(font_metrics);
      canvas->move_to(inner_bounds.x, inner_bounds.y + (inner_bounds.height - font_metrics.height) / 2 + font_metrics.ascent);
      canvas->set_color(label_styles->foreground_color(pseudo_classes()));
      canvas->show_text(_M_label);
    }
  }
}
