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
    TextBuffer *_M_buffer;
    std::uintptr_t _M_data1;
    std::uintptr_t _M_data2;

    TextByteIterator(TextBuffer *buffer, std::uintptr_t data1, std::uintptr_t data2) :
      _M_buffer(buffer), _M_data1(data1), _M_data2(data2) {}
  public:
    TextByteIterator() {}

    const char &operator*() const;

    const char *operator->() const;

    TextByteIterator &operator++();
    
    TextByteIterator &operator--();

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
    
    bool operator==(const TextByteIterator &iter) const;

    bool operator!=(const TextByteIterator &iter) const
    { return !(*this == iter); }

    bool operator<(const TextByteIterator &iter) const;

    bool operator>=(const TextByteIterator &iter) const
    { return !(*this < iter); }

    bool operator>(const TextByteIterator &iter) const
    { return iter < *this; }

    bool operator<=(const TextByteIterator &iter) const
    { return !(*this > iter); }

    friend class TextBuffer;
    friend class TextCharIterator;
    friend class TextLineIterator;
  };
  
  class TextCharIterator : public std::iterator<std::bidirectional_iterator_tag, TextByteIterator, std::ptrdiff_t, const TextByteIterator *, const TextByteIterator &>
  {
    TextBuffer *_M_buffer;
    std::uintptr_t _M_data1;
    std::uintptr_t _M_data2;

    TextCharIterator(TextBuffer *buffer, std::uintptr_t data1, std::uintptr_t data2) :
      _M_buffer(buffer), _M_data1(data1), _M_data2(data2) {}
  public:
    TextCharIterator() {}

    TextCharIterator(const TextByteIterator &iter);

    TextByteIterator operator*() const
    { return byte_iter(); }

    TextByteIterator *operator->() const
    { return nullptr; }

    TextCharIterator &operator++();

    TextCharIterator &operator--();

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

    TextByteIterator byte_iter() const
    { return TextByteIterator(_M_buffer, _M_data1, _M_data2); }

    friend class TextBuffer;
    friend class TextLineIterator;
  };

  class TextLineIterator  : public std::iterator<std::bidirectional_iterator_tag, TextCharIterator, std::ptrdiff_t, const TextCharIterator *, const TextCharIterator &>
  {
    TextBuffer *_M_buffer;
    std::uintptr_t _M_data1;
    std::uintptr_t _M_data2;

    TextLineIterator(TextBuffer *buffer, std::uintptr_t data1, std::uintptr_t data2) :
      _M_buffer(buffer), _M_data1(data1), _M_data2(data2) {}
  public:
    TextLineIterator();

    TextLineIterator(const TextByteIterator &iter);

    TextLineIterator(const TextCharIterator &iter);

    TextCharIterator operator*() const
    { return byte_iter(); }

    TextCharIterator *operator->() const
    { return nullptr; }

    TextLineIterator &operator++();

    TextLineIterator &operator--();

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

    TextByteIterator byte_iter() const
    { return TextByteIterator(_M_buffer, _M_data1, _M_data2); }

    TextCharIterator char_iter() const
    { return TextCharIterator(_M_buffer, _M_data1, _M_data2); }

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

    virtual TextPosition cursor_pos() const  = 0;

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

    virtual void set_gap_size(std::size_t gap_size) = 0;

    static std::size_t default_single_line_gap_size();

    static std::size_t default_multi_line_gap_size();
  };
}

#endif
