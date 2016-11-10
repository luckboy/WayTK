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
  CheckMenuItem::~CheckMenuItem() {}

  const char *CheckMenuItem::name() const
  { return "check_menu_item"; }

  void CheckMenuItem::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { this->CheckBox::update_content_size(canvas, area_size); }

  void CheckMenuItem::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  { this->CheckBox::draw_content(canvas, inner_bounds); }

  const char *CheckMenuItem::icon_name() const
  { return "check_menu_item.icon"; }

  IconSize CheckMenuItem::icon_size() const
  { return this->MenuItem::icon_size(); }

  bool CheckMenuItem::can_draw_icon() const
  { return true; }
  
  const char *CheckMenuItem::label_name() const
  { return "check_menu_item.label"; }

  const char *CheckMenuItem::check_name() const
  { return "check_menu_item.check"; }
}