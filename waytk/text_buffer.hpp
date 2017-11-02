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
#ifndef _TEXT_BUFFER_HPP
#define _TEXT_BUFFER_HPP

#include <vector>
#include <waytk.hpp>

namespace waytk
{
  namespace priv
  {
    class ImplTextBuffer : public TextBuffer
    {
      std::vector<char> _M_bytes;
      std::size_t _M_gap_begin_index;
      std::size_t _M_cursor_index;
      TextPosition _M_cursor_pos;
      Range<std::size_t> _M_selection_index_range;
      std::size_t _M_char_count;
      std::size_t _M_line_count;
      std::size_t _M_gap_size;
      std::size_t _M_tab_spaces;
      bool _M_has_saved_column;
      std::size_t _M_saved_column;
    public:
      ImplTextBuffer(const std::string &text, std::size_t gap_size) :
        _M_gap_size(gap_size), _M_tab_spaces(8), _M_has_saved_column(false),
        _M_saved_column(0) { priv_set_text(text); }

      virtual ~ImplTextBuffer();

      virtual TextByteIterator byte_begin() const;

      virtual TextByteIterator byte_end() const;

      virtual TextCharIterator char_begin() const;

      virtual TextCharIterator char_end() const;

      virtual TextLineIterator line_begin() const;

      virtual TextLineIterator line_end() const;

      virtual std::string text() const;

      virtual void set_text(const std::string &text);

      virtual std::size_t byte_count() const;

      virtual std::size_t char_count() const;

      virtual std::size_t line_count() const;
    
      virtual TextCharIterator cursor_iter() const;

      virtual TextPosition cursor_pos() const;

      virtual void set_cursor_iter(const TextCharIterator &iter);

      virtual Range<TextCharIterator> selection_range() const;

      virtual void set_selection_range(const Range<TextCharIterator> &range);

      virtual void insert_string(const std::string &str);

      virtual void delete_chars(std::size_t count);

      virtual void append_string(const std::string &str);

      virtual void set_gap_size(std::size_t gap_size);
      
      virtual std::size_t tab_spaces() const;

      virtual void set_tab_spaces(std::size_t tab_spaces);
    protected:
      virtual bool has_saved_column() const;

      virtual std::size_t saved_column() const;

      virtual void set_saved_column(std::size_t column);

      virtual void unset_saved_column();
    public:
      virtual void validate_byte_iter(TextByteIterator &iter, const TextCharIterator &old_cursor_iter) const;
    protected:
      virtual const char &byte(const TextByteIterator &iter) const;

      virtual const char *byte_ptr(const TextByteIterator &iter) const;

      virtual TextByteIterator &increase_byte_iter(TextByteIterator &iter) const;

      virtual TextByteIterator &decrease_byte_iter(TextByteIterator &iter) const;

      virtual bool is_equal_to(const TextByteIterator &iter1, const TextByteIterator &iter2) const;

      virtual bool is_less_than(const TextByteIterator &iter1, const TextByteIterator &iter2) const;

      virtual TextCharIterator &increase_char_iter(TextCharIterator &iter) const;

      virtual TextCharIterator &decrease_char_iter(TextCharIterator &iter) const;

      virtual TextLineIterator &increase_line_iter(TextLineIterator &iter) const;

      virtual TextLineIterator &decrease_line_iter(TextLineIterator &iter) const;
    private:
      void priv_set_text(const std::string &text);

      void priv_append_string(const std::string &str);

      void throw_runtime_exception_for_invalid_iterator(const TextByteIterator &iter) const
      {
        if(iter.buffer() != this || byte_iter_data1(iter) > _M_bytes.size())
          throw RuntimeException("invalid text iterator");
      }

      void throw_runtime_exception_for_invalid_iterator(const TextCharIterator &iter) const
      {
        if(iter.buffer() != this || char_iter_data1(iter) > _M_bytes.size())
          throw RuntimeException("invalid text iterator");
      }
    };
  }
}

#endif
