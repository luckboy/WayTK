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
#include <cmath>
#include <numeric>
#include <waytk.hpp>

using namespace std;

namespace waytk
{
  MenuBar::~MenuBar() {}

  void MenuBar::initialize(initializer_list<Menu *> menus)
  {
    _M_menus.clear();
    for(auto menu : menus) {
      _M_menus.push_back(unique_ptr<Menu>(menu));
      set_this_as_widget_parent(menu);
    }
    _M_menu_height = 0;
  }

  void MenuBar::add_menu(Menu *menu)
  {
    _M_menus.push_back(unique_ptr<Menu>(menu));
    set_this_as_widget_parent(menu);
  }

  bool MenuBar::delete_menu(Menu *menu)
  {
    auto iter = find_if(_M_menus.begin(), _M_menus.end(), [menu](const unique_ptr<Menu> &tmp_menu) {
      return tmp_menu.get() == menu;
    });
    if(iter != _M_menus.end()) {
      _M_menus.erase(iter);
      unset_this_as_widget_parent(iter->get());
      return true;
    }
    return false;
  }

  void MenuBar::delete_all_menus()
  {
    for(auto &menu : _M_menus) unset_this_as_widget_parent(menu.get());
    _M_menus.clear();
  }

  const char *MenuBar::name() const
  { return "menu_bar"; }

  void MenuBar::update_child_points()
  {
    Rectangle<int> inner_bounds = bounds_to_inner_bounds(bounds());
    HAlignment h_align = HAlignment::LEFT;
    VAlignment v_align = VAlignment::CENTER;
    inner_bounds.width = numeric_limits<int>::max();
    for(auto &menu : _M_menus) {
      if(menu->is_visible()) {
        Edges<int> widget_margin = menu->margin();
        int widget_margin_box_width = menu->bounds().width + widget_margin.left + widget_margin.right;
        Rectangle<int> widget_area_bounds = inner_bounds;
        widget_area_bounds.width = widget_margin_box_width;
        widget_area_bounds.x += widget_margin.left;
        widget_area_bounds.y += widget_margin.top;
        widget_area_bounds.width -= widget_margin.left + widget_margin.right;
        widget_area_bounds.height -= widget_margin.top + widget_margin.bottom;
        widget_area_bounds.width = max(widget_area_bounds.width, 0);
        widget_area_bounds.height = max(widget_area_bounds.height, 0);
        // Updates top left point of menu.
        menu->update_point(widget_area_bounds, &h_align, &v_align);
        inner_bounds.x += widget_margin_box_width;
      }
    }
  }

  void MenuBar::update_child_sizes(Canvas *canvas, const Dimension<int> &area_size)
  {
    Dimension<int> inner_area_size = area_size_to_inner_area_size(area_size);
    Widget *prev_menu = nullptr;
    for(auto &menu : _M_menus) {
      if(menu->is_visible()) {
        if(prev_menu != nullptr) {
          bool is_adjacent = menu->has_adjacency_to(prev_menu);
          is_adjacent = (is_adjacent && prev_menu->v_align() == menu->v_align());
          if(is_adjacent) {
            prev_menu->set_pseudo_classes(prev_menu->pseudo_classes() | PseudoClasses::ADJACENT_TO_RIGHT);
            menu->set_pseudo_classes(menu->pseudo_classes() | PseudoClasses::ADJACENT_TO_LEFT);
            if((menu->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
              prev_menu->set_pseudo_classes(prev_menu->pseudo_classes() | PseudoClasses::RIGHT_ACTIVE);
            if((prev_menu->pseudo_classes() & PseudoClasses::ACTIVE) == PseudoClasses::NONE)
              menu->set_pseudo_classes(menu->pseudo_classes() | PseudoClasses::LEFT_ACTIVE);
          } else {
            prev_menu->set_pseudo_classes(prev_menu->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_RIGHT | PseudoClasses::RIGHT_ACTIVE));
            menu->set_pseudo_classes(menu->pseudo_classes() & ~(PseudoClasses::ADJACENT_TO_LEFT | PseudoClasses::LEFT_ACTIVE));
          }
        }
        prev_menu = menu.get();
      }
    }
    HAlignment h_align = HAlignment::LEFT;
    VAlignment v_align = VAlignment::CENTER;
    bool is_height = (inner_area_size.height == numeric_limits<int>::max());
    bool can_again_update = false;
    _M_menu_height = 0;
    for(auto &menu : _M_menus) {
      if(menu->is_visible()) {
        Edges<int> menu_margin = menu->margin();
        Dimension<int> menu_area_size;
        menu_area_size.width = inner_area_size.width;
        menu_area_size.height = inner_area_size.height;
        if(menu_area_size.width != numeric_limits<int>::max())
          menu_area_size.width -= menu_margin.left + menu_margin.right;
        if(menu_area_size.height != numeric_limits<int>::max())
          menu_area_size.height -= menu_margin.top + menu_margin.bottom;
        menu_area_size.height = max(menu_area_size.height, 0);
        // Updates menu size.
        menu->update_size(canvas, menu_area_size, &h_align, &v_align);
        if(inner_area_size.width != numeric_limits<int>::max()) {
          inner_area_size.width -= menu->bounds().width;
          inner_area_size.width -= menu_margin.left + menu_margin.right;
          inner_area_size.width = max(inner_area_size.width, 0);
        }
        int menu_margin_box_height = menu->bounds().height;
        menu_margin_box_height -= menu_margin.top + menu_margin.bottom;
        menu_margin_box_height = max(menu_margin_box_height, 0);
        _M_menu_height = max(_M_menu_height, menu_margin_box_height);
        can_again_update |= (!is_height && can_again_update_height());
      }
    }
    if(can_again_update) {
      for(auto &menu : _M_menus) {
        if(menu->is_visible()) {
          Dimension<int> menu_area_size = menu->bounds().size();
          if(!is_height && menu->can_again_update_height()) {
            Edges<int> menu_margin = menu->margin();
            menu_area_size.height = _M_menu_height;
            menu_area_size.height -= menu_margin.top + menu_margin.bottom;
            if(menu_area_size != menu->bounds().size()) {
              // Updates widget size.
              menu->update_size(canvas, menu_area_size);
            }
          }
        }
      }
    }
  }

  void MenuBar::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  {
    int content_width = accumulate(_M_menus.begin(), _M_menus.end(), 0, [](int width, const unique_ptr<Menu> &menu) {
      if(menu->is_visible()) {
        Edges<int> menu_margin = menu->margin();
        int menu_margin_box_width = menu->bounds().width;
        menu_margin_box_width += menu_margin.left + menu_margin.right;
        return width + menu_margin_box_width;
      } else
        return width;
    });
    set_content_size(Dimension<int>(content_width, _M_menu_height));
  }

  void MenuBar::draw_children(Canvas *canvas, const Rectangle<int> &inner_bounds)
  {
    for(auto &menu : _M_menus) {
      if(menu->is_visible()) {
        canvas->save();
        canvas->rect(menu->bounds().x, menu->bounds().y, menu->bounds().width, menu->bounds().height);
        canvas->clip();
        canvas->new_path();
        menu->draw(canvas);
        canvas->restore();
      }
    }
  }

  bool MenuBar::invoke_fun_for_event(const Point<double> &point, const function<bool (Widget *, const Point<double> &)> &fun)
  {
    Point<int> int_point(round(point.x), round(point.x));
    bool cant_invoke = false;
    for(auto &menu : _M_menus) {
      Rectangle<int> result;
      if(child_event_bounds().intersect(menu->bounds(), result) && result.contain(int_point)) {
        cant_invoke = menu->invoke_fun_for_event(point, fun);
        break;
      }
    }
    return !cant_invoke ? fun(this, point) : true;
  }
}
