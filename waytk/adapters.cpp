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
#include <waytk.hpp>

using namespace std;

namespace waytk
{
  //
  // A ComboBoxAdapter class.
  //

  ComboBoxAdapter::~ComboBoxAdapter() {}

  //
  // A StringComboBoxAdapter class.
  //

  StringComboBoxAdapter::~StringComboBoxAdapter() {}

  Widget *StringComboBoxAdapter::widget(size_t pos) const
  { return new Label(_M_items[pos]); }

  size_t StringComboBoxAdapter::item_count() const
  { return _M_items.size(); }

  //
  // A ListAdapter class.
  //

  ListAdapter::~ListAdapter() {}

  Widget *ListAdapter::udpate_widget(Widget *widget, size_t pos, bool is_selected) const
  {
    WidgetFlags flags = widget->flags();
    widget->set_flags(flags | (is_selected ? WidgetFlags::SELECTED : WidgetFlags::NONE));
    return widget;
  }

  //
  // A StringListAdapter class.
  //

  StringListAdapter::~StringListAdapter() {}

  Widget *StringListAdapter::widget(size_t pos) const
  { return new Label(_M_items[pos]); }
    
  size_t StringListAdapter::item_count() const
  { return _M_items.size(); }

  //
  // A TableAdapter class.
  //

  TableAdapter::~TableAdapter() {}

  Widget *TableAdapter::update_widget(Widget *widget, const TablePosition &pos, bool is_selected) const
  {
    WidgetFlags flags = widget->flags();
    widget->set_flags(flags | (is_selected ? WidgetFlags::SELECTED : WidgetFlags::NONE));
    return widget;
  }

  //
  // A StringTableAdapter class. 
  //

  StringTableAdapter::StringTableAdapter(initializer_list<initializer_list<std::string>> cells)
  {
    for(auto &row : cells) {
      vector<string> tmp_row;
      for(auto cell : row) tmp_row.push_back(cell);
      _M_cells.push_back(tmp_row);
    }
    _M_column_count = (!_M_cells.empty() ?_M_cells[0].size() : 0);
  }

  StringTableAdapter::~StringTableAdapter() {}

  Widget *StringTableAdapter::widget(const TablePosition &pos) const
  { return new Label(_M_cells[pos.row][pos.column]); }

  size_t StringTableAdapter::row_count() const
  { return _M_cells.size(); }

  size_t StringTableAdapter::column_count() const
  { return _M_column_count; }

  //
  // A TreeAdapter class.
  //

  TreeAdapter::~TreeAdapter() {}

  Widget *TreeAdapter::update_widget(Widget *widget, const TreePath &path, bool is_selected) const
  {
    WidgetFlags flags = widget->flags();
    widget->set_flags(flags | (is_selected ? WidgetFlags::SELECTED : WidgetFlags::NONE));
    return widget;
  }

  //
  // A StringTreeAdapter class.
  //

  StringTreeAdapter::~StringTreeAdapter() {}

  Widget *StringTreeAdapter::widget(const TreePath &path) const
  { return new Label(node(path).value); }

  size_t StringTreeAdapter::child_count(const TreePath &path) const
  { return node(path).childs.size(); }

  const StringTreeNode &StringTreeAdapter::node(const TreePath &path) const
  {
    const vector<StringTreeNode> *tmp_nodes = &_M_nodes;
    vector<StringTreeNode>::const_iterator tmp_node_iter;
    for(auto &node_idx : path.nodes) {
      tmp_nodes = &((*tmp_nodes)[node_idx].childs);
      tmp_node_iter = (*tmp_nodes).begin() + node_idx;
    }
    return *tmp_node_iter;
  }
}
