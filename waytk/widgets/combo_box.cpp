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
  namespace priv
  {
    //
    // A ComboBoxListBase class.
    //

    ComboBoxListBase::~ComboBoxListBase() {}
  }

  namespace
  {
    //
    // A ComboBoxList class.
    //

    class ComboBoxList : public Widget, public priv::ComboBoxListBase
    {
    public:
      ComboBoxList(const shared_ptr<priv::ComboBoxFields> &fields);

      virtual ~ComboBoxList();
    };

    ComboBoxList::ComboBoxList(const shared_ptr<priv::ComboBoxFields> &fields)
      : priv::ComboBoxListBase(fields)
    { throw exception(); }

    ComboBoxList::~ComboBoxList() {}
  }

  //
  // A ComboBox class.
  //
  
  ComboBox::~ComboBox()
  { delete_surface(_M_fields->_M_popup_surface); }

  void ComboBox::initialize(const shared_ptr<ComboBoxAdapter> &adapter)
  {
    using namespace placeholders;
    this->Button::initialize(Icon(), string(), [](Widget *widget) {});
    _M_fields = shared_ptr<priv::ComboBoxFields>(new priv::ComboBoxFields());
    _M_fields->_M_selected_pos = 0;
    _M_fields->_M_adapter = adapter;
    _M_fields->_M_on_selection_callback.set_listener([](Widget *widget, size_t pos) {});
    _M_fields->_M_selected_item_y = 0;
    _M_fields->_M_popup_surface = shared_ptr<Surface>(new Surface(([&]() -> Widget * {
      Scroll *scroll = new Scroll(new ComboBoxList(_M_fields));
      scroll->set_auto_v_scroll_bar(true);
      return scroll;
    })()));
    _M_fields->_M_on_selection = [this](size_t pos) { on_selection(pos); };
    add_surface(_M_fields->_M_popup_surface);
  }

  void ComboBox::set_selected_pos(size_t pos)
  {
    if(pos < _M_fields->_M_adapter->item_count()) {
      size_t old_pos = _M_fields->_M_selected_pos;
      _M_fields->_M_selected_pos = pos;
      if(_M_fields->_M_selected_pos != old_pos) on_selection(_M_fields->_M_selected_pos);
    }
  }

  const char *ComboBox::name() const
  { return "combo_box"; }

  void ComboBox::draw(Canvas &canvas)
  { throw exception(); }

  void ComboBox::on_click()
  {
    Point<int> point(bounds().x, bounds().y - _M_fields->_M_selected_item_y);
    _M_fields->_M_popup_surface->set_popup(surface().lock(), point);
    _M_fields->_M_popup_surface->set_visible(true);
  }

  void ComboBox::on_selection(size_t pos)
  { _M_fields->_M_on_selection_callback(this, pos); }
}
