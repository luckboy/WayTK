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
#ifndef _WAYTK_TEXT_BUFFER_HPP
#define _WAYTK_TEXT_BUFFER_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <waytk/structs.hpp>

namespace waytk
{
  class Text;
  class TextBuffer;
  class TextCharIterator;
  class TextLineIterator;

  ///
  /// A class of iterator of text bytes.
  ///
  class TextByteIterator : public std::iterator<std::bidirectional_iterator_tag, char, std::ptrdiff_t, const char *, const char &>
  {
    const TextBuffer *_M_buffer;
    std::uintptr_t _M_data1;
    std::uintptr_t _M_data2;

    TextByteIterator(const TextBuffer *buffer, std::uintptr_t data1, std::uintptr_t data2) :
      _M_buffer(buffer), _M_data1(data1), _M_data2(data2) {}
  public:
    /// Default Constructor.
    TextByteIterator() :
      _M_buffer(nullptr) {}

    /// Dereference operator.
    inline const char &operator*() const;

    /// Member access operator.
    inline const char *operator->() const;

    /// Increases and returns the iterator.
    inline TextByteIterator &operator++();
    
    /// Decreases and returns the iterator.
    inline TextByteIterator &operator--();

    /// Increases the iterator and returns a previous value of the iterator.
    TextByteIterator operator++(int)
    {
      TextByteIterator tmp(*this);
      ++(*this);
      return tmp;
    }

    /// Decreases the iterator and returns a previous value of the iterator.
    TextByteIterator operator--(int)
    {
      TextByteIterator tmp(*this);
      --(*this);
      return tmp;
    }

    /// Returns \c true if the iterator is equal to \p iter, otherwise \c false.
    inline bool operator==(const TextByteIterator &iter) const;

    /// Returns \c true if the iterator isn't equal to \p iter, otherwise
    /// \c false.
    bool operator!=(const TextByteIterator &iter) const
    { return !(*this == iter); }

    /// Returns \c true if the iterator is less than \p iter, otherwise
    /// \c false.
    inline bool operator<(const TextByteIterator &iter) const;

    /// Returns \c true if the iterator is greater than or equal to \p iter,
    /// otherwise \c false.
    bool operator>=(const TextByteIterator &iter) const
    { return !(*this < iter); }

    /// Returns \c true if the iterator is greater than \p iter, otherwise
    /// \c false.
    bool operator>(const TextByteIterator &iter) const
    { return iter < *this; }

    /// Returns \c true if the iterator is less than or equal to \p iter,
    /// otherwise \c false.
    bool operator<=(const TextByteIterator &iter) const
    { return !(*this > iter); }

    /// Returns the text buffer of the iterator.
    const TextBuffer *buffer() const
    { return _M_buffer; }

    friend class TextBuffer;
    friend class TextCharIterator;
    friend class TextLineIterator;
  };

  ///
  /// A class of iterator of text characters.
  ///
  class TextCharIterator : public std::iterator<std::bidirectional_iterator_tag, TextByteIterator, std::ptrdiff_t, const TextByteIterator *, const TextByteIterator &>
  {
    const TextBuffer *_M_buffer;
    std::uintptr_t _M_data1;
    std::uintptr_t _M_data2;

    TextCharIterator(const TextBuffer *buffer, std::uintptr_t data1, std::uintptr_t data2) :
      _M_buffer(buffer), _M_data1(data1), _M_data2(data2) {}
  public:
    /// Default constructor.
    TextCharIterator() :
      _M_buffer(nullptr) {}

    /// Creates a new iterator of text characters from an iterator of text
    /// bytes.
    explicit TextCharIterator(const TextByteIterator &iter);

    /// Dereference operator.
    TextByteIterator operator*() const
    { return byte_iter(); }

    /// Member access operator.
    TextByteIterator *operator->() const
    { return nullptr; }

    /// Increases and returns the iterator.
    inline TextCharIterator &operator++();

    /// Decreases and returns the iterator.
    inline TextCharIterator &operator--();

    /// Increases the iterator and returns a previous value of the iterator.
    TextCharIterator operator++(int)
    {
      TextCharIterator tmp(*this);
      ++(*this);
      return tmp;
    }

    /// Decreases the iterator and returns a previous value of the iterator.
    TextCharIterator operator--(int)
    {
      TextCharIterator tmp(*this);
      --(*this);
      return tmp;
    }

    /// Returns \c true if the iterator is equal to \p iter, otherwise \c false.
    bool operator==(const TextCharIterator &iter) const
    { return byte_iter() == iter.byte_iter(); }

    /// Returns \c true if the iterator isn't equal to \p iter, otherwise
    /// \c false.
    bool operator!=(const TextCharIterator &iter) const
    { return !(*this == iter); }

    /// Returns \c true if the iterator is less than \p iter, otherwise
    /// \c false.
    bool operator<(const TextCharIterator &iter) const
    { return byte_iter() < iter.byte_iter(); }

    /// Returns \c true if the iterator is greater than or equal to \p iter,
    /// otherwise \c false.
    bool operator>=(const TextCharIterator &iter) const
    { return !(*this < iter); }

    /// Returns \c true if the iterator is greater than \p iter, otherwise
    /// \c false.
    bool operator>(const TextCharIterator &iter) const
    { return iter < *this; }

    /// Returns \c true if the iterator is less than or equal to \p iter,
    /// otherwise \c false.
    bool operator<=(const TextCharIterator &iter) const
    { return !(*this > iter); }

    /// Returns the text buffer of the iterator.
    const TextBuffer *buffer() const
    { return _M_buffer; }

    /// Returns an iterator of text bytes from the iterator.
    TextByteIterator byte_iter() const
    { return TextByteIterator(_M_buffer, _M_data1, _M_data2); }

    friend class TextBuffer;
    friend class TextLineIterator;
  };

  ///
  /// A class of iterator of text lines.
  ///
  class TextLineIterator  : public std::iterator<std::bidirectional_iterator_tag, TextCharIterator, std::ptrdiff_t, const TextCharIterator *, const TextCharIterator &>
  {
    const TextBuffer *_M_buffer;
    std::uintptr_t _M_data1;
    std::uintptr_t _M_data2;

    TextLineIterator(const TextBuffer *buffer, std::uintptr_t data1, std::uintptr_t data2) :
      _M_buffer(buffer), _M_data1(data1), _M_data2(data2) {}
  public:
    /// Default constructor.
    TextLineIterator() :
      _M_buffer(nullptr) {}

    /// Creates a new iterator of text lines from an iterator of text bytes.
    explicit TextLineIterator(const TextByteIterator &iter);

    /// Creates a new iterator of text lines from an iterator of text
    /// characters.
    explicit TextLineIterator(const TextCharIterator &iter);

    /// Dereference operator.
    TextCharIterator operator*() const
    { return char_iter(); }

    /// Member access operator.
    TextCharIterator *operator->() const
    { return nullptr; }

    /// Increases and returns the iterator.
    inline TextLineIterator &operator++();

    /// Decreases and returns the iterator.
    inline TextLineIterator &operator--();

    /// Increases the iterator and returns a previous value of the iterator.
    TextLineIterator operator++(int)
    {
      TextLineIterator tmp(*this);
      ++(*this);
      return tmp;
    }

    /// Decreases the iterator and returns a previous value of the iterator.
    TextLineIterator operator--(int)
    {
      TextLineIterator tmp(*this);
      --(*this);
      return tmp;
    }

    /// Returns \c true if the iterator is equal to \p iter, otherwise \c false.
    bool operator==(const TextLineIterator &iter) const
    { return byte_iter() == iter.byte_iter(); }

    /// Returns \c true if the iterator isn't equal to \p iter, otherwise
    /// \c false.
    bool operator!=(const TextLineIterator &iter) const
    { return !(*this == iter); }

    /// Returns \c true if the iterator is less than \p iter, otherwise
    /// \c false.
    bool operator<(const TextLineIterator &iter) const
    { return byte_iter() < iter.byte_iter(); }

    /// Returns \c true if the iterator is greater than or equal to \p iter,
    /// otherwise \c false.
    bool operator>=(const TextLineIterator &iter) const
    { return !(*this < iter); }

    /// Returns \c true if the iterator is greater than \p iter, otherwise
    /// \c false.
    bool operator>(const TextLineIterator &iter) const
    { return iter < *this; }

    /// Returns \c true if the iterator is less than or equal to \p iter,
    /// otherwise \c false.
    bool operator<=(const TextLineIterator &iter) const
    { return !(*this > iter); }

    /// Returns the text buffer of the iterator.
    const TextBuffer *buffer() const
    { return _M_buffer; }

    /// Returns an iterator of text bytes from the iterator.
    TextByteIterator byte_iter() const
    { return TextByteIterator(_M_buffer, _M_data1, _M_data2); }

    /// Returns an iterator of text characters from the iterator.
    TextCharIterator char_iter() const
    { return TextCharIterator(_M_buffer, _M_data1, _M_data2); }
  private:
    void initialize(const TextCharIterator &iter);

    friend class TextBuffer;
  };

  ///
  /// A class of text buffer that is used for storing a text of a text widget.
  ///
  /// A cursor position and a selection range also are stored in a text buffer
  /// for the text widget. The text buffer allows to modify the above properties
  /// of the text widget.
  ///
  class TextBuffer
  {
  protected:
    /// Default constructor.
    TextBuffer() {}
  public:
    /// Destructor.
    virtual ~TextBuffer();

    /// Returns an iterator that indicates on the beginning of the text bytes.
    virtual TextByteIterator byte_begin() const = 0;

    /// Returns an iterator that indicates on the end of the text bytes.
    virtual TextByteIterator byte_end() const = 0;

    /// Returns an iterator that indicates on the beginning of the text
    /// characters.
    virtual TextCharIterator char_begin() const = 0;

    /// Returns an iterator that indicates on the end of the text characters.
    virtual TextCharIterator char_end() const = 0;

    /// Returns an iterator that indicates on the beginning of the text lines.
    virtual TextLineIterator line_begin() const = 0;

    /// Returns an iterator that indicates on the end of the text lines.
    virtual TextLineIterator line_end() const = 0;

    ///
    /// Returns an iterator that indicates on the text beginning.
    ///
    /// This method is equivalent to the \ref byte_begin method.
    ///
    TextByteIterator begin() const
    { return byte_begin(); }

    ///
    /// Returns an iterator that indicates on the text end.
    ///
    /// This method is equivalent to the \ref byte_end method.
    ///
    TextByteIterator end() const
    { return byte_end(); }

    /// Returns the text of the text buffer.
    virtual std::string text() const = 0;

    ///
    /// Sets the text of the text buffer.
    ///
    /// The cusror position and a selection range reset after setting the text
    /// of the buffer.
    ///
    void set_text(const char *text)
    { set_text(std::string(text)); }

    /// \copydoc set_text(const char *text)
    virtual void set_text(const std::string &text) = 0;

    /// Returns the text length.
    std::size_t length() const
    { return char_count(); }

    /// Returns the number of the text bytes.
    virtual std::size_t byte_count() const  = 0;

    /// Returns the number of the text character.
    virtual std::size_t char_count() const = 0;

    /// Returns the number of the text lines.
    virtual std::size_t line_count() const = 0;

    /// Returns the cursor iterator of the text buffer.
    virtual TextCharIterator cursor_iter() const = 0;

    /// Returns the cursor position of the text buffer.
    virtual TextPosition cursor_pos() const = 0;

    /// Sets the cursor iterator of the text buffer.
    virtual void set_cursor_iter(const TextCharIterator &iter) = 0;

    /// Returns the selection range of the text buffer.
    virtual Range<TextCharIterator> selection_range() const = 0;

    /// Sets the selection range of the text buffer.
    void set_selection_range(const TextCharIterator &begin, const TextCharIterator &end)
    { set_selection_range(Range<TextCharIterator>(begin, end)); }

    /// \copydoc set_selection_range(const TextCharIterator &begin, const TextCharIterator &end)
    virtual void set_selection_range(const Range<TextCharIterator> &range) = 0;

    /// Returns the selected text of the text buffer.
    std::string selected_text() const
    {
      Range<TextCharIterator> range = selection_range();
      return std::string(range.begin.byte_iter(), range.end.byte_iter());
    }

    ///
    /// Inserts a new text into the text of the text buffer.
    ///
    /// The new text is inserted after the cursor. The cursor is moved after the
    /// last inserted character.
    ///
    void insert_string(const char *str)
    { insert_string(std::string(str)); }

    /// \copydoc insert_string(const char *str)
    virtual void insert_string(const std::string &str) = 0;

    ///
    /// Deletes characters from the text of the text buffer.
    ///
    /// The characters are deleted from the text of the text buffer after the
    /// cursor.
    ///
    virtual void delete_chars(std::size_t count) = 0;

    ///
    /// Appends a new text to the text of the text buffer at end of the text of
    /// the text widget.
    ///
    virtual void append_string(const std::string &str) = 0;

    ///
    /// Sets the initial gap size for an implementation of gap buffer.
    ///
    /// If the text buffer doesn't implement gap buffer, this method doesn't
    /// affect on the text buffer.
    ///
    virtual void set_gap_size(std::size_t gap_size) = 0;

    /// Returns the number of the tab spaces of the text buffer.
    virtual std::size_t tab_spaces() const = 0;

    /// Sets the number of the tab spaces of the text buffer.
    virtual void set_tab_spaces(std::size_t tab_spaces) = 0;
  protected:
    virtual bool has_saved_column() const = 0;

    virtual std::size_t saved_column() const = 0;

    virtual void set_saved_column(std::size_t column) = 0;

    virtual void unset_saved_column() = 0;
  public:
    virtual void validate_byte_iter(TextByteIterator &iter, const TextCharIterator &old_cursor_iter) const = 0;
    
    void validate_char_iter(TextCharIterator &iter, const TextCharIterator &old_cursor_iter) const;

    void validate_line_iter(TextLineIterator &iter, const TextCharIterator &old_cursor_iter) const;

    /// Returns the default initial gap size for single-line.
    static std::size_t default_single_line_gap_size();

    /// Returns the default initial gap size for multi-line.
    static std::size_t default_multi_line_gap_size();
  protected:
    /// Returns the reference to the byte that is indicated by an iterator of
    /// the text bytes.
    virtual const char &byte(const TextByteIterator &iter) const = 0;

    /// Returns the pointer to the byte that is indicated by an iterator of the
    /// text bytes.
    virtual const char *byte_ptr(const TextByteIterator &iter) const = 0;

    /// Increases and returns an iterator of the text bytes.
    virtual TextByteIterator &increase_byte_iter(TextByteIterator &iter) const = 0;

    /// Decreases and returns an iterator of the text bytes.
    virtual TextByteIterator &decrease_byte_iter(TextByteIterator &iter) const = 0;

    /// Returns \c true if \p iter1 is equal to \p iter2, otherwise \c false.
    virtual bool is_equal_to(const TextByteIterator &iter1, const TextByteIterator &iter2) const = 0;

    /// Returns \c true if \p iter1 is less than \p iter2, otherwise \c false.
    virtual bool is_less_than(const TextByteIterator &iter1, const TextByteIterator &iter2) const = 0;

    /// Increases and returns an iterator of the text characters.
    virtual TextCharIterator &increase_char_iter(TextCharIterator &iter) const;

    /// Dereases and returns an iterator of the text characters.
    virtual TextCharIterator &decrease_char_iter(TextCharIterator &iter) const;

    /// Increases and returns an iterator of the text lines.
    virtual TextLineIterator &increase_line_iter(TextLineIterator &iter) const;

    /// Decreases and returns an iterator of the text lines.
    virtual TextLineIterator &decrease_line_iter(TextLineIterator &iter) const;

    /// Returns an iterator of the text bytes that is created from a value of
    /// the \c data1 field and a value of the \c data2 field.
    TextByteIterator make_byte_iter(std::uintptr_t data1, std::uintptr_t data2) const
    { return TextByteIterator(this, data1, data2); }

    /// Returns the \c data1 field of an iterator of the text bytes.
    std::uintptr_t byte_iter_data1(const TextByteIterator &iter) const
    { return iter._M_data1; }

    /// Returns a reference to the \c data1 field of an iterator of the text
    /// bytes.
    std::uintptr_t &byte_iter_data1(TextByteIterator &iter) const
    { return iter._M_data1; }

    /// Returns a value of the \c data2 field of an iterator of the text bytes.
    std::uintptr_t byte_iter_data2(const TextByteIterator &iter) const
    { return iter._M_data2; }

    /// Returns a reference to the \c data2 field of an iterator of the text
    /// bytes.
    std::uintptr_t &byte_iter_data2(TextByteIterator &iter) const
    { return iter._M_data2; }

    /// Returns an iterator of the text characters that is created from a value
    /// of the \c data1 field and a value of the \c data2 field.
    TextCharIterator make_char_iter(std::uintptr_t data1, std::uintptr_t data2) const
    { return TextCharIterator(this, data1, data2); }

    /// Returns the \c data1 field of an iterator of the text characters.
    std::uintptr_t char_iter_data1(const TextCharIterator &iter) const
    { return iter._M_data1; }

    /// Returns a reference to the \c data1 field of an iterator of the text
    /// characters.
    std::uintptr_t &char_iter_data1(TextCharIterator &iter) const
    { return iter._M_data1; }

    /// Returns the \c data2 field of an iterator of the text characters.
    std::uintptr_t char_iter_data2(const TextCharIterator &iter) const
    { return iter._M_data2; }

    /// Returns a reference to the \c data2 field of an iterator of the text
    /// characters.
    std::uintptr_t &char_iter_data2(TextCharIterator &iter) const
    { return iter._M_data2; }

    /// Returns an iterator of the text lines that is created from a value of
    /// the \c data1 field and a value of the \c data2 field.
    TextLineIterator make_line_iter(std::uintptr_t data1, std::uintptr_t data2) const
    { return TextLineIterator(this, data1, data2); }

    /// Returns the \c data1 field of an iterator of the text lines.
    std::uintptr_t line_iter_data1(const TextLineIterator &iter) const
    { return iter._M_data1; }

    /// Returns a reference to the \c data1 field of an iterator of the text
    /// lines.
    std::uintptr_t &line_iter_data1(TextLineIterator &iter) const
    { return iter._M_data1; }

    /// Returns the \c data2 field of an iterator of the text lines.
    std::uintptr_t line_iter_data2(const TextLineIterator &iter) const
    { return iter._M_data2; }

    /// Returns a reference to the \c data2 field of an iterator of the text
    /// lines.
    std::uintptr_t &line_iter_data2(TextLineIterator &iter) const
    { return iter._M_data2; }

    friend class Text;
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
