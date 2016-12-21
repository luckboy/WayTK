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
#include "text_buffer.hpp"

using namespace std;

namespace waytk
{
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

  Color Text::foreground_color(size_t pos)
  {
    if(!_M_has_foreground_color) {
      _M_foreground_color = styles()->foreground_color(pseudo_classes());
      _M_has_foreground_color = true;
    }
    return _M_foreground_color;
  }
}
