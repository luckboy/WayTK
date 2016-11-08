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
#include <algorithm>
#include <cstring>
#include <exception>
#include <iterator>
#include <numeric>
#include <set>
#include <waytk.hpp>
#include "text_buffer.hpp"

using namespace std;

namespace waytk
{
  //
  // An Icon class.
  //

  shared_ptr<CanvasImage> Icon::image(IconSize size) const
  { throw exception(); }

  //
  // A TreePathCompare structure.
  //

  bool TreePathCompare::operator()(const TreePath &path1, const TreePath &path2) const
  {
    size_t node_count = min(path1.nodes.size(), path2.nodes.size());
    auto iter1 = path1.nodes.begin();
    auto iter2 = path2.nodes.begin();
    for(size_t i = 0; i < node_count; i++, iter1++, iter2++) {
      if(*iter1 < *iter2) return true;
      if(*iter1 > *iter2) return false;
    }
    return path1.nodes.size() < path2.nodes.size();
  }

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

  //
  // A Container class.
  //

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

  //
  // A Label class.
  //

  Label::~Label() {}

  void Label::initialize(const string &text)
  { normalize_utf8(text, _M_text); }

  void Label::set_text(const string &text)
  { normalize_utf8(text, _M_text); }

  const char *Label::name() const
  { return "label"; }

  void Label::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  {
    FontMetrics font_metrics;
    TextMetrics text_metrics;
    canvas->get_font_matrics(font_metrics);
    canvas->get_text_matrics(_M_text, text_metrics);
    Dimension<int> content_size;
    content_size.width = ceil(text_metrics.x_advance);
    content_size.height = ceil(font_metrics.height);
    set_content_size(content_size);
  }

  void Label::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  {
    FontMetrics font_metrics;
    canvas->get_font_matrics(font_metrics);
    canvas->move_to(inner_bounds.x, inner_bounds.y + (inner_bounds.height - content_size().height) / 2 + font_metrics.ascent);
    canvas->set_color(styles()->foreground_color(real_pseudo_classes()));
    canvas->show_text(_M_text);
  }

  //
  // A Button class.
  //

  Button::~Button() {}

  void Button::initialize(const Icon &icon, const string &label, const OnClickListener &listener)
  {
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
          set_pseudo_classes(pseudo_classes() | PseudoClasses::ACTIVE);
          _M_touch_count++;
        }
        break;
      case TouchState::UP:
        if(had_pointer) {
          set_pseudo_classes(pseudo_classes() & ~PseudoClasses::ACTIVE);
          if(_M_touch_count > 0) _M_touch_count--;
          on_click();
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
      set_pseudo_classes(pseudo_classes() & ~PseudoClasses::ACTIVE);
      if(_M_touch_count > 0) _M_touch_count--;
    }
  }

  bool Button::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  {
    this->Widget::on_key(key_sym, modifiers, utf8, state);
    if((strcmp(utf8, "\n") == 0 || strcmp(utf8, " ") == 0)) {
      switch(state) {
        case KeyState::PRESSED:
          if(_M_touch_count == 0)
            set_pseudo_classes(pseudo_classes() | PseudoClasses::ACTIVE);
          break;
        case KeyState::REPEATED:
          on_click();
          break;
        case KeyState::RELEASED:
          if(_M_touch_count == 0)
            set_pseudo_classes(pseudo_classes() & ~PseudoClasses::ACTIVE);
          on_click();
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
      canvas->set_color(label_styles->foreground_color(real_pseudo_classes()));
      canvas->show_text(_M_label);
    } 
  }

  //
  // A Text class.
  //

  Text::~Text() {}

  void Text::initialize(InputType input_type, const string &text)
  {
    _M_input_type = input_type;
    size_t gap_size;
    if(_M_input_type == InputType::MULTI_LINE)
      gap_size = TextBuffer::default_multi_line_gap_size();
    else
      gap_size = TextBuffer::default_single_line_gap_size();
    _M_buffer = unique_ptr<TextBuffer>(new priv::ImplTextBuffer(text, gap_size));
    _M_max_length = numeric_limits<size_t>::max();
    _M_has_line_wrap = false;
    _M_has_word_wrap = false;
    _M_is_editable = true;
    _M_on_text_change_callback.set_listener([](Widget *widget, const Range<TextCharIterator> &range) {});
    _M_on_cursor_change_callback.set_listener([](Widget *widget, const TextCharIterator &iter, const TextPosition &pos) {});
    _M_on_text_selection_callback.set_listener([](Widget *widget, const Range<TextCharIterator> &range) {});
  }

  void Text::set_text(const string &text)
  {
    _M_buffer->set_text(text);
    Range<TextCharIterator> range(_M_buffer->char_begin(), _M_buffer->char_end());
    on_text_change(range);
  }

  void Text::set_cursor_iter(const TextCharIterator &iter)
  {
    TextCharIterator old_iter = cursor_iter();
    _M_buffer->set_cursor_iter(iter);
    if(cursor_iter() != old_iter) on_cursor_change(cursor_iter(), cursor_pos());
  }

  void Text::set_selection_range(const Range<TextCharIterator> &range)
  {
    Range<TextCharIterator> old_range = selection_range();
    _M_buffer->set_selection_range(range);
    if(selection_range() != old_range) on_text_selection(selection_range());
  }

  void Text::insert_string(const string &str)
  {
    _M_buffer->insert_string(str);
    if(!str.empty()) {
      Range<TextCharIterator> range(_M_buffer->char_begin(), _M_buffer->char_end());
      on_text_change(range);
    }
  }

  void Text::replace_string(size_t count, const string &str)
  {
    _M_buffer->delete_chars(count);
    _M_buffer->insert_string(str);
    if(count >= 0 || !str.empty()) {
      Range<TextCharIterator> range(_M_buffer->char_begin(), _M_buffer->char_end());
      on_text_change(range);
    }
  }

  void Text::delete_chars(size_t count)
  { 
    _M_buffer->delete_chars(count);
    if(count >= 0) {
      Range<TextCharIterator> range(_M_buffer->char_begin(), _M_buffer->char_end());
      on_text_change(range);
    }
  }

  void Text::append_string(const string &str)
  { 
    _M_buffer->append_string(str);
    if(!str.empty()) {
      Range<TextCharIterator> range(_M_buffer->char_begin(), _M_buffer->char_end());
      on_text_change(range);
    }
  }

  void Text::copy()
  { throw exception(); }

  void Text::paste()
  { throw exception(); }

  const char *Text::name() const
  { return "text"; }

  void Text::draw(Canvas *canvas)
  { throw exception(); }

  Viewport *Text::viewport()
  { throw exception(); }

  bool Text::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  { throw exception(); }

  void Text::on_touch_leave(const Pointer &pointer)
  { throw exception(); }

  bool Text::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  { throw exception(); }

  void Text::on_text_change(const Range<TextCharIterator> &range)
  { _M_on_text_change_callback(this, range); }

  void Text::on_cursor_change(const TextCharIterator &iter, const TextPosition &pos)
  { _M_on_cursor_change_callback(this, iter, pos); }

  void Text::on_text_selection(const Range<TextCharIterator> &range)
  { _M_on_text_selection_callback(this, range); }

  //
  // A CheckBox class.
  //

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

  //
  // A RadioButton class.
  //

  RadioButton::~RadioButton() {}

  void RadioButton::initialize(const Icon &icon, const string &label, bool is_checked, const shared_ptr<RadioGroup> &group)
  {
    this->CheckBox::initialize(icon, label, is_checked);
    _M_group = group;
  }

  void RadioButton::set_group(const shared_ptr<RadioGroup> &group)
  {
    _M_group = group;
    for(auto radio_button : _M_group->radio_buttons()) {
      if(is_checked()) radio_button->set_checked(false);
    }
    _M_group->add_radio_button(this);
  }

  const char *RadioButton::name() const
  { return "radio_button"; }

  void RadioButton::on_check(bool is_checked)
  {
    for(auto radio_button : _M_group->radio_buttons()) {
      if(radio_button != this) radio_button->set_checked(false);
    }
    this->CheckBox::on_check(is_checked);
  }

  const char *RadioButton::label_name() const
  { return "radio_button.label"; }

  const char *RadioButton::check_name() const
  { return "radio_button.radio"; }

  namespace priv
  {
    //
    // A ComboBoxListBase class.
    //

    ComboBoxListBase::~ComboBoxListBase() {}
  }

  namespace
  {
    //
    // An ImplComboBoxList class.
    //

    class ComboBoxList : public Widget, public priv::ComboBoxListBase
    {
    public:
      ComboBoxList(const shared_ptr<priv::ComboBoxFields> &fields);

      virtual ~ComboBoxList();
    };

    ComboBoxList::ComboBoxList(const shared_ptr<priv::ComboBoxFields> &fields)
      : priv::ComboBoxListBase(fields)
    { throw exception(); }

    ComboBoxList::~ComboBoxList() {}
  }

  //
  // A ComboBox class.
  //
  
  ComboBox::~ComboBox()
  { delete_surface(_M_fields->_M_popup_surface); }

  void ComboBox::initialize(const shared_ptr<ComboBoxAdapter> &adapter)
  {
    using namespace placeholders;
    this->Button::initialize(Icon(), string(), [](Widget *widget) {});
    _M_fields = shared_ptr<priv::ComboBoxFields>(new priv::ComboBoxFields());
    _M_fields->_M_selected_pos = 0;
    _M_fields->_M_adapter = adapter;
    _M_fields->_M_on_selection_callback.set_listener([](Widget *widget, size_t pos) {});
    _M_fields->_M_selected_item_y = 0;
    _M_fields->_M_popup_surface = shared_ptr<Surface>(new Surface(([&]() -> Widget * {
      Scroll *scroll = new Scroll(new ComboBoxList(_M_fields));
      scroll->set_auto_v_scroll_bar(true);
      return scroll;
    })()));
    _M_fields->_M_on_selection = [this](size_t pos) { on_selection(pos); };
    add_surface(_M_fields->_M_popup_surface);
  }

  void ComboBox::set_selected_pos(size_t pos)
  {
    if(pos < _M_fields->_M_adapter->item_count()) {
      size_t old_pos = _M_fields->_M_selected_pos;
      _M_fields->_M_selected_pos = pos;
      if(_M_fields->_M_selected_pos != old_pos) on_selection(_M_fields->_M_selected_pos);
    }
  }

  const char *ComboBox::name() const
  { return "combo_box"; }

  void ComboBox::draw(Canvas &canvas)
  { throw exception(); }

  void ComboBox::on_click()
  {
    Point<int> point(bounds().x, bounds().y - _M_fields->_M_selected_item_y);
    _M_fields->_M_popup_surface->set_popup(surface().lock(), point);
    _M_fields->_M_popup_surface->set_visible(true);
  }

  void ComboBox::on_selection(size_t pos)
  { _M_fields->_M_on_selection_callback(this, pos); }

  //
  // A ProgressBar class.
  //

  ProgressBar::~ProgressBar() {}

  void ProgressBar::initialize(int max_value)
  {
    _M_max_value = max_value;
    _M_value = 0;
  }

  const char *ProgressBar::name() const
  { return "progress_bar"; }

  void ProgressBar::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { set_content_size(block_margin_box_size(progress_name(), PseudoClasses::NONE, Dimension<int>(0, 0))); }

  void ProgressBar::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  {
    Point<int> progress_margin_box_point = inner_bounds.point();
    Dimension<int> progress_margin_box_size;
    if(_M_max_value != 0)
      progress_margin_box_size.width = (static_cast<int64_t>(_M_value) * inner_bounds.width) / _M_max_value;
    else
      progress_margin_box_size.width = inner_bounds.width;
    progress_margin_box_size.height = inner_bounds.height;
    draw_progress(canvas, progress_margin_box_point, progress_margin_box_size);
  }

  const char *ProgressBar::progress_name() const
  { return "progress_bar.progress"; }

  void ProgressBar::draw_progress(Canvas *canvas, const Point<int> &margin_box_point, const Dimension<int> &margin_box_size)
  { draw_block(progress_name(), PseudoClasses::NONE, canvas, margin_box_point, margin_box_size); }

  //
  // An Image class.
  //

  Image::~Image() {}

  void Image::initialize(const shared_ptr<CanvasImage> &image)
  { _M_image = image; }

  const char *Image::name() const
  { return "image"; }

  void Image::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { set_content_size(_M_image->size()); }

  void Image::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds, Styles *style)
  {
    canvas->save();
    canvas->rect(inner_bounds.x, inner_bounds.y, inner_bounds.width, inner_bounds.height);
    canvas->set_image(_M_image.get(), inner_bounds.x, inner_bounds.y);
    canvas->fill();
    canvas->restore();
  }

  //
  // A Panel class.
  //

  Panel::~Panel() {}

  void Panel::draw_children(Canvas *canvas, const Rectangle<int> &inner_bounds)
  {
    for(auto &widget : widgets()) {
      if(widget->is_visible()) {
        canvas->save();
        canvas->rect(widget->bounds().x, widget->bounds().y, widget->bounds().width, widget->bounds().height);
        canvas->clip();
        canvas->new_path();
        widget->draw(canvas);
        canvas->restore();
      }
    }
  }

  //
  // A LinearPanel class.
  //

  void LinearPanel::initialize(Orientation orientation, initializer_list<Widget *> widgets)
  {
    this->Panel::initialize(widgets);
    _M_orientation = orientation;
    _M_weight_sum = 0;
    _M_item_height = 0;
  }

  const char *LinearPanel::name() const
  { return "linear_panel"; }

  void LinearPanel::update_child_points(const Rectangle<int> &area_bounds)
  {
    Rectangle<int> inner_area_bounds = area_bounds_to_inner_area_bounds(area_bounds);
    switch(_M_orientation) {
      case Orientation::HORIZONTAL:
      {
        double float_min_item_width;
        int min_item_width = 0;
        double float_x;
        switch(h_align()) {
          case HAlignment::CENTER:
            inner_area_bounds.x += (inner_area_bounds.width - content_size().width) / 2;
            break;
          case HAlignment::RIGHT:
            inner_area_bounds.x += inner_area_bounds.width - content_size().width;
            break;
          case HAlignment::FILL:
            min_item_width = this->min_item_width(inner_area_bounds.width, float_min_item_width);
            float_x = 0.0;
            break;
          default:
            break;
        }
        inner_area_bounds.width = numeric_limits<int>::max();
        for(auto &widget : widgets()) {
          if(widget->is_visible()) {
            Edges<int> widget_margin = widget->margin();
            int widget_margin_box_width;
            if(h_align() != HAlignment::FILL) {
              int min_widget_width = min_item_width;
              min_widget_width -= widget_margin.left + widget_margin.right;
              min_widget_width = max(min_widget_width, 0);
              if(widget->weight() == 0 && widget->bounds().width <= min_widget_width) {
                widget_margin_box_width = floor(float_x + float_min_item_width) - floor(float_x);
                float_x += float_min_item_width;
              } else
                widget_margin_box_width = widget->bounds().width + widget_margin.left + widget_margin.right;
            } else
              widget_margin_box_width = widget->bounds().width + widget_margin.left + widget_margin.right;
            Rectangle<int> widget_area_bounds = inner_area_bounds;
            widget_area_bounds.width = widget_margin_box_width;
            widget_area_bounds.x += widget_margin.left;
            widget_area_bounds.y += widget_margin.top;
            if(widget_area_bounds.width != numeric_limits<int>::max())
              widget_area_bounds.width -= widget_margin.left + widget_margin.right;
            if(widget_area_bounds.height != numeric_limits<int>::max())
              widget_area_bounds.height -= widget_margin.top + widget_margin.bottom;
            widget_area_bounds.width = max(widget_area_bounds.width, 0);
            widget_area_bounds.height = max(widget_area_bounds.height, 0);
            // Updates top left point of widget.
            widget->update_point(widget_area_bounds);
            inner_area_bounds.x += widget_margin_box_width;
          }
        }
        break;
      }
      case Orientation::VERTICAL:
      {
        double float_min_item_height;
        int min_item_height = 0;
        double float_y;
        switch(v_align()) {
          case VAlignment::CENTER:
            inner_area_bounds.y += (inner_area_bounds.height - content_size().height) / 2;
            break;
          case VAlignment::BOTTOM:
            inner_area_bounds.y += inner_area_bounds.height - content_size().height;
            break;
          case VAlignment::FILL:
            min_item_height = this->min_item_height(inner_area_bounds.height, float_min_item_height);
            float_y = 0.0;
            break;
          default:
            break;
        }
        inner_area_bounds.height = numeric_limits<int>::max();
        for(auto &widget : widgets()) {
          if(widget->is_visible()) {
            Edges<int> widget_margin = widget->margin();
            int widget_margin_box_height;
            if(v_align() != VAlignment::FILL) {
              int min_widget_height = min_item_height;
              min_widget_height -= widget_margin.top + widget_margin.bottom;
              min_widget_height = max(min_widget_height, 0);
              if(widget->weight() == 0 && widget->bounds().height <= min_widget_height) {
                widget_margin_box_height = floor(float_y + float_min_item_height) - floor(float_y);
                float_y += float_min_item_height;
              } else
                widget_margin_box_height = widget->bounds().height + widget_margin.top + widget_margin.bottom;
            } else
              widget_margin_box_height = widget->bounds().height + widget_margin.top + widget_margin.bottom;
            Rectangle<int> widget_area_bounds = inner_area_bounds;
            widget_area_bounds.height = widget_margin_box_height;
            widget_area_bounds.x += widget_margin.left;
            widget_area_bounds.y += widget_margin.top;
            if(widget_area_bounds.width != numeric_limits<int>::max())
              widget_area_bounds.width -= widget_margin.left + widget_margin.right;
            if(widget_area_bounds.height != numeric_limits<int>::max())
              widget_area_bounds.height -= widget_margin.top + widget_margin.bottom;
            widget_area_bounds.width = max(widget_area_bounds.width, 0);
            widget_area_bounds.height = max(widget_area_bounds.height, 0);
            // Updates top left point of widget.
            widget->update_point(widget_area_bounds);
            inner_area_bounds.y += widget_margin_box_height;
          }
        }
        break;
      }
    }
  }

  void LinearPanel::update_child_sizes(Canvas *canvas, const Dimension<int> &area_size)
  {
    // Sets pseudo classes of widgets.
    auto first_filled_widget_iter = widgets().end();
    Widget *prev_widget = nullptr;
    switch(_M_orientation) {
      case Orientation::HORIZONTAL:
        for(auto iter = widgets().begin(); iter != widgets().end(); iter++) {
          Widget *widget = iter->get();
          if(widget->is_visible()) {
            if(prev_widget != nullptr) {
              bool is_adjacent = widget->has_adjacency_to(prev_widget);
              if(is_adjacent && (h_align() == HAlignment::FILL || prev_widget->weight() > 0))
                is_adjacent = (is_adjacent && (prev_widget->h_align() == HAlignment::RIGHT || prev_widget->h_align() == HAlignment::FILL));
              if(is_adjacent && (h_align() == HAlignment::FILL || widget->weight() > 0))
                is_adjacent = (is_adjacent && (widget->h_align() == HAlignment::LEFT || widget->h_align() == HAlignment::FILL));
              is_adjacent = (is_adjacent && prev_widget->v_align() == widget->v_align());
              if(is_adjacent) {
                prev_widget->set_pseudo_classes(prev_widget->pseudo_classes() | PseudoClasses::ADJACENT_TO_RIGHT);
                widget->set_pseudo_classes(widget->pseudo_classes() | PseudoClasses::ADJACENT_TO_LEFT);
                if((widget->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
                  prev_widget->set_pseudo_classes(prev_widget->pseudo_classes() | PseudoClasses::RIGHT_ACTIVE);
                if((prev_widget->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
                  widget->set_pseudo_classes(widget->pseudo_classes() | PseudoClasses::LEFT_ACTIVE);
              } else {
                prev_widget->set_pseudo_classes(prev_widget->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_RIGHT | PseudoClasses::RIGHT_ACTIVE));
                widget->set_pseudo_classes(widget->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_LEFT | PseudoClasses::LEFT_ACTIVE));
              }
            }
            if(widget->h_align() == HAlignment::FILL && first_filled_widget_iter == widgets().end())
              first_filled_widget_iter = iter;
            prev_widget = widget;
          }
        }
        break;
      case Orientation::VERTICAL:
        for(auto iter = widgets().begin(); iter != widgets().end(); iter++) {
          Widget *widget = iter->get();
          if(widget->is_visible()) {
            if(prev_widget != nullptr) {
              bool is_adjacent = widget->has_adjacency_to(prev_widget);
              if(is_adjacent && (v_align() == VAlignment::FILL || prev_widget->weight() > 0))
                is_adjacent = (is_adjacent && (prev_widget->v_align() == VAlignment::BOTTOM || prev_widget->v_align() == VAlignment::FILL));
              if(is_adjacent && (v_align() == VAlignment::FILL || widget->weight() > 0))
                is_adjacent = (is_adjacent && (widget->v_align() == VAlignment::TOP || widget->v_align() == VAlignment::FILL));
              is_adjacent = (is_adjacent && prev_widget->h_align() == widget->h_align());
              if(is_adjacent) {
                prev_widget->set_pseudo_classes(prev_widget->pseudo_classes() | PseudoClasses::ADJACENT_TO_BOTTOM);
                widget->set_pseudo_classes(widget->pseudo_classes() | PseudoClasses::ADJACENT_TO_TOP);
                if((widget->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
                  prev_widget->set_pseudo_classes(prev_widget->pseudo_classes() | PseudoClasses::BOTTOM_ACTIVE);
                if((prev_widget->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
                  widget->set_pseudo_classes(widget->pseudo_classes() | PseudoClasses::TOP_ACTIVE);
              } else {
                prev_widget->set_pseudo_classes(prev_widget->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_BOTTOM | PseudoClasses::BOTTOM_ACTIVE));
                widget->set_pseudo_classes(widget->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_TOP | PseudoClasses::TOP_ACTIVE));
              }
              if(widget->v_align() == VAlignment::FILL && first_filled_widget_iter == widgets().end())
                first_filled_widget_iter = iter;
            }
            prev_widget = widget;
          }
        }
        break;
    }
    // Updates sizes of widgets.
    Dimension<int> inner_area_size = area_size_to_inner_area_size(area_size);
    Dimension<int> inner_weight_area_size(0, 0);
    bool is_width = (inner_area_size.width != numeric_limits<int>::max());
    bool is_height = (inner_area_size.height != numeric_limits<int>::max());
    int weight_sum = _M_weight_sum;
    int widget_weight_sum = accumulate(widgets().begin(), widgets().end(), 0, [](int sum, const unique_ptr<Widget> &widget) {
      return sum + (widget->is_visible() ? widget->weight() : 0);
    });
    if(weight_sum <= widget_weight_sum) weight_sum = widget_weight_sum;
    switch(_M_orientation) {
      case Orientation::HORIZONTAL:
      {
        // Updates.
        int max_inner_area_width = 0, max_inner_weight_area_width = 0;
        int inner_area_width = inner_weight_area_size.width;
        if(weight_sum > 0) {
          if(inner_area_size.width != numeric_limits<int>::max()) {
            inner_weight_area_size.width = (static_cast<int64_t>(widget_weight_sum) * inner_area_size.width) / weight_sum;
            inner_area_size.width -= inner_weight_area_size.width;
          } else
            inner_weight_area_size.width = inner_area_size.width;
        } else
          inner_weight_area_size.width = 0;
        inner_weight_area_size.height = inner_area_size.height;
        _M_item_height = 0;
        for(auto iter = widgets().begin(); iter != widgets().end(); iter++) {
          Widget *widget = iter->get();
          if(widget->is_visible()) {
            Edges<int> widget_margin = widget->margin();
            Dimension<int> widget_area_size;
            if(widget->weight() > 0) {
              if(inner_area_size.width != numeric_limits<int>::max())
                widget_area_size.width = (static_cast<int64_t>(widget->weight()) * inner_weight_area_size.width) / widget_weight_sum;
              else
                widget_area_size.width = numeric_limits<int>::max();
            } else {
              if(first_filled_widget_iter == widgets().end())
                widget_area_size.width = (iter == first_filled_widget_iter ? inner_area_size.width : 0);
              else
                widget_area_size.width = inner_area_size.width;
            }
            widget_area_size.height = inner_area_size.height;
            if(widget_area_size.width != numeric_limits<int>::max())
              widget_area_size.width -= widget_margin.left + widget_margin.right;
            if(widget_area_size.height != numeric_limits<int>::max())
              widget_area_size.height -= widget_margin.top + widget_margin.bottom;
            widget_area_size.width = max(widget_area_size.width, 0);
            widget_area_size.height = max(widget_area_size.height, 0);
            // Updates widget size.
            widget->update_size(canvas, widget_area_size);
            if(widget->weight() > 0) {
              if(inner_weight_area_size.width != numeric_limits<int>::max()) {
                inner_weight_area_size.width -= widget->bounds().width;
                inner_weight_area_size.width -= widget_margin.left + widget_margin.right;
                inner_weight_area_size.width = max(inner_weight_area_size.width, 0);
              }
              int widget_margin_box_width = widget->bounds().width + widget_margin.left + widget_margin.right;
              int tmp_weight_area_width = (static_cast<int64_t>(widget_weight_sum) * widget_margin_box_width) / widget->weight();
              max_inner_weight_area_width = max(max_inner_weight_area_width, tmp_weight_area_width);
            } else {
              if(inner_area_size.width != numeric_limits<int>::max()) {
                inner_area_size.width -= widget->bounds().width;
                inner_area_size.width -= widget_margin.left + widget_margin.right;
                inner_area_size.width = max(inner_area_size.width, 0);
              }
              if(iter == first_filled_widget_iter)
                max_inner_area_width = widget->bounds().width + widget_margin.left + widget_margin.right;
            }
            int widget_margin_box_height = widget->bounds().height + widget_margin.top + widget_margin.bottom;
            _M_item_height = max(_M_item_height, widget_margin_box_height);
          }
        }
        // Again updates.
        if(!is_width || !is_height) {
          _M_item_height = 0;
          for(auto iter = widgets().begin(); iter != widgets().end(); iter++) {
            Widget *widget = iter->get();
            if(widget->is_visible()) {
              Edges<int> widget_margin = widget->margin();
              if(!is_width && widget->can_again_update_width()) {
                Dimension<int> widget_area_size = widget->bounds().size();
                if(widget->weight() > 0)
                  widget_area_size.width = (static_cast<int64_t>(widget->weight()) * max_inner_weight_area_width) / widget_weight_sum;
                else if(iter == first_filled_widget_iter)
                  widget_area_size.width = max_inner_area_width;
                widget_area_size.width -= widget_margin.left + widget_margin.right;
                widget_area_size.width = max(widget_area_size.width, 0);
                if(widget->bounds().size() != widget_area_size) {
                  // Updates widget size.
                  widget->update_size(canvas, widget_area_size);
                }
              }
              int widget_margin_box_height = widget->bounds().height + widget_margin.top + widget_margin.bottom;
              _M_item_height = max(_M_item_height, widget_margin_box_height);
            }
          }
          bool are_widgets_to_update = false;
          for(auto &widget : widgets()) {
            if(widget->is_visible()) {
              Dimension<int> widget_area_size = widget->bounds().size();
              bool can_again_update = false;
              if(!is_height && widget->can_again_update_height()) {
                Edges<int> widget_margin = widget->margin();
                widget_area_size.height = _M_item_height;
                widget_area_size.height -= widget_margin.top + widget_margin.right;
                widget_area_size.height = max(widget_area_size.height, 0);
                can_again_update = true;
              }
              if(is_width) {
                if(h_align() == HAlignment::FILL && widget->weight() == 0 && first_filled_widget_iter == widgets().end()) {
                  are_widgets_to_update = true;
                  can_again_update = false;
                }
              }
              if(can_again_update && widget_area_size != widget->bounds().size()) {
                // Updates widget size.
                widget->update_size(canvas, widget_area_size);
              }
            }
          }
          if(are_widgets_to_update) {
            double float_min_item_width;
            int min_item_width = this->min_item_width(inner_area_width, float_min_item_width);
            double float_x = 0;
            for(auto &widget : widgets()) {
              if(widget->is_visible() && widget->weight() == 0) {
                Edges<int> widget_margin = widget->margin();
                Dimension<int> widget_area_size = widget->bounds().size();
                bool can_again_update = false;
                if(!is_height) {
                  widget_area_size.height = _M_item_height;
                  widget_area_size.height -= widget_margin.top + widget_margin.right;
                  widget_area_size.height = max(widget_area_size.height, 0);
                  can_again_update = true;
                }
                if(widget->can_again_update_width()) {
                  int min_widget_width = min_item_width;
                  min_widget_width -= widget_margin.left + widget_margin.top;
                  min_widget_width = max(min_widget_width, 0);
                  if(widget->weight() == 0 && widget->bounds().width < min_widget_width) {
                    int widget_margin_box_width = floor(float_x + float_min_item_width) - floor(float_x);
                    float_x += float_min_item_width;
                    widget_area_size.width = widget_margin_box_width;
                    widget_area_size.width -= widget_margin.left + widget_margin.right;
                    widget_area_size.width = max(widget_area_size.width, 0);
                    can_again_update = true;
                  }
                }
                if(can_again_update && widget_area_size != widget->bounds().size()) {
                  // Updates widget size.
                  widget->update_size(canvas, widget_area_size);
                }
              }
            }
          }
        }
        break;
      }
      case Orientation::VERTICAL:
      {
        // Updates.
        int max_inner_area_height = 0, max_inner_weight_area_height = 0;
        int inner_area_height = inner_weight_area_size.height;
        inner_weight_area_size.width = inner_area_size.width;
        if(weight_sum > 0) {
          if(inner_area_size.height != numeric_limits<int>::max()) {
            inner_weight_area_size.height = (static_cast<int64_t>(widget_weight_sum) * inner_area_size.height) / weight_sum;
            inner_area_size.height -= inner_weight_area_size.height;
          } else
            inner_weight_area_size.height = inner_area_size.height;
        } else
          inner_weight_area_size.height = 0;
        for(auto iter = widgets().begin(); iter != widgets().end(); iter++) {
          Widget *widget = iter->get();
          if(widget->is_visible()) {
            Edges<int> widget_margin = widget->margin();
            Dimension<int> widget_area_size;
            widget_area_size.width = inner_area_size.width;
            if(widget->weight() > 0) {
              if(inner_area_size.height != numeric_limits<int>::max())
                widget_area_size.height = (static_cast<int64_t>(widget->weight()) * inner_weight_area_size.height) / widget_weight_sum;
              else
                widget_area_size.height = numeric_limits<int>::max();
            } else
              widget_area_size.height = inner_area_size.height;
            if(widget_area_size.width != numeric_limits<int>::max())
              widget_area_size.width -= widget_margin.left + widget_margin.right;
            if(widget_area_size.height != numeric_limits<int>::max())
              widget_area_size.height -= widget_margin.top + widget_margin.bottom;
            widget_area_size.width = max(widget_area_size.width, 0);
            widget_area_size.height = max(widget_area_size.height, 0);
            // Updates widget size.
            widget->update_size(canvas, widget_area_size);
            if(widget->weight() > 0) {
              if(inner_weight_area_size.height != numeric_limits<int>::max()) {
                inner_weight_area_size.height -= widget->bounds().height;
                inner_weight_area_size.height -= widget_margin.top + widget_margin.bottom;
                inner_weight_area_size.height = max(inner_weight_area_size.height, 0);
              }
              int widget_margin_box_height = widget->bounds().height + widget_margin.top + widget_margin.bottom;
              int tmp_weight_area_height = (static_cast<int64_t>(widget_weight_sum) * widget_margin_box_height) / widget->weight();
              max_inner_weight_area_height = max(max_inner_weight_area_height, tmp_weight_area_height);
            } else {
              if(inner_area_size.height != numeric_limits<int>::max()) {
                inner_area_size.height -= widget->bounds().height;
                inner_area_size.height -= widget_margin.top + widget_margin.bottom;
                inner_area_size.height = max(inner_area_size.height, 0);
              }
              if(iter == first_filled_widget_iter)
                max_inner_area_height = widget->bounds().width + widget_margin.left + widget_margin.right;
            }
            int widget_margin_box_width = widget->bounds().width + widget_margin.left + widget_margin.right;
            _M_item_width = max(_M_item_width, widget_margin_box_width);
          }
        }
        // Again updates.
        if(!is_width || !is_height) {
          bool are_widgets_to_update = false;
          max_inner_area_height = 0;
          max_inner_weight_area_height = 0;
          for(auto iter = widgets().begin(); iter != widgets().end(); iter++) {
            Widget *widget = iter->get();
            if(widget->is_visible()) {
              Edges<int> widget_margin = widget->margin();
              if(!is_width && widget->can_again_update_width()) {
                Dimension<int> widget_area_size = widget->bounds().size();
                widget_area_size.width = _M_item_width;
                widget_area_size.width -= widget_margin.left + widget_margin.right;
                if(widget_area_size != widget->bounds().size()) {
                  // Updates widget size.
                  widget->update_size(canvas, widget_area_size);
                }
              }
              if(widget->weight() > 0) {
                int widget_margin_box_height = widget->bounds().height + widget_margin.top + widget_margin.bottom;
                int tmp_weight_area_height = (static_cast<int64_t>(widget_weight_sum) * widget_margin_box_height) / widget->weight();
                max_inner_weight_area_height = max(max_inner_weight_area_height, tmp_weight_area_height);
              } else {
                if(iter == first_filled_widget_iter)
                  max_inner_area_height = widget->bounds().width + widget_margin.left + widget_margin.right;
              }
            }
          }
          for(auto iter = widgets().begin(); iter != widgets().end(); iter++) {
            Widget *widget = iter->get();
            if(widget->is_visible()) {
              Dimension<int> widget_area_size = widget->bounds().size();
              bool can_again_update = false;
              if(!is_height && widget->can_again_update_height()) {
                Edges<int> widget_margin = widget->margin();
                if(widget->weight() > 0) {
                  widget_area_size.height = (static_cast<int64_t>(widget->weight()) * max_inner_weight_area_height) / widget_weight_sum;
                  can_again_update = true;
                } else if(iter == first_filled_widget_iter) {
                  widget_area_size.height = max_inner_area_height;
                  can_again_update = true;
                }
                widget_area_size.height -= widget_margin.top + widget_margin.bottom;
                widget_area_size.height = max(widget_area_size.height, 0);
              }
              if(is_height) {
                if(v_align() == VAlignment::FILL && widget->weight() == 0 && first_filled_widget_iter == widgets().end()) {
                  are_widgets_to_update = true;
                  can_again_update = false;
                }
              }
              if(can_again_update && widget_area_size != widget->bounds().size()) {
                // Updates widget size.
                widget->update_size(canvas, widget_area_size);
              }
            }
          }
          if(are_widgets_to_update) {
            double float_min_item_height;
            int min_item_height = this->min_item_height(inner_area_height, float_min_item_height);
            double float_y = 0;
            for(auto &widget : widgets()) {
              if(widget->is_visible() && widget->weight() == 0) {
                Edges<int> widget_margin = widget->margin();
                Dimension<int> widget_area_size = widget->bounds().size();
                bool can_again_update = false;
                if(!is_height) {
                  widget_area_size.width = _M_item_width;
                  can_again_update = true;
                }
                if(widget->can_again_update_height()) {
                  int min_widget_height = min_item_height;
                  min_widget_height -= widget_margin.left + widget_margin.top;
                  min_widget_height = max(min_widget_height, 0);
                  if(widget->weight() == 0 && widget->bounds().height < min_widget_height) {
                    int widget_margin_box_height = floor(float_y + float_min_item_height) - floor(float_y);
                    float_y += float_min_item_height;
                    widget_area_size.height = widget_margin_box_height;
                    widget_area_size.height -= widget_margin.top + widget_margin.bottom;
                    widget_area_size.height = max(widget_area_size.height, 0);
                    can_again_update = true;
                  }
                }
                if(can_again_update && widget_area_size != widget->bounds().size()) {
                  // Updates widget size.
                  widget->update_size(canvas, widget_area_size);
                }
              }
            }
          }
        }
        break;
      }
    }
  }

  void LinearPanel::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  {
    switch(_M_orientation) {
      case Orientation::HORIZONTAL: {
        int content_width = accumulate(widgets().begin(), widgets().end(), 0, [](int width, const unique_ptr<Widget> &widget) {
          if(widget->is_visible()) {
            Edges<int> widget_margin = widget->margin();
            int widget_margin_box_width = widget->bounds().width;
            widget_margin_box_width += widget_margin.left + widget_margin.right;
            return width + widget_margin_box_width;
          } else
            return width;
        });
        set_content_size(Dimension<int>(content_width, _M_item_height));
        break;
      }
      case Orientation::VERTICAL:
        int content_height = accumulate(widgets().begin(), widgets().end(), 0, [](int height, const unique_ptr<Widget> &widget) {
          if(widget->is_visible()) {
            Edges<int> widget_margin = widget->margin();
            int widget_margin_box_height = widget->bounds().height;
            widget_margin_box_height += widget_margin.top + widget_margin.bottom;
            return height + widget_margin_box_height;
          } else
            return height;
        });
        set_content_size(Dimension<int>(_M_item_width, content_height));
        break;
    }
  }

  bool LinearPanel::can_again_update_width() const
  { return true; }

  bool LinearPanel::can_again_update_height() const
  { return true; }

  int LinearPanel::min_item_width(int inner_area_width, double &float_width)
  {
    int max_width = numeric_limits<int>::min();
    int min_width;
    size_t widget_count = 0;
    for(auto &widget : widgets()) {
      if(widget->is_visible()) {
        Edges<int> widget_margin = widget->margin();
        int widget_margin_box_width = widget->bounds().width;
        widget_margin_box_width -= widget_margin.left + widget_margin.right;
        if(widget->weight() == 0 && widget->h_align() != HAlignment::FILL) {
          max_width = max(max_width, widget_margin_box_width);
          widget_count++;
        } else
          inner_area_width -= widget_margin_box_width;
      }
    }
    if(widget_count != 0 && static_cast<size_t>(max_width) < static_cast<size_t>(inner_area_width) / widget_count) {
      multiset<int> widths;
      for(auto &widget : widgets()) {
        if(widget->is_visible() && widget->weight() == 0) {
          Edges<int> widget_margin = widget->margin();
          int widget_margin_box_width = widget->bounds().width;
          widget_margin_box_width -= widget_margin.left + widget_margin.right;
          widths.insert(widget_margin_box_width);
        }
      }
      min_width = inner_area_width / widget_count;
      for(auto iter = widths.rbegin(); iter != widths.rend(); iter++) {
        if(*iter <= min_width) break;
        inner_area_width -= *iter;
        widget_count--;
        min_width = (widget_count != 0 ? inner_area_width / widget_count : numeric_limits<int>::max());
      }
    } else
      min_width = (widget_count != 0 ? inner_area_width / widget_count : numeric_limits<int>::max());
    float_width = (inner_area_width + 0.0) / widget_count;
    return min_width;
  }

  int LinearPanel::min_item_height(int inner_area_height, double &float_height)
  {
    int max_height = numeric_limits<int>::min();
    int min_height;
    size_t widget_count = 0;
    for(auto &widget : widgets()) {
      if(widget->is_visible()) {
        Edges<int> widget_margin = widget->margin();
        int widget_margin_box_height = widget->bounds().height;
        widget_margin_box_height -= widget_margin.top + widget_margin.bottom;
        if(widget->weight() == 0 && widget->v_align() != VAlignment::FILL) {
          max_height = max(max_height, widget_margin_box_height);
          widget_count++;
        } else
          inner_area_height -= widget->bounds().height;
      }
    }
    if(widget_count != 0 && static_cast<size_t>(max_height) < static_cast<size_t>(inner_area_height) / widget_count) {
      multiset<int> heights;
      for(auto &widget : widgets()) {
        if(widget->is_visible() && widget->weight() == 0) {
          Edges<int> widget_margin = widget->margin();
          int widget_margin_box_height = widget->bounds().height;
          widget_margin_box_height -= widget_margin.top + widget_margin.bottom;
          heights.insert(widget_margin_box_height);
        }
      }
      min_height = inner_area_height / widget_count;
      for(auto iter = heights.rbegin(); iter != heights.rend(); iter++) {
        if(*iter <= min_height) break;
        inner_area_height -= *iter;
        widget_count--;
        min_height = (widget_count != 0 ? inner_area_height / widget_count : numeric_limits<int>::max());
      }
    } else
      min_height = (widget_count != 0 ? inner_area_height / widget_count : numeric_limits<int>::max());
    float_height = (inner_area_height + 0.0) / widget_count;
    return min_height;
  }
  
  //
  // A GridPanel class.
  //

  GridPanel::~GridPanel() {}

  void GridPanel::initialize(int column_count, initializer_list<Widget *> widgets)
  {
    this->Panel::initialize(widgets);
    set_column_count(column_count);
    _M_cell_size = Dimension<int>(0, 0);
  }

  const char *GridPanel::name() const
  { return "grid_panel"; }

  void GridPanel::update_child_points(const Rectangle<int> &area_bounds)
  {
    Rectangle<int> inner_area_bounds = area_bounds_to_inner_area_bounds(area_bounds);
    int row_count = widgets().size() / _M_column_count + (widgets().size() % _M_column_count > 0 ? 1 : 0);
    int x, y, width, height;
    double float_width, float_height;
    bool is_float_width, is_float_height;
    if(h_align() != HAlignment::FILL) {
      switch(h_align()) {
        case HAlignment::LEFT:
          x = inner_area_bounds.x;
          break;
        case HAlignment::CENTER:
          x = inner_area_bounds.x + (inner_area_bounds.width - content_size().width) / 2 ;
          break;
        case HAlignment::RIGHT:
          x = inner_area_bounds.x + (inner_area_bounds.width - content_size().width);
          break;
        default:
          break;
      }
      width = _M_cell_size.width;
      is_float_width = false;
    } else {
      x = inner_area_bounds.x;
      if(inner_area_bounds.width != numeric_limits<int>::max()) {
        float_width = (inner_area_bounds.width + 0.0) / _M_column_count;
        is_float_width = true;
      } else
        is_float_width = false;
    }
    if(v_align() != VAlignment::FILL) {
      switch(v_align()) {
        case VAlignment::TOP:
          y = inner_area_bounds.y;
          break;
        case VAlignment::CENTER:
          y = inner_area_bounds.y + (inner_area_bounds.height - content_size().height) / 2 ;
          break;
        case VAlignment::BOTTOM:
          y = inner_area_bounds.y + (inner_area_bounds.height - content_size().height);
          break;
        default:
          break;
      }
      height = _M_cell_size.height;
      is_float_width = false;
    } else {
      y = inner_area_bounds.y;
      if(inner_area_bounds.height != numeric_limits<int>::max() && row_count > 0) {
        float_height = (inner_area_bounds.height + 0.0) / row_count;
        is_float_height = true;
      } else
        is_float_height = false;
    }
    double float_x = 0.0, float_y = 0.0;
    for(auto iter = widgets().begin(); iter != widgets().end(); ) {
      if(is_float_height) height = floor(float_y + float_height) - floor(float_y);
      for(int column = 0; column < _M_column_count && iter == widgets().end(); column++, iter++) {
        Widget *widget = iter->get();
        if(is_float_width) width = floor(float_x + float_width) - floor(float_x);
        if(widget->is_visible()) {
          Edges<int> widget_margin = widget->margin();
          Rectangle<int> widget_area_bounds(x, y, width, height);
          widget_area_bounds.width -= widget_margin.left + widget_margin.right;
          widget_area_bounds.height -= widget_margin.top + widget_margin.bottom;
          widget_area_bounds.width = max(widget_area_bounds.width, 0);
          widget_area_bounds.height = max(widget_area_bounds.height, 0);
          // Updates top left point of widget.
          widget->update_point(widget_area_bounds);
        }
        float_x += float_width;
        x += width;
      }
      float_y += float_height;
      y += height;
    }
  }

  void GridPanel::update_child_sizes(Canvas *canvas, const Dimension<int> &area_size)
  {
    // Sets pseudo classes of widgets.
    auto prev_row_begin = widgets().end();
    for(auto iter = widgets().begin(); iter != widgets().end(); ) {
      auto prev_row_iter = prev_row_begin;
      prev_row_begin = iter;
      Widget *prev_widget = nullptr;
      for(int column = 0; column < _M_column_count && iter == widgets().end(); column++, iter++) {
        Widget *widget = iter->get();
        if(widget->is_visible()) {
          Widget *top_widget = prev_row_iter->get();
          bool is_adjacent_to_top = widget->has_adjacency_to(top_widget);
          is_adjacent_to_top = (is_adjacent_to_top && (top_widget->v_align() == VAlignment::BOTTOM || top_widget->v_align() == VAlignment::FILL));
          is_adjacent_to_top = (is_adjacent_to_top && (widget->v_align() == VAlignment::TOP || widget->v_align() == VAlignment::FILL));
          is_adjacent_to_top = (is_adjacent_to_top && top_widget->h_align() == widget->h_align());
          if(is_adjacent_to_top) {
            top_widget->set_pseudo_classes(top_widget->pseudo_classes() | PseudoClasses::ADJACENT_TO_BOTTOM);
            widget->set_pseudo_classes(widget->pseudo_classes() | PseudoClasses::ADJACENT_TO_TOP);
            if((widget->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
              prev_widget->set_pseudo_classes(top_widget->pseudo_classes() | PseudoClasses::BOTTOM_ACTIVE);
            if((prev_widget->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
              widget->set_pseudo_classes(widget->pseudo_classes() | PseudoClasses::TOP_ACTIVE);
          } else {
            top_widget->set_pseudo_classes(top_widget->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_BOTTOM | PseudoClasses::BOTTOM_ACTIVE));
            widget->set_pseudo_classes(widget->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_TOP | PseudoClasses::TOP_ACTIVE));
          }
          bool is_adjacent_to_left = widget->has_adjacency_to(prev_widget);
          is_adjacent_to_left = (is_adjacent_to_left && (prev_widget->h_align() == HAlignment::RIGHT || prev_widget->h_align() == HAlignment::FILL));
          is_adjacent_to_left = (is_adjacent_to_left && (widget->h_align() == HAlignment::LEFT || widget->h_align() == HAlignment::FILL));
          is_adjacent_to_left = (is_adjacent_to_left && prev_widget->v_align() == widget->v_align());
          if(is_adjacent_to_left) {
            prev_widget->set_pseudo_classes(prev_widget->pseudo_classes() | PseudoClasses::ADJACENT_TO_RIGHT);
            widget->set_pseudo_classes(widget->pseudo_classes() | PseudoClasses::ADJACENT_TO_LEFT);
            if((widget->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
              prev_widget->set_pseudo_classes(prev_widget->pseudo_classes() | PseudoClasses::RIGHT_ACTIVE);
            if((prev_widget->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
              widget->set_pseudo_classes(widget->pseudo_classes() | PseudoClasses::LEFT_ACTIVE);
          } else {
            prev_widget->set_pseudo_classes(prev_widget->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_RIGHT | PseudoClasses::RIGHT_ACTIVE));
            widget->set_pseudo_classes(widget->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_LEFT | PseudoClasses::LEFT_ACTIVE));
          }
          prev_widget = widget;
        } else
          prev_widget = nullptr;
        if(prev_row_iter != widgets().end()) prev_row_iter++;
      }
    }
    // Updates sizes of widgets.
    Dimension<int> inner_area_size = area_size_to_inner_area_size(area_size);
    int row_count = widgets().size() / _M_column_count + (widgets().size() % _M_column_count > 0 ? 1 : 0);
    double float_width = numeric_limits<double>::infinity();
    double float_height = numeric_limits<double>::infinity();
    bool is_width, is_height;
    if(inner_area_size.width != numeric_limits<int>::max()) {
      float_width = (inner_area_size.width + 0.0) / _M_column_count;
      is_width = true;
    } else
      is_width = false;
    if(inner_area_size.height != numeric_limits<int>::max() && row_count > 0) {
      float_height = (inner_area_size.height + 0.0) / row_count;
      is_height = true;
    } else
      is_height = false;
    double float_x = 0.0, float_y = 0.0;
    _M_cell_size = Dimension<int>(0, 0);
    for(auto iter = widgets().begin(); iter != widgets().end(); ) {
      int height = (is_height ? floor(float_y + float_height) - floor(float_y) : numeric_limits<int>::max());
      for(int column = 0; column < _M_column_count && iter == widgets().end(); column++, iter++) {
        Widget *widget = iter->get();
        int width = (is_width ? floor(float_x + float_width) - floor(float_x) : numeric_limits<int>::max());
        if(widget->is_visible()) {
          Edges<int> widget_margin = widget->margin();
          Dimension<int> widget_area_size(width, height);
          if(widget_area_size.width != numeric_limits<int>::max())
            widget_area_size.width -= widget_margin.left + widget_margin.right;
          if(widget_area_size.height != numeric_limits<int>::max())
            widget_area_size.height -= widget_margin.top + widget_margin.bottom;
          widget_area_size.width = max(widget_area_size.width, 0);
          widget_area_size.height = max(widget_area_size.height, 0);
          // Updates widget size.
          widget->update_size(canvas, widget_area_size);
          int widget_margin_box_width = widget->bounds().width;
          int widget_margin_box_height = widget->bounds().height;
          widget_margin_box_width += widget_margin.left + widget_margin.right;
          widget_margin_box_height += widget_margin.top + widget_margin.bottom;
          _M_cell_size.width = max(_M_cell_size.width, widget_margin_box_width);
          _M_cell_size.height = max(_M_cell_size.height, widget_margin_box_height);
        }
        float_x += float_width;
      }
      float_y += float_height;
    }
    if(!is_width || !is_height) {
      _M_cell_size.height = 0;
      for(auto &widget : widgets()) {
        if(widget->is_visible()) {
          Edges<int> widget_margin = widget->margin();
          Dimension<int> widget_area_size = widget->bounds().size();
          if(!is_width && widget->can_again_update_width()) {
            widget_area_size.width = _M_cell_size.width;
            widget_area_size.width -= widget_margin.left + widget_margin.right;
            widget_area_size.width = max(widget_area_size.width, 0);
            if(widget_area_size != widget->bounds().size()) {
              // Updates widget size.
              widget->update_size(canvas, widget_area_size);
            }
          }
          int widget_margin_box_height = widget->bounds().height;
          widget_margin_box_height += widget_margin.top + widget_margin.bottom;
          _M_cell_size.height = max(_M_cell_size.height, widget_margin_box_height);
        }
      }
      for(auto &widget : widgets()) {
        if(widget->is_visible()) {
          Dimension<int> widget_area_size = widget->bounds().size();
          if(!is_height && widget->can_again_update_height()) {
            Edges<int> widget_margin = widget->margin();
            widget_area_size.height = _M_cell_size.height;
            widget_area_size.height -= widget_margin.top + widget_margin.bottom;
            if(widget_area_size != widget->bounds().size()) {
              // Updates widget size.
              widget->update_size(canvas, widget_area_size);
            }
          }
        }
      }
    }
  }

  void GridPanel::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  {
    int row_count = widgets().size() / _M_column_count + (widgets().size() % _M_column_count > 0 ? 1 : 0);
    Dimension<int> content_size;
    content_size.width = _M_cell_size.width * _M_column_count;
    content_size.height = _M_cell_size.height * row_count;
    set_content_size(content_size);
  }

  bool GridPanel::can_again_update_width() const
  { return true; }

  bool GridPanel::can_again_update_height() const
  { return true; }

  //
  // A SplitPane class.
  //

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

  void SplitPane::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds, Styles *style)
  { throw exception(); }

  //
  // A List class.
  //

  List::~List() {}

  void List::initialize(SelectionMode mode, const shared_ptr<ListAdapter> &adapter)
  {
    _M_selection_mode = mode;
    _M_selected_poses.clear();
    _M_adapter = adapter;
    _M_on_list_selection_callback.set_listener([](Widget *widget, const set<size_t> &poses) {});
  }

  void List::set_selected_poses(const set<size_t> &poses)
  {
    _M_selected_poses.clear();
    auto tmp_iter = _M_selected_poses.begin();
    auto tmp_inserter = inserter(_M_selected_poses, tmp_iter);
    copy_if(poses.begin(), poses.end(), tmp_inserter, [this](size_t pos) {
      return pos < _M_adapter->item_count();
    });
    on_list_selection(_M_selected_poses);
  }

  void List::select_all()
  {
    _M_selected_poses.clear();
    bool were_all_selected = true;
    for(size_t pos = 0; pos < _M_adapter->item_count(); pos++) {
      if(_M_selected_poses.find(pos) == _M_selected_poses.end()) {
        were_all_selected = false;
        _M_selected_poses.insert(pos);
      }
    }
    if(!were_all_selected) on_list_selection(_M_selected_poses);
  }

  void List::change_selection(const Range<size_t> &range)
  {
    size_t end_pos = min(range.end, _M_adapter->item_count());
    if(range.begin < end_pos) {
      for(size_t pos = range.begin; pos < end_pos; pos++) {
        if(_M_selected_poses.find(pos) == _M_selected_poses.end())
          _M_selected_poses.insert(pos);
        else
          _M_selected_poses.erase(pos);
      }
      on_list_selection(_M_selected_poses);
    }
  }

  void List::clear_selection()
  {
    bool was_empty = _M_selected_poses.empty();
    _M_selected_poses.clear();
    if(!was_empty) on_list_selection(_M_selected_poses);
  }

  const char *List::name() const
  { return "list"; }

  void List::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { throw exception(); }

  void List::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds, Styles *style)
  { throw exception(); }

  Viewport *List::viewport()
  { throw exception(); }

  bool List::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  { throw exception(); }

  void List::on_touch_leave(const Pointer &pointer)
  { throw exception(); }

  bool List::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  { throw exception(); }

  void List::on_list_selection(const set<size_t> &poses)
  { _M_on_list_selection_callback(this, poses); }

  //
  // A Table class.
  //

  Table::~Table() {}

  void Table::initialize(SelectionMode mode, initializer_list<string> header_labels, const shared_ptr<TableAdapter> &adapter)
  {
    _M_selection_mode = mode;
    _M_selected_poses.clear();
    _M_adapter = adapter;
    _M_on_table_selection_callback.set_listener([](Widget *widget, const set<TablePosition, TablePositionCompare> &poses) {});
  }

  void Table::set_selected_poses(const set<TablePosition, TablePositionCompare> &poses)
  {
    _M_selected_poses.clear();
    auto tmp_iter = _M_selected_poses.begin();
    auto tmp_inserter = inserter(_M_selected_poses, tmp_iter);
    copy_if(poses.begin(), poses.end(), tmp_inserter, [this](const TablePosition &pos) {
      return pos.row < _M_adapter->row_count() && pos.column < _M_adapter->column_count();
    });
    on_table_selection(_M_selected_poses);
  }

  void Table::select_all()
  {
    _M_selected_poses.clear();
    bool were_all_selected = true;
    for(size_t row = 0; row < _M_adapter->row_count(); row++) {
      for(size_t column = 0; column < _M_adapter->column_count(); column++) {
        TablePosition pos(row, column);
        if(_M_selected_poses.find(pos) == _M_selected_poses.end()) {
          were_all_selected = false;
          _M_selected_poses.insert(pos);
        }
      }
    }
    if(!were_all_selected) on_table_selection(_M_selected_poses);
  }

  void Table::change_selection(const Range<size_t> &row_range, const Range<size_t> &column_range)
  {
    size_t end_row = min(row_range.end, _M_adapter->row_count());
    size_t end_column = min(column_range.end, _M_adapter->column_count());
    if(row_range.begin < end_row && column_range.begin < end_column) {
      for(size_t row = row_range.begin; row < end_row; row++) {
        for(size_t column = column_range.begin; column < column_range.end; column++) {
          TablePosition pos(row, column);
          if(_M_selected_poses.find(pos) == _M_selected_poses.end())
            _M_selected_poses.insert(pos);
          else
            _M_selected_poses.erase(pos);
        }
      }
      on_table_selection(_M_selected_poses);
    }
  }

  void Table::clear_selection()
  {
    bool was_empty = _M_selected_poses.empty();
    _M_selected_poses.clear();
    if(!was_empty) on_table_selection(_M_selected_poses);
  }

  const char *Table::name() const
  { return "table"; }

  void Table::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { throw exception(); }

  void Table::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds, Styles *style)
  { throw exception(); }

  Viewport *Table::viewport()
  { throw exception(); }

  bool Table::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  { throw exception(); }

  void Table::on_touch_leave(const Pointer &pointer)
  { throw exception(); }

  bool Table::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  { throw exception(); }

  void Table::on_table_selection(const set<TablePosition, TablePositionCompare> &poses)
  { _M_on_table_selection_callback(this, poses); }

  //
  // A Tree class.
  //

  Tree::~Tree() {}

  void Tree::initialize(SelectionMode mode, const shared_ptr<TreeAdapter> &adapter)
  {
    _M_selection_mode = mode;
    _M_selected_paths.clear();
    _M_adapter = adapter;
    _M_on_tree_selection_callback.set_listener([](Widget *widget, const set<TreePath, TreePathCompare> &paths) {}); 
  }

  void Tree::set_selected_paths(const set<TreePath, TreePathCompare> &paths)
  {
    _M_selected_paths.clear();
    auto tmp_iter = _M_selected_paths.begin();
    auto tmp_inserter = inserter(_M_selected_paths, tmp_iter);
    copy_if(paths.begin(), paths.end(), tmp_inserter, [this](const TreePath &path) {
      return _M_adapter->has_node(path);
    });
    on_tree_selection(_M_selected_paths);
  }

  void Tree::select_all()
  {
    bool were_all_selected = true;
    for(size_t root_idx = 0; root_idx < _M_adapter->root_count(); root_idx++) {
      TreePath path({ root_idx });
      were_all_selected &= select_branch(path);
    }
    if(were_all_selected) on_tree_selection(_M_selected_paths);
  }

  void Tree::change_selection(const Range<size_t> &item_pos_range)
  {
    size_t end_pos = min(item_pos_range.begin, _M_item_count);
    if(item_pos_range.begin < end_pos) {
      size_t pos = 0;
      for(auto &extended_node : _M_extended_nodes) {
        TreePath path;
        change_selection_for_branch(item_pos_range, extended_node, path, pos);
      }
      on_tree_selection(_M_selected_paths);
    }
  }

  void Tree::clear_selection()
  {
    bool was_empty = _M_selected_paths.empty();
    _M_selected_paths.clear();
    if(!was_empty) on_tree_selection(_M_selected_paths);
  }

  bool Tree::select_branch(TreePath &path)
  {
    size_t count = _M_adapter->child_count(path);
    bool were_all_selected = true;
    if(_M_selected_paths.find(path) == _M_selected_paths.end()) {
      were_all_selected = false;
      _M_selected_paths.insert(path);
    }
    for(size_t node_idx = 0; node_idx < count; node_idx++) {
      path.nodes.push_back(node_idx);
      were_all_selected &= select_branch(path);
      path.nodes.pop_back();
    }
    return were_all_selected;
  }
  
  void Tree::change_selection_for_branch(const Range<size_t> &item_pos_range, const ExtendedNode &extended_node, TreePath &path, size_t &pos)
  {
    path.nodes.push_back(extended_node.index);
    if(pos >= item_pos_range.begin && pos < item_pos_range.end) {
      if(_M_selected_paths.find(path) == _M_selected_paths.end())
        _M_selected_paths.insert(path);
      else
        _M_selected_paths.erase(path);
    }
    pos++;
    for(auto &child : extended_node.extended_children) {
      change_selection_for_branch(item_pos_range, child, path, pos);
    }
    path.nodes.pop_back();
  }

  const char *Tree::name() const
  { return "tree"; }

  void Tree::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { throw exception(); }

  void Tree::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds, Styles *style)
  { throw exception(); }

  Viewport *Tree::viewport()
  { throw exception(); }

  bool Tree::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  { throw exception(); }

  void Tree::on_touch_leave(const Pointer &pointer)
  { throw exception(); }

  bool Tree::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  { throw exception(); }

  void Tree::on_tree_selection(const set<TreePath, TreePathCompare> &paths)
  { _M_on_tree_selection_callback(this, paths); }

  //
  // A Scroll class.
  //

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
  
  //
  // A MenuItem class.
  //

  MenuItem::~MenuItem() {}

  const char *MenuItem::name() const
  { return "menu_item"; }

  void MenuItem::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  {
    Point<int> content_point(inner_bounds.x, inner_bounds.y);
    content_point.y += (inner_bounds.height - content_size().height) / 2;
    int x_offset = 0;
    if(icon() != Icon()) {
      Point<int> icon_margin_box_point = content_point;
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

  const char *MenuItem::icon_name() const
  { return "menu_item.icon"; }

  IconSize MenuItem::icon_size() const
  { return IconSize::SMALL; }

  const char *MenuItem::label_name() const
  { return "menu_item.label"; }

  //
  // A CheckMenuItem class.
  //

  CheckMenuItem::~CheckMenuItem() {}

  const char *CheckMenuItem::name() const
  { return "check_menu_item"; }

  void CheckMenuItem::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { this->CheckBox::update_content_size(canvas, area_size); }

  void CheckMenuItem::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  { this->CheckBox::draw_content(canvas, inner_bounds); }

  const char *CheckMenuItem::icon_name() const
  { return "check_menu_item.icon"; }

  IconSize CheckMenuItem::icon_size() const
  { return this->MenuItem::icon_size(); }

  bool CheckMenuItem::can_draw_icon() const
  { return true; }
  
  const char *CheckMenuItem::label_name() const
  { return "check_menu_item.label"; }

  const char *CheckMenuItem::check_name() const
  { return "check_menu_item.check"; }

  //
  // A RadioMenuItem class.
  //

  RadioMenuItem::~RadioMenuItem() {}

  const char *RadioMenuItem::name() const
  { return "radio_menu_item"; }

  void RadioMenuItem::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { this->RadioButton::update_content_size(canvas, area_size); }

  void RadioMenuItem::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  { this->RadioButton::draw_content(canvas, inner_bounds); }

  const char *RadioMenuItem::icon_name() const
  { return "radio_menu_item.icon"; }

  IconSize RadioMenuItem::icon_size() const
  { return this->MenuItem::icon_size(); }

  bool RadioMenuItem::can_draw_icon() const
  { return true; }

  const char *RadioMenuItem::label_name() const
  { return "radio_menu_item.label"; }

  const char *RadioMenuItem::check_name() const
  { return "radio_menu_item.radio"; }

  //
  // A SeperatorMenuItem class.
  //

  SeparatorMenuItem::~SeparatorMenuItem() {}

  const char *SeparatorMenuItem::name() const
  { return "separator_menu_item"; }

  void SeparatorMenuItem::update_content_size(Canvas *canvas, const Dimension<int> &area_size) {}

  void SeparatorMenuItem::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds, Styles *style) {}

  //
  // A Menu class.
  //

  Menu::~Menu() {}

  void Menu::initialize(const Icon &icon, const string &label, initializer_list<MenuItem *> menu_items)
  {
    this->MenuItem::initialize(icon, label, [](Widget *widget) {});
    _M_menu_items.clear();
    for(auto menu_item : menu_items) {
      _M_menu_items.push_back(unique_ptr<MenuItem>(menu_item));
      set_this_as_widget_parent(menu_item);
    }
  }

  void Menu::add_menu_item(MenuItem *menu_item)
  {
    _M_menu_items.push_back(unique_ptr<MenuItem>(menu_item));
    set_this_as_widget_parent(menu_item);
  }

  bool Menu::delete_menu_item(MenuItem *menu_item)
  {
    auto iter = find_if(_M_menu_items.begin(), _M_menu_items.end(), [menu_item](const unique_ptr<MenuItem> &tmp_menu_item) {
      return tmp_menu_item.get() == menu_item;
    });
    if(iter != _M_menu_items.end()) {
      _M_menu_items.erase(iter);
      unset_this_as_widget_parent(iter->get());
      return true;
    }
    return false;
  }

  void Menu::delete_all_menu_items()
  {
    for(auto &menu_item : _M_menu_items) unset_this_as_widget_parent(menu_item.get());
    _M_menu_items.clear();
  }

  const char *Menu::name() const
  { return "menu"; }

  const char *Menu::icon_name() const
  { return "menu.icon"; }

  const char *Menu::label_name() const
  { return "menu.label"; }

  //
  // A MenuBar class.
  //

  MenuBar::~MenuBar() {}

  void MenuBar::initialize(initializer_list<Menu *> menus)
  {
    _M_menus.clear();
    for(auto menu : menus) {
      _M_menus.push_back(unique_ptr<Menu>(menu));
      set_this_as_widget_parent(menu);
    }
    _M_menu_height = 0;
  }

  void MenuBar::add_menu(Menu *menu)
  {
    _M_menus.push_back(unique_ptr<Menu>(menu));
    set_this_as_widget_parent(menu);
  }

  bool MenuBar::delete_menu(Menu *menu)
  {
    auto iter = find_if(_M_menus.begin(), _M_menus.end(), [menu](const unique_ptr<Menu> &tmp_menu) {
      return tmp_menu.get() == menu;
    });
    if(iter != _M_menus.end()) {
      _M_menus.erase(iter);
      unset_this_as_widget_parent(iter->get());
      return true;
    }
    return false;
  }

  void MenuBar::delete_all_menus()
  {
    for(auto &menu : _M_menus) unset_this_as_widget_parent(menu.get());
    _M_menus.clear();
  }

  const char *MenuBar::name() const
  { return "menu_bar"; }

  void MenuBar::update_child_points(const Rectangle<int> &area_bounds)
  {
    Rectangle<int> inner_area_bounds = area_bounds_to_inner_area_bounds(area_bounds);
    HAlignment h_align = HAlignment::LEFT;
    VAlignment v_align = VAlignment::CENTER;
    inner_area_bounds.width = numeric_limits<int>::max();
    for(auto &menu : _M_menus) {
      if(menu->is_visible()) {
        Edges<int> widget_margin = menu->margin();
        int widget_margin_box_width = menu->bounds().width + widget_margin.left + widget_margin.right;
        Rectangle<int> widget_area_bounds = inner_area_bounds;
        widget_area_bounds.width = widget_margin_box_width;
        widget_area_bounds.x += widget_margin.left;
        widget_area_bounds.y += widget_margin.top;
        if(widget_area_bounds.width != numeric_limits<int>::max())
          widget_area_bounds.width -= widget_margin.left + widget_margin.right;
        if(widget_area_bounds.height != numeric_limits<int>::max())
          widget_area_bounds.height -= widget_margin.top + widget_margin.bottom;
        widget_area_bounds.width = max(widget_area_bounds.width, 0);
        widget_area_bounds.height = max(widget_area_bounds.height, 0);
        // Updates top left point of menu.
        menu->update_point(widget_area_bounds, &h_align, &v_align);
        inner_area_bounds.x += widget_margin_box_width;
      }
    }
  }

  void MenuBar::update_child_sizes(Canvas *canvas, const Dimension<int> &area_size)
  {
    Dimension<int> inner_area_size = area_size_to_inner_area_size(area_size);
    Widget *prev_menu = nullptr;
    for(auto &menu : _M_menus) {
      if(menu->is_visible()) {
        if(prev_menu != nullptr) {
          bool is_adjacent = menu->has_adjacency_to(prev_menu);
          is_adjacent = (is_adjacent && prev_menu->v_align() == menu->v_align());
          if(is_adjacent) {
            prev_menu->set_pseudo_classes(prev_menu->pseudo_classes() | PseudoClasses::ADJACENT_TO_RIGHT);
            menu->set_pseudo_classes(menu->pseudo_classes() | PseudoClasses::ADJACENT_TO_LEFT);
            if((menu->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
              prev_menu->set_pseudo_classes(prev_menu->pseudo_classes() | PseudoClasses::RIGHT_ACTIVE);
            if((prev_menu->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
              menu->set_pseudo_classes(menu->pseudo_classes() | PseudoClasses::LEFT_ACTIVE);
          } else {
            prev_menu->set_pseudo_classes(prev_menu->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_RIGHT | PseudoClasses::RIGHT_ACTIVE));
            menu->set_pseudo_classes(menu->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_LEFT | PseudoClasses::LEFT_ACTIVE));
          }
        }
        prev_menu = menu.get();
      }
    }
    HAlignment h_align = HAlignment::LEFT;
    VAlignment v_align = VAlignment::CENTER;
    bool is_height = (inner_area_size.height == numeric_limits<int>::max());
    bool can_again_update = false;
    _M_menu_height = 0;
    for(auto &menu : _M_menus) {
      if(menu->is_visible()) {
        Edges<int> menu_margin = menu->margin();
        Dimension<int> menu_area_size;
        menu_area_size.width = inner_area_size.width;
        menu_area_size.height = inner_area_size.height;
        if(menu_area_size.width != numeric_limits<int>::max())
          menu_area_size.width -= menu_margin.left + menu_margin.right;
        if(menu_area_size.height != numeric_limits<int>::max())
          menu_area_size.height -= menu_margin.top + menu_margin.bottom;
        menu_area_size.height = max(menu_area_size.height, 0);
        // Updates menu size.
        menu->update_size(canvas, menu_area_size, &h_align, &v_align);
        if(inner_area_size.width != numeric_limits<int>::max()) {
          inner_area_size.width -= menu->bounds().width;
          inner_area_size.width -= menu_margin.left + menu_margin.right;
          inner_area_size.width = max(inner_area_size.width, 0);
        }
        int menu_margin_box_height = menu->bounds().height;
        menu_margin_box_height -= menu_margin.top + menu_margin.bottom;
        menu_margin_box_height = max(menu_margin_box_height, 0);
        _M_menu_height = max(_M_menu_height, menu_margin_box_height);
        can_again_update |= (!is_height && can_again_update_height());
      }
    }
    if(can_again_update) {
      for(auto &menu : _M_menus) {
        if(menu->is_visible()) {
          Dimension<int> menu_area_size = menu->bounds().size();
          if(!is_height && menu->can_again_update_height()) {
            Edges<int> menu_margin = menu->margin();
            menu_area_size.height = _M_menu_height;
            menu_area_size.height -= menu_margin.top + menu_margin.bottom;
            if(menu_area_size != menu->bounds().size()) {
              // Updates widget size.
              menu->update_size(canvas, menu_area_size);
            }
          }
        }
      }
    }
  }

  void MenuBar::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  {
    int content_width = accumulate(_M_menus.begin(), _M_menus.end(), 0, [](int width, const unique_ptr<Menu> &menu) {
      if(menu->is_visible()) {
        Edges<int> menu_margin = menu->margin();
        int menu_margin_box_width = menu->bounds().width;
        menu_margin_box_width += menu_margin.left + menu_margin.right;
        return width + menu_margin_box_width;
      } else
        return width;
    });
    set_content_size(Dimension<int>(content_width, _M_menu_height));
  }

  void MenuBar::draw_children(Canvas *canvas, const Rectangle<int> &inner_bounds)
  {
    for(auto &menu : _M_menus) {
      if(menu->is_visible()) {
        canvas->save();
        canvas->rect(menu->bounds().x, menu->bounds().y, menu->bounds().width, menu->bounds().height);
        canvas->clip();
        canvas->new_path();
        menu->draw(canvas);
        canvas->restore();
      }
    }
  }
}
