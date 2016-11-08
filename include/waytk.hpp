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
#ifndef _WAYTK_HPP
#define _WAYTK_HPP

#include <memory>
#include <waytk/adapters.hpp>
#include <waytk/callback.hpp>
#include <waytk/canvas.hpp>
#include <waytk/exceptions.hpp>
#include <waytk/modifiers.hpp>
#include <waytk/structs.hpp>
#include <waytk/styles.hpp>
#include <waytk/surface.hpp>
#include <waytk/text_buffer.hpp>
#include <waytk/util.hpp>
#include <waytk/widgets.hpp>

namespace waytk
{
  void add_surface(const std::shared_ptr<Surface> &surface);

  inline void add_surface(Surface *surface)
  { add_surface(std::shared_ptr<Surface>(surface)); }

  bool delete_surface(const std::shared_ptr<Surface> &surface);

  inline bool delete_surface(Surface *surface)
  { return delete_surface(std::shared_ptr<Surface>(surface)); }

  int run_loop_main();
}

#endif
