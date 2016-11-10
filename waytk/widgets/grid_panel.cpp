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
#include <cmath>
#include <limits>
#include <waytk.hpp>

using namespace std;

namespace waytk
{
  GridPanel::~GridPanel() {}

  void GridPanel::initialize(int column_count, initializer_list<Widget *> widgets)
  {
    this->Panel::initialize(widgets);
    set_column_count(column_count);
    _M_cell_size = Dimension<int>(0, 0);
  }

  const char *GridPanel::name() const
  { return "grid_panel"; }

  void GridPanel::update_child_points(const Rectangle<int> &area_bounds)
  {
    Rectangle<int> inner_area_bounds = area_bounds_to_inner_area_bounds(area_bounds);
    int row_count = widgets().size() / _M_column_count + (widgets().size() % _M_column_count > 0 ? 1 : 0);
    int x, y, width, height;
    double float_width, float_height;
    bool is_float_width, is_float_height;
    if(h_align() != HAlignment::FILL) {
      switch(h_align()) {
        case HAlignment::LEFT:
          x = inner_area_bounds.x;
          break;
        case HAlignment::CENTER:
          x = inner_area_bounds.x + (inner_area_bounds.width - content_size().width) / 2 ;
          break;
        case HAlignment::RIGHT:
          x = inner_area_bounds.x + (inner_area_bounds.width - content_size().width);
          break;
        default:
          break;
      }
      width = _M_cell_size.width;
      is_float_width = false;
    } else {
      x = inner_area_bounds.x;
      if(inner_area_bounds.width != numeric_limits<int>::max()) {
        float_width = (inner_area_bounds.width + 0.0) / _M_column_count;
        is_float_width = true;
      } else
        is_float_width = false;
    }
    if(v_align() != VAlignment::FILL) {
      switch(v_align()) {
        case VAlignment::TOP:
          y = inner_area_bounds.y;
          break;
        case VAlignment::CENTER:
          y = inner_area_bounds.y + (inner_area_bounds.height - content_size().height) / 2 ;
          break;
        case VAlignment::BOTTOM:
          y = inner_area_bounds.y + (inner_area_bounds.height - content_size().height);
          break;
        default:
          break;
      }
      height = _M_cell_size.height;
      is_float_width = false;
    } else {
      y = inner_area_bounds.y;
      if(inner_area_bounds.height != numeric_limits<int>::max() && row_count > 0) {
        float_height = (inner_area_bounds.height + 0.0) / row_count;
        is_float_height = true;
      } else
        is_float_height = false;
    }
    double float_x = 0.0, float_y = 0.0;
    for(auto iter = widgets().begin(); iter != widgets().end(); ) {
      if(is_float_height) height = floor(float_y + float_height) - floor(float_y);
      for(int column = 0; column < _M_column_count && iter == widgets().end(); column++, iter++) {
        Widget *widget = iter->get();
        if(is_float_width) width = floor(float_x + float_width) - floor(float_x);
        if(widget->is_visible()) {
          Edges<int> widget_margin = widget->margin();
          Rectangle<int> widget_area_bounds(x, y, width, height);
          widget_area_bounds.width -= widget_margin.left + widget_margin.right;
          widget_area_bounds.height -= widget_margin.top + widget_margin.bottom;
          widget_area_bounds.width = max(widget_area_bounds.width, 0);
          widget_area_bounds.height = max(widget_area_bounds.height, 0);
          // Updates top left point of widget.
          widget->update_point(widget_area_bounds);
        }
        float_x += float_width;
        x += width;
      }
      float_y += float_height;
      y += height;
    }
  }

  void GridPanel::update_child_sizes(Canvas *canvas, const Dimension<int> &area_size)
  {
    // Sets pseudo classes of widgets.
    auto prev_row_begin = widgets().end();
    for(auto iter = widgets().begin(); iter != widgets().end(); ) {
      auto prev_row_iter = prev_row_begin;
      prev_row_begin = iter;
      Widget *prev_widget = nullptr;
      for(int column = 0; column < _M_column_count && iter == widgets().end(); column++, iter++) {
        Widget *widget = iter->get();
        if(widget->is_visible()) {
          Widget *top_widget = prev_row_iter->get();
          bool is_adjacent_to_top = widget->has_adjacency_to(top_widget);
          is_adjacent_to_top = (is_adjacent_to_top && (top_widget->v_align() == VAlignment::BOTTOM || top_widget->v_align() == VAlignment::FILL));
          is_adjacent_to_top = (is_adjacent_to_top && (widget->v_align() == VAlignment::TOP || widget->v_align() == VAlignment::FILL));
          is_adjacent_to_top = (is_adjacent_to_top && top_widget->h_align() == widget->h_align());
          if(is_adjacent_to_top) {
            top_widget->set_pseudo_classes(top_widget->pseudo_classes() | PseudoClasses::ADJACENT_TO_BOTTOM);
            widget->set_pseudo_classes(widget->pseudo_classes() | PseudoClasses::ADJACENT_TO_TOP);
            if((widget->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
              prev_widget->set_pseudo_classes(top_widget->pseudo_classes() | PseudoClasses::BOTTOM_ACTIVE);
            if((prev_widget->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
              widget->set_pseudo_classes(widget->pseudo_classes() | PseudoClasses::TOP_ACTIVE);
          } else {
            top_widget->set_pseudo_classes(top_widget->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_BOTTOM | PseudoClasses::BOTTOM_ACTIVE));
            widget->set_pseudo_classes(widget->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_TOP | PseudoClasses::TOP_ACTIVE));
          }
          bool is_adjacent_to_left = widget->has_adjacency_to(prev_widget);
          is_adjacent_to_left = (is_adjacent_to_left && (prev_widget->h_align() == HAlignment::RIGHT || prev_widget->h_align() == HAlignment::FILL));
          is_adjacent_to_left = (is_adjacent_to_left && (widget->h_align() == HAlignment::LEFT || widget->h_align() == HAlignment::FILL));
          is_adjacent_to_left = (is_adjacent_to_left && prev_widget->v_align() == widget->v_align());
          if(is_adjacent_to_left) {
            prev_widget->set_pseudo_classes(prev_widget->pseudo_classes() | PseudoClasses::ADJACENT_TO_RIGHT);
            widget->set_pseudo_classes(widget->pseudo_classes() | PseudoClasses::ADJACENT_TO_LEFT);
            if((widget->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
              prev_widget->set_pseudo_classes(prev_widget->pseudo_classes() | PseudoClasses::RIGHT_ACTIVE);
            if((prev_widget->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
              widget->set_pseudo_classes(widget->pseudo_classes() | PseudoClasses::LEFT_ACTIVE);
          } else {
            prev_widget->set_pseudo_classes(prev_widget->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_RIGHT | PseudoClasses::RIGHT_ACTIVE));
            widget->set_pseudo_classes(widget->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_LEFT | PseudoClasses::LEFT_ACTIVE));
          }
          prev_widget = widget;
        } else
          prev_widget = nullptr;
        if(prev_row_iter != widgets().end()) prev_row_iter++;
      }
    }
    // Updates sizes of widgets.
    Dimension<int> inner_area_size = area_size_to_inner_area_size(area_size);
    int row_count = widgets().size() / _M_column_count + (widgets().size() % _M_column_count > 0 ? 1 : 0);
    double float_width = numeric_limits<double>::infinity();
    double float_height = numeric_limits<double>::infinity();
    bool is_width, is_height;
    if(inner_area_size.width != numeric_limits<int>::max()) {
      float_width = (inner_area_size.width + 0.0) / _M_column_count;
      is_width = true;
    } else
      is_width = false;
    if(inner_area_size.height != numeric_limits<int>::max() && row_count > 0) {
      float_height = (inner_area_size.height + 0.0) / row_count;
      is_height = true;
    } else
      is_height = false;
    double float_x = 0.0, float_y = 0.0;
    _M_cell_size = Dimension<int>(0, 0);
    for(auto iter = widgets().begin(); iter != widgets().end(); ) {
      int height = (is_height ? floor(float_y + float_height) - floor(float_y) : numeric_limits<int>::max());
      for(int column = 0; column < _M_column_count && iter == widgets().end(); column++, iter++) {
        Widget *widget = iter->get();
        int width = (is_width ? floor(float_x + float_width) - floor(float_x) : numeric_limits<int>::max());
        if(widget->is_visible()) {
          Edges<int> widget_margin = widget->margin();
          Dimension<int> widget_area_size(width, height);
          if(widget_area_size.width != numeric_limits<int>::max())
            widget_area_size.width -= widget_margin.left + widget_margin.right;
          if(widget_area_size.height != numeric_limits<int>::max())
            widget_area_size.height -= widget_margin.top + widget_margin.bottom;
          widget_area_size.width = max(widget_area_size.width, 0);
          widget_area_size.height = max(widget_area_size.height, 0);
          // Updates widget size.
          widget->update_size(canvas, widget_area_size);
          int widget_margin_box_width = widget->bounds().width;
          int widget_margin_box_height = widget->bounds().height;
          widget_margin_box_width += widget_margin.left + widget_margin.right;
          widget_margin_box_height += widget_margin.top + widget_margin.bottom;
          _M_cell_size.width = max(_M_cell_size.width, widget_margin_box_width);
          _M_cell_size.height = max(_M_cell_size.height, widget_margin_box_height);
        }
        float_x += float_width;
      }
      float_y += float_height;
    }
    if(!is_width || !is_height) {
      _M_cell_size.height = 0;
      for(auto &widget : widgets()) {
        if(widget->is_visible()) {
          Edges<int> widget_margin = widget->margin();
          Dimension<int> widget_area_size = widget->bounds().size();
          if(!is_width && widget->can_again_update_width()) {
            widget_area_size.width = _M_cell_size.width;
            widget_area_size.width -= widget_margin.left + widget_margin.right;
            widget_area_size.width = max(widget_area_size.width, 0);
            if(widget_area_size != widget->bounds().size()) {
              // Updates widget size.
              widget->update_size(canvas, widget_area_size);
            }
          }
          int widget_margin_box_height = widget->bounds().height;
          widget_margin_box_height += widget_margin.top + widget_margin.bottom;
          _M_cell_size.height = max(_M_cell_size.height, widget_margin_box_height);
        }
      }
      for(auto &widget : widgets()) {
        if(widget->is_visible()) {
          Dimension<int> widget_area_size = widget->bounds().size();
          if(!is_height && widget->can_again_update_height()) {
            Edges<int> widget_margin = widget->margin();
            widget_area_size.height = _M_cell_size.height;
            widget_area_size.height -= widget_margin.top + widget_margin.bottom;
            if(widget_area_size != widget->bounds().size()) {
              // Updates widget size.
              widget->update_size(canvas, widget_area_size);
            }
          }
        }
      }
    }
  }

  void GridPanel::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  {
    int row_count = widgets().size() / _M_column_count + (widgets().size() % _M_column_count > 0 ? 1 : 0);
    Dimension<int> content_size;
    content_size.width = _M_cell_size.width * _M_column_count;
    content_size.height = _M_cell_size.height * row_count;
    set_content_size(content_size);
  }

  bool GridPanel::can_again_update_width() const
  { return true; }

  bool GridPanel::can_again_update_height() const
  { return true; }
}
