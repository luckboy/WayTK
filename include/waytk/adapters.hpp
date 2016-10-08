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
#ifndef _WAYTK_ADAPTERS_HPP
#define _WAYTK_ADAPTERS_HPP

#include <cstddef>
#include <initializer_list>
#include <list>
#include <string>
#include <vector>
#include <waytk/structs.hpp>

namespace waytk
{
  class Widget;

  class ComboBoxAdapter
  {
  protected:
    ComboBoxAdapter() {}
  public:
    virtual ~ComboBoxAdapter();

    virtual Widget *widget(std::size_t pos) const = 0;

    virtual std::size_t item_count() const = 0;
  };

  class StringComboBoxAdapter : public ComboBoxAdapter
  {
    std::vector<std::string> _M_items;
  public:
    StringComboBoxAdapter() {}

    explicit StringComboBoxAdapter(std::initializer_list<std::string> items) :
      _M_items(items) {}

    explicit StringComboBoxAdapter(const std::vector<std::string> &items) :
      _M_items(items) {}

    virtual ~StringComboBoxAdapter();
      
    virtual Widget *widget(std::size_t pos) const;
    
    virtual std::size_t item_count() const;
    
    const std::vector<std::string> &items() const
    { return _M_items; }

    std::vector<std::string> &items()
    { return _M_items; }
  };

  class ListAdapter
  {
  protected:
    ListAdapter() {}
  public:
    virtual ~ListAdapter();

    virtual Widget *widget(std::size_t pos) const = 0;

    virtual Widget *udpate_widget(Widget *widget, std::size_t pos, bool is_selected) const;

    virtual std::size_t item_count() const = 0;
  };

  class StringListAdapter : public ListAdapter
  {
    std::vector<std::string> _M_items;
  public:
    StringListAdapter() {}

    explicit StringListAdapter(std::initializer_list<std::string> items) :
      _M_items(items) {}

    explicit StringListAdapter(const std::vector<std::string> &items) :
      _M_items(items) {}

    virtual ~StringListAdapter();

    virtual Widget *widget(std::size_t pos) const;

    virtual std::size_t item_count() const;
    
    const std::vector<std::string> &items() const
    { return _M_items; }

    std::vector<std::string> &items()
    { return _M_items; }
  };

  class TableAdapter
  {
  protected:
    TableAdapter() {}
  public:
    virtual ~TableAdapter();

    virtual Widget *widget(const TablePosition &pos) const = 0;

    virtual Widget *update_widget(Widget *widget, const TablePosition &pos, bool is_selected) const;

    virtual std::size_t row_count() const = 0;

    virtual std::size_t column_count() const = 0;
  };

  class StringTableAdapter : public TableAdapter
  {
    std::size_t _M_column_count;
    std::vector<std::vector<std::string>> _M_cells;
  public:
    StringTableAdapter(std::size_t column_count) :
      _M_column_count(column_count) {}

    StringTableAdapter(std::initializer_list<std::initializer_list<std::string>> cells);

    StringTableAdapter(std::size_t column_count, const std::vector<std::vector<std::string>> cells) :
      _M_column_count(column_count), _M_cells(cells) {}

    virtual ~StringTableAdapter();

    virtual Widget *widget(const TablePosition &pos) const;

    virtual std::size_t row_count() const;

    virtual std::size_t column_count() const;

    const std::vector<std::vector<std::string>> &cells() const
    { return _M_cells; }

    std::vector<std::vector<std::string>> &cells()
    { return _M_cells; }
  };

  class TreeAdapter
  {
  protected:
    TreeAdapter() {}
  public:
    virtual ~TreeAdapter();

    virtual Widget *widget(const TreePath &path) const = 0;

    virtual Widget *update_widget(Widget *widget, const TreePath &path, bool is_selected) const;

    virtual std::size_t child_count(const TreePath &path) const = 0;

    std::size_t root_count() const
    { return child_count(TreePath()); }
  };

  struct StringTreeNode
  {
    std::string value;
    std::vector<StringTreeNode> childs;
      
    StringTreeNode(const std::string &value) :
      value(value) {}

    StringTreeNode(const std::string &value, std::initializer_list<StringTreeNode> childs) :
      value(value), childs(childs) {}

    StringTreeNode(const std::string &value, const std::vector<StringTreeNode> &childs) :
      value(value), childs(childs) {}
  };

  class StringTreeAdapter : public TreeAdapter
  {
    std::vector<StringTreeNode> _M_nodes;
  public:
    StringTreeAdapter() {}
    
    explicit StringTreeAdapter(std::initializer_list<StringTreeNode> nodes) :
      _M_nodes(nodes) {}

    explicit StringTreeAdapter(const std::vector<StringTreeNode> &nodes) :
      _M_nodes(nodes) {}

    virtual ~StringTreeAdapter();
    
    virtual Widget *widget(const TreePath &path) const;

    virtual std::size_t child_count(const TreePath &path) const;

    const std::vector<StringTreeNode> &nodes() const
    { return _M_nodes; }

    std::vector<StringTreeNode> &nodes()
    { return _M_nodes; }
  private:
    const StringTreeNode &node(const TreePath &path) const;
  };
}

#endif
