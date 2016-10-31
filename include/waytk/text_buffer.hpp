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
#ifndef _WAYTK_TEXT_BUFFER_HPP
#define _WAYTK_TEXT_BUFFER_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <waytk/structs.hpp>

namespace waytk
{
  class TextBuffer;
  class TextCharIterator;
  class TextLineIterator;

  class TextByteIterator : public std::iterator<std::bidirectional_iterator_tag, char, std::ptrdiff_t, const char *, const char &>
  {
    const TextBuffer *_M_buffer;
    std::uintptr_t _M_data1;
    std::uintptr_t _M_data2;

    TextByteIterator(const TextBuffer *buffer, std::uintptr_t data1, std::uintptr_t data2) :
      _M_buffer(buffer), _M_data1(data1), _M_data2(data2) {}
  public:
    TextByteIterator() :
      _M_buffer(nullptr) {}

    inline const char &operator*() const;

    inline const char *operator->() const;

    inline TextByteIterator &operator++();
    
    inline TextByteIterator &operator--();

    TextByteIterator operator++(int)
    {
      TextByteIterator tmp(*this);
      ++(*this);
      return tmp;
    }

    TextByteIterator operator--(int)
    {
      TextByteIterator tmp(*this);
      --(*this);
      return tmp;
    }
    
    inline bool operator==(const TextByteIterator &iter) const;

    bool operator!=(const TextByteIterator &iter) const
    { return !(*this == iter); }

    inline bool operator<(const TextByteIterator &iter) const;

    bool operator>=(const TextByteIterator &iter) const
    { return !(*this < iter); }

    bool operator>(const TextByteIterator &iter) const
    { return iter < *this; }

    bool operator<=(const TextByteIterator &iter) const
    { return !(*this > iter); }

    const TextBuffer *buffer() const
    { return _M_buffer; }

    friend class TextBuffer;
    friend class TextCharIterator;
    friend class TextLineIterator;
  };
  
  class TextCharIterator : public std::iterator<std::bidirectional_iterator_tag, TextByteIterator, std::ptrdiff_t, const TextByteIterator *, const TextByteIterator &>
  {
    const TextBuffer *_M_buffer;
    std::uintptr_t _M_data1;
    std::uintptr_t _M_data2;

    TextCharIterator(const TextBuffer *buffer, std::uintptr_t data1, std::uintptr_t data2) :
      _M_buffer(buffer), _M_data1(data1), _M_data2(data2) {}
  public:
    TextCharIterator() :
      _M_buffer(nullptr) {}

    explicit TextCharIterator(const TextByteIterator &iter);

    TextByteIterator operator*() const
    { return byte_iter(); }

    TextByteIterator *operator->() const
    { return nullptr; }

    inline TextCharIterator &operator++();

    inline TextCharIterator &operator--();

    TextCharIterator operator++(int)
    {
      TextCharIterator tmp(*this);
      ++(*this);
      return tmp;
    }

    TextCharIterator operator--(int)
    {
      TextCharIterator tmp(*this);
      --(*this);
      return tmp;
    }

    bool operator==(const TextCharIterator &iter) const
    { return byte_iter() == iter.byte_iter(); }

    bool operator!=(const TextCharIterator &iter) const
    { return !(*this == iter); }

    bool operator<(const TextCharIterator &iter) const
    { return byte_iter() < iter.byte_iter(); }

    bool operator>=(const TextCharIterator &iter) const
    { return !(*this < iter); }

    bool operator>(const TextCharIterator &iter) const
    { return iter < *this; }

    bool operator<=(const TextCharIterator &iter) const
    { return !(*this > iter); }

    const TextBuffer *buffer() const
    { return _M_buffer; }

    TextByteIterator byte_iter() const
    { return TextByteIterator(_M_buffer, _M_data1, _M_data2); }

    friend class TextBuffer;
    friend class TextLineIterator;
  };

  class TextLineIterator  : public std::iterator<std::bidirectional_iterator_tag, TextCharIterator, std::ptrdiff_t, const TextCharIterator *, const TextCharIterator &>
  {
    const TextBuffer *_M_buffer;
    std::uintptr_t _M_data1;
    std::uintptr_t _M_data2;

    TextLineIterator(const TextBuffer *buffer, std::uintptr_t data1, std::uintptr_t data2) :
      _M_buffer(buffer), _M_data1(data1), _M_data2(data2) {}
  public:
    TextLineIterator() :
      _M_buffer(nullptr) {}

    explicit TextLineIterator(const TextByteIterator &iter);

    explicit TextLineIterator(const TextCharIterator &iter);

    TextCharIterator operator*() const
    { return char_iter(); }

    TextCharIterator *operator->() const
    { return nullptr; }

    inline TextLineIterator &operator++();

    inline TextLineIterator &operator--();

    TextLineIterator operator++(int)
    {
      TextLineIterator tmp(*this);
      ++(*this);
      return tmp;
    }

    TextLineIterator operator--(int)
    {
      TextLineIterator tmp(*this);
      --(*this);
      return tmp;
    }

    bool operator==(const TextLineIterator &iter) const
    { return byte_iter() == iter.byte_iter(); }

    bool operator!=(const TextLineIterator &iter) const
    { return !(*this == iter); }

    bool operator<(const TextLineIterator &iter) const
    { return byte_iter() < iter.byte_iter(); }

    bool operator>=(const TextLineIterator &iter) const
    { return !(*this < iter); }

    bool operator>(const TextLineIterator &iter) const
    { return iter < *this; }

    bool operator<=(const TextLineIterator &iter) const
    { return !(*this > iter); }

    const TextBuffer *buffer() const
    { return _M_buffer; }

    TextByteIterator byte_iter() const
    { return TextByteIterator(_M_buffer, _M_data1, _M_data2); }

    TextCharIterator char_iter() const
    { return TextCharIterator(_M_buffer, _M_data1, _M_data2); }
  private:
    void intialize(const TextCharIterator &iter);

    friend class TextBuffer;
  };

  class TextBuffer
  {
  protected:
    TextBuffer() {}
  public:
    virtual ~TextBuffer();

    virtual TextByteIterator byte_begin() const = 0;

    virtual TextByteIterator byte_end() const = 0;

    virtual TextCharIterator char_begin() const = 0;

    virtual TextCharIterator char_end() const = 0;

    virtual TextLineIterator line_begin() const = 0;
    
    virtual TextLineIterator line_end() const = 0;

    TextByteIterator begin() const
    { return byte_begin(); }
 
    TextByteIterator end() const
    { return byte_end(); }

    virtual std::string text() const = 0;

    void set_text(const char *text)
    { set_text(std::string(text)); }

    virtual void set_text(const std::string &text) = 0;

    std::size_t length() const
    { return char_count(); }
    
    virtual std::size_t byte_count() const  = 0;

    virtual std::size_t char_count() const = 0;

    virtual std::size_t line_count() const = 0;

    virtual TextCharIterator cursor_iter() const = 0;

    virtual TextPosition cursor_pos() const = 0;

    virtual void set_cursor_iter(const TextCharIterator &iter) = 0;

    virtual Range<TextCharIterator> selection_range() const = 0;

    void set_selection_range(const TextCharIterator &begin, const TextCharIterator &end)
    { set_selection_range(Range<TextCharIterator>(begin, end)); }

    virtual void set_selection_range(const Range<TextCharIterator> &range) = 0;

    std::string selected_text() const
    {
      Range<TextCharIterator> range = selection_range();
      return std::string(range.begin.byte_iter(), range.end.byte_iter());
    }

    void insert_string(const char *str)
    { insert_string(std::string(str)); }

    virtual void insert_string(const std::string &str) = 0;

    virtual void delete_chars(std::size_t count) = 0;

    virtual void append_string(const std::string &str) = 0;
    
    virtual void set_gap_size(std::size_t gap_size) = 0;
    
    static std::size_t default_single_line_gap_size();

    static std::size_t default_multi_line_gap_size();
  protected:
    virtual const char &byte(const TextByteIterator &iter) const = 0;

    virtual const char *byte_ptr(const TextByteIterator &iter) const = 0;

    virtual TextByteIterator &increase_byte_iter(TextByteIterator &iter) const = 0;

    virtual TextByteIterator &decrease_byte_iter(TextByteIterator &iter) const = 0;

    virtual bool is_equal_to(const TextByteIterator &iter1, const TextByteIterator &iter2) const = 0;

    virtual bool is_less_than(const TextByteIterator &iter1, const TextByteIterator &iter2) const = 0;

    virtual TextCharIterator &increase_char_iter(TextCharIterator &iter) const;

    virtual TextCharIterator &decrease_char_iter(TextCharIterator &iter) const;

    virtual TextLineIterator &increase_line_iter(TextLineIterator &iter) const;

    virtual TextLineIterator &decrease_line_iter(TextLineIterator &iter) const;

    TextByteIterator make_byte_iter(std::uintptr_t data1, std::uintptr_t data2) const
    { return TextByteIterator(this, data1, data2); }

    std::uintptr_t byte_iter_data1(const TextByteIterator &iter) const
    { return iter._M_data1; }

    std::uintptr_t &byte_iter_data1(TextByteIterator &iter) const
    { return iter._M_data1; }

    std::uintptr_t byte_iter_data2(const TextByteIterator &iter) const
    { return iter._M_data2; }

    std::uintptr_t &byte_iter_data2(TextByteIterator &iter) const
    { return iter._M_data2; }

    TextCharIterator make_char_iter(std::uintptr_t data1, std::uintptr_t data2) const
    { return TextCharIterator(this, data1, data2); }

    std::uintptr_t char_iter_data1(const TextCharIterator &iter) const
    { return iter._M_data1; }

    std::uintptr_t &char_iter_data1(TextCharIterator &iter) const
    { return iter._M_data1; }

    std::uintptr_t char_iter_data2(const TextCharIterator &iter) const
    { return iter._M_data2; }

    std::uintptr_t &char_iter_data2(TextCharIterator &iter) const
    { return iter._M_data2; }

    TextLineIterator make_line_iter(std::uintptr_t data1, std::uintptr_t data2) const
    { return TextLineIterator(this, data1, data2); }

    std::uintptr_t line_iter_data1(const TextLineIterator &iter) const
    { return iter._M_data1; }

    std::uintptr_t &line_iter_data1(TextLineIterator &iter) const
    { return iter._M_data1; }

    std::uintptr_t line_iter_data2(const TextLineIterator &iter) const
    { return iter._M_data2; }

    std::uintptr_t &line_iter_data2(TextLineIterator &iter) const
    { return iter._M_data2; }

    friend class TextByteIterator;
    friend class TextCharIterator;
    friend class TextLineIterator;
  };

  inline const char &TextByteIterator::operator*() const
  { return _M_buffer->byte(*this); }

  inline const char *TextByteIterator::operator->() const
  { return _M_buffer->byte_ptr(*this); }

  inline TextByteIterator &TextByteIterator::operator++()
  { return _M_buffer->increase_byte_iter(*this); }

  inline TextByteIterator &TextByteIterator::operator--()
  { return _M_buffer->decrease_byte_iter(*this); }

  inline bool TextByteIterator::operator==(const TextByteIterator &iter) const
  { return _M_buffer->is_equal_to(*this, iter); }

  inline bool TextByteIterator::operator<(const TextByteIterator &iter) const
  { return _M_buffer->is_less_than(*this, iter); }

  inline TextCharIterator &TextCharIterator::operator++()
  { return _M_buffer->increase_char_iter(*this); }

  inline TextCharIterator &TextCharIterator::operator--()
  { return _M_buffer->decrease_char_iter(*this); }

  inline TextLineIterator &TextLineIterator::operator++()
  { return _M_buffer->increase_line_iter(*this); }

  inline TextLineIterator &TextLineIterator::operator--()
  { return _M_buffer->decrease_line_iter(*this); }
}

#endif
