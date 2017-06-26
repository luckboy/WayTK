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
#include "waytk_priv.hpp"

using namespace std;

namespace waytk
{
  namespace priv
  {
    namespace
    { list<Surface *> visible_modal_surface_stack; }

    Surface *top_visible_modal_surface()
    { return visible_modal_surface_stack.empty() ? visible_modal_surface_stack.back() : nullptr; }

    void push_visible_modal_surface(Surface *surface)
    { visible_modal_surface_stack.push_back(surface); }

    bool delete_visible_modal_surface(Surface *surface)
    {
      auto iter = find_if(visible_modal_surface_stack.rbegin(), visible_modal_surface_stack.rend(), [surface](Surface *tmp_surface) {
        return tmp_surface == surface;
      });
      auto tmp_iter = --(iter.base());
      if(iter != visible_modal_surface_stack.rend()) {
        visible_modal_surface_stack.erase(tmp_iter);
        return true;
      }
      return false;
    }
  }
}
