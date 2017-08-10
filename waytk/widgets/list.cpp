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
#include <algorithm>
#include <iterator>
#include <waytk.hpp>

using namespace std;

namespace waytk
{
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

  void List::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { throw exception(); }

  void List::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  { throw exception(); }

  Viewport *List::viewport()
  { throw exception(); }
  
  bool List::invoke_fun_for_event(const Point<double> &point, const function<bool (Widget *, const Point<double> &)> &fun)
  { throw exception(); }

  bool List::on_touch(const Pointer &pointer, const Point<double> &point, TouchState state)
  { throw exception(); }

  void List::on_touch_leave(const Pointer &pointer)
  { throw exception(); }

  bool List::on_key(uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)
  { throw exception(); }

  void List::on_list_selection(const set<size_t> &poses)
  { _M_on_list_selection_callback(this, poses); }
}
