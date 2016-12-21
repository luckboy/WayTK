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
#ifndef _WIDGET_VIEWPORT_HPP
#define _WIDGET_VIEWPORT_HPP

#include <waytk.hpp>

namespace waytk
{
  namespace priv
  {
    class WidgetViewport : public Viewport
    {
      Widget *_M_widget;
      Rectangle<int> _M_bounds;
      Point<int> _M_view_point;
    public:
      WidgetViewport(Widget *widget) :
        _M_widget(widget), _M_bounds(0, 0, 0, 0), _M_view_point(0, 0) {}

      virtual ~WidgetViewport();

      virtual Edges<int> margin() const;

      virtual Point<int> point() const;

      virtual Dimension<int> size() const;

      virtual void set_size(const Dimension<int> &size);
      
      virtual void update_point(Canvas *canvas);

      virtual void move_view_to_top();

      virtual void move_view_to_bottom();

      virtual void h_move_view(int x);

      virtual void v_move_view(int y);

      virtual int h_scroll_slider_x(int width) const;

      virtual void set_h_scroll_slider_x(int x, int width);

      virtual void add_onto_h_scroll_slider_x(int x, int width);

      virtual int h_scroll_slider_width(int width) const;

      virtual int v_scroll_slider_y(int height) const;

      virtual void set_v_scroll_slider_y(int y, int height); 
      
      virtual void add_onto_v_scroll_slider_y(int y, int height);

      virtual int v_scroll_slider_height(int height) const;

      virtual bool width_is_less_than_clien_width() const;

      virtual bool height_is_less_than_clien_height() const;

      virtual int max_width() const;

      virtual int max_height() const;

      virtual void update_client_point(const Point<int> &viewport_point);

      virtual void update_client_size(Canvas *canvas);

      virtual Edges<int> widget_margin() const;

      virtual void update_widget_point(const Rectangle<int> &area_bounds);

      virtual void update_widget_size(Canvas *canvas, const Dimension<int> &area_size);

      Widget *widget() const
      { return _M_widget; }

      const Rectangle<int> &bounds() const
      { return _M_bounds; }

      const Point<int> &view_point() const
      { return _M_view_point; }

      virtual Dimension<int> client_size() const;
    };
  }
}

#endif
