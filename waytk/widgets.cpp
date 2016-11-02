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
#include <waytk.hpp>
#include "text_buffer.hpp"

using namespace std;

namespace waytk
{
  //
  // An Icon class.
  //

  shared_ptr<CanvasImage> Icon::image() const
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
    _M_flags(WidgetFlags::NONE),
    _M_has_focus(false),
    _M_is_visible(true),
    _M_h_align(HAlignment::CENTER),
    _M_v_align(VAlignment::CENTER),
    _M_max_width(0),
    _M_max_height(0),
    _M_min_width(0),
    _M_min_height(0),
    _M_bounds(0, 0, 0, 0),
    _M_parent(nullptr),
    _M_on_touch_callback([](Widget *widget, const Pointer &pointer, const Point<double> &point, TouchState state) {}),
    _M_on_pointer_motion_callback([](Widget *widget, const Point<double> &point) {}),
    _M_on_pointer_leave_callback([](Widget *widget) {}),
    _M_on_pointer_axis_callback([](Widget *widget, Axis axis, double value) {}),
    _M_on_key_callback([](Widget *widget, uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state) {}),
    _M_on_scroll_callback([](Widget *widgwet, Viewport *viewport) {}) {}

  Widget::~Widget() {}

  void Widget::set_focus(bool has_focus)
  { throw exception(); }

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

  Edges<int> Widget::margin() const
  { throw exception(); }

  const char *Widget::name() const
  { return "widget"; }

  void Widget::draw(Canvas *canvas)
  { throw exception(); }

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
    }
    if(pointer.is_touch()) _M_flags |= WidgetFlags::HOVER;
    _M_on_touch_callback(this, pointer, point, state);
    return false;
  }

  void Widget::on_touch_leave(const Pointer &pointer)
  {
    delete_pointer(pointer);
    _M_on_touch_leave_callback(this, pointer);
  }

  bool Widget::on_pointer_motion(const Point<double> &point)
  {
    _M_flags |= WidgetFlags::HOVER;
    _M_on_pointer_motion_callback(this, point);
    return false;
  }

  void Widget::on_pointer_leave()
  {
    _M_flags &= ~WidgetFlags::HOVER;
    _M_on_pointer_leave_callback(this);
  }

  bool Widget::on_pointer_axis(Axis axis, double value)
  { _M_on_pointer_axis_callback(this, axis, value); return false; }

  bool Widget::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  { _M_on_key_callback(this, key_sym, modifiers, utf8, state); return false; }

  void Widget::on_scoll(Viewport *viewport)
  { _M_on_scroll_callback(this, viewport); }

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

  void Label::draw(Canvas *canvas)
  { throw exception(); }

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
  }

  void Button::set_label(const string &label)
  { normalize_utf8(label, _M_label); }

  const char *Button::name() const
  { return "button"; }

  void Button::draw(Canvas *canvas)
  { throw exception(); }

  bool Button::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  {
    bool had_pointer = has_pointer(pointer);
    this->Widget::on_touch(pointer, point, state);
    switch(state) {
      case TouchState::DOWN:
        if(!had_pointer) {
          set_flags(flags() | WidgetFlags::ACTIVE);
          _M_touch_count++;
        }
        break;
      case TouchState::UP:
        if(had_pointer) {
          set_flags(flags() & ~WidgetFlags::ACTIVE);
          if(_M_touch_count > 0) _M_touch_count--;
          on_click();
        }
        break;
    }
    return true;
  }

  void Button::on_touch_leave(const Pointer &pointer)
  {
    bool had_pointer = has_pointer(pointer);
    this->Widget::on_touch_leave(pointer);
    if(had_pointer) {
      set_flags(flags() & ~WidgetFlags::ACTIVE);
      if(_M_touch_count > 0) _M_touch_count--;
    }
  }

  bool Button::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  {
    this->Widget::on_key(key_sym, modifiers, utf8, state);
    if((strcmp(utf8, "\n") == 0 || strcmp(utf8, " ") == 0)) {
      switch(state) {
        case KeyState::PRESSED:
          if(_M_touch_count == 0) set_flags(flags() | WidgetFlags::ACTIVE);
          break;
        case KeyState::REPEATED:
          on_click();
          break;
        case KeyState::RELEASED:
          if(_M_touch_count == 0) set_flags(flags() & ~WidgetFlags::ACTIVE);
          on_click();
      }
      return true;
    } else
      return false;
  }

  void Button::on_click()
  { _M_on_click_callback(this); }

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
  }

  void CheckBox::set_checked(bool is_checked)
  {
    if(_M_is_checked != is_checked) {
      _M_is_checked = is_checked;
      on_check(_M_is_checked);
    }
  }

  const char *CheckBox::name() const
  { return "check_box"; }

  void CheckBox::draw(Canvas *canvas)
  { throw exception(); }  

  void CheckBox::on_click()
  {
    this->Button::on_click();
    set_checked(!_M_is_checked);
  }

  void CheckBox::on_check(bool is_checked)
  { _M_on_check_callback(this, is_checked); }

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

  void RadioButton::draw(Canvas *canvas)
  { throw exception(); }

  void RadioButton::on_check(bool is_checked)
  {
    for(auto radio_button : _M_group->radio_buttons()) {
      if(radio_button != this) radio_button->set_checked(false);
    }
    this->CheckBox::on_check(is_checked);
  }

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

  void ProgressBar::draw(Canvas &canvas)
  { throw exception(); }

  //
  // An Image class.
  //

  Image::~Image() {}

  void Image::initialize(const shared_ptr<CanvasImage> &image)
  { _M_image = image; }

  const char *Image::name() const
  { return "image"; }

  void Image::draw(Canvas &canvas)
  { throw exception(); }

  //
  // A Panel class.
  //

  Panel::~Panel() {}

  //
  // A LinearPanel class.
  //

  void LinearPanel::initialize(Orientation orientation, initializer_list<Widget *> widgets)
  {
    this->Panel::initialize(widgets);
    _M_orientation = orientation;
  }

  void LinearPanel::draw(Canvas *canvas)
  { throw exception(); }

  const char *LinearPanel::name() const
  { return "linear_panel"; }

  //
  // A GridPanel class.
  //

  GridPanel::~GridPanel() {}

  void GridPanel::initialize(int column_count, initializer_list<Widget *> widgets)
  {
    this->Panel::initialize(widgets);
    _M_column_count = column_count;
  }

  const char *GridPanel::name() const
  { return "grid_panel"; }

  void GridPanel::draw(Canvas *canvas)
  { throw exception(); }

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

  void SplitPane::draw(Canvas *canvas)
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

  void List::draw(Canvas *canvas)
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

  void Table::draw(Canvas *canvas)
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

  void Tree::draw(Canvas *canvas)
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

  void Scroll::draw(Canvas *canvas)
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

  void MenuItem::draw(Canvas *canvas)
  { throw exception(); }

  //
  // A CheckMenuItem class.
  //

  CheckMenuItem::~CheckMenuItem() {}

  const char *CheckMenuItem::name() const
  { return "check_menu_item"; }

  void CheckMenuItem::draw(Canvas *canvas)
  { throw exception(); }

  //
  // A RadioMenuItem class.
  //

  RadioMenuItem::~RadioMenuItem() {}

  const char *RadioMenuItem::name() const
  { return "radio_menu_item"; }

  void RadioMenuItem::draw(Canvas *canvas)
  { throw exception(); }

  //
  // A SeperatorMenuItem class.
  //

  SeparatorMenuItem::~SeparatorMenuItem() {}

  const char *SeparatorMenuItem::name() const
  { return "separator_menu_item"; }

  void SeparatorMenuItem::draw(Canvas *canvas)
  { throw exception(); }

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

  void Menu::draw(Canvas *canvas)
  { throw exception(); }

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

  void MenuBar::draw(Canvas *canvas)
  { throw exception(); }
}
