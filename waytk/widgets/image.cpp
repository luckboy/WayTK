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
  Image::~Image() {}

  void Image::initialize(const shared_ptr<CanvasImage> &image)
  { _M_image = image; }

  const char *Image::name() const
  { return "image"; }

  void Image::update_content_size(Canvas *canvas, const Dimension<int> &area_size)
  { set_content_size(_M_image->size()); }

  void Image::draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds)
  {
    canvas->save();
    canvas->rect(inner_bounds.x, inner_bounds.y, inner_bounds.width, inner_bounds.height);
    canvas->set_image(_M_image.get(), inner_bounds.x, inner_bounds.y);
    canvas->fill();
    canvas->restore();
  }
}
