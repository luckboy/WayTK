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
  RadioButton::~RadioButton() {}

  void RadioButton::initialize(const Icon &icon, const string &label, bool is_checked, const shared_ptr<RadioGroup> &group)
  {
    this->CheckBox::initialize(icon, label, is_checked);
    _M_group = group;
  }

  void RadioButton::set_group(const shared_ptr<RadioGroup> &group)
  {
    _M_group = group;
    for(auto radio_button : _M_group->radio_buttons()) {
      if(is_checked()) radio_button->set_checked(false);
    }
    _M_group->add_radio_button(this);
  }

  const char *RadioButton::name() const
  { return "radio_button"; }

  void RadioButton::on_check(bool is_checked)
  {
    for(auto radio_button : _M_group->radio_buttons()) {
      if(radio_button != this) radio_button->set_checked(false);
    }
    this->CheckBox::on_check(is_checked);
  }

  const char *RadioButton::label_name() const
  { return "radio_button.label"; }

  const char *RadioButton::check_name() const
  { return "radio_button.radio"; }
}

