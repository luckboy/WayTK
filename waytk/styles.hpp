/*
 * Copyright (c) 2017 Łukasz Szpakowski
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
#ifndef _STYLES_HPP
#define _STYLES_HPP

#include <list>
#include <utility>
#include <waytk.hpp>

namespace waytk
{
  namespace priv
  {
    enum class Direction
    {
      TOP_TO_BOTTOM,
      LEFT_TO_RIGHT,
      TOP_LEFT_TO_BOTTOM_RIGHT,
      TOP_RIGHT_TO_BOTTOM_LEFT
    };
    
    enum class Shape
    {
      ELIPSE,
      CIRCLE
    };

    struct Gradient
    { std::vector<ColorStop> color_stops; };
    
    struct LinearGradient : public Gradient
    { Direction direction; };

    struct RadialGradient : public Gradient
    { Shape shape; };
    
    typedef std::unique_ptr<Gradient> GradientUniquePtr;

    class ImplStyles : public Styles
    {
      Edges<std::list<std::pair<PseudoClasses, int>>> _M_margins;
      Edges<std::list<std::pair<PseudoClasses, int>>> _M_borders;
      Edges<std::list<std::pair<PseudoClasses, int>>> _M_paddings;
      std::list<std::pair<PseudoClasses, Color>> _M_foreground_colors;
      Corners<std::list<std::pair<PseudoClasses, double>>> _M_border_radiuses;
      Corners<std::list<std::pair<PseudoClasses, double>>> _M_padding_radiuses;
      std::list<std::pair<PseudoClasses, Color>> _M_background_colors;
      std::list<std::pair<PseudoClasses, GradientUniquePtr>> _M_background_gradients;
      Edges<std::list<std::pair<PseudoClasses, Color>>> _M_border_colors;
      Edges<std::list<std::pair<PseudoClasses, GradientUniquePtr>>> _M_border_gradients;
    public:
      virtual ~ImplStyles();

      virtual Edges<int> margin(PseudoClasses pseudo_classes) const;

      virtual Edges<int> border(PseudoClasses pseudo_classes) const;

      virtual Edges<int> padding(PseudoClasses pseudo_classes) const;

      virtual void draw_background(PseudoClasses pseudo_classes, Canvas *canvas, const Rectangle<int> &rect) const;

      virtual Color foreground_color(PseudoClasses pseudo_classes) const;

      virtual bool has_adjacency_to();

      void add_margin_top(PseudoClasses pseudo_classes, int top)
      { _M_margins.top.push_back(std::make_pair(pseudo_classes, top)); }

      void add_margin_right(PseudoClasses pseudo_classes, int right)
      { _M_margins.right.push_back(std::make_pair(pseudo_classes, right)); }

      void add_margin_bottom(PseudoClasses pseudo_classes, int bottom)
      { _M_margins.bottom.push_back(std::make_pair(pseudo_classes, bottom)); }

      void add_margin_left(PseudoClasses pseudo_classes, int left)
      { _M_margins.left.push_back(std::make_pair(pseudo_classes, left)); }

      void add_margin(PseudoClasses pseudo_classes, const Edges<int> &margin)
      {
        add_margin_top(pseudo_classes, margin.top);
        add_margin_right(pseudo_classes, margin.right);
        add_margin_bottom(pseudo_classes, margin.bottom);
        add_margin_left(pseudo_classes, margin.left);
      }

      void add_border_top(PseudoClasses pseudo_classes, int top)
      { _M_borders.top.push_back(std::make_pair(pseudo_classes, top)); }

      void add_border_right(PseudoClasses pseudo_classes, int right)
      { _M_borders.right.push_back(std::make_pair(pseudo_classes, right)); }

      void add_border_bottom(PseudoClasses pseudo_classes, int bottom)
      { _M_borders.bottom.push_back(std::make_pair(pseudo_classes, bottom)); }

      void add_border_left(PseudoClasses pseudo_classes, int left)
      { _M_borders.left.push_back(std::make_pair(pseudo_classes, left)); }

      void add_border(PseudoClasses pseudo_classes, const Edges<int> &border)
      {
        add_border_top(pseudo_classes, border.top);
        add_border_right(pseudo_classes, border.right);
        add_border_bottom(pseudo_classes, border.bottom);
        add_border_left(pseudo_classes, border.left);
      }

      void add_padding_top(PseudoClasses pseudo_classes, int top)
      { _M_paddings.top.push_back(std::make_pair(pseudo_classes, top)); }

      void add_padding_right(PseudoClasses pseudo_classes, int right)
      { _M_paddings.right.push_back(std::make_pair(pseudo_classes, right)); }

      void add_padding_bottom(PseudoClasses pseudo_classes, int bottom)
      { _M_paddings.bottom.push_back(std::make_pair(pseudo_classes, bottom)); }

      void add_padding_left(PseudoClasses pseudo_classes, int left)
      { _M_paddings.left.push_back(std::make_pair(pseudo_classes, left)); }

      void add_padding(PseudoClasses pseudo_classes, const Edges<int> &padding)
      {
        add_padding_top(pseudo_classes, padding.top);
        add_padding_right(pseudo_classes, padding.right);
        add_padding_bottom(pseudo_classes, padding.bottom);
        add_padding_left(pseudo_classes, padding.left);
      }

      void add_foreground_color(PseudoClasses pseudo_classes, Color color)
      { _M_foreground_colors.push_back(std::make_pair(pseudo_classes, color)); }

      void add_border_radius_top_left(PseudoClasses pseudo_classes, double top_left)
      { _M_border_radiuses.top_left.push_back(std::make_pair(pseudo_classes, top_left)); }

      void add_border_radius_top_right(PseudoClasses pseudo_classes, double top_right)
      { _M_border_radiuses.top_right.push_back(std::make_pair(pseudo_classes, top_right)); }

      void add_border_radius_bottom_right(PseudoClasses pseudo_classes, double bottom_right)
      { _M_border_radiuses.top_left.push_back(std::make_pair(pseudo_classes, bottom_right)); }

      void add_border_radius_bottom_left(PseudoClasses pseudo_classes, double bottom_left)
      { _M_border_radiuses.top_left.push_back(std::make_pair(pseudo_classes, bottom_left)); }

      void add_border_radius(PseudoClasses pseudo_classes, const Corners<double> &radius)
      {
        add_border_radius_top_left(pseudo_classes, radius.top_left);
        add_border_radius_top_right(pseudo_classes, radius.top_right);
        add_border_radius_bottom_right(pseudo_classes, radius.bottom_right);
        add_border_radius_bottom_left(pseudo_classes, radius.bottom_left);
      }

      void add_padding_radius_top_left(PseudoClasses pseudo_classes, double top_left)
      { _M_padding_radiuses.top_left.push_back(std::make_pair(pseudo_classes, top_left)); }

      void add_padding_radius_top_right(PseudoClasses pseudo_classes, double top_right)
      { _M_padding_radiuses.top_right.push_back(std::make_pair(pseudo_classes, top_right)); }

      void add_padding_radius_bottom_right(PseudoClasses pseudo_classes, double bottom_right)
      { _M_padding_radiuses.top_left.push_back(std::make_pair(pseudo_classes, bottom_right)); }

      void add_padding_radius_bottom_left(PseudoClasses pseudo_classes, double bottom_left)
      { _M_padding_radiuses.top_left.push_back(std::make_pair(pseudo_classes, bottom_left)); }

      void add_padding_radius(PseudoClasses pseudo_classes, const Corners<double> &radius)
      {
        add_padding_radius_top_left(pseudo_classes, radius.top_left);
        add_padding_radius_top_right(pseudo_classes, radius.top_right);
        add_padding_radius_bottom_right(pseudo_classes, radius.bottom_right);
        add_padding_radius_bottom_left(pseudo_classes, radius.bottom_left);
      }
      
      void add_background_color(PseudoClasses pseudo_classes, Color color)
      { _M_background_colors.push_back(std::make_pair(pseudo_classes, color)); }

      void add_background_gradient(PseudoClasses pseudo_classes, Gradient *gradient)
      { _M_background_gradients.push_back(std::make_pair(pseudo_classes, GradientUniquePtr(gradient))); }

      void add_background_gradient(PseudoClasses pseudo_classes, GradientUniquePtr &gradient)
      { add_background_gradient(pseudo_classes, gradient.release()); }

      void add_border_gradient_top(PseudoClasses pseudo_classes, Gradient *top)
      { _M_border_gradients.top.push_back(std::make_pair(pseudo_classes, GradientUniquePtr(top))); }

      void add_border_gradient_top(PseudoClasses pseudo_classes, GradientUniquePtr &top)
      { add_border_gradient_top(pseudo_classes, top.release()); }
      
      void add_border_gradient_right(PseudoClasses pseudo_classes, Gradient *right)
      { _M_border_gradients.right.push_back(std::make_pair(pseudo_classes, GradientUniquePtr(right))); }

      void add_border_gradient_right(PseudoClasses pseudo_classes, GradientUniquePtr &right)
      { add_border_gradient_right(pseudo_classes, right.release()); }

      void add_border_gradient_bottom(PseudoClasses pseudo_classes, Gradient *bottom)
      { _M_border_gradients.bottom.push_back(std::make_pair(pseudo_classes, GradientUniquePtr(bottom))); }

      void add_border_gradient_bottom(PseudoClasses pseudo_classes, GradientUniquePtr &bottom)
      { add_border_gradient_bottom(pseudo_classes, bottom.release()); }

      void add_border_gradient_left(PseudoClasses pseudo_classes, Gradient *left)
      { _M_border_gradients.left.push_back(std::make_pair(pseudo_classes, GradientUniquePtr(left))); }

      void add_border_gradient_left(PseudoClasses pseudo_classes, GradientUniquePtr &left)
      { add_border_gradient_left(pseudo_classes, left.release()); }

      void add_border_gradients(PseudoClasses pseudo_classes, const Edges<Gradient *> &gradients)
      {
        add_border_gradient_top(pseudo_classes, gradients.top);
        add_border_gradient_right(pseudo_classes, gradients.right);
        add_border_gradient_bottom(pseudo_classes, gradients.bottom);
        add_border_gradient_left(pseudo_classes, gradients.left);
      }

      void add_border_gradients(PseudoClasses pseudo_classes, Edges<GradientUniquePtr> &gradients)
      {
        add_border_gradient_top(pseudo_classes, gradients.top);
        add_border_gradient_right(pseudo_classes, gradients.right);
        add_border_gradient_bottom(pseudo_classes, gradients.bottom);
        add_border_gradient_left(pseudo_classes, gradients.left);
      }
    };
  }
}

#endif
