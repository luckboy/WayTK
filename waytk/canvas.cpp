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
#include <cmath>
#include "canvas.hpp"

using namespace std;

namespace waytk
{
  namespace priv
  {    
    //
    // An ImplCanvasPattern class.
    //

    ImplCanvasPattern::~ImplCanvasPattern() {}

    CanvasPattern::Native *ImplCanvasPattern::native()
    { return reinterpret_cast<Native *>(_M_pattern.get()); }

    //
    // An ImplCanvasImage class.
    //

    ImplCanvasImage::~ImplCanvasImage() {}

    Dimension<int> ImplCanvasImage::size()
    {
      int width = cairo_image_surface_get_width(_M_surface.get());
      throw_canvas_exception_for_failure(_M_surface.get());
      int height = cairo_image_surface_get_height(_M_surface.get());
      throw_canvas_exception_for_failure(_M_surface.get());
      return Dimension<int>(width, height);
    }

    CanvasImage::Native *ImplCanvasImage::native()
    { return reinterpret_cast<Native *>(_M_surface.get()); }

    //
    // An ImplCanvasModifiableImage class.
    //

    ImplCanvasModifiableImage::~ImplCanvasModifiableImage() {}
    
    bool ImplCanvasModifiableImage::is_modifiable() const
    { return this->CanvasModifiableImage::is_modifiable(); }

    Canvas *ImplCanvasModifiableImage::canvas()
    {
      CairoUniquePtr context(::cairo_create(_M_surface.get()));
      throw_canvas_exception_for_failure(::cairo_status(context.get()));
      throw_canvas_exception_for_failure(_M_surface.get());
      return new ImplCanvas(context);
    }

    //
    // An ImplCanvasScalableImage class.
    //

    ImplCanvasScalableImage::~ImplCanvasScalableImage() {}

    Dimension<int> ImplCanvasScalableImage::size()
    {
      Dimension<int> tmp_size;
      ::RsvgDimensionData dim_data;
      ::rsvg_handle_get_dimensions(_M_handle.get(), &dim_data);
      tmp_size.width = ceil(dim_data.width * _M_sp.x);
      tmp_size.height = ceil(dim_data.height * _M_sp.y);
      return tmp_size;
    }

    bool ImplCanvasScalableImage::is_scalable() const
    { return this->CanvasScalableImage::is_scalable(); }

    CanvasImage *ImplCanvasScalableImage::scale(const Point<double> &sp)
    {
      unique_ptr<ImplCanvasScalableImage> image(new ImplCanvasScalableImage(_M_handle, Point<double>(_M_sp.x * sp.x, _M_sp.y * sp.y)));
      image->update_surface();
      return image.release();
    }

    CanvasModifiableImage *ImplCanvasScalableImage::modifiable_image()
    {
      if(_M_surface.get() != nullptr)
        return this->ImplCanvasImage::modifiable_image();
      else
        return new ImplCanvasModifiableImage(new_surface());
    }

    CanvasImage::Native *ImplCanvasScalableImage::native()
    {
      if(_M_surface.get() == nullptr) update_surface();
      return reinterpret_cast<Native *>(_M_surface.get());
    }

    ::cairo_surface_t *ImplCanvasScalableImage::new_surface()
    {
      Dimension<int> tmp_size = size();
      CairoSurfaceUniquePtr surface(::cairo_image_surface_create(::CAIRO_FORMAT_ARGB32, tmp_size.width, tmp_size.height));
      throw_canvas_exception_for_failure(surface.get());
      CairoUniquePtr context(::cairo_create(surface.get()));
      ::cairo_scale(context.get(), _M_sp.x, _M_sp.y);
      throw_canvas_exception_for_failure(context.get());
      if(::rsvg_handle_render_cairo(_M_handle.get(), context.get()) == FALSE)
        throw new CanvasException("can't render SVG image");
      return surface.release();
    }

    //
    // An ImplCanvasPath class.
    //

    ImplCanvasPath::~ImplCanvasPath() {}

    CanvasPath::Native *ImplCanvasPath::native()
    { return reinterpret_cast<Native *>(_M_path.get()); }

    //
    // An ImplCanvasTransformation class.
    //

    ImplCanvasTransformation::~ImplCanvasTransformation() {}

    ImplCanvasTransformation::Native *ImplCanvasTransformation::native()
    { return reinterpret_cast<Native *>(&_M_matrix); }

    //
    // An ImplCanvasFontFace class.
    //

    ImplCanvasFontFace::~ImplCanvasFontFace() {}

    CanvasFontFace::Native *ImplCanvasFontFace::native()
    { return reinterpret_cast<Native *>(_M_font_face.get()); }

    //
    // An ImplCanvas class.
    //

    ImplCanvas::~ImplCanvas() {}

    void ImplCanvas::save()
    {
      ::cairo_save(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::restore()
    {
      ::cairo_restore(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
    }

    CanvasPattern *ImplCanvas::pattern()
    {
      ::cairo_pattern_t *pattern = ::cairo_get_source(_M_context.get());
      throw_canvas_exception_for_failure(pattern);
      throw_canvas_exception_for_failure(_M_context.get());
      ::cairo_pattern_reference(pattern);
      return new ImplCanvasPattern(pattern);
    }

    void ImplCanvas::set_pattern(CanvasPattern *pattern)
    {
      ::cairo_set_source(_M_context.get(), cairo_pattern(pattern));
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::set_color(Color color)
    {
      ::cairo_set_source_rgba(_M_context.get(), color.red(), color.green(), color.blue(), color.alpha());
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::set_linear_gradient(const Point<double> &p1, const Point<double> &p2, initializer_list<ColorStop> color_stops)
    { set_linear_gradient(p1, p2, color_stops.begin(), color_stops.end()); }

    void ImplCanvas::set_linear_gradient(const Point<double> &p1, const Point<double> &p2, const vector<ColorStop> &color_stops)
    { set_linear_gradient(p1, p2, color_stops.begin(), color_stops.end()); }

    void ImplCanvas::set_radial_gradient(const Point<double> &p1, double radius1, const Point<double> &p2, double radius2, initializer_list<ColorStop> color_stops)
    { set_radial_gradient(p1, radius1, p2, radius2, color_stops.begin(), color_stops.end()); }

    void ImplCanvas::set_radial_gradient(const Point<double> &p1, double radius1, const Point<double> &p2, double radius2, const vector<ColorStop> &color_stops)
    { set_radial_gradient(p1, radius1, p2, radius2, color_stops.begin(), color_stops.end()); }

    void ImplCanvas::set_image(CanvasImage *image, const Point<double> &p)
    {
      ::cairo_set_source_surface(_M_context.get(), cairo_surface(image), p.x, p.y);
      throw_canvas_exception_for_failure(_M_context.get());
    }
    
    Antialias ImplCanvas::antialias()
    {
      ::cairo_antialias_t crairo_antialias = ::cairo_get_antialias(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
      switch(crairo_antialias) {
        case ::CAIRO_ANTIALIAS_DEFAULT:
          return Antialias::DEFAULT;
        case ::CAIRO_ANTIALIAS_NONE:
          return Antialias::NONE;
        case ::CAIRO_ANTIALIAS_GRAY:
          return Antialias::GRAY;
        case ::CAIRO_ANTIALIAS_SUBPIXEL:
          return Antialias::SUBPIXEL;
        default:
          throw RuntimeException("unknown antialias");
      }
    }

    void ImplCanvas::set_antialias(Antialias antialias)
    {
      switch(antialias) {
        case Antialias::DEFAULT:
          ::cairo_set_antialias(_M_context.get(), ::CAIRO_ANTIALIAS_DEFAULT);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Antialias::NONE:
          ::cairo_set_antialias(_M_context.get(), ::CAIRO_ANTIALIAS_NONE);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Antialias::GRAY:
          ::cairo_set_antialias(_M_context.get(), ::CAIRO_ANTIALIAS_GRAY);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Antialias::SUBPIXEL:
          ::cairo_set_antialias(_M_context.get(), ::CAIRO_ANTIALIAS_SUBPIXEL);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
      }
    }

    void ImplCanvas::get_dash(vector<double> &dashes, double &offset)
    {
      dashes.clear();
      int dash_count = ::cairo_get_dash_count(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
      dashes.resize(dash_count);
      ::cairo_get_dash(_M_context.get(), dashes.data(), &offset);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::set_dash(initializer_list<double> dashes, double offset)
    {
      ::cairo_set_dash(_M_context.get(), dashes.begin(), dashes.size(), offset);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::set_dash(const vector<double> &dashes, double offset)
    {
      ::cairo_set_dash(_M_context.get(), dashes.data(), dashes.size(), offset);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    LineCap ImplCanvas::line_cap()
    {
      ::cairo_line_cap_t cairo_line_cap = ::cairo_get_line_cap(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
      switch(cairo_line_cap) {
        case ::CAIRO_LINE_CAP_BUTT:
          return LineCap::BUTT;
        case ::CAIRO_LINE_CAP_ROUND:
          return LineCap::ROUND;
        case ::CAIRO_LINE_CAP_SQUARE:
          return LineCap::SQUARE;
        default:
          throw RuntimeException("unknown line cap");
      }
    }

    void ImplCanvas::set_line_cap(LineCap line_cap)
    {
      switch(line_cap) {
        case LineCap::BUTT:
          ::cairo_set_line_cap(_M_context.get(), ::CAIRO_LINE_CAP_BUTT);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case LineCap::ROUND:
          ::cairo_set_line_cap(_M_context.get(), ::CAIRO_LINE_CAP_ROUND);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case LineCap::SQUARE:
          ::cairo_set_line_cap(_M_context.get(), ::CAIRO_LINE_CAP_SQUARE);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
      }
    }

    LineJoin ImplCanvas::line_join()
    {
      ::cairo_line_join_t cairo_join_cap = ::cairo_get_line_join(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
      switch(cairo_join_cap) {
        case ::CAIRO_LINE_JOIN_MITER:
          return LineJoin::MITER;
        case ::CAIRO_LINE_JOIN_ROUND:
          return LineJoin::ROUND;
        case ::CAIRO_LINE_JOIN_BEVEL:
          return LineJoin::BEVEL;
        default:
          throw RuntimeException("line join");
      }
    }

    void ImplCanvas::set_line_join(LineJoin line_join)
    {
      switch(line_join) {
        case LineJoin::MITER:
          ::cairo_set_line_join(_M_context.get(), ::CAIRO_LINE_JOIN_MITER);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case LineJoin::ROUND:
          ::cairo_set_line_join(_M_context.get(), ::CAIRO_LINE_JOIN_ROUND);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case LineJoin::BEVEL:
          ::cairo_set_line_join(_M_context.get(), ::CAIRO_LINE_JOIN_BEVEL);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
      }
    }

    double ImplCanvas::line_width()
    { 
      double tmp_line_width = ::cairo_get_line_width(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
      return tmp_line_width;
    }

    void ImplCanvas::set_line_width(double width)
    {
      ::cairo_set_line_width(_M_context.get(), width);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    double ImplCanvas::miter_limit()
    {
      double tmp_miter_limit = ::cairo_get_miter_limit(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
      return tmp_miter_limit;
    }

    void ImplCanvas::set_miter_limit(double limit)
    {
      ::cairo_set_miter_limit(_M_context.get(), limit);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    Operator ImplCanvas::op()
    {
      ::cairo_operator_t cairo_op = ::cairo_get_operator(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
      switch(cairo_op) {
        case ::CAIRO_OPERATOR_CLEAR:
          return Operator::CLEAR;
        case ::CAIRO_OPERATOR_SOURCE:
          return Operator::SOURCE;
        case ::CAIRO_OPERATOR_OVER:
          return Operator::OVER;
        case ::CAIRO_OPERATOR_IN:
          return Operator::IN;
        case ::CAIRO_OPERATOR_OUT:
          return Operator::OUT;
        case ::CAIRO_OPERATOR_ATOP:
          return Operator::ATOP;
        case ::CAIRO_OPERATOR_DEST:
          return Operator::DESTINATION;
        case ::CAIRO_OPERATOR_DEST_OVER:
          return Operator::DESTINATION_OVER;
        case ::CAIRO_OPERATOR_DEST_IN:
          return Operator::DESTINATION_IN;
        case ::CAIRO_OPERATOR_DEST_OUT:
          return Operator::DESTINATION_OUT;
        case ::CAIRO_OPERATOR_DEST_ATOP:
          return Operator::DESTINATION_ATOP;
        case ::CAIRO_OPERATOR_XOR:
          return Operator::XOR;
        case ::CAIRO_OPERATOR_ADD:
          return Operator::ADD;
        case ::CAIRO_OPERATOR_SATURATE:
          return Operator::SATURATE;
        default:
          throw RuntimeException("unknown operator");
      }
    }

    void ImplCanvas::set_op(Operator op)
    {
      switch(op) {
        case Operator::CLEAR:
          ::cairo_set_operator(_M_context.get(), ::CAIRO_OPERATOR_CLEAR);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Operator::SOURCE:
          ::cairo_set_operator(_M_context.get(), ::CAIRO_OPERATOR_SOURCE);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Operator::OVER:
          ::cairo_set_operator(_M_context.get(), ::CAIRO_OPERATOR_OVER);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Operator::IN:
          ::cairo_set_operator(_M_context.get(), ::CAIRO_OPERATOR_IN);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Operator::OUT:
          ::cairo_set_operator(_M_context.get(), ::CAIRO_OPERATOR_OUT);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Operator::ATOP:
          ::cairo_set_operator(_M_context.get(), ::CAIRO_OPERATOR_ATOP);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Operator::DESTINATION:
          ::cairo_set_operator(_M_context.get(), ::CAIRO_OPERATOR_DEST);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Operator::DESTINATION_OVER:
          ::cairo_set_operator(_M_context.get(), ::CAIRO_OPERATOR_DEST_OVER);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Operator::DESTINATION_IN:
          ::cairo_set_operator(_M_context.get(), ::CAIRO_OPERATOR_DEST_IN);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Operator::DESTINATION_OUT:
          ::cairo_set_operator(_M_context.get(), ::CAIRO_OPERATOR_DEST_OUT);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Operator::DESTINATION_ATOP:
          ::cairo_set_operator(_M_context.get(), ::CAIRO_OPERATOR_DEST_ATOP);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Operator::XOR:
          ::cairo_set_operator(_M_context.get(), ::CAIRO_OPERATOR_XOR);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Operator::ADD:
          ::cairo_set_operator(_M_context.get(), ::CAIRO_OPERATOR_ADD);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
        case Operator::SATURATE:
          ::cairo_set_operator(_M_context.get(), ::CAIRO_OPERATOR_SATURATE);
          throw_canvas_exception_for_failure(_M_context.get());
          break;
      }
    }

    void ImplCanvas::clip()
    {
      ::cairo_clip(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::reset_clip()
    {
      ::cairo_reset_clip(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::fill()
    {
      ::cairo_fill(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::paint()
    {
      ::cairo_paint(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::paint(unsigned alpha)
    {
      ::cairo_paint_with_alpha(_M_context.get(), alpha);
      throw_canvas_exception_for_failure(_M_context.get());
      
    }
    
    void ImplCanvas::stroke()
    {
      ::cairo_stroke(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
    }

    CanvasPath *ImplCanvas::path()
    {
      CairoPathUniquePtr path(::cairo_copy_path(_M_context.get()));
      throw_canvas_exception_for_failure(path.get());
      throw_canvas_exception_for_failure(_M_context.get());
      return new ImplCanvasPath(path);
    }

    CanvasPath *ImplCanvas::flat_path()
    {
      CairoPathUniquePtr path(::cairo_copy_path_flat(_M_context.get()));
      throw_canvas_exception_for_failure(path.get());
      throw_canvas_exception_for_failure(_M_context.get());
      return new ImplCanvasPath(path);
    }

    void ImplCanvas::append_path(CanvasPath *path)
    {
      ::cairo_append_path(_M_context.get(), cairo_path(path));
      throw_canvas_exception_for_failure(_M_context.get());
    }

    bool ImplCanvas::has_point()
    {
      bool tmp_has_point = (::cairo_has_current_point(_M_context.get()) != 0);
      throw_canvas_exception_for_failure(_M_context.get());
      return tmp_has_point;
    }

    void ImplCanvas::get_point(Point<double> &p)
    {
      ::cairo_get_current_point(_M_context.get(), &(p.x), &(p.y));
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::new_path()
    {
      ::cairo_new_path(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::close_path()
    {
      ::cairo_close_path(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::arc(const Point<double> &p, double radius, double angle1, double angle2, bool is_negative)
    {
      if(!is_negative)
        ::cairo_arc(_M_context.get(), p.x, p.y, radius, angle1, angle2);
      else
        ::cairo_arc_negative(_M_context.get(), p.x, p.y, radius, angle1, angle2);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::curve_to(const Point<double> &p1, const Point<double> &p2, const Point<double> &p3)
    {
      ::cairo_curve_to(_M_context.get(), p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::line_to(const Point<double> &p)
    {
      ::cairo_line_to(_M_context.get(), p.x, p.y);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::move_to(const Point<double> &p)
    {
      ::cairo_move_to(_M_context.get(), p.x, p.y);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::rect(const Rectangle<double> &r)
    {
      ::cairo_rectangle(_M_context.get(), r.x, r.y, r.width, r.height);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::text_path(const char *utf8)
    {
      ::cairo_text_path(_M_context.get(), utf8);
      throw_canvas_exception_for_failure(_M_context.get());
      
    }

    void ImplCanvas::text_path(const string &utf8)
    {
      ::cairo_text_path(_M_context.get(), utf8.c_str());
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::translate(const Point<double> &tp)
    {
      ::cairo_translate(_M_context.get(), tp.x, tp.y);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::scale(const Point<double> &sp)
    {
      ::cairo_scale(_M_context.get(), sp.x, sp.y);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::rotate(double angle)
    {
      ::cairo_rotate(_M_context.get(), angle);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    CanvasTransformation *ImplCanvas::transformation()
    {
      unique_ptr<ImplCanvasTransformation> transformation(new ImplCanvasTransformation());
      ::cairo_get_matrix(_M_context.get(), cairo_matrix(transformation.get()));
      throw_canvas_exception_for_failure(_M_context.get());
      return transformation.release();
    }

    void ImplCanvas::set_transformation(CanvasTransformation *transformation)
    {
      ::cairo_set_matrix(_M_context.get(), cairo_matrix(transformation));
      throw_canvas_exception_for_failure(_M_context.get());
    }

    CanvasFontFace *ImplCanvas::font_face()
    { 
      ::cairo_font_face_t *font_face = ::cairo_get_font_face(_M_context.get());
      throw_canvas_exception_for_failure(_M_context.get());
      throw_canvas_exception_for_failure(font_face);
      ::cairo_font_face_reference(font_face);
      return new ImplCanvasFontFace(font_face);
    }

    void ImplCanvas::set_font_face(const string &name, FontSlant slant, FontWeight weight)
    {
      ::cairo_font_slant_t cairo_slant = ::CAIRO_FONT_SLANT_NORMAL;
      ::cairo_font_weight_t cairo_weight = ::CAIRO_FONT_WEIGHT_NORMAL;
      switch(slant) {
        case FontSlant::NORMAL:
          cairo_slant = ::CAIRO_FONT_SLANT_NORMAL;
          break;
        case FontSlant::ITALIC:
          cairo_slant = ::CAIRO_FONT_SLANT_ITALIC;
          break;
        case FontSlant::OBLIQUE:
          cairo_slant = ::CAIRO_FONT_SLANT_OBLIQUE;
          break;
      }
      switch(weight) {
        case FontWeight::NORMAL:
          cairo_weight = ::CAIRO_FONT_WEIGHT_NORMAL;
          break;
        case FontWeight::BOLD:
          cairo_weight = ::CAIRO_FONT_WEIGHT_BOLD;
          break;
      }
      ::cairo_select_font_face(_M_context.get(), name.c_str(), cairo_slant, cairo_weight);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::set_font_face(CanvasFontFace *font_face)
    {
      ::cairo_set_font_face(_M_context.get(), cairo_font_face(font_face));
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::set_font_size(double size)
    {
      ::cairo_set_font_size(_M_context.get(), size);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::translate_font(const Point<double> &tp)
    {
      ::cairo_matrix_t matrix;
      ::cairo_get_font_matrix(_M_context.get(), &matrix);
      throw_canvas_exception_for_failure(_M_context.get());
      ::cairo_matrix_translate(&matrix, tp.x, tp.y);
      ::cairo_set_font_matrix(_M_context.get(), &matrix);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::scale_font(const Point<double> &sp)
    {
      ::cairo_matrix_t matrix;
      ::cairo_get_font_matrix(_M_context.get(), &matrix);
      throw_canvas_exception_for_failure(_M_context.get());
      ::cairo_matrix_scale(&matrix, sp.x, sp.y);
      ::cairo_set_font_matrix(_M_context.get(), &matrix);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::rotate_font(double angle)
    {
      ::cairo_matrix_t matrix;
      ::cairo_get_font_matrix(_M_context.get(), &matrix);
      throw_canvas_exception_for_failure(_M_context.get());
      ::cairo_matrix_rotate(&matrix, angle);
      ::cairo_set_font_matrix(_M_context.get(), &matrix);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    CanvasTransformation *ImplCanvas::font_transformation()
    {
      unique_ptr<ImplCanvasTransformation> transformation(new ImplCanvasTransformation());
      ::cairo_get_font_matrix(_M_context.get(), cairo_matrix(transformation.get()));
      throw_canvas_exception_for_failure(_M_context.get());
      return transformation.release();
    }

    void ImplCanvas::set_font_transformation(CanvasTransformation *transformation)
    {
      ::cairo_set_font_matrix(_M_context.get(), cairo_matrix(transformation));
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::show_text(const char *utf8)
    {
      ::cairo_show_text(_M_context.get(), utf8);
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::show_text(const string &utf8)
    {
      ::cairo_show_text(_M_context.get(), utf8.c_str());
      throw_canvas_exception_for_failure(_M_context.get());
    }

    void ImplCanvas::get_font_matrics(FontMetrics &font_metrics)
    {
      ::cairo_font_extents_t font_extents;
      ::cairo_font_extents(_M_context.get(), &font_extents);
      throw_canvas_exception_for_failure(_M_context.get());
      font_metrics.ascent = font_extents.ascent;
      font_metrics.descent = font_extents.descent;
      font_metrics.height = font_extents.height;
      font_metrics.max_x_advance = font_extents.max_x_advance;
      font_metrics.max_y_advance = font_extents.max_y_advance;
    }

    void ImplCanvas::get_text_matrics(const char *utf8, TextMetrics &text_metrics)
    {
      ::cairo_text_extents_t text_extents;
      ::cairo_text_extents(_M_context.get(), utf8, &text_extents);
      throw_canvas_exception_for_failure(_M_context.get());
      text_metrics.x_bearing = text_extents.x_bearing;
      text_metrics.y_bearing = text_extents.y_bearing;
      text_metrics.width = text_extents.width;
      text_metrics.height = text_extents.height;
      text_metrics.x_advance = text_extents.x_advance;
      text_metrics.y_advance = text_extents.y_advance;
    }

    void ImplCanvas::get_text_matrics(const string &utf8, TextMetrics &text_metrics)
    {
      ::cairo_text_extents_t text_extents;
      ::cairo_text_extents(_M_context.get(), utf8.c_str(), &text_extents);
      throw_canvas_exception_for_failure(_M_context.get());
      text_metrics.x_bearing = text_extents.x_bearing;
      text_metrics.y_bearing = text_extents.y_bearing;
      text_metrics.width = text_extents.width;
      text_metrics.height = text_extents.height;
      text_metrics.x_advance = text_extents.x_advance;
      text_metrics.y_advance = text_extents.y_advance;
    }
  }

  //
  // A CanvasPattern class.
  //

  CanvasPattern::~CanvasPattern() {}

  //
  // A CanvasImage class.
  //

  CanvasImage::~CanvasImage() {}

  bool CanvasImage::is_modifiable() const
  { return  false; }

  Canvas *CanvasImage::canvas()
  { return nullptr; }

  bool CanvasImage::is_scalable() const
  { return false; }

  CanvasImage *CanvasImage::scale(const Point<double> &sp)
  { return nullptr; }

  CanvasModifiableImage *CanvasImage::modifiable_image()
  {
    Dimension<int> tmp_size = size();
    unique_ptr<CanvasModifiableImage> image(new_canvas_modifiable_image(tmp_size));
    unique_ptr<Canvas> canvas(image->canvas());
    canvas->set_image(this, 0.0, 0.0);
    canvas->rect(0.0, 0.0, tmp_size.width, tmp_size.height);
    canvas->fill();
    return image.release();
  }

  //
  // A CanvasModifiableImage class.
  //

  CanvasModifiableImage::~CanvasModifiableImage() {}

  bool CanvasModifiableImage::is_modifiable() const
  { return true; }

  //
  // A CanvasScalableImage class.
  //

  CanvasScalableImage::~CanvasScalableImage() {}

  bool CanvasScalableImage::is_scalable() const
  { return true; }

  //
  // A CanvasPath class.
  //

  CanvasPath::~CanvasPath() {}

  //
  // A CanvasTransformation class.
  //

  CanvasTransformation::~CanvasTransformation() {}

  //
  // A CanvasFontFace class.
  //

  CanvasFontFace::~CanvasFontFace() {}

  //
  // A Canvas class.
  //

  Canvas::~Canvas() {}

  //
  // Functions.
  //

  CanvasModifiableImage *new_canvas_modifiable_image(const Dimension<int> &size)
  {
    priv::CairoSurfaceUniquePtr surface(::cairo_image_surface_create(::CAIRO_FORMAT_ARGB32, size.width, size.height));
    priv::throw_canvas_exception_for_failure(surface.get());
    return new priv::ImplCanvasModifiableImage(surface);
  }

  CanvasModifiableImage *new_canvas_modifiable_image(const Dimension<int> &size, int stride, void *data)
  {
    priv::CairoSurfaceUniquePtr surface(::cairo_image_surface_create_for_data(reinterpret_cast<unsigned char *>(data), ::CAIRO_FORMAT_ARGB32, size.width, size.height, stride));
    priv::throw_canvas_exception_for_failure(surface.get());
    return new priv::ImplCanvasModifiableImage(surface);
  }

  CanvasImage *load_canvas_image(const string &file_name)
  {
    // Loads a PNG file.
    {
      priv::CairoSurfaceUniquePtr surface(::cairo_image_surface_create_from_png(file_name.c_str()));
      ::cairo_status_t status = ::cairo_surface_status(surface.get());
      if(status == ::CAIRO_STATUS_SUCCESS) {
        return new priv::ImplCanvasModifiableImage(surface);
      } else {
        if(status == ::CAIRO_STATUS_FILE_NOT_FOUND)
          throw IOException("file not found");
        else if(status == ::CAIRO_STATUS_NO_MEMORY)
          throw CanvasException(::cairo_status_to_string(status));
      }
    }
    // Loads a SVG file.
    {
      ::GError *error;
      priv::RsvgHandleUniquePtr handle(::rsvg_handle_new_from_file(file_name.c_str(), &error));
      if(handle.get() != nullptr) {
        return  new priv::ImplCanvasScalableImage(handle);
      } else {
        priv::GErrorUniquePtr tmp_error(error);
        if(tmp_error->domain == G_FILE_ERROR) {
          switch(tmp_error->code) {
            case ::G_FILE_ERROR_NAMETOOLONG:
              throw IOException("name too long");
            case ::G_FILE_ERROR_NOENT:
              throw IOException("file not found");
            case ::G_FILE_ERROR_NOMEM:
              throw IOException("out of memory");
            default:
              throw IOException("file error");
          }
        } else if(tmp_error->domain == G_IO_CHANNEL_ERROR)
          throw IOException("io channel error");
      }
    }
    throw FileFormatException("unsupported image file format");
  }
}
