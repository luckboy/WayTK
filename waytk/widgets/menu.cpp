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
#include <waytk.hpp>

using namespace std;

namespace waytk
{
  Menu::~Menu() {}

  void Menu::initialize(const Icon &icon, const string &label, initializer_list<MenuItem *> menu_items)
  {
    this->MenuItem::initialize(icon, label, [](Widget *widget) {});
    _M_menu_items.clear();
    for(auto menu_item : menu_items) {
      _M_menu_items.push_back(unique_ptr<MenuItem>(menu_item));
      set_this_as_widget_parent(menu_item);
    }
  }

  void Menu::add_menu_item(MenuItem *menu_item)
  {
    _M_menu_items.push_back(unique_ptr<MenuItem>(menu_item));
    set_this_as_widget_parent(menu_item);
  }

  bool Menu::delete_menu_item(MenuItem *menu_item)
  {
    auto iter = find_if(_M_menu_items.begin(), _M_menu_items.end(), [menu_item](const unique_ptr<MenuItem> &tmp_menu_item) {
      return tmp_menu_item.get() == menu_item;
    });
    if(iter != _M_menu_items.end()) {
      _M_menu_items.erase(iter);
      unset_this_as_widget_parent(iter->get());
      return true;
    }
    return false;
  }

  void Menu::delete_all_menu_items()
  {
    for(auto &menu_item : _M_menu_items) unset_this_as_widget_parent(menu_item.get());
    _M_menu_items.clear();
  }

  const char *Menu::name() const
  { return "menu"; }

  const char *Menu::icon_name() const
  { return "menu.icon"; }

  const char *Menu::label_name() const
  { return "menu.label"; }
}
