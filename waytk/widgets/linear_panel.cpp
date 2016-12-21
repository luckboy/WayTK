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
#include <numeric>
#include <set>
#include <waytk.hpp>

using namespace std;

namespace waytk
{
  void LinearPanel::initialize(Orientation orientation, initializer_list<Widget *> widgets)
  {
    this->Panel::initialize(widgets);
    _M_orientation = orientation;
    _M_weight_sum = 0;
    _M_item_height = 0;
  }

  const char *LinearPanel::name() const
  { return "linear_panel"; }

  void LinearPanel::update_child_points()
  {
    Rectangle<int> inner_bounds = bounds_to_inner_bounds(bounds());
    switch(_M_orientation) {
      case Orientation::HORIZONTAL:
      {
        double float_min_item_width;
        int min_item_width = 0;
        double float_x;
        switch(h_align()) {
          case HAlignment::CENTER:
            inner_bounds.x += (inner_bounds.width - content_size().width) / 2;
            break;
          case HAlignment::RIGHT:
            inner_bounds.x += inner_bounds.width - content_size().width;
            break;
          case HAlignment::FILL:
            min_item_width = this->min_item_width(inner_bounds.width, float_min_item_width);
            float_x = 0.0;
            break;
          default:
            break;
        }
        inner_bounds.width = numeric_limits<int>::max();
        for(auto &widget : widgets()) {
          if(widget->is_visible()) {
            Edges<int> widget_margin = widget->margin();
            int widget_margin_box_width;
            if(h_align() != HAlignment::FILL) {
              int min_widget_width = min_item_width;
              min_widget_width -= widget_margin.left + widget_margin.right;
              min_widget_width = max(min_widget_width, 0);
              if(widget->weight() == 0 && widget->bounds().width <= min_widget_width) {
                widget_margin_box_width = floor(float_x + float_min_item_width) - floor(float_x);
                float_x += float_min_item_width;
              } else
                widget_margin_box_width = widget->bounds().width + widget_margin.left + widget_margin.right;
            } else
              widget_margin_box_width = widget->bounds().width + widget_margin.left + widget_margin.right;
            Rectangle<int> widget_area_bounds = inner_bounds;
            widget_area_bounds.width = widget_margin_box_width;
            widget_area_bounds.x += widget_margin.left;
            widget_area_bounds.y += widget_margin.top;
            widget_area_bounds.width -= widget_margin.left + widget_margin.right;
            widget_area_bounds.height -= widget_margin.top + widget_margin.bottom;
            widget_area_bounds.width = max(widget_area_bounds.width, 0);
            widget_area_bounds.height = max(widget_area_bounds.height, 0);
            // Updates top left point of widget.
            widget->update_point(widget_area_bounds);
            inner_bounds.x += widget_margin_box_width;
          }
        }
        break;
      }
      case Orientation::VERTICAL:
      {
        double float_min_item_height;
        int min_item_height = 0;
        double float_y;
        switch(v_align()) {
          case VAlignment::CENTER:
            inner_bounds.y += (inner_bounds.height - content_size().height) / 2;
            break;
          case VAlignment::BOTTOM:
            inner_bounds.y += inner_bounds.height - content_size().height;
            break;
          case VAlignment::FILL:
            min_item_height = this->min_item_height(inner_bounds.height, float_min_item_height);
            float_y = 0.0;
            break;
          default:
            break;
        }
        inner_bounds.height = numeric_limits<int>::max();
        for(auto &widget : widgets()) {
          if(widget->is_visible()) {
            Edges<int> widget_margin = widget->margin();
            int widget_margin_box_height;
            if(v_align() != VAlignment::FILL) {
              int min_widget_height = min_item_height;
              min_widget_height -= widget_margin.top + widget_margin.bottom;
              min_widget_height = max(min_widget_height, 0);
              if(widget->weight() == 0 && widget->bounds().height <= min_widget_height) {
                widget_margin_box_height = floor(float_y + float_min_item_height) - floor(float_y);
                float_y += float_min_item_height;
              } else
                widget_margin_box_height = widget->bounds().height + widget_margin.top + widget_margin.bottom;
            } else
              widget_margin_box_height = widget->bounds().height + widget_margin.top + widget_margin.bottom;
            Rectangle<int> widget_area_bounds = inner_bounds;
            widget_area_bounds.height = widget_margin_box_height;
            widget_area_bounds.x += widget_margin.left;
            widget_area_bounds.y += widget_margin.top;
            widget_area_bounds.width -= widget_margin.left + widget_margin.right;
            widget_area_bounds.height -= widget_margin.top + widget_margin.bottom;
            widget_area_bounds.width = max(widget_area_bounds.width, 0);
            widget_area_bounds.height = max(widget_area_bounds.height, 0);
            // Updates top left point of widget.
            widget->update_point(widget_area_bounds);
            inner_bounds.y += widget_margin_box_height;
          }
        }
        break;
      }
    }
  }

  void LinearPanel::update_child_sizes(Canvas *canvas, const Dimension<int> &area_size)
  {
    // Sets pseudo classes of widgets.
    auto first_filled_widget_iter = widgets().end();
    Widget *prev_widget = nullptr;
    switch(_M_orientation) {
      case Orientation::HORIZONTAL:
        for(auto iter = widgets().begin(); iter != widgets().end(); iter++) {
          Widget *widget = iter->get();
          if(widget->is_visible()) {
            if(prev_widget != nullptr) {
              bool is_adjacent = widget->has_adjacency_to(prev_widget);
              if(is_adjacent && (h_align() == HAlignment::FILL || prev_widget->weight() > 0))
                is_adjacent = (is_adjacent && (prev_widget->h_align() == HAlignment::RIGHT || prev_widget->h_align() == HAlignment::FILL));
              if(is_adjacent && (h_align() == HAlignment::FILL || widget->weight() > 0))
                is_adjacent = (is_adjacent && (widget->h_align() == HAlignment::LEFT || widget->h_align() == HAlignment::FILL));
              is_adjacent = (is_adjacent && prev_widget->v_align() == widget->v_align());
              if(is_adjacent) {
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
            }
            if(widget->h_align() == HAlignment::FILL && first_filled_widget_iter == widgets().end())
              first_filled_widget_iter = iter;
            prev_widget = widget;
          }
        }
        break;
      case Orientation::VERTICAL:
        for(auto iter = widgets().begin(); iter != widgets().end(); iter++) {
          Widget *widget = iter->get();
          if(widget->is_visible()) {
            if(prev_widget != nullptr) {
              bool is_adjacent = widget->has_adjacency_to(prev_widget);
              if(is_adjacent && (v_align() == VAlignment::FILL || prev_widget->weight() > 0))
                is_adjacent = (is_adjacent && (prev_widget->v_align() == VAlignment::BOTTOM || prev_widget->v_align() == VAlignment::FILL));
              if(is_adjacent && (v_align() == VAlignment::FILL || widget->weight() > 0))
                is_adjacent = (is_adjacent && (widget->v_align() == VAlignment::TOP || widget->v_align() == VAlignment::FILL));
              is_adjacent = (is_adjacent && prev_widget->h_align() == widget->h_align());
              if(is_adjacent) {
                prev_widget->set_pseudo_classes(prev_widget->pseudo_classes() | PseudoClasses::ADJACENT_TO_BOTTOM);
                widget->set_pseudo_classes(widget->pseudo_classes() | PseudoClasses::ADJACENT_TO_TOP);
                if((widget->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
                  prev_widget->set_pseudo_classes(prev_widget->pseudo_classes() | PseudoClasses::BOTTOM_ACTIVE);
                if((prev_widget->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
                  widget->set_pseudo_classes(widget->pseudo_classes() | PseudoClasses::TOP_ACTIVE);
              } else {
                prev_widget->set_pseudo_classes(prev_widget->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_BOTTOM | PseudoClasses::BOTTOM_ACTIVE));
                widget->set_pseudo_classes(widget->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_TOP | PseudoClasses::TOP_ACTIVE));
              }
              if(widget->v_align() == VAlignment::FILL && first_filled_widget_iter == widgets().end())
                first_filled_widget_iter = iter;
            }
            prev_widget = widget;
          }
        }
        break;
    }
    // Updates sizes of widgets.
    Dimension<int> inner_area_size = area_size_to_inner_area_size(area_size);
    Dimension<int> inner_weight_area_size(0, 0);
    bool is_width = (inner_area_size.width != numeric_limits<int>::max());
    bool is_height = (inner_area_size.height != numeric_limits<int>::max());
    int weight_sum = _M_weight_sum;
    int widget_weight_sum = accumulate(widgets().begin(), widgets().end(), 0, [](int sum, const unique_ptr<Widget> &widget) {
      return sum + (widget->is_visible() ? widget->weight() : 0);
    });
    if(weight_sum <= widget_weight_sum) weight_sum = widget_weight_sum;
    switch(_M_orientation) {
      case Orientation::HORIZONTAL:
      {
        // Updates.
        int max_inner_area_width = 0, max_inner_weight_area_width = 0;
        int inner_area_width = inner_weight_area_size.width;
        if(weight_sum > 0) {
          if(inner_area_size.width != numeric_limits<int>::max()) {
            inner_weight_area_size.width = (static_cast<int64_t>(widget_weight_sum) * inner_area_size.width) / weight_sum;
            inner_area_size.width -= inner_weight_area_size.width;
          } else
            inner_weight_area_size.width = inner_area_size.width;
        } else
          inner_weight_area_size.width = 0;
        inner_weight_area_size.height = inner_area_size.height;
        _M_item_height = 0;
        for(auto iter = widgets().begin(); iter != widgets().end(); iter++) {
          Widget *widget = iter->get();
          if(widget->is_visible()) {
            Edges<int> widget_margin = widget->margin();
            Dimension<int> widget_area_size;
            if(widget->weight() > 0) {
              if(inner_area_size.width != numeric_limits<int>::max())
                widget_area_size.width = (static_cast<int64_t>(widget->weight()) * inner_weight_area_size.width) / widget_weight_sum;
              else
                widget_area_size.width = numeric_limits<int>::max();
            } else {
              if(first_filled_widget_iter == widgets().end())
                widget_area_size.width = (iter == first_filled_widget_iter ? inner_area_size.width : 0);
              else
                widget_area_size.width = inner_area_size.width;
            }
            widget_area_size.height = inner_area_size.height;
            if(widget_area_size.width != numeric_limits<int>::max())
              widget_area_size.width -= widget_margin.left + widget_margin.right;
            if(widget_area_size.height != numeric_limits<int>::max())
              widget_area_size.height -= widget_margin.top + widget_margin.bottom;
            widget_area_size.width = max(widget_area_size.width, 0);
            widget_area_size.height = max(widget_area_size.height, 0);
            // Updates widget size.
            widget->update_size(canvas, widget_area_size);
            if(widget->weight() > 0) {
              if(inner_weight_area_size.width != numeric_limits<int>::max()) {
                inner_weight_area_size.width -= widget->bounds().width;
                inner_weight_area_size.width -= widget_margin.left + widget_margin.right;
                inner_weight_area_size.width = max(inner_weight_area_size.width, 0);
              }
              int widget_margin_box_width = widget->bounds().width + widget_margin.left + widget_margin.right;
              int tmp_weight_area_width = (static_cast<int64_t>(widget_weight_sum) * widget_margin_box_width) / widget->weight();
              max_inner_weight_area_width = max(max_inner_weight_area_width, tmp_weight_area_width);
            } else {
              if(inner_area_size.width != numeric_limits<int>::max()) {
                inner_area_size.width -= widget->bounds().width;
                inner_area_size.width -= widget_margin.left + widget_margin.right;
                inner_area_size.width = max(inner_area_size.width, 0);
              }
              if(iter == first_filled_widget_iter)
                max_inner_area_width = widget->bounds().width + widget_margin.left + widget_margin.right;
            }
            int widget_margin_box_height = widget->bounds().height + widget_margin.top + widget_margin.bottom;
            _M_item_height = max(_M_item_height, widget_margin_box_height);
          }
        }
        // Again updates.
        if(!is_width || !is_height) {
          _M_item_height = 0;
          for(auto iter = widgets().begin(); iter != widgets().end(); iter++) {
            Widget *widget = iter->get();
            if(widget->is_visible()) {
              Edges<int> widget_margin = widget->margin();
              if(!is_width && widget->can_again_update_width()) {
                Dimension<int> widget_area_size = widget->bounds().size();
                if(widget->weight() > 0)
                  widget_area_size.width = (static_cast<int64_t>(widget->weight()) * max_inner_weight_area_width) / widget_weight_sum;
                else if(iter == first_filled_widget_iter)
                  widget_area_size.width = max_inner_area_width;
                widget_area_size.width -= widget_margin.left + widget_margin.right;
                widget_area_size.width = max(widget_area_size.width, 0);
                if(widget->bounds().size() != widget_area_size) {
                  // Updates widget size.
                  widget->update_size(canvas, widget_area_size);
                }
              }
              int widget_margin_box_height = widget->bounds().height + widget_margin.top + widget_margin.bottom;
              _M_item_height = max(_M_item_height, widget_margin_box_height);
            }
          }
          bool are_widgets_to_update = false;
          for(auto &widget : widgets()) {
            if(widget->is_visible()) {
              Dimension<int> widget_area_size = widget->bounds().size();
              bool can_again_update = false;
              if(!is_height && widget->can_again_update_height()) {
                Edges<int> widget_margin = widget->margin();
                widget_area_size.height = _M_item_height;
                widget_area_size.height -= widget_margin.top + widget_margin.right;
                widget_area_size.height = max(widget_area_size.height, 0);
                can_again_update = true;
              }
              if(is_width) {
                if(h_align() == HAlignment::FILL && widget->weight() == 0 && first_filled_widget_iter == widgets().end()) {
                  are_widgets_to_update = true;
                  can_again_update = false;
                }
              }
              if(can_again_update && widget_area_size != widget->bounds().size()) {
                // Updates widget size.
                widget->update_size(canvas, widget_area_size);
              }
            }
          }
          if(are_widgets_to_update) {
            double float_min_item_width;
            int min_item_width = this->min_item_width(inner_area_width, float_min_item_width);
            double float_x = 0;
            for(auto &widget : widgets()) {
              if(widget->is_visible() && widget->weight() == 0) {
                Edges<int> widget_margin = widget->margin();
                Dimension<int> widget_area_size = widget->bounds().size();
                bool can_again_update = false;
                if(!is_height) {
                  widget_area_size.height = _M_item_height;
                  widget_area_size.height -= widget_margin.top + widget_margin.right;
                  widget_area_size.height = max(widget_area_size.height, 0);
                  can_again_update = true;
                }
                if(widget->can_again_update_width()) {
                  int min_widget_width = min_item_width;
                  min_widget_width -= widget_margin.left + widget_margin.top;
                  min_widget_width = max(min_widget_width, 0);
                  if(widget->weight() == 0 && widget->bounds().width < min_widget_width) {
                    int widget_margin_box_width = floor(float_x + float_min_item_width) - floor(float_x);
                    float_x += float_min_item_width;
                    widget_area_size.width = widget_margin_box_width;
                    widget_area_size.width -= widget_margin.left + widget_margin.right;
                    widget_area_size.width = max(widget_area_size.width, 0);
                    can_again_update = true;
                  }
                }
                if(can_again_update && widget_area_size != widget->bounds().size()) {
                  // Updates widget size.
                  widget->update_size(canvas, widget_area_size);
                }
              }
            }
          }
        }
        break;
      }
      case Orientation::VERTICAL:
      {
        // Updates.
        int max_inner_area_height = 0, max_inner_weight_area_height = 0;
        int inner_area_height = inner_weight_area_size.height;
        inner_weight_area_size.width = inner_area_size.width;
        if(weight_sum > 0) {
          if(inner_area_size.height != numeric_limits<int>::max()) {
            inner_weight_area_size.height = (static_cast<int64_t>(widget_weight_sum) * inner_area_size.height) / weight_sum;
            inner_area_size.height -= inner_weight_area_size.height;
          } else
            inner_weight_area_size.height = inner_area_size.height;
        } else
          inner_weight_area_size.height = 0;
        for(auto iter = widgets().begin(); iter != widgets().end(); iter++) {
          Widget *widget = iter->get();
          if(widget->is_visible()) {
            Edges<int> widget_margin = widget->margin();
            Dimension<int> widget_area_size;
            widget_area_size.width = inner_area_size.width;
            if(widget->weight() > 0) {
              if(inner_area_size.height != numeric_limits<int>::max())
                widget_area_size.height = (static_cast<int64_t>(widget->weight()) * inner_weight_area_size.height) / widget_weight_sum;
              else
                widget_area_size.height = numeric_limits<int>::max();
            } else
              widget_area_size.height = inner_area_size.height;
            if(widget_area_size.width != numeric_limits<int>::max())
              widget_area_size.width -= widget_margin.left + widget_margin.right;
            if(widget_area_size.height != numeric_limits<int>::max())
              widget_area_size.height -= widget_margin.top + widget_margin.bottom;
            widget_area_size.width = max(widget_area_size.width, 0);
            widget_area_size.height = max(widget_area_size.height, 0);
            // Updates widget size.
            widget->update_size(canvas, widget_area_size);
            if(widget->weight() > 0) {
              if(inner_weight_area_size.height != numeric_limits<int>::max()) {
                inner_weight_area_size.height -= widget->bounds().height;
                inner_weight_area_size.height -= widget_margin.top + widget_margin.bottom;
                inner_weight_area_size.height = max(inner_weight_area_size.height, 0);
              }
              int widget_margin_box_height = widget->bounds().height + widget_margin.top + widget_margin.bottom;
              int tmp_weight_area_height = (static_cast<int64_t>(widget_weight_sum) * widget_margin_box_height) / widget->weight();
              max_inner_weight_area_height = max(max_inner_weight_area_height, tmp_weight_area_height);
            } else {
              if(inner_area_size.height != numeric_limits<int>::max()) {
                inner_area_size.height -= widget->bounds().height;
                inner_area_size.height -= widget_margin.top + widget_margin.bottom;
                inner_area_size.height = max(inner_area_size.height, 0);
              }
              if(iter == first_filled_widget_iter)
                max_inner_area_height = widget->bounds().width + widget_margin.left + widget_margin.right;
            }
            int widget_margin_box_width = widget->bounds().width + widget_margin.left + widget_margin.right;
            _M_item_width = max(_M_item_width, widget_margin_box_width);
          }
        }
        // Again updates.
        if(!is_width || !is_height) {
          bool are_widgets_to_update = false;
          max_inner_area_height = 0;
          max_inner_weight_area_height = 0;
          for(auto iter = widgets().begin(); iter != widgets().end(); iter++) {
            Widget *widget = iter->get();
            if(widget->is_visible()) {
              Edges<int> widget_margin = widget->margin();
              if(!is_width && widget->can_again_update_width()) {
                Dimension<int> widget_area_size = widget->bounds().size();
                widget_area_size.width = _M_item_width;
                widget_area_size.width -= widget_margin.left + widget_margin.right;
                if(widget_area_size != widget->bounds().size()) {
                  // Updates widget size.
                  widget->update_size(canvas, widget_area_size);
                }
              }
              if(widget->weight() > 0) {
                int widget_margin_box_height = widget->bounds().height + widget_margin.top + widget_margin.bottom;
                int tmp_weight_area_height = (static_cast<int64_t>(widget_weight_sum) * widget_margin_box_height) / widget->weight();
                max_inner_weight_area_height = max(max_inner_weight_area_height, tmp_weight_area_height);
              } else {
                if(iter == first_filled_widget_iter)
                  max_inner_area_height = widget->bounds().width + widget_margin.left + widget_margin.right;
              }
            }
          }
          for(auto iter = widgets().begin(); iter != widgets().end(); iter++) {
            Widget *widget = iter->get();
            if(widget->is_visible()) {
              Dimension<int> widget_area_size = widget->bounds().size();
              bool can_again_update = false;
              if(!is_height && widget->can_again_update_height()) {
                Edges<int> widget_margin = widget->margin();
                if(widget->weight() > 0) {
                  widget_area_size.height = (static_cast<int64_t>(widget->weight()) * max_inner_weight_area_height) / widget_weight_sum;
                  can_again_update = true;
                } else if(iter == first_filled_widget_iter) {
                  widget_area_size.height = max_inner_area_height;
                  can_again_update = true;
                }
                widget_area_size.height -= widget_margin.top + widget_margin.bottom;
                widget_area_size.height = max(widget_area_size.height, 0);
              }
              if(is_height) {
                if(v_align() == VAlignment::FILL && widget->weight() == 0 && first_filled_widget_iter == widgets().end()) {
                  are_widgets_to_update = true;
                  can_again_update = false;
                }
              }
              if(can_again_update && widget_area_size != widget->bounds().size()) {
                // Updates widget size.
                widget->update_size(canvas, widget_area_size);
              }
            }
          }
          if(are_widgets_to_update) {
            double float_min_item_height;
            int min_item_height = this->min_item_height(inner_area_height, float_min_item_height);
            double float_y = 0;
            for(auto &widget : widgets()) {
              if(widget->is_visible() && widget->weight() == 0) {
                Edges<int> widget_margin = widget->margin();
                Dimension<int> widget_area_size = widget->bounds().size();
                bool can_again_update = false;
                if(!is_height) {
                  widget_area_size.width = _M_item_width;
                  can_again_update = true;
                }
                if(widget->can_again_update_height()) {
                  int min_widget_height = min_item_height;
                  min_widget_height -= widget_margin.left + widget_margin.top;
                  min_widget_height = max(min_widget_height, 0);
                  if(widget->weight() == 0 && widget->bounds().height < min_widget_height) {
                    int widget_margin_box_height = floor(float_y + float_min_item_height) - floor(float_y);
                    float_y += float_min_item_height;
                    widget_area_size.height = widget_margin_box_height;
                    widget_area_size.height -= widget_margin.top + widget_margin.bottom;
                    widget_area_size.height = max(widget_area_size.height, 0);
                    can_again_update = true;
                  }
                }
                if(can_again_update && widget_area_size != widget->bounds().size()) {
                  // Updates widget size.
                  widget->update_size(canvas, widget_area_size);
                }
              }
            }
          }
        }
        break;
      }
    }
  }

  void LinearPanel::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  {
    switch(_M_orientation) {
      case Orientation::HORIZONTAL: {
        int content_width = accumulate(widgets().begin(), widgets().end(), 0, [](int width, const unique_ptr<Widget> &widget) {
          if(widget->is_visible()) {
            Edges<int> widget_margin = widget->margin();
            int widget_margin_box_width = widget->bounds().width;
            widget_margin_box_width += widget_margin.left + widget_margin.right;
            return width + widget_margin_box_width;
          } else
            return width;
        });
        set_content_size(Dimension<int>(content_width, _M_item_height));
        break;
      }
      case Orientation::VERTICAL:
        int content_height = accumulate(widgets().begin(), widgets().end(), 0, [](int height, const unique_ptr<Widget> &widget) {
          if(widget->is_visible()) {
            Edges<int> widget_margin = widget->margin();
            int widget_margin_box_height = widget->bounds().height;
            widget_margin_box_height += widget_margin.top + widget_margin.bottom;
            return height + widget_margin_box_height;
          } else
            return height;
        });
        set_content_size(Dimension<int>(_M_item_width, content_height));
        break;
    }
  }

  bool LinearPanel::can_again_update_width() const
  { return true; }

  bool LinearPanel::can_again_update_height() const
  { return true; }

  int LinearPanel::min_item_width(int inner_area_width, double &float_width)
  {
    int max_width = numeric_limits<int>::min();
    int min_width;
    size_t widget_count = 0;
    for(auto &widget : widgets()) {
      if(widget->is_visible()) {
        Edges<int> widget_margin = widget->margin();
        int widget_margin_box_width = widget->bounds().width;
        widget_margin_box_width -= widget_margin.left + widget_margin.right;
        if(widget->weight() == 0 && widget->h_align() != HAlignment::FILL) {
          max_width = max(max_width, widget_margin_box_width);
          widget_count++;
        } else
          inner_area_width -= widget_margin_box_width;
      }
    }
    if(widget_count != 0 && static_cast<size_t>(max_width) < static_cast<size_t>(inner_area_width) / widget_count) {
      multiset<int> widths;
      for(auto &widget : widgets()) {
        if(widget->is_visible() && widget->weight() == 0) {
          Edges<int> widget_margin = widget->margin();
          int widget_margin_box_width = widget->bounds().width;
          widget_margin_box_width -= widget_margin.left + widget_margin.right;
          widths.insert(widget_margin_box_width);
        }
      }
      min_width = inner_area_width / widget_count;
      for(auto iter = widths.rbegin(); iter != widths.rend(); iter++) {
        if(*iter <= min_width) break;
        inner_area_width -= *iter;
        widget_count--;
        min_width = (widget_count != 0 ? inner_area_width / widget_count : numeric_limits<int>::max());
      }
    } else
      min_width = (widget_count != 0 ? inner_area_width / widget_count : numeric_limits<int>::max());
    float_width = (inner_area_width + 0.0) / widget_count;
    return min_width;
  }

  int LinearPanel::min_item_height(int inner_area_height, double &float_height)
  {
    int max_height = numeric_limits<int>::min();
    int min_height;
    size_t widget_count = 0;
    for(auto &widget : widgets()) {
      if(widget->is_visible()) {
        Edges<int> widget_margin = widget->margin();
        int widget_margin_box_height = widget->bounds().height;
        widget_margin_box_height -= widget_margin.top + widget_margin.bottom;
        if(widget->weight() == 0 && widget->v_align() != VAlignment::FILL) {
          max_height = max(max_height, widget_margin_box_height);
          widget_count++;
        } else
          inner_area_height -= widget->bounds().height;
      }
    }
    if(widget_count != 0 && static_cast<size_t>(max_height) < static_cast<size_t>(inner_area_height) / widget_count) {
      multiset<int> heights;
      for(auto &widget : widgets()) {
        if(widget->is_visible() && widget->weight() == 0) {
          Edges<int> widget_margin = widget->margin();
          int widget_margin_box_height = widget->bounds().height;
          widget_margin_box_height -= widget_margin.top + widget_margin.bottom;
          heights.insert(widget_margin_box_height);
        }
      }
      min_height = inner_area_height / widget_count;
      for(auto iter = heights.rbegin(); iter != heights.rend(); iter++) {
        if(*iter <= min_height) break;
        inner_area_height -= *iter;
        widget_count--;
        min_height = (widget_count != 0 ? inner_area_height / widget_count : numeric_limits<int>::max());
      }
    } else
      min_height = (widget_count != 0 ? inner_area_height / widget_count : numeric_limits<int>::max());
    float_height = (inner_area_height + 0.0) / widget_count;
    return min_height;
  }
}

