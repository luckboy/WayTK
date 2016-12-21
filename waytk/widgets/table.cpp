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

  void Table::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { throw exception(); }

  void Table::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
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
}
