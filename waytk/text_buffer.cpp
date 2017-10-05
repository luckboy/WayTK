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
#include "text_buffer.hpp"
#include "util.hpp"

using namespace std;

namespace waytk
{
  namespace priv
  {
    //
    // An ImplTextBuffer class.
    //

    ImplTextBuffer::~ImplTextBuffer() {}

    TextByteIterator ImplTextBuffer::byte_begin() const
    { return make_byte_iter(0, 0); }

    TextByteIterator ImplTextBuffer::byte_end() const
    { return make_byte_iter(_M_bytes.size(), 0); }

    TextCharIterator ImplTextBuffer::char_begin() const
    { return make_char_iter(0, 0); }

    TextCharIterator ImplTextBuffer::char_end() const
    { return make_char_iter(_M_bytes.size(), 0); }

    TextLineIterator ImplTextBuffer::line_begin() const
    { return make_line_iter(0, 0); }

    TextLineIterator ImplTextBuffer::line_end() const
    { return make_line_iter(_M_bytes.size(), 0); }

    string ImplTextBuffer::text() const
    { 
      string tmp_text;
      tmp_text.append(_M_bytes.begin(), _M_bytes.begin() + _M_gap_begin_index);
      tmp_text.append(_M_bytes.begin() + _M_cursor_index, _M_bytes.end());
      return tmp_text;
    }

    void ImplTextBuffer::set_text(const string &text)
    { priv_set_text(text); }
    
    size_t ImplTextBuffer::byte_count() const
    { return _M_bytes.size() - (_M_cursor_index - _M_gap_begin_index); }

    size_t ImplTextBuffer::char_count() const
    { return _M_char_count; }

    size_t ImplTextBuffer::line_count() const
    { return _M_line_count; }

    TextCharIterator ImplTextBuffer::cursor_iter() const
    { return make_char_iter(_M_cursor_index, 0); }

    TextPosition ImplTextBuffer::cursor_pos() const
    { return _M_cursor_pos; }

    void ImplTextBuffer::set_cursor_iter(const TextCharIterator &iter)
    {
      throw_runtime_exception_for_invalid_iterator(iter);
      size_t old_cursor_index = _M_cursor_index;
      size_t new_cursor_index = char_iter_data1(iter);
      // Calculates number of line and number of column.
      const TextCharIterator &new_char_iter = iter;
      TextCharIterator old_char_iter = make_char_iter(old_cursor_index, 0);
      if(new_char_iter > old_char_iter) {
        TextLineIterator new_line_iter(new_char_iter);
        TextLineIterator line_iter(old_char_iter);
        for(; line_iter < new_line_iter; line_iter++) _M_cursor_pos.line++;
        TextCharIterator char_iter = line_iter.char_iter();
        _M_cursor_pos.column = 0;
        for(; char_iter < new_char_iter; char_iter++) _M_cursor_pos.column++;
      } else if(new_char_iter < old_char_iter) {
        TextLineIterator old_line_iter(old_char_iter);
        TextLineIterator line_iter(new_char_iter);
        for(; line_iter > old_line_iter; line_iter--) _M_cursor_pos.line--;
        TextCharIterator char_iter = line_iter.char_iter();
        _M_cursor_pos.column = 0;
        for(; char_iter < new_char_iter; char_iter++) _M_cursor_pos.column++;
      }
      // Moves gap and updates selection index range.
      size_t i = _M_gap_begin_index;
      size_t j = old_cursor_index;
      if(i < j) {
        if(new_cursor_index > old_cursor_index) {
          do {
            _M_bytes[i] = _M_bytes[j];
            i++; j++;
            if(_M_selection_index_range.begin >= i && _M_selection_index_range.begin < j)
              _M_selection_index_range.begin = i;
            if(_M_selection_index_range.end >= i && _M_selection_index_range.end < j)
              _M_selection_index_range.end = i;
          } while(j < new_cursor_index);
        } else if(new_cursor_index < old_cursor_index) {
          do {
            _M_bytes[j] = _M_bytes[i];
            i--; j--;
            if(_M_selection_index_range.begin >= i && _M_selection_index_range.begin < j)
              _M_selection_index_range.begin = j;
            if(_M_selection_index_range.end >= i && _M_selection_index_range.end < j)
              _M_selection_index_range.end = j;
          } while(i > new_cursor_index);
        }
      } else
        i = j = new_cursor_index;
      // Sets cursor index.
      _M_gap_begin_index = i;
      _M_cursor_index = j;
    }

    Range<TextCharIterator> ImplTextBuffer::selection_range() const
    {
      TextCharIterator char_begin = make_char_iter(_M_selection_index_range.begin, 0);
      TextCharIterator char_end = make_char_iter(_M_selection_index_range.end, 0);
      return Range<TextCharIterator>(char_begin, char_end);
    }
    
    void ImplTextBuffer::set_selection_range(const Range<TextCharIterator> &range)
    {
      throw_runtime_exception_for_invalid_iterator(range.begin);
      throw_runtime_exception_for_invalid_iterator(range.end);
      _M_selection_index_range.begin = char_iter_data1(range.begin);
      _M_selection_index_range.end = char_iter_data1(range.end);
      if(_M_selection_index_range.begin >= _M_selection_index_range.end)
        _M_selection_index_range.begin = _M_selection_index_range.end = 0;
    }

    void ImplTextBuffer::insert_string(const string &str)
    {
      auto iter = str.begin();
      while(iter != str.end()) {
        char char_bytes[MAX_NORMALIZED_UTF8_CHAR_LENGTH];
        size_t input_char_length, output_char_length;
        normalize_utf8_char(iter, str.end(), char_bytes, input_char_length, output_char_length);
        for(size_t i = 0; i < output_char_length; i++) {
          if(_M_gap_begin_index >= _M_cursor_index) {
            _M_bytes.resize(_M_bytes.size() + _M_gap_size + 1);
            copy_backward(_M_bytes.begin() + _M_gap_begin_index, _M_bytes.end() - (_M_gap_size + 1), _M_bytes.begin() + _M_gap_begin_index + (_M_gap_size + 1));
            if(_M_selection_index_range.begin >= _M_gap_begin_index)
              _M_selection_index_range.begin += _M_gap_size + 1;
            if(_M_selection_index_range.end >= _M_gap_begin_index)
              _M_selection_index_range.end += _M_gap_size + 1;
          }
          _M_bytes[_M_gap_begin_index] = char_bytes[i];
          _M_gap_begin_index++;
          if(char_bytes[i] == '\n') {
            _M_cursor_pos.line++;
            _M_cursor_pos.column = 0;
            _M_line_count++;
          } else
            _M_cursor_pos.column++;
          _M_char_count++;
        }
        iter += input_char_length;
      }
    }

    void ImplTextBuffer::delete_chars(size_t count)
    {
      for(size_t i = 0; i < count && _M_cursor_index < _M_bytes.size(); i++) {
        size_t char_length = current_utf8_char_length(_M_bytes.begin() + _M_cursor_index, _M_bytes.end());
        if(_M_bytes[_M_cursor_index] == '\n') _M_line_count--;
        _M_cursor_index += char_length;
        _M_char_count--;
      }
    }

    void ImplTextBuffer::append_string(const string &str)
    { priv_append_string(str); }

    void ImplTextBuffer::set_gap_size(size_t gap_size)
    { _M_gap_size = gap_size; }

    const char &ImplTextBuffer::byte(const TextByteIterator &iter) const
    {
      uintptr_t index = byte_iter_data1(iter) == _M_gap_begin_index ? _M_cursor_index : byte_iter_data1(iter);
      return _M_bytes[index];
    }

    const char *ImplTextBuffer::byte_ptr(const TextByteIterator &iter) const
    {
      uintptr_t index = byte_iter_data1(iter) == _M_gap_begin_index ? _M_cursor_index : byte_iter_data1(iter);
      return &(_M_bytes[index]);
    }

    TextByteIterator &ImplTextBuffer::increase_byte_iter(TextByteIterator &iter) const
    {
      if(byte_iter_data1(iter) < _M_bytes.size()) {
        byte_iter_data1(iter)++;
        if(byte_iter_data1(iter) == _M_gap_begin_index)
          byte_iter_data1(iter) = _M_cursor_index;
      }
      return iter;
    }

    TextByteIterator &ImplTextBuffer::decrease_byte_iter(TextByteIterator &iter) const
    {
      if(byte_iter_data1(iter) > 0) {
        if(byte_iter_data1(iter) == _M_cursor_index)
          byte_iter_data1(iter) = _M_gap_begin_index;
        byte_iter_data1(iter)--;
      }
      return iter;
    }

    bool ImplTextBuffer::is_equal_to(const TextByteIterator &iter1, const TextByteIterator &iter2) const
    {
      if(iter1.buffer() == iter2.buffer()) {
        uintptr_t index1 = byte_iter_data1(iter1) == _M_gap_begin_index ? _M_cursor_index : byte_iter_data1(iter1);
        uintptr_t index2 = byte_iter_data1(iter2) == _M_gap_begin_index ? _M_cursor_index : byte_iter_data1(iter1);
        return index1 == index2;
      } else
        return false;
    }

    bool ImplTextBuffer::is_less_than(const TextByteIterator &iter1, const TextByteIterator &iter2) const
    {
      if(iter1.buffer() == iter2.buffer()) {
        uintptr_t index1 = byte_iter_data1(iter1) == _M_gap_begin_index ? _M_cursor_index : byte_iter_data1(iter1);
        uintptr_t index2 = byte_iter_data1(iter2) == _M_gap_begin_index ? _M_cursor_index : byte_iter_data1(iter1);
        return index1 < index2;
      } else
        return iter1.buffer() < iter2.buffer();
    }

    TextCharIterator &ImplTextBuffer::increase_char_iter(TextCharIterator &iter) const
    {
      if(char_iter_data1(iter) < _M_bytes.size()) {
        size_t char_length = current_utf8_char_length(_M_bytes.begin() + char_iter_data1(iter), _M_bytes.end());
        char_iter_data1(iter) += char_length;
        if(char_iter_data1(iter) == _M_gap_begin_index)
          char_iter_data1(iter) = _M_cursor_index;
      }
      return iter;
    }

    TextCharIterator &ImplTextBuffer::decrease_char_iter(TextCharIterator &iter) const
    {
      if(char_iter_data1(iter) > 0) {
        if(char_iter_data1(iter) == _M_cursor_index)
          char_iter_data1(iter) = _M_gap_begin_index;
        size_t char_length = previous_utf8_char_length(_M_bytes.begin() + char_iter_data1(iter), _M_bytes.begin());
        char_iter_data1(iter) -= char_length;
      }
      return iter;
    }

    void ImplTextBuffer::priv_set_text(const string &text)
    {
      _M_bytes.clear();
      _M_bytes.resize(_M_gap_size);
      _M_gap_begin_index = 0;
      _M_cursor_index = _M_gap_size;
      _M_cursor_pos.line = 0;
      _M_cursor_pos.column = 0;
      _M_selection_index_range.begin = 0;
      _M_selection_index_range.end = 0;
      _M_char_count = 0;
      _M_line_count = 0;
      priv_append_string(text);
    }

    void ImplTextBuffer::priv_append_string(const string &str)
    {
      for(auto iter = str.begin(); iter != str.end(); ) {
        char char_bytes[MAX_NORMALIZED_UTF8_CHAR_LENGTH];
        size_t input_char_length, output_char_length;
        normalize_utf8_char(iter, str.end(), char_bytes, input_char_length, output_char_length);
        for(size_t i = 0; i < output_char_length; i++) {
          _M_bytes.push_back(char_bytes[i]);
          if(char_bytes[i] == '\n') _M_line_count++;
          _M_char_count++;
        }
        iter += input_char_length;
      }
    }
  }

  //
  // A TextCharIterator class.
  //

  TextCharIterator::TextCharIterator(const TextByteIterator &iter)
  {
    TextByteIterator tmp_iter = priv::first_utf8_char_byte_iter(iter, iter._M_buffer->byte_begin());
    _M_buffer = tmp_iter._M_buffer;
    _M_data1 = tmp_iter._M_data1;
    _M_data2 = tmp_iter._M_data2;
  }

  //
  // A TextLineIterator class.
  //

  TextLineIterator::TextLineIterator(const TextByteIterator &iter)
  { intialize(TextCharIterator(iter)); }

  TextLineIterator::TextLineIterator(const TextCharIterator &iter)
  { intialize(iter); }

  void TextLineIterator::intialize(const TextCharIterator &iter)
  {
    TextCharIterator tmp_iter = iter;
    while(tmp_iter > tmp_iter.buffer()->char_begin()) {
      tmp_iter--;
      if(**tmp_iter == '\n') {
        tmp_iter++;
        break;
      }
    }
    _M_buffer = tmp_iter._M_buffer;
    _M_data1 = tmp_iter._M_data1;
    _M_data2 = tmp_iter._M_data2;
  }

  //
  // A TextBuffer class.
  //

  TextBuffer::~TextBuffer() {}

  TextCharIterator &TextBuffer::increase_char_iter(TextCharIterator &iter) const
  {
    size_t char_length = priv::current_utf8_char_length(iter.byte_iter(), byte_end());
    for(size_t i = 0; i < char_length; i++) iter++;
    return iter;
  }

  TextCharIterator &TextBuffer::decrease_char_iter(TextCharIterator &iter) const
  {
    size_t char_length = priv::previous_utf8_char_length(iter.byte_iter(), byte_begin());
    for(size_t i = 0; i < char_length; i++) iter--;
    return iter;
  }

  TextLineIterator &TextBuffer::increase_line_iter(TextLineIterator &iter) const
  {
    TextCharIterator char_iter = iter.char_iter();
    for(; char_iter < char_end() && **char_iter != '\n'; char_iter++);
    char_iter++;
    iter = TextLineIterator(char_iter);
    return iter;
  }

  TextLineIterator &TextBuffer::decrease_line_iter(TextLineIterator &iter) const
  {
    TextCharIterator char_iter = iter.char_iter();
    while(char_iter > char_begin()) {
      char_iter--;
      if(**char_iter == '\n') break;
    }
    iter = TextLineIterator(char_iter);
    return iter;
  }

  size_t TextBuffer::default_single_line_gap_size()
  { return 0; }

  size_t TextBuffer::default_multi_line_gap_size()
  { return 16384; }
}
