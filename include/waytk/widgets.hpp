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
#ifndef _WAYTK_WIDGETS_HPP
#define _WAYTK_WIDGETS_HPP

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <waytk/adapters.hpp>
#include <waytk/canvas.hpp>
#include <waytk/modifiers.hpp>
#include <waytk/structs.hpp>
#include <waytk/text_buffer.hpp>
#include <waytk/util.hpp>

namespace waytk
{
  class MenuItem;
  class RadioButton;
  class Surface;
  class Widget;

  enum class WidgetFlags
  {
    NONE = 0,
    ACTIVE = 1,
    HOVER = 2,
    SELECTED = 4
  };

  inline WidgetFlags operator&(WidgetFlags flags1, WidgetFlags flags2)
  { return static_cast<WidgetFlags>(static_cast<int>(flags1) & static_cast<int>(flags2)); }

  inline WidgetFlags operator&=(WidgetFlags &flags1, WidgetFlags flags2)
  { flags1 = flags1 & flags2; return flags1; }

  inline WidgetFlags operator|(WidgetFlags flags1, WidgetFlags flags2)
  { return static_cast<WidgetFlags>(static_cast<int>(flags1) | static_cast<int>(flags2)); }

  inline WidgetFlags operator|=(WidgetFlags &flags1, WidgetFlags flags2)
  { flags1 = flags1 | flags2; return flags1; }

  enum class TouchState
  {
    DOWN,
    MOTION,
    UP
  };

  enum class Axis
  {
    HORIZONTAL_SCROLL,
    VERTICAL_SCROLL
  };

  enum class KeyState
  {
    PRESSED,
    REPEATED,
    RELEASED
  };

  enum class InputType
  {
    SINGLE_LINE,
    MULTI_LINE,
    PASSWORD
  };

  enum class Orientation
  {
    HORIZONTAL,
    VERTICAL
  };

  enum class SelectionMode
  {
    SINGLE,
    MULTI
  };
  
  class Pointer
  {
    bool _M_is_touch;
    std::uint32_t _M_touch_id;
  public:
    Pointer() : _M_is_touch(false) {}

    explicit Pointer(std::uint32_t touch_id) : _M_is_touch(true), _M_touch_id(touch_id) {}

    bool is_touch() const { return _M_is_touch; }

    std::uint32_t touch_id() const { return _M_touch_id; }

    bool operator==(const Pointer &pointer) const
    { return (_M_is_touch && pointer._M_is_touch) ? _M_touch_id == pointer._M_touch_id : _M_is_touch == pointer._M_is_touch; }

    bool operator!=(const Pointer &pointer) const
    { return !(*this == pointer); }
  };

  class Icon
  {
    std::string _M_name;
  public:
    Icon() {}

    explicit Icon(const std::string &name) :
      _M_name(name) {}

    bool operator==(const Icon &icon) const
    { return _M_name == icon._M_name; }

    bool operator!=(const Icon &icon) const
    { return !(*this == icon); }

    const std::string &name() const
    { return _M_name; }

    std::shared_ptr<CanvasImage> image() const;
  };

  struct TablePositionCompare
  {
    bool operator()(const TablePosition &pos1, const TablePosition &pos2) const
    { return pos1.row == pos2.row ? pos1.column < pos2.column : pos1.column < pos2.column; }
  };

  struct TreePathCompare
  {
    bool operator()(const TreePath &path1, const TreePath &path2);
  };

  class Viewport
  {
  protected:
    Viewport() {}
  public:
    virtual ~Viewport();

    virtual Rectangle<int> bounds() const = 0;

    virtual int h_scroll_slider_x(int width) const = 0;

    virtual int h_scroll_slider_x_step(int width) const = 0;
    
    virtual void set_h_scroll_slider_x(int x, int width) = 0; 

    virtual int h_scroll_slider_width(int width) const = 0;

    virtual int v_scroll_slider_y(int height) const = 0;

    virtual int v_scroll_slider_y_step(int height) const = 0;

    virtual void set_v_scroll_slider_y(int y, int height) = 0; 

    virtual int v_scroll_slider_height(int height) const = 0;
  };
  
  typedef std::function<void (Widget *, const Pointer &, const Point<double> &, TouchState &)> OnTouchListener;
  typedef std::function<void (Widget *, const Point<double> &)> OnPointerMotionListener;
  typedef std::function<void (Widget *, Axis, double)> OnPointerAxisListener;
  typedef std::function<void (Widget *, std::uint32_t, Modifiers, const char *, KeyState)> OnKeyListener;
  typedef std::function<void (Widget *, Viewport *)> OnScrollListener;
  typedef std::function<void (Widget *)> OnClickListener;
  typedef std::function<void (Widget *, bool)> OnCheckListener;
  typedef std::function<void (Widget *, std::size_t)> OnSelectionListener;
  typedef std::function<void (Widget *, const Range<TextCharIterator> &)> OnTextChangeListener;
  typedef std::function<void (Widget *, const TextCharIterator &, const TextPosition &)> OnCursorChangeListener;
  typedef std::function<void (Widget *, const Range<TextCharIterator> &)> OnTextSelectionListener;
  typedef std::function<void (Widget *, const std::set<std::size_t> &)> OnListSelectionListener;
  typedef std::function<void (Widget *, const std::set<TablePosition, TablePositionCompare> &)> OnTableSelectionListener;
  typedef std::function<void (Widget *, const std::set<TreePath, TreePathCompare> &)> OnTreeSelectionListener;
  
  class Widget
  {
  protected:
    class Unused { char _M_pad; };
  private:
    bool _M_is_enabled;
    WidgetFlags _M_flags;
    bool _M_has_focus;
    bool _M_is_visible;
    int _M_max_width;
    int _M_max_height;
    int _M_min_width;
    int _M_min_height;
    Rectangle<int> _M_bounds;
    Surface *_M_surface;
    Widget *_M_parent;
    OnTouchListener _M_on_touch_listener;
    OnPointerMotionListener _M_on_pointer_motion_listener;
    OnPointerAxisListener _M_on_pointer_axis_listener;
    OnKeyListener _M_on_key_listener;
    OnScrollListener _M_on_scroll_listener;
  public:
    Widget();

    virtual ~Widget();

    bool is_enabled() const
    { return _M_is_enabled; }

    void set_enabled(bool is_enabled)
    { _M_is_enabled = is_enabled; }

    WidgetFlags flags() const
    { return _M_flags; }

    void set_flags(WidgetFlags flags)
    { _M_flags = flags; }

    bool has_focus() const
    { return _M_has_focus; }

    void set_focus(bool has_focus);

    bool is_visible() const
    { return _M_is_visible; }

    void set_visibale(bool is_visible)
    { _M_is_visible = is_visible; }

    int max_width() const
    { return _M_max_width; }

    void set_max_width(int max_width)
    { _M_max_width = max_width; }
    
    int max_height() const
    { return _M_max_height; }

    void set_max_height(int max_height)
    { _M_max_height = max_height; }

    int min_width() const
    { return _M_min_width; }

    void set_min_width(int min_width)
    { _M_min_width = min_width; }

    int min_height() const
    { return _M_min_height; }

    void set_min_height(int min_height)
    { _M_min_height = min_height; }
    
    const Rectangle<int> &bounds() const
    { return _M_bounds; }

    void set_bounds(const Rectangle<int> &bounds)
    { _M_bounds = bounds; }

    Surface *surface() const
    { return _M_surface; }

    Widget *parent() const
    { return _M_parent; }

    const OnTouchListener &on_touch_listener() const
    { return _M_on_touch_listener; }

    void set_on_touch_listener(const OnTouchListener &listener)
    { _M_on_touch_listener = listener; }

    const OnPointerMotionListener &on_pointer_montion_listener() const
    { return _M_on_pointer_motion_listener; }

    void set_on_pointer_motion_listener(const OnPointerMotionListener &listener)
    { _M_on_pointer_motion_listener = listener; }

    const OnPointerAxisListener &on_pointer_axis_listener() const
    { return _M_on_pointer_axis_listener; }

    void set_on_pointer_axis_listener(const OnPointerAxisListener &listener)
    { _M_on_pointer_axis_listener = listener; }

    const OnKeyListener &on_key_listener() const
    { return _M_on_key_listener; }

    void set_on_key_listener(const OnKeyListener &listener)
    { _M_on_key_listener = listener; }

    const OnScrollListener &on_scroll_listener() const
    { return _M_on_scroll_listener; }

    void set_on_scroll_listener(const OnScrollListener &listener)
    { _M_on_scroll_listener = listener; }

    virtual void draw(Canvas *canvas);

    virtual Viewport *viewport();
    
    virtual void on_touch(const Pointer &pointer, const Point<double> &point, TouchState state);

    virtual void on_pointer_motion(const Point<double> &point);

    virtual void on_pointer_axis(Axis axis, double value);

    virtual void on_key(std::uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state);

    virtual void on_scoll(Viewport *viewport);
  };

  class Container : public Widget
  {
    std::list<std::unique_ptr<Widget>> _M_widgets;
  protected:
    Container() {}
  public:
    virtual ~Container();
  protected:
    void initialize(std::initializer_list<Widget *> widgets);
  public:
    const std::list<std::unique_ptr<Widget>> &widgets() const
    { return _M_widgets; }

    void add_widget(Widget *widget)
    { _M_widgets.push_back(std::unique_ptr<Widget>(widget)); }

    void delete_widget(Widget *widget);

    void delete_all_widgets()
    { _M_widgets.clear(); }
  };

  class Label : public Widget
  {
    std::string _M_text;
  public:
    explicit Label(const std::string &text)
    { initialize(text); }

    virtual ~Label();
  protected:
    void initialize(const std::string &text);
  public:
    const std::string &text() const
    { return _M_text; }

    void set_text(const std::string &text);

    virtual void draw(Canvas *canvas);
  };

  class Button : public Widget
  {
    Icon _M_icon;
    std::string _M_label;
    OnClickListener _M_on_click_listener;
  protected:
    Button() {}
  public:
    explicit Button(const std::string &label)
    { initialize(Icon(), label, [](Widget *widget) {}); }

    explicit Button(const std::string &label, const OnClickListener &listener)
    { initialize(Icon(), label, listener); }

    explicit Button(const Icon &icon)
    { initialize(icon, "", [](Widget *widget) {}); }

    Button(const Icon &icon, const OnClickListener &listener)
    { initialize(icon, "", listener); }

    Button(const Icon &icon, const std::string &label)
    { initialize(icon, label, [](Widget *widget) {}); }

    Button(const Icon &icon, const std::string &label, const OnClickListener &listener)
    { initialize(icon, label, listener); }

    virtual ~Button();
  protected:
    void initialize(const Icon &icon, const std::string &label, const OnClickListener &listener);
  public:
    const Icon icon() const
    { return _M_icon; }

    void set_icon(const Icon &icon)
    { _M_icon = icon; }
    
    const std::string &label() const
    { return _M_label; }

    void set_label(const std::string &label);

    const OnClickListener &on_click_listener() const
    { return _M_on_click_listener; }

    void set_on_click_listener(const OnClickListener &listener)
    { _M_on_click_listener = listener; }

    virtual void draw(Canvas *canvas);

    virtual void on_touch(const Pointer &pointer, const Point<double> &point, TouchState state);

    virtual void on_key(std::uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state);

    virtual void on_click();
  };

  class Text : public Widget
  {
    InputType _M_input_type;
    std::unique_ptr<TextBuffer> _M_buffer;
    TextLineIterator _M_first_visible_iter;
    TextLineIterator _M_last_visible_iter;
    std::size_t _M_max_length;
    bool _M_has_line_wrap;
    bool _M_has_word_wrap;
    bool _M_is_editable;
    OnTextChangeListener _M_on_text_change_listener;
    OnCursorChangeListener _M_on_cursor_change_listener;
    OnTextSelectionListener _M_on_text_selection_listener;
  protected:
    Text(Unused unused) {}
  public:
    Text()
    { initialize(InputType::SINGLE_LINE, std::string()); }

    explicit Text(const std::string &text)
    { initialize(InputType::SINGLE_LINE, text); }

    explicit Text(InputType input_type)
    { initialize(input_type, std::string()); }

    Text(InputType input_type, const std::string &text)
    { initialize(input_type, text); }

    virtual ~Text();
  protected:
    void initialize(InputType input_type, const std::string &text);
  public:
    InputType input_type() const
    { return _M_input_type; }

    void set_input_type(InputType input_type)
    { _M_input_type = input_type; }

    const std::string text() const
    { return _M_buffer->text(); }

    void set_text(const std::string &text);

    const TextBuffer *buffer() const
    { return _M_buffer.get(); }

    TextBuffer *buffer()
    { return _M_buffer.get(); }

    std::size_t length() const
    { return _M_buffer->length(); }

    TextCharIterator cursor_iter() const
    { return _M_buffer->cursor_iter(); }

    void set_cursor_iter(const TextCharIterator &iter);

    TextPosition cursor_line() const
    { return _M_buffer->cursor_pos(); }

    void set_selection_range(const TextCharIterator &begin, const TextCharIterator &end)
    { set_selection_range(Range<TextCharIterator>(begin, end)); }

    void set_selection_range(const Range<TextCharIterator> &range);

    void insert_string(const std::string &str);
    
    void replace_string(std::size_t count, const std::string &str);
    
    void delete_chars(std::size_t count);

    std::size_t max_length() const
    { return _M_max_length; }

    void set_max_length(std::size_t max_length)
    { _M_max_length = max_length; }

    bool has_line_wrap() const
    { return _M_has_line_wrap; }

    void set_line_wrap(bool has_line_wrap)
    { _M_has_line_wrap = has_line_wrap; }

    bool has_word_wrap() const
    { return _M_has_word_wrap; }

    void set_word_wrap(bool has_word_wrap)
    { _M_has_word_wrap = has_word_wrap; }

    bool is_editable() const
    { return _M_is_editable; }

    void set_editable(bool is_editable)
    { _M_is_editable = is_editable; }

    std::string selected_text() const
    { return _M_buffer->selected_text(); }

    void all_select()
    { select(_M_buffer->char_begin(), _M_buffer->char_end()); }

    void select(const TextCharIterator &begin, const TextCharIterator &end)
    { set_selection_range(Range<TextCharIterator>(begin, end)); }

    void clear_selection()
    { set_selection_range(Range<TextCharIterator>(_M_buffer->char_begin(), _M_buffer->char_begin())); }

    void copy();

    void paste();

    const OnTextChangeListener &on_text_change_listener() const
    { return _M_on_text_change_listener; }

    void set_on_text_change_listener(const OnTextChangeListener &listener)
    { _M_on_text_change_listener = listener; }

    const OnCursorChangeListener &on_cursor_change_listener() const
    { return _M_on_cursor_change_listener; }

    void set_on_cursor_change_listener(const OnCursorChangeListener &listener)
    { _M_on_cursor_change_listener = listener; }

    const OnTextSelectionListener &on_text_selection_listener() const
    { return _M_on_text_selection_listener; }

    void set_on_text_selection_listener(const OnTextSelectionListener &listener)
    { _M_on_text_selection_listener = listener; }

    virtual void draw(Canvas *canvas);

    virtual Viewport *viewport();

    virtual void on_touch(const Pointer &pointer, const Point<double> &point, TouchState state);

    virtual void on_key(std::uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state);

    virtual void on_text_change(const Range<TextCharIterator> &range);

    virtual void on_cursor_change(const TextCharIterator &iter, const TextPosition &pos);

    virtual void on_text_selection(const Range<TextCharIterator> &range);
  };

  class CheckBox : public virtual Button
  {
    bool _M_is_checked;
    OnCheckListener _M_on_check_listener;
  protected:
    CheckBox(Unused unused) {}
  public:
    CheckBox()
    { initialize(Icon(), std::string(), false); }

    explicit CheckBox(bool is_checked)
    { initialize(Icon(), std::string(), is_checked); }

    explicit CheckBox(const std::string &label)
    { initialize(Icon(), label, false); }

    CheckBox(const std::string &label, bool is_checked)
    { initialize(Icon(), label, is_checked); }

    virtual ~CheckBox();
  protected:
    void initialize(const Icon &icon, const std::string &label, bool is_checked);
  public:
    bool is_checked() const
    { return _M_is_checked; }

    void set_checked(bool is_checked);

    const OnCheckListener &on_check_listener() const
    { return _M_on_check_listener; }

    void set_on_check_listener(const OnCheckListener &listener)
    { _M_on_check_listener = listener; }

    virtual void draw(Canvas *canvas);

    virtual void on_click();

    virtual void on_check(bool is_checked);
  };

  class RadioGroup
  {
    std::list<RadioButton *> _M_radio_buttons;
  public:
    RadioGroup() {}

    const std::list<RadioButton *> &radio_buttons() const
    { return _M_radio_buttons; } 
    
    void add_radio_button(RadioButton *radio_button);
  };
  
  class RadioButton : public CheckBox
  {
    std::shared_ptr<RadioGroup> _M_group;
  protected:
    RadioButton(Unused unused) {}
  public:
    RadioButton() :
      Button(), CheckBox(Unused())
    { initialize(Icon(), std::string(), false, std::shared_ptr<RadioGroup>()); }

    explicit RadioButton(bool is_checked) :
      Button(), CheckBox(Unused())
    { initialize(Icon(), std::string(), is_checked, std::shared_ptr<RadioGroup>()); }

    explicit RadioButton(const std::shared_ptr<RadioGroup> &group) :
      Button(), CheckBox(Unused())
    { initialize(Icon(), std::string(), false, group); }

    RadioButton(bool is_checked, const std::shared_ptr<RadioGroup> &group) :
      Button(), CheckBox(Unused())
    { initialize(Icon(), std::string(), is_checked, group); }

    explicit RadioButton(const std::string &label) :
      Button(), CheckBox(Unused())
    { initialize(Icon(), label, false, std::shared_ptr<RadioGroup>()); }

    RadioButton(const std::string &label, bool is_checked) :
      Button(), CheckBox(Unused())
    { initialize(Icon(), label, is_checked, std::shared_ptr<RadioGroup>()); }

    RadioButton(const std::string &label, const std::shared_ptr<RadioGroup> &group) :
      Button(), CheckBox(Unused())
    { initialize(Icon(), label, false, group); }

    RadioButton(const std::string &label, bool is_checked, const std::shared_ptr<RadioGroup> &group) :
      Button(), CheckBox(Unused())
    { initialize(Icon(), label, is_checked, group); }

    virtual ~RadioButton();
  protected:
    void initialize(const Icon &icon, const std::string &label, bool is_checked, const std::shared_ptr<RadioGroup> &group);
  public:
    const std::shared_ptr<RadioGroup> &group() const
    { return _M_group; }
    
    void set_group(const std::shared_ptr<RadioGroup> &group);

    virtual void draw(Canvas *canvas);

    virtual void on_check(bool is_checked);
  };

  class ComboBox : public Button
  {
    std::size_t _M_selected_pos;
    std::shared_ptr<ComboBoxAdapter> _M_adapter;
    OnSelectionListener _M_on_selection_listener;
  protected:
    ComboBox(Unused unused) {}
  public:
    ComboBox()
    { initialize(std::shared_ptr<ComboBoxAdapter>(new StringComboBoxAdapter())); }

    explicit ComboBox(std::initializer_list<std::string> items)
    { initialize(std::shared_ptr<ComboBoxAdapter>(new StringComboBoxAdapter(items))); }

    explicit ComboBox(ComboBoxAdapter *adapter)
    { initialize(std::shared_ptr<ComboBoxAdapter>(adapter)); }

    explicit ComboBox(const std::shared_ptr<ComboBoxAdapter> &adapter)
    { initialize(adapter); }

    virtual ~ComboBox();
  protected:
    void initialize(const std::shared_ptr<ComboBoxAdapter> &adapter);
  public:
    std::size_t selected_pos() const
    { return _M_selected_pos; }

    void set_selected_pos(std::size_t pos);

    const std::shared_ptr<ComboBoxAdapter> &adapter() const
    { return _M_adapter; }

    void set_adapter(ComboBoxAdapter *adapter)
    { _M_adapter = std::shared_ptr<ComboBoxAdapter>(adapter); }

    void set_adapter(const std::shared_ptr<ComboBoxAdapter> &adapter)
    { _M_adapter = adapter; }

    void select(std::size_t pos)
    { set_selected_pos(pos); }

    const OnSelectionListener &on_selection_listener() const
    { return _M_on_selection_listener; }

    void set_selection_listener(const OnSelectionListener &listener)
    { _M_on_selection_listener = listener; }

    virtual void draw(Canvas &canvas);

    virtual void on_click();

    virtual void on_selection(std::size_t pos);
  };

  class ProgressBar : public Widget
  {
    int _M_max_value;
    int _M_value;
  protected:
    ProgressBar(Unused unused) {}
  public:
    ProgressBar()
    { initialize(100); }

    explicit ProgressBar(int max_value)
    { initialize(max_value); }

    virtual ~ProgressBar();
  protected:
    void initialize(int max_value);
  public:
    int max_value() const
    { return _M_max_value; }

    void set_max_value(int max_value)
    { _M_max_value = max_value; }

    int value() const
    { return _M_value; }

    void set_value(int value)
    { _M_value = (value < _M_max_value ? value : _M_max_value); }
  };

  class Image : public Widget
  {
    std::shared_ptr<CanvasImage> _M_image;
  protected:
    Image() {}
  public:
    Image(int width, int height)
    { initialize(std::shared_ptr<CanvasImage>(new_canvas_image(width, height))); }

    explicit Image(Dimension<int> size)
    { initialize(std::shared_ptr<CanvasImage>(new_canvas_image(size))); }

    Image(int width, int height, int stride, void *data)
    { initialize(std::shared_ptr<CanvasImage>(new_canvas_image(width, height, stride, data))); }

    Image(Dimension<int> size, int stride, void *data)
    { initialize(std::shared_ptr<CanvasImage>(new_canvas_image(size, stride, data))); }

    explicit Image(const std::string &file_name)
    { initialize(std::shared_ptr<CanvasImage>(load_canvas_image(file_name)));}

    explicit Image(CanvasImage *image)
    { initialize(std::shared_ptr<CanvasImage>(image)); }

    explicit Image(const std::shared_ptr<CanvasImage> &image)
    { initialize(image); }

    virtual ~Image();
  protected:
    void initialize(const std::shared_ptr<CanvasImage> &image);
  public:
    const std::shared_ptr<CanvasImage> &image() const
    { return _M_image; }

    void set_image(int width, int height)
    { set_image(new_canvas_image(width, height)); }

    void set_image(Dimension<int> size)
    { set_image(new_canvas_image(size)); }

    void set_image(int width, int height, int stride, void *data)
    { set_image(new_canvas_image(width, height)); }

    void set_image(Dimension<int> size, int stride, void *data)
    { set_image(new_canvas_image(size)); }

    void set_image(CanvasImage *image)
    { set_image(std::shared_ptr<CanvasImage>(image)); }

    void set_image(const std::shared_ptr<CanvasImage> &image);

    void load(const std::string &file_name)
    { set_image(std::shared_ptr<CanvasImage>(load_canvas_image(file_name))); }
  };

  class Panel : public Container
  {
  protected:
    Panel() {}
  public:
    virtual ~Panel();
  };

  class LinearPanel : public Panel
  {
    Orientation _M_orientation;
  protected:
    LinearPanel(Unused unused) {}
  public:
    LinearPanel()
    { initialize(Orientation::HORIZONTAL, {}); }

    explicit LinearPanel(std::initializer_list<Widget *> widgets)
    { initialize(Orientation::HORIZONTAL, widgets); }

    explicit LinearPanel(Orientation orientation)
    { initialize(orientation, {}); }

    LinearPanel(Orientation orientation, std::initializer_list<Widget *> widgets)
    { initialize(orientation, widgets); }

    virtual ~LinearPanel();
  protected:
    void initialize(Orientation orientation, std::initializer_list<Widget *> widgets);
  public:
    Orientation orientation() const
    { return _M_orientation; }

    void set_orientation(Orientation orientation)
    { _M_orientation = orientation; }

    virtual void draw(Canvas *canvas);
  };

  class GridPanel : public Panel
  {
    int _M_column_count;
  protected:
    GridPanel() {}
  public:
    explicit GridPanel(int column_count)
    { initialize(column_count, {}); }

    GridPanel(int column_count, std::initializer_list<Widget *> widgets)
    { initialize(column_count, widgets); }

    virtual ~GridPanel();
  protected:
    void initialize(int column_count, std::initializer_list<Widget *> widgets);
  public:
    int column_count() const
    { return _M_column_count; }

    void set_columns(int column_count)
    { _M_column_count = column_count; }

    virtual void draw(Canvas *canvas);
  };

  class SplitPane : public Widget
  {
    std::unique_ptr<Widget> _M_first_widget;
    std::unique_ptr<Widget> _M_second_widget;
  public:
    SplitPane(Widget *first_widget, Widget *second_widget)
    { initialize(first_widget, second_widget); }

    virtual ~SplitPane();
  protected:
    void initialize(Widget *first_widget, Widget *second_widget);
  public:
    Widget *first_widget() const
    { return _M_first_widget.get(); }

    void set_first_widget(Widget *widget)
    { _M_first_widget = std::unique_ptr<Widget>(widget); }
    
    Widget *second_widget() const
    { return _M_second_widget.get(); }

    void set_second_widget(Widget *widget)
    { _M_second_widget = std::unique_ptr<Widget>(widget); }
  };

  class List : public Widget
  {
    SelectionMode _M_selection_mode;
    std::set<std::size_t> _M_selected_poses;
    std::shared_ptr<ListAdapter> _M_adapter;
    OnListSelectionListener _M_on_list_selection_listener;
  protected:
    List(Unused unused) {}
  public:
    List()
    { initialize(SelectionMode::SINGLE, std::shared_ptr<ListAdapter>(new StringListAdapter())); }

    explicit List(std::initializer_list<std::string> items)
    { initialize(SelectionMode::SINGLE, std::shared_ptr<ListAdapter>(new StringListAdapter(items))); }

    explicit List(ListAdapter *adapter)
    { initialize(SelectionMode::SINGLE, std::shared_ptr<ListAdapter>(adapter)); }

    explicit List(const std::shared_ptr<ListAdapter> &adapter)
    { initialize(SelectionMode::SINGLE, adapter); }

    explicit List(SelectionMode mode)
    { initialize(mode, std::shared_ptr<ListAdapter>(new StringListAdapter())); }

    List(SelectionMode mode, std::initializer_list<std::string> items)
    { initialize(mode, std::shared_ptr<ListAdapter>(new StringListAdapter(items))); }

    List(SelectionMode mode, ListAdapter *adapter)
    { initialize(mode, std::shared_ptr<ListAdapter>(adapter)); }

    List(SelectionMode mode, const std::shared_ptr<ListAdapter> &adapter)
    { initialize(mode, adapter); }

    virtual ~List();
  protected:
    void initialize(SelectionMode mode, const std::shared_ptr<ListAdapter> &adapter);
  public:
    SelectionMode selection_mode() const
    { return _M_selection_mode; }

    void set_selection_mode(SelectionMode mode)
    { _M_selection_mode = mode; }

    const std::set<std::size_t> &selected_poses() const
    { return _M_selected_poses; }

    void set_selected_poses(std::initializer_list<std::size_t> poses)
    { set_selected_poses(std::set<std::size_t>(poses)); }
    
    void set_selected_poses(const std::list<std::size_t> &poses)
    { set_selected_poses(std::set<std::size_t>(poses.begin(), poses.end())); }

    void set_selected_poses(const std::vector<std::size_t> &poses)
    { set_selected_poses(std::set<std::size_t>(poses.begin(), poses.end())); }

    void set_selected_poses(const std::set<std::size_t> &poses);

    const std::shared_ptr<ListAdapter> &adapter() const
    { return _M_adapter; }

    void set_adapter(ListAdapter *adapter)
    { _M_adapter = std::shared_ptr<ListAdapter>(adapter); }

    void set_adapter(const std::shared_ptr<ListAdapter> &adapter)
    { _M_adapter = adapter; }

    void select_all();
    
    void change_selection(std::size_t pos)
    { change_selection(Range<std::size_t>(pos, pos + 1)); }

    void change_selection(const Range<std::size_t> &range);

    void clear_selection();

    const OnListSelectionListener &on_list_selection_listener() const
    { return _M_on_list_selection_listener; }

    void set_on_list_selection_listener(const OnListSelectionListener &listener)
    { _M_on_list_selection_listener = listener; }

    virtual void draw(Canvas *canvas);

    virtual void on_touch(const Pointer &pointer, const Point<double> &point, TouchState state);

    virtual void on_key(std::uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state);

    virtual void on_list_selection(const std::set<std::size_t> &poses);
  };

  class Table : public Widget
  {
    SelectionMode _M_selection_mode;
    std::vector<std::string> _M_header_labels;
    std::set<TablePosition, TablePositionCompare> _M_selected_poses;
    std::shared_ptr<TableAdapter> _M_adapter;
    OnTableSelectionListener _M_on_table_selection_listener;
  protected:
    Table(Unused unused) {}
  public:
    explicit Table(std::size_t column_count)
    { initialize(SelectionMode::SINGLE, {}, std::shared_ptr<TableAdapter>(new StringTableAdapter(column_count))); }

    explicit Table(std::initializer_list<std::initializer_list<std::string>> cells)
    { initialize(SelectionMode::SINGLE, {}, std::shared_ptr<TableAdapter>(new StringTableAdapter(cells))); }

    explicit Table(TableAdapter *adapter)
    { initialize(SelectionMode::SINGLE, {}, std::shared_ptr<TableAdapter>(adapter)); }

    explicit Table(const std::shared_ptr<TableAdapter> &adapter)
    { initialize(SelectionMode::SINGLE, {}, adapter); }

    explicit Table(std::initializer_list<std::string> header_labels)
    { initialize(SelectionMode::SINGLE, header_labels, std::shared_ptr<TableAdapter>(new StringTableAdapter(header_labels.size()))); }

    Table(std::initializer_list<std::string> header_labels, std::initializer_list<std::initializer_list<std::string>> cells)
    { initialize(SelectionMode::SINGLE, header_labels, std::shared_ptr<TableAdapter>(new StringTableAdapter(cells))); }

    Table(std::initializer_list<std::string> header_labels, TableAdapter *adapter)
    { initialize(SelectionMode::SINGLE, header_labels, std::shared_ptr<TableAdapter>(adapter)); }

    Table(std::initializer_list<std::string> header_labels, const std::shared_ptr<TableAdapter> &adapter)
    { initialize(SelectionMode::SINGLE, header_labels, adapter); }

    Table(SelectionMode mode, std::size_t column_count)
    { initialize(mode, {}, std::shared_ptr<TableAdapter>(new StringTableAdapter(column_count))); }

    Table(SelectionMode mode, std::initializer_list<std::initializer_list<std::string>> cells)
    { initialize(mode, {}, std::shared_ptr<TableAdapter>(new StringTableAdapter(cells))); }

    Table(SelectionMode mode, TableAdapter *adapter)
    { initialize(mode, {}, std::shared_ptr<TableAdapter>(adapter)); }

    Table(SelectionMode mode, const std::shared_ptr<TableAdapter> &adapter)
    { initialize(mode, {}, adapter); }

    Table(SelectionMode mode, std::initializer_list<std::string> header_labels)
    { initialize(mode, header_labels, std::shared_ptr<TableAdapter>(new StringTableAdapter(header_labels.size()))); }

    Table(SelectionMode mode, std::initializer_list<std::string> header_labels, std::initializer_list<std::initializer_list<std::string>> cells)
    { initialize(mode, header_labels, std::shared_ptr<TableAdapter>(new StringTableAdapter(cells))); }

    Table(SelectionMode mode, std::initializer_list<std::string> header_labels, TableAdapter *adapter)
    { initialize(mode, header_labels, std::shared_ptr<TableAdapter>(adapter)); }

    Table(SelectionMode mode, std::initializer_list<std::string> header_labels, const std::shared_ptr<TableAdapter> &adapter)
    { initialize(mode, header_labels, adapter); }
    
    virtual ~Table();
  protected:
    void initialize(SelectionMode selection_mode, std::initializer_list<std::string> header_labels, const std::shared_ptr<TableAdapter> &adapter);
  public:
    SelectionMode selection_mode() const
    { return _M_selection_mode; }

    void set_selection_mode(SelectionMode mode)
    { _M_selection_mode = mode; }

    const std::vector<std::string> &header_labels() const
    { return _M_header_labels; }

    void set_header_labels(std::initializer_list<std::string> header_labels)
    { set_header_labels(std::vector<std::string>(header_labels)); }

    void set_header_labels(const std::list<std::string> &header_labels)
    { set_header_labels(std::vector<std::string>(header_labels.begin(), header_labels.end())); }

    void set_header_labels(const std::vector<std::string> &header_labels)
    { _M_header_labels = header_labels; }

    const std::set<TablePosition, TablePositionCompare> &selected_poses() const
    { return _M_selected_poses; }

    void set_selected_poses(std::initializer_list<TablePosition> poses)
    { set_selected_poses(std::set<TablePosition, TablePositionCompare>(poses)); }

    void set_selected_poses(const std::list<TablePosition> &poses)
    { set_selected_poses(std::set<TablePosition, TablePositionCompare>(poses.begin(), poses.end())); }
    
    void set_selected_poses(const std::vector<TablePosition> &poses)
    { set_selected_poses(std::set<TablePosition, TablePositionCompare>(poses.begin(), poses.end())); }

    void set_selected_poses(const std::set<TablePosition, TablePositionCompare> &poses);

    const std::shared_ptr<TableAdapter> &adapter() const
    { return _M_adapter; }

    void set_adapter(TableAdapter *adapter)
    { _M_adapter = std::shared_ptr<TableAdapter>(adapter); }

    void set_adapter(const std::shared_ptr<TableAdapter> &adapter)
    { _M_adapter = adapter; }

    void select_all();
    
    void change_selection(std::size_t row, std::size_t column)
    { change_selection(Range<std::size_t>(row, row + 1), Range<std::size_t>(column, column + 1)); }

    void change_selection(const Range<std::size_t> &row_range, const Range<std::size_t> &column_range);

    void clear_selection();

    const OnTableSelectionListener &on_table_selection_listener() const
    { return _M_on_table_selection_listener; }

    void set_on_table_selection_listener(const OnTableSelectionListener &listener)
    { _M_on_table_selection_listener = listener; }

    virtual void draw(Canvas *canvas);

    virtual void on_touch(const Pointer &pointer, const Point<double> &point, TouchState state);

    virtual void on_key(std::uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state);

    virtual void on_table_selection(const std::set<TablePosition, TablePositionCompare> &poses);
  };

  class Tree : public Widget
  {
    struct ExtendedNode
    {
      std::size_t index;
      std::list<ExtendedNode> extended_childs;
    };

    SelectionMode _M_selection_mode;
    std::set<TreePath, TreePathCompare> _M_selected_paths;
    std::shared_ptr<TreeAdapter> _M_adapter;
    std::list<ExtendedNode> _M_extended_nodes;
    std::size_t _M_row_count;
    OnTreeSelectionListener _M_on_tree_selection_listener;
  protected:
    Tree(Unused unused) {}
  public:
    Tree()
    { initialize(SelectionMode::SINGLE, std::shared_ptr<TreeAdapter>(new StringTreeAdapter())); }

    explicit Tree(std::initializer_list<StringTreeNode> nodes)
    { initialize(SelectionMode::SINGLE, std::shared_ptr<TreeAdapter>(new StringTreeAdapter(nodes))); }

    explicit Tree(TreeAdapter *adapter)
    { initialize(SelectionMode::SINGLE, std::shared_ptr<TreeAdapter>(adapter)); }

    explicit Tree(const std::shared_ptr<TreeAdapter> &adapter)
    { initialize(SelectionMode::SINGLE, adapter); }

    explicit Tree(SelectionMode selection_mode)
    { initialize(selection_mode, std::shared_ptr<TreeAdapter>(new StringTreeAdapter())); }

    Tree(SelectionMode selection_mode, std::initializer_list<StringTreeNode> nodes)
    { initialize(selection_mode, std::shared_ptr<TreeAdapter>(new StringTreeAdapter(nodes))); }

    Tree(SelectionMode selection_mode, TreeAdapter *adapter)
    { initialize(selection_mode, std::shared_ptr<TreeAdapter>(adapter)); }

    Tree(SelectionMode selection_mode, const std::shared_ptr<TreeAdapter> &adapter)
    { initialize(selection_mode, adapter); }

    virtual ~Tree();
  protected:
    void initialize(SelectionMode selection_mode, const std::shared_ptr<TreeAdapter> &adapter);
  public:
    SelectionMode selection_mode() const
    { return _M_selection_mode; }

    void set_selection_mode(SelectionMode mode)
    { _M_selection_mode = mode; }

    const std::set<TreePath, TreePathCompare> &selected_paths() const
    { return _M_selected_paths; }

    void set_selected_paths(std::initializer_list<TreePath> paths)
    { set_selected_paths(std::set<TreePath, TreePathCompare>(paths)); }
    
    void set_selected_paths(const std::list<TreePath> &paths)
    { set_selected_paths(std::set<TreePath, TreePathCompare>(paths.begin(), paths.end())); }
    
    void set_selected_paths(const std::vector<TreePath> &paths)
    { set_selected_paths(std::set<TreePath, TreePathCompare>(paths.begin(), paths.end())); }

    void set_selected_paths(const std::set<TreePath, TreePathCompare> &paths);

    const std::shared_ptr<TreeAdapter> &adapter() const
    { return _M_adapter; }

    void set_adapter(TreeAdapter *adapter)
    { _M_adapter = std::shared_ptr<TreeAdapter>(adapter); }

    void set_adapter(const std::shared_ptr<TreeAdapter> &adapter)
    { _M_adapter = adapter; }
    
    const OnTreeSelectionListener &on_tree_selection_listener() const
    { return _M_on_tree_selection_listener; }

    void set_on_tree_selection_listener(const OnTreeSelectionListener &listener)
    { _M_on_tree_selection_listener = listener; }

    virtual void draw(Canvas *canvas);

    virtual void on_touch(const Pointer &pointer, const Point<double> &point, TouchState state);

    virtual void on_key(std::uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state);

    virtual void on_table_selection(const std::set<TreePath, TreePathCompare> &paths);
  };

  class Scroll : public Widget
  {
    std::unique_ptr<Widget> _M_widget;
    bool _M_has_h_scroll_bar;
    bool _M_has_v_scroll_bar;
  protected:
    Scroll() {}
  public:
    explicit Scroll(Widget *widget);

    virtual ~Scroll();
  protected:
    void initialize(Widget *widget);
  public:
    Widget *widget() const
    { return _M_widget.get(); }

    void set_widget(Widget *widget)
    { _M_widget = std::unique_ptr<Widget>(widget); }

    bool has_h_scroll_bar() const
    { return _M_has_h_scroll_bar; }

    void set_h_scroll_bar(bool is_scroll_bar)
    { _M_has_h_scroll_bar = is_scroll_bar; }

    bool has_v_scroll_bar() const
    { return _M_has_v_scroll_bar; }

    void set_v_scroll_bar(bool is_scroll_bar)
    { _M_has_v_scroll_bar = is_scroll_bar; }

    virtual void draw(Canvas *canvas);

    virtual void on_touch(const Pointer &pointer, const Point<double> &point, TouchState state);

    virtual void on_pointer_motion(const Point<double> &point);

    virtual void on_pointer_axis(Axis axis, double value);
  };

  class MenuItem : public virtual Button
  {
  protected:
    MenuItem() {}
  public:
    explicit MenuItem(const std::string &label)
    { initialize(Icon(), label, [](Widget *widget) {}); } 

    MenuItem(const std::string &label, const OnClickListener &listener)
    { initialize(Icon(), label, listener); }
    
    MenuItem(const Icon &icon, const std::string &label)
    { initialize(icon, label, [](Widget *widget) {}); } 

    MenuItem(const Icon &icon, const std::string &label, const OnClickListener &listener)
    { initialize(icon, label, listener); }
    virtual ~MenuItem();

    virtual void draw(Canvas *canvas);
  };

  class CheckMenuItem : public MenuItem, public CheckBox
  {
  protected:
    CheckMenuItem() :
      Button(), MenuItem(), CheckBox(Unused()) {}
  public:
    explicit CheckMenuItem(const std::string &label) :
      Button(), MenuItem(), CheckBox(Unused())
    { this->CheckBox::initialize(Icon(), label, false); }

    CheckMenuItem(const std::string &label, bool is_checked) :
      Button(), MenuItem(), CheckBox(Unused())
    { this->CheckBox::initialize(Icon(), label, is_checked); }

    CheckMenuItem(const Icon &icon, const std::string &label) :
      Button(), MenuItem(), CheckBox(Unused())
    { this->CheckBox::initialize(icon, label, false); }

    CheckMenuItem(const Icon &icon, const std::string &label, bool is_checked) :
      Button(), MenuItem(), CheckBox(Unused())
    { this->CheckBox::initialize(icon, label, is_checked); }

    virtual ~CheckMenuItem();

    virtual void draw(Canvas *canvas);
  };

  class RadioMenuItem : public MenuItem, public RadioButton
  {
  protected:
    RadioMenuItem() :
      Button(), MenuItem(), RadioButton(Unused()) {}
  public:
    explicit RadioMenuItem(const std::string &label) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(Icon(), label, false, std::shared_ptr<RadioGroup>()); }

    RadioMenuItem(const std::string &label, bool is_checked) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(Icon(), label, is_checked, std::shared_ptr<RadioGroup>()); }

    RadioMenuItem(const std::string &label, const std::shared_ptr<RadioGroup> &group) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(Icon(), label, false, group); }

    RadioMenuItem(const std::string &label, bool is_checked, const std::shared_ptr<RadioGroup> &group) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(Icon(), label, false, group); }

    RadioMenuItem(const Icon &icon, const std::string &label) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(icon, label, false, std::shared_ptr<RadioGroup>()); }

    RadioMenuItem(const Icon &icon, const std::string &label, bool is_checked) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(icon, label, is_checked, std::shared_ptr<RadioGroup>()); }

    RadioMenuItem(const Icon &icon, const std::string &label, const std::shared_ptr<RadioGroup> &group) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(icon,label, false, group); }

    RadioMenuItem(const Icon &icon, const std::string &label, bool is_checked, const std::shared_ptr<RadioGroup> &group) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(icon, label, false, group); }

    virtual ~RadioMenuItem();

    virtual void draw(Canvas *canvas);
  };

  class SeparatorMenuItem : public MenuItem
  {
  protected:
    SeparatorMenuItem(Unused unused) :
      Button(), MenuItem() {}
  public:
    SeparatorMenuItem() :
      Button(), MenuItem()
    { initialize(Icon(), "", [](Widget *widget) {}); }

    virtual ~SeparatorMenuItem();

    virtual void draw(Canvas *canvas);
  };

  class Menu : public MenuItem
  {
    std::list<std::unique_ptr<MenuItem>> _M_menu_items;
    Menu() :
      Button(), MenuItem() {}
  public:
    explicit Menu(const std::string &label) :
      Button(), MenuItem()
    { initialize(Icon(), label, {}); }

    Menu(const std::string &label, std::initializer_list<MenuItem *> menu_items) :
      Button(), MenuItem()
    { initialize(Icon(), label, menu_items); }

    Menu(const Icon &icon, const std::string &label) :
      Button(), MenuItem()
    { initialize(icon, label, {}); }

    Menu(const Icon &icon, const std::string &label, std::initializer_list<MenuItem *> menu_items) :
      Button(), MenuItem()
    { initialize(icon, label, menu_items); }

    virtual ~Menu();
  protected:
    void initialize(const Icon &icon, const std::string &label, std::initializer_list<MenuItem *> menu_items);
  public:
    const std::list<std::unique_ptr<MenuItem>> &menu_items() const
    { return _M_menu_items; }

    void add_menu_item(MenuItem *menu_item)
    { _M_menu_items.push_back(std::unique_ptr<MenuItem>(menu_item));}

    void delete_menu_item(MenuItem *menu_item);

    void delete_all_menu_items()
    { _M_menu_items.clear(); }

    virtual void draw(Canvas *canvas);
  };

  class MenuBar : public Widget
  {
    std::list<std::unique_ptr<Menu>> _M_menus;
  protected:
    MenuBar(Unused unused) {}
  public:
    MenuBar()
    { initialize({}); }

    explicit MenuBar(std::initializer_list<Menu *> menus)
    { initialize(menus); }

    virtual ~MenuBar();
  protected:
    void initialize(std::initializer_list<Menu *> menus);
  public:
    const std::list<std::unique_ptr<Menu>> &menus() const
    { return _M_menus; }

    void add_menu(Menu *menu)
    { _M_menus.push_back(std::unique_ptr<Menu>(menu)); }

    void delete_menu(Menu *menu);

    void delete_all_menus()
    { _M_menus.clear(); }

    virtual void draw(Canvas *canvas);    
  };
}

#endif
