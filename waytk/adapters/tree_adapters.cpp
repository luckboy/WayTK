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
  // A TreeAdapter class.
  //

  TreeAdapter::~TreeAdapter() {}

  Widget *TreeAdapter::update_widget(Widget *widget, const TreePath &path, bool is_selected) const
  {
    PseudoClasses flags = widget->pseudo_classes();
    widget->set_pseudo_classes(flags | (is_selected ? PseudoClasses::SELECTED : PseudoClasses::NONE));
    return widget;
  }

  //
  // A StringTreeAdapter class.
  //

  StringTreeAdapter::~StringTreeAdapter() {}

  Widget *StringTreeAdapter::widget(const TreePath &path) const
  { return new Label(node(path).value); }

  size_t StringTreeAdapter::child_count(const TreePath &path) const
  { return node(path).children.size(); }

  bool StringTreeAdapter::has_node(const TreePath &path) const
  {
    const vector<StringTreeNode> *tmp_nodes = &_M_nodes;
    for(auto node_idx : path.nodes) {
      if(node_idx >= tmp_nodes->size()) return false;
      tmp_nodes = &((*tmp_nodes)[node_idx].children);
    }
    return true;
  }

  const StringTreeNode &StringTreeAdapter::node(const TreePath &path) const
  {
    const vector<StringTreeNode> *tmp_nodes = &_M_nodes;
    vector<StringTreeNode>::const_iterator tmp_node_iter;
    for(auto node_idx : path.nodes) {
      tmp_nodes = &((*tmp_nodes)[node_idx].children);
      tmp_node_iter = (*tmp_nodes).begin() + node_idx;
    }
    return *tmp_node_iter;
  }
}
