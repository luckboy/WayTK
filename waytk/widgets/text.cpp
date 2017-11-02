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
#include <cstring>
#include <limits>
#include "text_buffer.hpp"
#include "util.hpp"

using namespace std;

namespace waytk
{
  namespace
  {
    void get_utf8(const TextCharIterator &iter, const TextCharIterator &end, char *buf)
    {
      auto tmp_iter = iter;
      if(tmp_iter < end) tmp_iter++;
      size_t i = 0;
      for(auto tmp_iter2 = *iter; tmp_iter2 != *tmp_iter; tmp_iter2++, i++) {
        buf[i] = *tmp_iter2;
      }
      buf[i] = 0;
    }
  }

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
    _M_has_font = false;
    _M_font_name.clear();
    _M_font_slant = FontSlant::NORMAL;
    _M_font_weight = FontWeight::NORMAL;
    _M_has_font_size = false;
    _M_font_size = 0;
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
    if(count > 0 || !str.empty()) {
      Range<TextCharIterator> range(_M_buffer->char_begin(), _M_buffer->char_end());
      on_text_change(range);
    }
  }

  void Text::delete_chars(size_t count)
  { 
    _M_buffer->delete_chars(count);
    if(count > 0) {
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

  void Text::set_font(const string &name, FontSlant slant, FontWeight weight)
  {
    _M_has_font = true;
    _M_font_name = name;
    _M_font_slant = slant;
    _M_font_weight = weight;
  }

  void Text::unset_font()
  {
    _M_has_font = false;
    _M_font_name.clear();
    _M_font_slant = FontSlant::NORMAL;
    _M_font_weight = FontWeight::NORMAL;
  }
  
  void Text::set_font_size(int size)
  {
    _M_has_font_size = true;
    _M_font_size = size;
  }

  void Text::unset_font_size()
  {
    _M_has_font_size = false;
    _M_font_size = 0;
  }

  const char *Text::name() const
  { return _M_input_type == InputType::MULTI_LINE ? "text.multi_line" : "text.single_line"; }

  void Text::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  {
    Point<int> content_point = inner_bounds.point();
    content_point.x += (inner_bounds.width - content_size().width) / 2;
    content_point.y += (inner_bounds.height - content_size().height) / 2;
    Color selected_background_color = styles()->background_color(pseudo_classes() | PseudoClasses::SELECTED);
    Color selected_foreground_color = styles()->foreground_color(pseudo_classes() | PseudoClasses::SELECTED);
    TextPoint old_point(0, 0, 0);
    size_t old_column = 0;
    bool was_line_break = false;
    size_t color_index = _M_first_visible_color_index;
    canvas->save();
    canvas->rect(content_point.x, content_point.y, content_size().width, content_size().height);
    canvas->clip();
    canvas->new_path();
    for_text(canvas, _M_first_visible_iter,
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      if(iter == _M_buffer->char_end()) {
        if(_M_is_editable && iter == cursor_iter()) {
          int font_height = ceil(font_metrics.height);
          int y = point.y_line * font_height + point.y_offset + _M_visible_point.y;
          int width = ceil(text_metrics.x_advance);
          Rectangle<int> rect(content_point.x + point.x + _M_visible_point.x, content_point.y + y, width, font_height);
          draw_cursor(canvas, rect);
        }
        return make_pair(false, true);
      }
      if(!is_line_break) {
        int font_height = ceil(font_metrics.height);
        int y = point.y_line * font_height + point.y_offset + _M_visible_point.y;
        return make_pair(_M_input_type != InputType::MULTI_LINE || y < content_size().height, false);
      }
      return make_pair(true, false);
    }, 
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      char buf[priv::MAX_NORMALIZED_UTF8_CHAR_LENGTH + 1];
      if(_M_input_type == InputType::PASSWORD)
        strcpy(buf, "\342\227\217"); // Black circle.
      else
        get_utf8(iter, _M_buffer->char_end(), buf);
      Color color;
      Point<int> tmp_point;
      int font_height = ceil(font_metrics.height);
      tmp_point.x = content_point.x + point.x + _M_visible_point.x;
      tmp_point.y = content_point.y + point.y_line * font_height + point.y_offset + _M_visible_point.y;
      int width;
      if(old_point.y_line != point.y_line) {
        auto tmp_iter = iter;
        if(iter <= _M_buffer->char_begin()) tmp_iter--;
        if(tmp_iter >= selection_range().begin && tmp_iter < selection_range().end) {
          char buf2[priv::MAX_NORMALIZED_UTF8_CHAR_LENGTH + 1];
          get_utf8(tmp_iter, _M_buffer->char_end(), buf2);
          if(*buf2 == '\n' || was_line_break || (iter >= selection_range().begin && iter < selection_range().end)) {
            Point<int> tmp_point2;
            tmp_point2.x = content_point.x + old_point.x + _M_visible_point.x;
            tmp_point2.y = content_point.y + old_point.y_line * font_height + point.y_offset + _M_visible_point.y;
            if(*buf2 != '\n' && !was_line_break) {
              TextMetrics text_metrics2;
              canvas->get_text_matrics(buf2, text_metrics2);
              if(*buf2 == '\t')
                tmp_point2.x += ceil(text_metrics2.x_advance) * (tab_spaces() - old_column % tab_spaces());
              else
                tmp_point2.x += ceil(text_metrics2.x_advance);
            }
            Dimension<int> tmp_size2;
            tmp_size2.width = content_size().width - (tmp_point2.x - content_point.x);
            tmp_size2.height = ceil(font_metrics.height);
            if(tmp_size2.width > 0) {
              canvas->set_color(selected_background_color);
              canvas->rect(tmp_point2.x, tmp_point2.y, tmp_size2.width, tmp_size2.height);
              canvas->fill();
            }
          }
        }
      }
      if(_M_input_type != InputType::MULTI_LINE || (*buf != '\n' && !is_line_break)) {
        const char *str = buf;
        if(iter >= selection_range().begin && iter < selection_range().end) {
          Dimension<int> tmp_size;
          if(*buf == '\t') {
            tmp_size.width = ceil(text_metrics.x_advance) * (tab_spaces() - column % tab_spaces());
          } else 
            tmp_size.width = ceil(text_metrics.x_advance);
          tmp_size.height = font_height;
          canvas->set_color(selected_background_color);
          canvas->rect(tmp_point.x, tmp_point.y, tmp_size.width, tmp_size.height);
          canvas->fill();
          color = selected_foreground_color;
        } else {
          if(_M_input_type == InputType::PASSWORD)
            color = styles()->foreground_color(pseudo_classes());
          else
            color = foreground_color(color_index);
        }
        canvas->set_color(color);
        canvas->move_to(tmp_point.x, tmp_point.y + font_metrics.ascent);
        if(*buf == '\t') {
          size_t count = tab_spaces() - column % tab_spaces();
          for(size_t i = 0; i < count; i++) {
            canvas->show_text(str);
          }
        } else
          canvas->show_text(str);
        width = ceil(text_metrics.x_advance);
      } else
        width = ceil(text_metrics.x_advance);
      if(_M_is_editable && iter == cursor_iter()) {
        Rectangle<int> rect(tmp_point.x, tmp_point.y, width, font_height);
        draw_cursor(canvas, rect);
      }
      old_point = point;
      old_column = column;
      was_line_break = is_line_break;
      color_index++;
    });
    canvas->restore();
  }

  Viewport *Text::viewport()
  { throw exception(); }

  bool Text::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  { throw exception(); }

  void Text::on_touch_leave(const Pointer &pointer)
  { throw exception(); }

  bool Text::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  {
    this->Widget::on_key(key_sym, modifiers, utf8, state);
    Canvas *canvas =  surface().lock()->canvas();
    switch(key_sym) {
      case XKB_KEY_Home:
      case XKB_KEY_KP_Home:
        if(_M_is_editable) {
          auto old_cursor_iter = cursor_iter();
          update_cursor_iter_for_home(canvas);
          _M_buffer->validate_char_iter(_M_first_visible_iter, old_cursor_iter);
          auto tmp_old_cursor_iter = old_cursor_iter;
          _M_buffer->validate_char_iter(tmp_old_cursor_iter, old_cursor_iter);
          update_selection_range(tmp_old_cursor_iter, (modifiers & Modifiers::SHIFT) != Modifiers::NONE);
          update_visible_point_for_move_left(canvas);
          return true;
        }
        break;
      case XKB_KEY_Left:
      case XKB_KEY_KP_Left:
        if(_M_is_editable) {
          auto old_cursor_iter = cursor_iter();
          update_cursor_iter_for_move_left_right(canvas, -1);
          _M_buffer->validate_char_iter(_M_first_visible_iter, old_cursor_iter);
          auto tmp_old_cursor_iter = old_cursor_iter;
          _M_buffer->validate_char_iter(tmp_old_cursor_iter, old_cursor_iter);
          update_selection_range(tmp_old_cursor_iter, (modifiers & Modifiers::SHIFT) != Modifiers::NONE);
          if(_M_input_type == InputType::MULTI_LINE)
            update_first_visible_iter_for_move_up(canvas);
          update_visible_point_for_move_left(canvas);
          return true;
        }
        break;
      case XKB_KEY_Up:
      case XKB_KEY_KP_Up: 
        if(_M_is_editable) {
          if(_M_input_type == InputType::MULTI_LINE) {
            auto old_cursor_iter = cursor_iter();
            update_cursor_iter_for_move_up_down(canvas, -1);
            _M_buffer->validate_char_iter(_M_first_visible_iter, old_cursor_iter);
            auto tmp_old_cursor_iter = old_cursor_iter;
            _M_buffer->validate_char_iter(tmp_old_cursor_iter, old_cursor_iter);
            update_selection_range(tmp_old_cursor_iter, (modifiers & Modifiers::SHIFT) != Modifiers::NONE);
            update_first_visible_iter_for_move_up(canvas);
            update_visible_point_for_move_right(canvas);
          }
          return true;
        }
        break;
      case XKB_KEY_Right:
      case XKB_KEY_KP_Right:
        if(_M_is_editable) {
          auto old_cursor_iter = cursor_iter();
          update_cursor_iter_for_move_left_right(canvas, -1);
          _M_buffer->validate_char_iter(_M_first_visible_iter, old_cursor_iter);
          auto tmp_old_cursor_iter = old_cursor_iter;
          _M_buffer->validate_char_iter(tmp_old_cursor_iter, old_cursor_iter);
          update_selection_range(tmp_old_cursor_iter, (modifiers & Modifiers::SHIFT) != Modifiers::NONE);
          if(_M_input_type == InputType::MULTI_LINE)
            update_first_visible_iter_for_move_down(canvas);
          update_visible_point_for_move_right(canvas);
          return true;
        }
        break;
      case XKB_KEY_Down:
      case XKB_KEY_KP_Down:
        if(_M_is_editable) {
          if(_M_input_type == InputType::MULTI_LINE) {
            auto old_cursor_iter = cursor_iter();
            update_cursor_iter_for_move_up_down(canvas, 1);
            _M_buffer->validate_char_iter(_M_first_visible_iter, old_cursor_iter);
            auto tmp_old_cursor_iter = old_cursor_iter;
            _M_buffer->validate_char_iter(tmp_old_cursor_iter, old_cursor_iter);
            update_selection_range(tmp_old_cursor_iter, (modifiers & Modifiers::SHIFT) != Modifiers::NONE);
            update_first_visible_iter_for_move_down(canvas);
            update_visible_point_for_move_right(canvas);
          }
          return true;
        }
        break;
      case XKB_KEY_Page_Up:
      case XKB_KEY_KP_Page_Up:
        if(_M_is_editable) {
          if(_M_input_type == InputType::MULTI_LINE) {
            canvas->save();
            if(_M_has_font) canvas->set_font_face(_M_font_name, _M_font_slant, _M_font_weight);
            if(_M_has_font_size) canvas->set_font_size(_M_font_size);
            FontMetrics font_metrics;
            canvas->get_font_matrics(font_metrics);
            int font_height = ceil(font_metrics.height);
            long y_line = content_size().height / font_height;
            if(y_line == 0) y_line++;
            canvas->restore();
            auto old_cursor_iter = cursor_iter();
            long old_cursor_y_line = cursor_y_line(canvas);
            update_cursor_iter_for_move_up_down(canvas, -y_line);
            _M_buffer->validate_char_iter(_M_first_visible_iter, old_cursor_iter);
            auto tmp_old_cursor_iter = old_cursor_iter;
            _M_buffer->validate_char_iter(tmp_old_cursor_iter, old_cursor_iter);
            update_selection_range(tmp_old_cursor_iter, (modifiers & Modifiers::SHIFT) != Modifiers::NONE);
            update_first_visible_iter_for_page_up_down(canvas, old_cursor_y_line);
            update_visible_point_for_move_right(canvas);
          }
          return true;
        }
        break;
      case XKB_KEY_Page_Down:
      case XKB_KEY_KP_Page_Down:
        if(_M_is_editable) {
          if(_M_input_type == InputType::MULTI_LINE) {
            canvas->save();
            if(_M_has_font) canvas->set_font_face(_M_font_name, _M_font_slant, _M_font_weight);
            if(_M_has_font_size) canvas->set_font_size(_M_font_size);
            FontMetrics font_metrics;
            canvas->get_font_matrics(font_metrics);
            int font_height = ceil(font_metrics.height);
            long y_line = content_size().height / font_height;
            if(y_line == 0) y_line++;
            canvas->restore();
            auto old_cursor_iter = cursor_iter();
            long old_cursor_y_line = cursor_y_line(canvas);
            update_cursor_iter_for_move_up_down(canvas, y_line);
            _M_buffer->validate_char_iter(_M_first_visible_iter, old_cursor_iter);
            auto tmp_old_cursor_iter = old_cursor_iter;
            _M_buffer->validate_char_iter(tmp_old_cursor_iter, old_cursor_iter);
            update_selection_range(tmp_old_cursor_iter, (modifiers & Modifiers::SHIFT) != Modifiers::NONE);
            update_first_visible_iter_for_page_up_down(canvas, old_cursor_y_line);
            update_visible_point_for_move_right(canvas);
          }
          return true;
        }
        break;
      case XKB_KEY_End:
      case XKB_KEY_KP_End:
        if(_M_is_editable) {
          auto old_cursor_iter = cursor_iter();
          update_cursor_iter_for_end(canvas);
          _M_buffer->validate_char_iter(_M_first_visible_iter, old_cursor_iter);
          auto tmp_old_cursor_iter = old_cursor_iter;
          _M_buffer->validate_char_iter(tmp_old_cursor_iter, old_cursor_iter);
          update_selection_range(tmp_old_cursor_iter, (modifiers & Modifiers::SHIFT) != Modifiers::NONE);
          update_visible_point_for_move_right(canvas);
          return true;
        }
        break;
      default:
        throw exception();
    }
    return false;
  }

  void Text::on_text_change(const Range<TextCharIterator> &range)
  { _M_on_text_change_callback(this, range); }

  void Text::on_cursor_change(const TextCharIterator &iter, const TextPosition &pos)
  { _M_on_cursor_change_callback(this, iter, pos); }

  void Text::on_text_selection(const Range<TextCharIterator> &range)
  { _M_on_text_selection_callback(this, range); }

  Color Text::foreground_color(size_t pos)
  {
    if(!_M_has_foreground_color) {
      _M_foreground_color = styles()->foreground_color(pseudo_classes());
      _M_has_foreground_color = true;
    }
    return _M_foreground_color;
  }

  TextDimension Text::for_text(Canvas *canvas, const TextCharIterator &first_iter, const function<pair<bool, bool> (const FontMetrics &, const TextMetrics &, const TextCharIterator &, const TextPoint &, size_t, bool)> &cond_fun, const function<void (const FontMetrics &, const TextMetrics &, const TextCharIterator &, const TextPoint &, size_t, bool)> &iter_fun)
  {
    canvas->save();
    if(_M_has_font) canvas->set_font_face(_M_font_name, _M_font_slant, _M_font_weight);
    if(_M_has_font_size) canvas->set_font_size(_M_font_size);
    FontMetrics font_metrics;
    canvas->get_font_matrics(font_metrics);
    TextDimension size = for_text(canvas, first_iter, font_metrics, cond_fun, iter_fun);
    canvas->restore();
    return size;
  }

  TextDimension Text::for_text(Canvas *canvas, const TextCharIterator &first_iter, const FontMetrics &font_metrics, const function<pair<bool, bool> (const FontMetrics &, const TextMetrics &, const TextCharIterator &, const TextPoint &, size_t, bool)> &cond_fun, const function<void (const FontMetrics &, const TextMetrics &, const TextCharIterator &, const TextPoint &, size_t, bool)> &iter_fun)
  {
    TextPoint point(0, 0, 0);
    int max_line_width = 0;
    size_t column = 0;
    pair<bool, bool> tmp_pair;
    if(_M_input_type == InputType::MULTI_LINE) {
      for(auto iter = first_iter; true; iter++) {
        char buf[priv::MAX_NORMALIZED_UTF8_CHAR_LENGTH + 1];
        get_utf8(iter, _M_buffer->char_end(), buf);
        TextMetrics text_metrics;
        if(_M_has_line_wrap && _M_has_word_wrap) {
          if(*buf != '\n') {
            int width;
            if(*buf == '\t') {
              canvas->get_text_matrics(" ", text_metrics);
              width = ceil(text_metrics.x_advance) * (tab_spaces() - column % tab_spaces());
            } else {
              canvas->get_text_matrics(buf, text_metrics);
              width = ceil(text_metrics.x_advance);
            }
            if(content_size().width < point.x + width && column >= 1) {
              max_line_width = max(max_line_width, point.x);
              point.y_line++;
              point.x = 0;
              column = 0;
            }
          }
        }
        bool is_line_break = false;
        if(_M_has_line_wrap && !_M_has_word_wrap) {
          if(*buf == ' ' || *buf == '\t') {
            int width;
            if(*buf == '\t') {
              canvas->get_text_matrics(" ", text_metrics);
              width = ceil(text_metrics.x_advance) * (tab_spaces() - column % tab_spaces());
            } else {
              canvas->get_text_matrics(buf, text_metrics);
              width = ceil(text_metrics.x_advance);
            }
            auto iter2 = iter;
            for(iter2++; iter2 != _M_buffer->char_end(); iter2++) {
              char buf2[priv::MAX_NORMALIZED_UTF8_CHAR_LENGTH + 1];
              get_utf8(iter2, _M_buffer->char_end(), buf2);
              if(*buf2 == ' ' || *buf2 == '\t' || *buf2 == '\n') break;
              TextMetrics text_metrics;
              canvas->get_text_matrics(buf, text_metrics);
              width += ceil(text_metrics.x_advance);
              if(content_size().width < point.x + width) {
                is_line_break = true;
                break;
              }
            }
          }
        }
        if(iter != _M_buffer->char_end() && *buf != '\n') {
          if(*buf == '\t')
            canvas->get_text_matrics(" ", text_metrics);
          else
            canvas->get_text_matrics(buf, text_metrics);
        } else
          canvas->get_text_matrics("a", text_metrics);
        // Condition.
        tmp_pair = cond_fun(font_metrics, text_metrics, iter, point, column, is_line_break);
        if(!tmp_pair.first) break;
        // Iteration.
        iter_fun(font_metrics, text_metrics, iter, point, column, is_line_break);
        if(*buf =='\n' || is_line_break) {
          TextMetrics text_metrics2;
          canvas->get_text_matrics("a", text_metrics2);
          int tmp_width = ceil(text_metrics2.x_advance);
          max_line_width = max(max_line_width, point.x + tmp_width);
          point.y_line++;
          point.x = 0;
          column = 0;
        } else if(*buf == '\t') {
          point.x += ceil(text_metrics.x_advance) * (tab_spaces() - column % tab_spaces());
          column += tab_spaces() - column % tab_spaces();
        } else {
          point.x += ceil(text_metrics.x_advance);
          column++;
        }
      }
    } else {
      for(auto iter = first_iter; true; iter++) {
        char buf[priv::MAX_NORMALIZED_UTF8_CHAR_LENGTH + 1];
        if(_M_input_type == InputType::PASSWORD) {
          if(iter != _M_buffer->char_end())
            strcpy(buf, "\342\227\217"); // Black circle.
          else
            *buf = 0;
        } else
          get_utf8(iter, _M_buffer->char_end(), buf);
        TextMetrics text_metrics;
        if(iter != _M_buffer->char_end()) {
          if(*buf == '\t')
            canvas->get_text_matrics(" ", text_metrics);
          else
            canvas->get_text_matrics(buf, text_metrics);
        }
        // Condition.
        tmp_pair = cond_fun(font_metrics, text_metrics, iter, point, column, false);
        if(!tmp_pair.first) break;
        // Iteration.
        iter_fun(font_metrics, text_metrics, iter, point, column, false);
        if(*buf == '\t') {
          point.x += ceil(text_metrics.x_advance) * (tab_spaces() - column % tab_spaces());
          column += tab_spaces() - column % tab_spaces();
        } else {
          point.x += ceil(text_metrics.x_advance);
          column++;
        }
      }
    }
    if(tmp_pair.second) {
      TextMetrics text_metrics2;
      canvas->get_text_matrics("a", text_metrics2);
      int tmp_width = ceil(text_metrics2.x_advance);
      max_line_width = max(max_line_width, point.x + tmp_width);
      point.y_line++;
    }
    return TextDimension(max_line_width, point.y_line + 1, point.y_offset);
  }

  TextDimension Text::for_text_backward(Canvas *canvas, const TextCharIterator &first_iter, const function<pair<bool, bool> (const FontMetrics &, const TextMetrics &, const TextCharIterator &, const TextPoint &, size_t, bool)> &cond_fun, const function<void (const FontMetrics &, const TextMetrics &, const TextCharIterator &, const TextPoint &, size_t, bool)> &iter_fun)
  {
    canvas->save();
    if(_M_has_font) canvas->set_font_face(_M_font_name, _M_font_slant, _M_font_weight);
    if(_M_has_font_size) canvas->set_font_size(_M_font_size);
    FontMetrics font_metrics;
    canvas->get_font_matrics(font_metrics);
    TextDimension size = for_text_backward(canvas, first_iter, font_metrics, cond_fun, iter_fun);
    canvas->restore();
    return size;
  }

  TextDimension Text::for_text_backward(Canvas *canvas, const TextCharIterator &first_iter, const FontMetrics &font_metrics, const function<pair<bool, bool> (const FontMetrics &, const TextMetrics &, const TextCharIterator &, const TextPoint &, size_t, bool)> &cond_fun, const function<void (const FontMetrics &, const TextMetrics &, const TextCharIterator &, const TextPoint &, size_t, bool)> &iter_fun)
  {
    if(_M_input_type == InputType::MULTI_LINE) {
      TextPoint point(0, 0, 0);
      TextDimension size(0, 0, 0);
      TextLineIterator line_iter(first_iter);
      bool can_decrease_line_iter = true;
      if(line_iter.char_iter() != first_iter) {
        can_decrease_line_iter = false;
      } else {
        if(line_iter <= _M_buffer->line_begin()) return TextDimension(0, 0, 0);
      }
      do {
        if(can_decrease_line_iter) line_iter--;
        can_decrease_line_iter = true;
        TextDimension tmp_size = for_text(canvas, line_iter.char_iter(), font_metrics,
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &tmp_point, size_t column, bool is_line_break) {
          if(iter == _M_buffer->char_end()) return make_pair(false, true);
          if(iter == first_iter) return make_pair(false, false);
          if(**iter == '\n') return make_pair(false, false);
          return make_pair(true, false);
        },
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &tmp_point, size_t column, bool is_line_break) {
        });
        point.y_line -= tmp_size.height_line;
        pair<bool, bool> tmp_pair(true, false);
        for_text(canvas, line_iter.char_iter(), font_metrics,
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &tmp_point, size_t column, bool is_line_break) {
          if(iter == _M_buffer->char_end()) return make_pair(false, true);
          if(iter == first_iter) return make_pair(false, false);
          TextPoint tmp_point2 = tmp_point;
          tmp_point2.y_line += point.y_line;
          tmp_pair = cond_fun(font_metrics, text_metrics, iter, tmp_point2, column, is_line_break);
          if(tmp_pair.first)
            iter_fun(font_metrics, text_metrics, iter, tmp_point2, column, is_line_break);
          else
            return tmp_pair;
          if(**iter == '\n') return make_pair(false, false);
          return tmp_pair;
        },
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &tmp_point, size_t column, bool is_line_break) {
        });
        size.width = max(size.width, tmp_size.width);
        size.height_line += tmp_size.height_line;
        size.height_offset += tmp_size.height_offset;
        if(!tmp_pair.first) break;
      } while(line_iter > _M_buffer->line_begin());
      return size;
    } else
      return TextDimension(0, 0, 0);
  }
  
  size_t Text::cursor_column(Canvas *canvas)
  { return iter_column(canvas, _M_buffer->cursor_iter()); }

  size_t Text::iter_column(Canvas *canvas, TextCharIterator iter)
  {
    TextLineIterator line_iter(iter);
    size_t tmp_iter_column;
    for_text(canvas, line_iter.char_iter(),
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &tmp_iter, const TextPoint &point, size_t column, bool is_line_break) {
      if(tmp_iter == iter) {
        tmp_iter_column = column;
        return make_pair(false, false); 
      }
      return make_pair(true, false);
    },
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &tmp_iter, const TextPoint &point, size_t column, bool is_line_break) {
    });
    return tmp_iter_column;
  }

  void Text::update_cursor_iter_for_home(Canvas *canvas)
  {
    auto new_cursor_iter = align_to_line(canvas, cursor_iter());
    size_t tmp_saved_column = iter_column(canvas, new_cursor_iter);
    set_cursor_iter(new_cursor_iter);
    _M_buffer->set_saved_column(tmp_saved_column);
  }
  
  void Text::update_cursor_iter_for_move_up_down(Canvas *canvas, long y_line)
  {
    TextCharIterator new_cursor_iter;
    if(y_line < 0) {
      TextLineIterator line_iter(_M_buffer->cursor_iter());
      long tmp_y_line = 0;
      TextDimension size = for_text(canvas, line_iter.char_iter(),
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        if(iter == _M_buffer->char_end()) return make_pair(false, true);
        if(**iter == '\n') return make_pair(false, false);
        return make_pair(true, false);
      },
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        if(iter == _M_buffer->cursor_iter()) tmp_y_line = point.y_line;
      });
      tmp_y_line = size.height_line - tmp_y_line - 1;
      line_iter++;
      new_cursor_iter = _M_buffer->char_begin();
      for_text_backward(canvas, line_iter.char_iter(),
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        if(point.y_line + tmp_y_line + 1 == y_line) {
          new_cursor_iter = iter;
          return make_pair(false, false);
        }
        return make_pair(true, false);
      },
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      });
    } else if(y_line > 0) {
      TextLineIterator line_iter(_M_buffer->cursor_iter());
      long tmp_y_line = 0;
      for_text(canvas, line_iter.char_iter(),
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        if(iter == _M_buffer->cursor_iter()) {
          tmp_y_line = point.y_line;
          make_pair(false, false);
        }
        return make_pair(true, false);
      },
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      });
      TextPoint old_point;
      for_text(canvas, line_iter.char_iter(),
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        if(iter == _M_buffer->char_end()) return make_pair(false, true);
        if(point.y_line - tmp_y_line == y_line) {
          new_cursor_iter = iter;
          return make_pair(false, false);
        }
        return make_pair(true, false);
      },
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        if(old_point.y_line != point.y_line) new_cursor_iter = iter;
        old_point = point;
      });
    }
    if(y_line != 0) {
      size_t tmp_cursor_column;
      if(_M_buffer->has_saved_column())
        tmp_cursor_column = _M_buffer->saved_column();
      else
        tmp_cursor_column = cursor_column(canvas);
      _M_buffer->set_saved_column(tmp_cursor_column);
      TextPoint old_point;
      TextCharIterator old_iter;
      for_text(canvas, new_cursor_iter,
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        if(column == tmp_cursor_column) {
          new_cursor_iter = iter;
          return make_pair(false, false);
        }
        if(**iter == '\n' || is_line_break) {
          new_cursor_iter = iter;
          return make_pair(false, false);
        }
        if(old_point.y_line != point.y_line) {
          new_cursor_iter = old_iter;
          return make_pair(false, false);
        }
        return make_pair(true, false);
      },
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        old_point = point;
        old_iter = iter;
      });
    } else
      new_cursor_iter = _M_buffer->cursor_iter();
    size_t tmp_saved_column = _M_buffer->saved_column();
    set_cursor_iter(new_cursor_iter);
    _M_buffer->set_saved_column(tmp_saved_column);
  }
  
  void Text::update_cursor_iter_for_move_left_right(Canvas *canvas, int x)
  {
    auto new_cursor_iter = _M_buffer->cursor_iter();
    if(x < 0) {
      for(int i = 0; i < -x && new_cursor_iter > _M_buffer->char_begin(); i++) {
        new_cursor_iter--;
      }
    } else if(x > 0) {
      for(int i = 0; i < x && new_cursor_iter < _M_buffer->char_end(); i++) {
        new_cursor_iter++;
      }
    }
    size_t tmp_saved_column = iter_column(canvas, new_cursor_iter);
    set_cursor_iter(new_cursor_iter);
    _M_buffer->set_saved_column(tmp_saved_column);
  }

  void Text::update_cursor_iter_for_end(Canvas *canvas)
  {
    TextCharIterator new_cursor_iter;
    if(_M_input_type == InputType::MULTI_LINE) {
      TextLineIterator line_iter(cursor_iter());
      TextPoint old_point(0, 0, 0);
      TextCharIterator old_iter;
      bool is_cursor = false;
      for_text(canvas, line_iter.char_iter(),
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        if(iter == _M_buffer->char_end()) {
          new_cursor_iter = iter;
          return make_pair(false, true);
        }
        if(**iter == '\n') {
          new_cursor_iter = iter;
          return make_pair(false, false);
        }
        if(is_cursor && old_point.y_line != point.y_line) {
          new_cursor_iter = old_iter;
          return make_pair(false, false);
        }
        return make_pair(true, false);
      },
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        if(iter == cursor_iter()) is_cursor = true;
        old_point = point;
        old_iter = iter;
      });
    } else
      new_cursor_iter = _M_buffer->char_end();
    size_t tmp_saved_column = iter_column(canvas, new_cursor_iter);
    set_cursor_iter(new_cursor_iter);
    _M_buffer->set_saved_column(tmp_saved_column);
  }
  
  void Text::update_selection_range(const TextCharIterator &old_cursor_iter, bool is_shift)
  {
    if(is_shift) {
      if(old_cursor_iter == selection_range().begin) {
        if(cursor_iter() > selection_range().end)
          _M_buffer->set_selection_range(selection_range().end, cursor_iter());
        else
          _M_buffer->set_selection_range(cursor_iter(), selection_range().end);
      } else if(old_cursor_iter == selection_range().end) {
        if(cursor_iter() < selection_range().begin)
          _M_buffer->set_selection_range(cursor_iter(), selection_range().begin);
        else
          _M_buffer->set_selection_range(selection_range().begin, cursor_iter());
      } else {
        if(old_cursor_iter < cursor_iter())
          _M_buffer->set_selection_range(old_cursor_iter, cursor_iter());
        else
          _M_buffer->set_selection_range(cursor_iter(), old_cursor_iter);
      }
    } else
      _M_buffer->set_selection_range(cursor_iter(), cursor_iter());
  }

  TextCharIterator Text::align_to_line(Canvas *canvas, const TextCharIterator &iter)
  {
    if(_M_input_type == InputType::MULTI_LINE) {
      TextLineIterator line_iter(iter);
      TextPoint old_point(0, 0, 0);
      TextCharIterator char_iter;
      bool is_first_char = true;
      for_text(canvas, line_iter.char_iter(),
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &tmp_iter, const TextPoint &point, size_t column, bool is_line_break) {
        if(tmp_iter == iter) return make_pair(false, false);
        if(tmp_iter == _M_buffer->char_end()) return make_pair(false, true);
        if(**tmp_iter == '\n') return make_pair(false, false);
        return make_pair(true, false);
      },
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &tmp_iter, const TextPoint &point, size_t column, bool is_line_break) {
        if(is_first_char || old_point.y_line != point.y_line) char_iter = iter;
        old_point = point;
        is_first_char = false;
      });
      return char_iter;
    } else
      return _M_buffer->char_begin();
  }

  void Text::draw_cursor(Canvas *canvas, const Rectangle<int> &rect)
  {
    canvas->set_color(styles()->foreground_color(pseudo_classes()));
    if(_M_has_insert_mode) {
      Color color = styles()->foreground_color(pseudo_classes());
      Color tmp_color(color.red(), color.green(), color.blue(), color.alpha() / 2);
      canvas->set_color(tmp_color);
      canvas->rect(rect.x, rect.y, rect.width, rect.height);
    } else {
      canvas->set_color(styles()->foreground_color(pseudo_classes()));
      canvas->rect(rect.x, rect.y, 2, rect.height);
    }
    canvas->fill();
  }

  void Text::update_first_visible_iter(Canvas *canvas)
  {
    _M_first_visible_color_index = 0;
    for_text(canvas, _M_buffer->char_begin(),
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      if(iter == _M_buffer->char_end() || _M_view_point.y_line == point.y_line) {
        _M_first_visible_iter = iter;
        _M_visible_point.x = -_M_view_point.x;
        _M_visible_point.y = -_M_view_point.y_offset;
        return make_pair(false, iter == _M_buffer->char_end());
      } else
        return make_pair(true, false);
    },
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      _M_first_visible_color_index++;
    });
  }

  void Text::update_first_visible_iter_for_move_up(Canvas *canvas)
  {
    bool is_cursor = false;
    bool is_cursor_in_first_line = false;
    for_text(canvas, _M_first_visible_iter,
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      if(iter == _M_buffer->char_end()) return make_pair(false, true);
      int font_height = ceil(font_metrics.height);
      int y = point.y_line * font_height + point.y_offset + _M_visible_point.y;
      return make_pair(y < content_size().height, false);
    },
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      if(cursor_iter() == iter) {
        is_cursor = true;
        if(point.y_line == 0) is_cursor_in_first_line = true;
      }
    });
    if(!is_cursor) {
      auto cursor_line_iter = align_to_line(canvas, cursor_iter());
      if(cursor_iter() < _M_first_visible_iter) {
        TextCharIterator old_iter = _M_first_visible_iter;
        for_text_backward(canvas, _M_first_visible_iter,
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
          if(iter == cursor_line_iter) {
            _M_view_point.y_line += point.y_line;
            _M_first_visible_iter = iter;
            return make_pair(false, false);
          }
          return make_pair(true, false);
        },
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
          if(old_iter < iter) {
            _M_first_visible_color_index++;
          } else if(old_iter > iter) {
            for(auto tmp_iter = iter; tmp_iter < old_iter; tmp_iter++) {
              _M_first_visible_color_index--;
            }
          }
        });
      } else if(cursor_iter() > _M_first_visible_iter) {
        for_text(canvas, _M_first_visible_iter,
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
          if(iter == cursor_line_iter) {
            _M_view_point.y_line += point.y_line;
            _M_first_visible_iter = iter;
            return make_pair(false, false);
          }
          int font_height = ceil(font_metrics.height);
          long height_line = _M_client_size.height_line - (content_size().height + font_height - 1) / font_height;
          if(point.y_line + _M_view_point.y_line < height_line) {
            _M_view_point.y_line += point.y_line;
            _M_first_visible_iter = iter;
            return make_pair(false, false);
          }
          return make_pair(true, false);
        },
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
          _M_first_visible_color_index++;
        });
      }
    }
    if(!is_cursor || is_cursor_in_first_line) { 
      _M_view_point.y_offset = 0;
      _M_visible_point.y = 0;
    }
  }

  void Text::update_first_visible_iter_for_move_down(Canvas *canvas)
  {
    bool is_cursor = false;
    bool is_cursor_in_last_line = false;
    for_text(canvas, _M_first_visible_iter,
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      if(iter == _M_buffer->char_end()) return make_pair(false, true);
      int font_height = ceil(font_metrics.height);
      int y = point.y_line * font_height + point.y_offset + _M_visible_point.y;
      return make_pair(y < content_size().height, false);
    },
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      if(cursor_iter() == iter) {
        is_cursor = true;
        int font_height = ceil(font_metrics.height);
        if(point.y_line == (content_size().height + font_height - 1) / font_height)
          is_cursor_in_last_line = true;
      }
    });
    if(!is_cursor) {
      auto new_cursor_line_iter = align_to_line(canvas, cursor_iter());
      TextCharIterator top_line_iter = new_cursor_line_iter;
      canvas->save();
      if(_M_has_font) canvas->set_font_face(_M_font_name, _M_font_slant, _M_font_weight);
      if(_M_has_font_size) canvas->set_font_size(_M_font_size);
      FontMetrics font_metrics;
      canvas->get_font_matrics(font_metrics);
      int font_height = ceil(font_metrics.height);
      if((content_size().height + font_height - 1) / font_height > 1) {
        bool is_top_line_iter = false;
        for_text_backward(canvas, new_cursor_line_iter,
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
          if(-point.y_line == (content_size().height + font_height - 1) / font_height - 1) {
            is_top_line_iter = true;
            top_line_iter = iter;
            return make_pair(false, false);
          }
          return make_pair(true, false);
        },
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        });
        if(!is_top_line_iter) top_line_iter = _M_buffer->char_begin();
      }
      canvas->restore();
      if(cursor_iter() < _M_first_visible_iter) {
        TextCharIterator old_iter = _M_first_visible_iter;
        for_text_backward(canvas, _M_first_visible_iter,
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
          if(iter == top_line_iter) {
            _M_view_point.y_line += point.y_line;
            _M_first_visible_iter = iter;
            return make_pair(false, false);
          }
          return make_pair(true, false);
        },
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
          if(old_iter < iter) {
            _M_first_visible_color_index++;
          } else if(old_iter > iter) {
            for(auto tmp_iter = iter; tmp_iter < old_iter; tmp_iter++) {
              _M_first_visible_color_index--;
            }
          }
        });
      } else if(cursor_iter() > _M_first_visible_iter) {
        for_text(canvas, _M_first_visible_iter,
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
          if(iter == top_line_iter) {
            _M_view_point.y_line += point.y_line;
            _M_first_visible_iter = iter;
            return make_pair(false, false);
          }
          int font_height = ceil(font_metrics.height);
          long height_line = _M_client_size.height_line - (content_size().height + font_height - 1) / font_height;
          if(point.y_line + _M_view_point.y_line < height_line) {
            _M_view_point.y_line += point.y_line;
            _M_first_visible_iter = iter;
            return make_pair(false, false);
          }
          return make_pair(true, false);
        },
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
          _M_first_visible_color_index++;
        });
      }
    }
    if(!is_cursor || is_cursor_in_last_line) {
      canvas->save();
      if(_M_has_font) canvas->set_font_face(_M_font_name, _M_font_slant, _M_font_weight);
      if(_M_has_font_size) canvas->set_font_size(_M_font_size);
      FontMetrics font_metrics;
      canvas->get_font_matrics(font_metrics);
      int font_height = ceil(font_metrics.height);
      _M_view_point.y_offset = font_height - content_size().height % font_height;
      if(_M_view_point.y_offset == font_height) _M_view_point.y_offset = 0;
      _M_visible_point.y = -_M_view_point.y_offset;
      canvas->restore();
    }
  }

  long Text::cursor_y_line(Canvas *canvas)
  {
    bool is_cursor = false;
    long y_line = 0;
    for_text(canvas, _M_first_visible_iter,
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      if(iter == _M_buffer->char_end()) return make_pair(true, true);
      int font_height = ceil(font_metrics.height);
      int y = point.y_line * font_height + point.y_offset + _M_visible_point.y;
      return make_pair(y < content_size().height, false);
    },
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      if(cursor_iter() == iter) {
        is_cursor = true;
        y_line = point.y_line;
      }
    });
    if(!is_cursor) {
      canvas->save();
      if(_M_has_font) canvas->set_font_face(_M_font_name, _M_font_slant, _M_font_weight);
      if(_M_has_font_size) canvas->set_font_size(_M_font_size);
      FontMetrics font_metrics;
      canvas->get_font_matrics(font_metrics);
      int font_height = ceil(font_metrics.height);
      y_line = (content_size().height / font_height) / 2;
      if(y_line == 0) y_line++;
      canvas->restore();
    }
    return y_line;
  }
  
  void Text::update_first_visible_iter_for_page_up_down(Canvas *canvas, long old_cursor_y_line)
  {
    bool is_cursor = false;
    for_text(canvas, _M_first_visible_iter,
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      if(iter == _M_buffer->char_end()) return make_pair(false, true);
      int font_height = ceil(font_metrics.height);
      int y = point.y_line * font_height + point.y_offset + _M_visible_point.y;
      return make_pair(y < content_size().height, false);
    },
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      if(cursor_iter() == iter) is_cursor = true;
    });
    if(!is_cursor) {
      auto cursor_line_iter = align_to_line(canvas, cursor_iter());
      TextCharIterator top_line_iter = cursor_line_iter;
      bool is_top_line_iter = false;
      bool is_top_line_iter2 = false;
      if(old_cursor_y_line > 1) {
        for_text_backward(canvas, cursor_line_iter,
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
          if(-point.y_line == old_cursor_y_line - 1) {
            is_top_line_iter = true;
            top_line_iter = iter;
            return make_pair(false, false);
          }
          return make_pair(true, false);
        },
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        });
        if(!is_top_line_iter) top_line_iter = _M_buffer->char_begin();
      } else
        is_top_line_iter = true;
      if(cursor_iter() < _M_first_visible_iter) {
        TextCharIterator old_iter = _M_first_visible_iter;
        for_text_backward(canvas, _M_first_visible_iter,
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
          if(iter == top_line_iter) {
            _M_view_point.y_line += point.y_line;
            _M_first_visible_iter = iter;
            return make_pair(false, false);
          }
          return make_pair(true, false);
        },
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
          if(old_iter < iter) {
            _M_first_visible_color_index++;
          } else if(old_iter > iter) {
            for(auto tmp_iter = iter; tmp_iter < old_iter; tmp_iter++) {
              _M_first_visible_color_index--;
            }
          }
        });
        is_top_line_iter2 = true;
      } else if(cursor_iter() > _M_first_visible_iter) {
        for_text(canvas, _M_first_visible_iter,
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
          if(iter == top_line_iter) {
            _M_view_point.y_line += point.y_line;
            _M_first_visible_iter = iter;
            is_top_line_iter2 = true;
            return make_pair(false, false);
          }
          int font_height = ceil(font_metrics.height);
          long height_line = _M_client_size.height_line - (content_size().height + font_height - 1) / font_height;
          if(point.y_line + _M_view_point.y_line < height_line) {
            _M_view_point.y_line += point.y_line;
            _M_first_visible_iter = iter;
            return make_pair(false, false);
          }
          return make_pair(true, false);
        },
        [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
          _M_first_visible_color_index++;
        });
      }
      if(!is_top_line_iter) {
        _M_view_point.y_offset = 0;
        _M_visible_point.y = 0;
      } else if(!is_top_line_iter2) {
        canvas->save();
        if(_M_has_font) canvas->set_font_face(_M_font_name, _M_font_slant, _M_font_weight);
        if(_M_has_font_size) canvas->set_font_size(_M_font_size);
        FontMetrics font_metrics;
        canvas->get_font_matrics(font_metrics);
        int font_height = ceil(font_metrics.height);
        _M_view_point.y_offset = font_height - content_size().height % font_height;
        if(_M_view_point.y_offset == font_height) _M_view_point.y_offset = 0;
        _M_visible_point.y = -_M_view_point.y_offset;
        canvas->restore();
      }
    }
  }

  void Text::update_visible_point_for_move_left(Canvas *canvas)
  {
    auto cursor_line_iter = align_to_line(canvas, cursor_iter());
    TextPoint old_point(0, 0, 0);
    bool is_cursor = false;
    for_text(canvas, cursor_line_iter,
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      if(iter == _M_buffer->char_end()) {
        if(iter == cursor_iter()) {
          if(point.x + _M_visible_point.x >= 0 && point.x + _M_visible_point.x < content_size().width)
            is_cursor = true;
        }
        return make_pair(false, true);
      }
      if(old_point.y_line != point.y_line) return make_pair(false, false);
      return make_pair(true, false);
    },
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      if(iter == cursor_iter()) {
        if(point.x + _M_visible_point.x >= 0 && point.x + _M_visible_point.x < content_size().width)
          is_cursor = true;
      }
      old_point = point;
    });
    if(!is_cursor) {
      for_text(canvas, cursor_line_iter,
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        if(point.x + content_size().width >= _M_client_size.width) {
          _M_view_point.x = _M_client_size.width - content_size().width;
          if(_M_view_point.x < 0) _M_view_point.x = 0;
          _M_visible_point.x = -_M_view_point.x;
          return make_pair(false, true);
        }
        if(iter == cursor_iter()) {
          _M_view_point.x = point.x;
          _M_visible_point.x = -_M_view_point.x;
          return make_pair(false, true);
        }
        if(iter == _M_buffer->char_end()) {
          _M_view_point.x = point.x;
          _M_visible_point.x = -_M_view_point.x;
          return make_pair(false, true);
        }
        if(old_point.y_line != point.y_line) {
          _M_view_point.x = point.x;
          _M_visible_point.x = -_M_view_point.x;
          return make_pair(false, false);
        }
        return make_pair(true, false);
      },
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        old_point = point;
      });
    }
  }

  void Text::update_visible_point_for_move_right(Canvas *canvas)
  {
    auto cursor_line_iter = align_to_line(canvas, cursor_iter());
    TextPoint old_point(0, 0, 0);
    bool is_cursor = false;
    for_text(canvas, cursor_line_iter,
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      if(iter == _M_buffer->char_end()) {
        if(iter == cursor_iter()) {
          if(point.x + _M_visible_point.x >= 0 && point.x + _M_visible_point.x < content_size().width)
            is_cursor = true;
        }
        return make_pair(false, true);
      }
      if(old_point.y_line != point.y_line) return make_pair(false, false);
      return make_pair(true, false);
    },
    [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
      if(iter == cursor_iter()) {
        if(point.x + _M_visible_point.x >= 0 && point.x + _M_visible_point.x < content_size().width)
          is_cursor = true;
      }
      old_point = point;
    });
    if(!is_cursor) {
      for_text(canvas, cursor_line_iter,
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        TextMetrics text_metrics2;
        canvas->get_text_matrics("a", text_metrics2);
        int tmp_width = ceil(text_metrics2.x_advance);
        if(point.x + tmp_width >= _M_client_size.width) {
          _M_view_point.x = _M_client_size.width - tmp_width;
          _M_view_point.x -= content_size().width - tmp_width;
          if(_M_view_point.x < 0) _M_view_point.x = 0;
          _M_visible_point.x = -_M_view_point.x;
          return make_pair(false, true);
        }
        if(iter == cursor_iter()) {
          _M_view_point.x = point.x - (content_size().width - tmp_width);
          if(_M_view_point.x < 0) _M_view_point.x = 0;
          _M_visible_point.x = -_M_view_point.x;
          return make_pair(false, true);
        }
        if(iter == _M_buffer->char_end()) {
          _M_view_point.x = point.x;
          _M_visible_point.x = -_M_view_point.x;
          return make_pair(false, true);
        }
        if(old_point.y_line != point.y_line) {
          _M_view_point.x = point.x;
          _M_visible_point.x = -_M_view_point.x;
          return make_pair(false, false);
        }
        return make_pair(true, false);
      },
      [&](const FontMetrics &font_metrics, const TextMetrics &text_metrics, const TextCharIterator &iter, const TextPoint &point, size_t column, bool is_line_break) {
        old_point = point;
      });
    }
  }
}
