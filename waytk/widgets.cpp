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
#include <exception>
#include <waytk.hpp>

using namespace std;

namespace waytk
{
  //
  // A Widget class.
  //

  Widget::Widget() :
    _M_is_enabled(true),
    _M_flags(WidgetFlags::NONE),
    _M_has_focus(false),
    _M_is_visible(true),
    _M_max_width(0),
    _M_max_height(0),
    _M_min_width(0),
    _M_min_height(0),
    _M_bounds(0, 0, 0, 0),
    _M_surface(nullptr),
    _M_parent(nullptr),
    _M_on_touch_listener([](Widget *widget, const Pointer &pointer, const Point<double> &point, TouchState &state) {}),
    _M_on_pointer_motion_listener([](Widget *widget, const Point<double> &point) {}),
    _M_on_pointer_axis_listener([](Widget *widget, Axis axis, double value) {}),
    _M_on_key_listener([](Widget *widget, uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state) {}),
    _M_on_scroll_listener([](Widget *widgwet, Viewport *viewport) {}) {}

  Widget::~Widget() {}

  void Widget::draw(Canvas *canvas) {}

  Viewport *Widget::viewport() {}
    
  void Widget::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state) {}

  void Widget::on_pointer_motion(const Point<double> &point) {}

  void Widget::on_pointer_axis(Axis axis, double value) {}

  void Widget::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state) {}

  void Widget::on_scoll(Viewport *viewport) {}

  //
  // A Container class.
  //

  Container::~Container() {}

  void Container::initialize(initializer_list<Widget *> widgets)
  {
    _M_widgets.clear();
    for(auto widget : widgets) {
      _M_widgets.push_back(unique_ptr<Widget>(widget));
    }
  }

  void Container::delete_widget(Widget *widget)
  {
    auto iter = find_if(_M_widgets.begin(), _M_widgets.end(), [widget](const unique_ptr<Widget> &tmp_widget) {
      return tmp_widget.get() == widget;
    });
    if(iter != _M_widgets.end()) _M_widgets.erase(iter);
  }

  //
  // A Label class.
  //

  Label::~Label() {}

  void Label::initialize(const string &text)
  { normalize_utf8(text, _M_text); }

  void Label::set_text(const string &text)
  { normalize_utf8(text, _M_text); }

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
    _M_on_click_listener = listener;
  }

  void Button::set_label(const string &label)
  { normalize_utf8(label, _M_label); }

  void Button::draw(Canvas *canvas)
  { throw exception(); }

  void Button::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  { throw exception(); }

  void Button::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  { throw exception(); }

  void Button::on_click()
  { throw exception(); }

  //
  // A Text class.
  //

  Text::~Text() {}

  void Text::initialize(InputType input_type, const string &text)
  { throw exception(); }

  void Text::set_text(const string &text)
  { throw exception(); }

  void Text::set_cursor_iter(const TextCharIterator &iter)
  { throw exception(); }

  void Text::set_selection_range(const Range<TextCharIterator> &range)
  { throw exception(); }

  void Text::insert_string(const string &str)
  { throw exception(); }

  void Text::replace_string(size_t count, const string &str)
  { throw exception(); }

  void Text::delete_chars(size_t count)
  { throw exception(); }

  void Text::copy()
  { throw exception(); }

  void Text::paste()
  { throw exception(); }

  void Text::draw(Canvas *canvas)
  { throw exception(); }

  Viewport *Text::viewport()
  { throw exception(); }

  void Text::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  { throw exception(); }

  void Text::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  { throw exception(); }

  void Text::on_text_change(const Range<TextCharIterator> &range)
  { throw exception(); }

  void Text::on_cursor_change(const TextCharIterator &iter, const TextPosition &pos)
  { throw exception(); }

  void Text::on_text_selection(const Range<TextCharIterator> &range)
  { throw exception(); }
  
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
  { throw exception(); }
  
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

  void RadioButton::draw(Canvas *canvas)
  { throw exception(); }

  void RadioButton::on_check(bool is_checked)
  {
    for(auto radio_button : _M_group->radio_buttons()) {
      if(radio_button != this) radio_button->set_checked(false);
    }
    this->CheckBox::on_check(is_checked);
  }

  //
  // A ComboBox class.
  //

  ComboBox::~ComboBox() {}

  void ComboBox::initialize(const shared_ptr<ComboBoxAdapter> &adapter)
  {
    this->Button::initialize(Icon(), string(), [](Widget *widget) {});
    _M_selected_pos = 0;
    _M_adapter = adapter;
    _M_on_selection_listener = [](Widget *widget, size_t pos) {};
  }

  void ComboBox::set_selected_pos(size_t pos)
  {
    _M_selected_pos = (pos < _M_adapter->item_count() ? pos : _M_adapter->item_count());
    on_selection(_M_selected_pos);
  }

  void ComboBox::draw(Canvas &canvas)
  { throw exception(); }

  void ComboBox::on_click()
  { throw exception(); }

  void ComboBox::on_selection(size_t pos)
  { throw exception(); }

  //
  // A ProgressBar class.
  //

  ProgressBar::~ProgressBar() {}

  void ProgressBar::initialize(int max_value)
  {
    _M_max_value = max_value;
    _M_value = 0;
  }

  //
  // An Image class.
  //

  Image::~Image() {}

  void Image::initialize(const shared_ptr<CanvasImage> &image)
  { _M_image = image; }

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

  //
  // A GridPanel class.
  //

  GridPanel::~GridPanel() {}

  void GridPanel::initialize(int column_count, initializer_list<Widget *> widgets)
  {
    this->Panel::initialize(widgets);
    _M_column_count = column_count;
  }

  //
  // A SplitPane class.
  //

  SplitPane::~SplitPane() {}

  void SplitPane::initialize(Widget *first_widget, Widget *second_widget)
  {
    _M_first_widget = unique_ptr<Widget>(first_widget);
    _M_second_widget = unique_ptr<Widget>(second_widget);
  }

  //
  // A List class.
  //

  List::~List() {}

  void List::initialize(SelectionMode mode, const shared_ptr<ListAdapter> &adapter)
  {
    _M_selection_mode = mode;
    _M_selected_poses.clear();
    _M_adapter = adapter;
    _M_on_list_selection_listener = [](Widget *widget, const set<size_t> &poses) {};
  }

  void List::set_selected_poses(const set<size_t> &poses)
  { throw exception(); }

  void List::select_all()
  { throw exception(); }

  void List::clear_selection()
  { throw exception(); }

  void List::draw(Canvas *canvas)
  { throw exception(); }

  void List::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  { throw exception(); }

  void List::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  { throw exception(); }

  void List::on_list_selection(const set<size_t> &poses)
  { throw exception(); }

  //
  // A Table class.
  //

  Table::~Table() {}

  void Table::initialize(SelectionMode mode, initializer_list<string> header_labels, const shared_ptr<TableAdapter> &adapter)
  {
    _M_selection_mode = mode;
    _M_selected_poses.clear();
    _M_adapter = adapter;
    _M_on_table_selection_listener = [](Widget *widget, const set<TablePosition, TablePositionCompare> &poses) {};
  }

  void Table::set_selected_poses(const set<TablePosition, TablePositionCompare> &poses)
  { throw exception(); }

  void Table::select_all()
  { throw exception(); }

  void Table::clear_selection()
  { throw exception(); }

  void Table::draw(Canvas *canvas)
  { throw exception(); }

  void Table::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  { throw exception(); }

  void Table::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  { throw exception(); }

  void Table::on_table_selection(const set<TablePosition, TablePositionCompare> &poses)
  { throw exception(); }

  //
  // A Tree class.
  //

  Tree::~Tree() {}

  void Tree::initialize(SelectionMode mode, const shared_ptr<TreeAdapter> &adapter)
  {
    _M_selection_mode = mode;
    _M_selected_paths.clear();
    _M_adapter = adapter;
    _M_on_tree_selection_listener = [](Widget *widget, const set<TreePath, TreePathCompare> &paths) {}; 
  }

  void Tree::set_selected_paths(const set<TreePath, TreePathCompare> &paths)
  { throw exception(); }

  void Tree::draw(Canvas *canvas)
  { throw exception(); }

  void Tree::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  { throw exception(); }

  void Tree::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  { throw exception(); }

  void Tree::on_table_selection(const set<TreePath, TreePathCompare> &paths)
  { throw exception(); }

  //
  // A Scroll class.
  //

  Scroll::~Scroll() {}

  void Scroll::initialize(Widget *widget)
  { _M_widget = unique_ptr<Widget>(widget); }

  void Scroll::draw(Canvas *canvas)
  { throw exception(); }  

  void Scroll::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  { throw exception(); }

  void Scroll::on_pointer_motion(const Point<double> &point)
  { throw exception(); }

  void Scroll::on_pointer_axis(Axis axis, double value)
  { throw exception(); }
  
  //
  // A MenuItem class.
  //

  MenuItem::~MenuItem() {}

  void MenuItem::draw(Canvas *canvas)
  { throw exception(); }

  //
  // A CheckMenuItem class.
  //

  CheckMenuItem::~CheckMenuItem() {}

  void CheckMenuItem::draw(Canvas *canvas)
  { throw exception(); }

  //
  // A RadioMenuItem class.
  //

  RadioMenuItem::~RadioMenuItem() {}

  void RadioMenuItem::draw(Canvas *canvas)
  { throw exception(); }

  //
  // A SeperatorMenuItem class.
  //

  SeparatorMenuItem::~SeparatorMenuItem() {}

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
    }
  }

  void Menu::delete_menu_item(MenuItem *menu_item)
  {
    auto iter = find_if(_M_menu_items.begin(), _M_menu_items.end(), [menu_item](const unique_ptr<MenuItem> &tmp_menu_item) {
      return tmp_menu_item.get() == menu_item;
    });
    if(iter != _M_menu_items.end()) _M_menu_items.erase(iter);
  }

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
    }
  }

  void MenuBar::delete_menu(Menu *menu)
  {
    auto iter = find_if(_M_menus.begin(), _M_menus.end(), [menu](const unique_ptr<Menu> &tmp_menu) {
      return tmp_menu.get() == menu;
    });
    if(iter != _M_menus.end()) _M_menus.erase(iter);
  }

  void MenuBar::draw(Canvas *canvas)
  { throw exception(); }
}
