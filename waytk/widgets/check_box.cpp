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
  CheckBox::~CheckBox() {}

  void CheckBox::initialize(const Icon &icon, const string &label, bool is_checked)
  {
    this->Button::initialize(icon, label, [](Widget *widget) {});
    _M_is_checked = is_checked;
    _M_check_margin_box_size = Dimension<int>(0, 0);
  }

  void CheckBox::set_checked(bool is_checked)
  {
    if(_M_is_checked != is_checked) {
      _M_is_checked = is_checked;
      if(_M_is_checked)
        set_pseudo_classes(pseudo_classes() | PseudoClasses::CHECKED);
      else
        set_pseudo_classes(pseudo_classes() & ~PseudoClasses::CHECKED);
      on_check(_M_is_checked);
    }
  }

  const char *CheckBox::name() const
  { return "check_box"; }

  void CheckBox::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  {
    update_check_margin_box_size(canvas);
    if(can_draw_icon()) update_icon_margin_box_size(canvas);
    update_label_margin_box_size(canvas);
    Dimension<int> content_size;
    content_size.width = _M_check_margin_box_size.width;
    content_size.height = _M_check_margin_box_size.height;
    if(can_draw_icon()) {
      content_size.width += icon_margin_box_size().width;
      content_size.height = max(content_size.height, icon_margin_box_size().height);
    }
    content_size.width += label_margin_box_size().width;
    content_size.height = max(content_size.height, label_margin_box_size().height);
    set_content_size(content_size);
  }

  void CheckBox::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  {
    Point<int> content_point = inner_bounds.point();
    content_point.y += (inner_bounds.height - content_size().height) / 2;
    int x_offset = 0;
    Point<int> check_margin_box_point = content_point;
    check_margin_box_point.y += (content_size().height - icon_margin_box_size().height) / 2;
    draw_icon(canvas, check_margin_box_point);
    x_offset = _M_check_margin_box_size.width;
    if(can_draw_icon() && icon() != Icon()) {
      Point<int> icon_margin_box_point = content_point;
      icon_margin_box_point.x += x_offset;
      icon_margin_box_point.y += (content_size().height - icon_margin_box_size().height) / 2;
      draw_icon(canvas, icon_margin_box_point);
      x_offset = icon_margin_box_size().width;
    }
    if(!label().empty()) {
      Point<int> label_margin_box_point = content_point;
      label_margin_box_point.x += x_offset;
      label_margin_box_point.y += (content_size().height - label_margin_box_size().height) / 2;
      draw_label(canvas, label_margin_box_point);
    }
  }

  void CheckBox::on_click()
  {
    this->Button::on_click();
    set_checked(!_M_is_checked);
  }

  void CheckBox::on_check(bool is_checked)
  { _M_on_check_callback(this, is_checked); }

  bool CheckBox::can_draw_icon() const
  { return false; }

  const char *CheckBox::label_name() const
  { return "check_box.label"; }

  const char *CheckBox::check_name() const
  { return "check_box.check"; }

  void CheckBox::update_check_margin_box_size(Canvas *canvas)
  {
    PseudoClasses pseudo_classes = (_M_is_checked ? PseudoClasses::CHECKED : PseudoClasses::NONE);
    _M_check_margin_box_size = block_margin_box_size(check_name(), pseudo_classes, Dimension<int>(0, 0));
  }

  void CheckBox::draw_check(Canvas *canvas, const Point<int> &margin_box_point)
  {
    PseudoClasses pseudo_classes = (_M_is_checked ? PseudoClasses::CHECKED : PseudoClasses::NONE);
    draw_block(check_name(), pseudo_classes, canvas, margin_box_point, _M_check_margin_box_size);
  }
}
