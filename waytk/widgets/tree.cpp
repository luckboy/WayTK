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
#include <iterator>
#include <waytk.hpp>

using namespace std;

namespace waytk
{
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

  void Tree::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { throw exception(); }

  void Tree::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
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
}
