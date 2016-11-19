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

  ///
  /// A class of combo box adapter that is base for all combo box adapters.
  ///
  /// A combo box adapter adapts a data to a target interface that is this
  /// class. A combo box uses the target interface to display the data.
  ///
  class ComboBoxAdapter
  {
  protected:
    /// Default constructor.
    ComboBoxAdapter() {}
  public:
    /// Destructor.
    virtual ~ComboBoxAdapter();

    /// Creates a new item widget.
    virtual Widget *widget(std::size_t pos) const = 0;

    /// Returns the number of the items.
    virtual std::size_t item_count() const = 0;
  };

  ///
  /// A combo box adapter that adapts a string vector.
  ///
  class StringComboBoxAdapter : public ComboBoxAdapter
  {
    std::vector<std::string> _M_items;
  public:
    /// Default constructor.
    StringComboBoxAdapter() {}

    /// Constructor.
    explicit StringComboBoxAdapter(std::initializer_list<std::string> items) :
      _M_items(items) {}

    /// Constructor.
    explicit StringComboBoxAdapter(const std::vector<std::string> &items) :
      _M_items(items) {}

    /// Destructor.
    virtual ~StringComboBoxAdapter();
      
    virtual Widget *widget(std::size_t pos) const;
    
    virtual std::size_t item_count() const;

    /// Returns the string vector.
    const std::vector<std::string> &items() const
    { return _M_items; }

    /// \copydoc items() const
    std::vector<std::string> &items()
    { return _M_items; }
  };

  ///
  /// A class of list adapter that is base for all list adapters.
  ///
  /// A list adapter adapts a data to an target interface that is this class. A
  /// list widget uses the target interface to display the data.
  ///
  class ListAdapter
  {
  protected:
    /// Default constructor.
    ListAdapter() {}
  public:
    /// Destructor.
    virtual ~ListAdapter();

    /// Creates a new item widget.
    virtual Widget *widget(std::size_t pos) const = 0;

    /// Sets an item widget as selected if \p is_selected is \c true, otherwise
    /// Sets the item widget as unselected.
    virtual Widget *udpate_widget(Widget *widget, std::size_t pos, bool is_selected) const;

    /// Returns the number of the items.
    virtual std::size_t item_count() const = 0;
  };

  ///
  /// A list adapter that adapts a string vector.
  ///
  class StringListAdapter : public ListAdapter
  {
    std::vector<std::string> _M_items;
  public:
    /// Default constructor.
    StringListAdapter() {}

    /// Constructor.
    explicit StringListAdapter(std::initializer_list<std::string> items) :
      _M_items(items) {}

    /// Constructor.
    explicit StringListAdapter(const std::vector<std::string> &items) :
      _M_items(items) {}

    /// Destructor.
    virtual ~StringListAdapter();

    virtual Widget *widget(std::size_t pos) const;

    virtual std::size_t item_count() const;

    /// Returns the items.
    const std::vector<std::string> &items() const
    { return _M_items; }

    /// \copydoc items() const
    std::vector<std::string> &items()
    { return _M_items; }
  };

  ///
  /// A class of table adapter that is base for all table adapters.
  ///
  /// A table adapter adapts a data to a target interface that is this class. A
  /// table widget uses the target interface to display the data.
  ///
  class TableAdapter
  {
  protected:
    /// Default constructor.
    TableAdapter() {}
  public:
    /// Destructor.
    virtual ~TableAdapter();

    /// Creates a new cell widget.
    virtual Widget *widget(const TablePosition &pos) const = 0;

    /// Sets a cell widget as selected if \p is_selected is \c true, otherwise
    /// Sets the cell widget as unselected.
    virtual Widget *update_widget(Widget *widget, const TablePosition &pos, bool is_selected) const;

    /// Returns the number of the rows.
    virtual std::size_t row_count() const = 0;

    /// Returns the number of the columns.
    virtual std::size_t column_count() const = 0;
  };

  ///
  /// A table adapter that adapts a vector of string vectors.
  ///
  class StringTableAdapter : public TableAdapter
  {
    std::size_t _M_column_count;
    std::vector<std::vector<std::string>> _M_cells;
  public:
    /// Constructor.
    StringTableAdapter(std::size_t column_count) :
      _M_column_count(column_count) {}

    /// Constructor.
    StringTableAdapter(std::initializer_list<std::initializer_list<std::string>> cells);

    /// Constructor.
    StringTableAdapter(std::size_t column_count, const std::vector<std::vector<std::string>> cells) :
      _M_column_count(column_count), _M_cells(cells) {}

    /// Destructor.
    virtual ~StringTableAdapter();

    virtual Widget *widget(const TablePosition &pos) const;

    virtual std::size_t row_count() const;

    virtual std::size_t column_count() const;

    /// Returns the cells.
    const std::vector<std::vector<std::string>> &cells() const
    { return _M_cells; }

    /// \copydoc cells() const
    std::vector<std::vector<std::string>> &cells()
    { return _M_cells; }
  };

  ///
  /// A class of tree adapter that is base for all tree adapters.
  ///
  /// A tree adapter adapts a data to a target interface that is this class. A
  /// table uses the target interface to display the data.
  ///
  class TreeAdapter
  {
  protected:
    /// Default constructor.
    TreeAdapter() {}
  public:
    /// Destructor.
    virtual ~TreeAdapter();

    /// Creates a node widget.
    virtual Widget *widget(const TreePath &path) const = 0;

    /// Sets a node widget as selected if \p is_selected is \c true, otherwise
    /// Sets the node widget as unselected.
    virtual Widget *update_widget(Widget *widget, const TreePath &path, bool is_selected) const;

    /// Returns the number of the node children.
    virtual std::size_t child_count(const TreePath &path) const = 0;

    /// Returns the number of the roots.
    std::size_t root_count() const
    { return child_count(TreePath()); }

    /// Returns \c true if the node exists, otherwise \c false.
    virtual bool has_node(const TreePath &path) const = 0;
  };

  ///
  /// A structure of string tree node.
  ///
  struct StringTreeNode
  {
    std::string value;                          ///< The value.
    std::vector<StringTreeNode> children;       ///< The children.

    /// Constructor.
    StringTreeNode(const std::string &value) :
      value(value) {}

    /// Constructor.
    StringTreeNode(const std::string &value, std::initializer_list<StringTreeNode> children) :
      value(value), children(children) {}

    /// Constructor.
    StringTreeNode(const std::string &value, const std::vector<StringTreeNode> &children) :
      value(value), children(children) {}
  };

  ///
  /// A tree adapter that adapts a vector of string tree nodes.
  ///
  class StringTreeAdapter : public TreeAdapter
  {
    std::vector<StringTreeNode> _M_nodes;
  public:
    /// Default constructor.
    StringTreeAdapter() {}

    /// Constructor.
    explicit StringTreeAdapter(std::initializer_list<StringTreeNode> nodes) :
      _M_nodes(nodes) {}

    /// Constructor.
    explicit StringTreeAdapter(const std::vector<StringTreeNode> &nodes) :
      _M_nodes(nodes) {}

    /// Destructor.
    virtual ~StringTreeAdapter();
    
    virtual Widget *widget(const TreePath &path) const;

    virtual std::size_t child_count(const TreePath &path) const;

    virtual bool has_node(const TreePath &path) const;

    /// Returns the nodes.
    const std::vector<StringTreeNode> &nodes() const
    { return _M_nodes; }

    /// \copydoc nodes() const
    std::vector<StringTreeNode> &nodes()
    { return _M_nodes; }
  private:
    const StringTreeNode &node(const TreePath &path) const;
  };
}

#endif
