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
#ifndef _CANVAS_HPP
#define _CANVAS_HPP

#include <librsvg/rsvg.h>
#include <algorithm>
#include <memory>
#include <cairo.h>
#include <waytk.hpp>

namespace waytk
{
  namespace priv
  {
    struct CairoPatternDelete
    {
      void operator()(::cairo_pattern_t *pattern) const
      { ::cairo_pattern_destroy(pattern); }
    };

    struct CairoSurfaceDelete
    {
      void operator()(::cairo_surface_t *surface) const
      { ::cairo_surface_destroy(surface); }
    };

    struct CairoPathDelete
    {
      void operator()(::cairo_path_t *path) const
      { ::cairo_path_destroy(path); }
    };

    struct CairoFontFaceDelete
    {
      void operator()(::cairo_font_face_t *font_face) const
      { ::cairo_font_face_destroy(font_face); }
    };

    struct CairoDelete
    {
      void operator()(::cairo_t *context) const
      { ::cairo_destroy(context); }
    };

    struct RsvgHandleDelete
    {
      void operator()(::RsvgHandle *handle) const
      { ::g_object_unref(reinterpret_cast<::gpointer>(handle)); }
    };

    struct GErrorDelete
    {
      void operator()(::GError *error) const
      { ::g_error_free(error); }
    };

    typedef std::unique_ptr<::cairo_pattern_t, CairoPatternDelete> CairoPatternUniquePtr;
    typedef std::unique_ptr<::cairo_surface_t, CairoSurfaceDelete> CairoSurfaceUniquePtr;
    typedef std::unique_ptr<::cairo_path_t, CairoPathDelete> CairoPathUniquePtr;
    typedef std::unique_ptr<::cairo_font_face_t, CairoFontFaceDelete> CairoFontFaceUniquePtr;
    typedef std::unique_ptr<::cairo_t, CairoDelete> CairoUniquePtr;
    typedef std::unique_ptr<::RsvgHandle, RsvgHandleDelete> RsvgHandleUniquePtr;
    typedef std::unique_ptr<::GError, GErrorDelete> GErrorUniquePtr;

    inline void throw_canvas_exception_for_failure(::cairo_t *context);
    inline void throw_canvas_exception_for_failure(::cairo_pattern_t *pattern);
    inline void throw_canvas_exception_for_failure(::cairo_surface_t *surface);
    inline void throw_canvas_exception_for_failure(::cairo_path_t *path);
    inline void throw_canvas_exception_for_failure(::cairo_font_face_t *font_face);
    
    class ImplCanvasPattern : public CanvasPattern
    {
      CairoPatternUniquePtr _M_pattern;
    public:
      explicit ImplCanvasPattern(::cairo_pattern_t *pattern) :
        _M_pattern(pattern) {}

      explicit ImplCanvasPattern(CairoPatternUniquePtr &pattern) :
        _M_pattern(pattern.release()) {}

      virtual ~ImplCanvasPattern();
    protected:
      virtual Native *native();
    };

    class ImplCanvasImage : public virtual CanvasImage
    {
    protected:
      CairoSurfaceUniquePtr _M_surface;

      ImplCanvasImage() {}

      ImplCanvasImage(::cairo_surface_t *surface) :
        _M_surface(surface) {}
    public:
      virtual ~ImplCanvasImage();

      virtual Dimension<int> size();
    protected:
      virtual Native *native();
    };

    class ImplCanvasModifiableImage : public ImplCanvasImage, public CanvasModifiableImage
    {
      ImplCanvasModifiableImage() :
        CanvasImage(), ImplCanvasImage(), CanvasModifiableImage() {}
    public:
      explicit ImplCanvasModifiableImage(::cairo_surface_t *surface) :
        CanvasImage(), ImplCanvasImage(surface), CanvasModifiableImage() {}

      explicit ImplCanvasModifiableImage(CairoSurfaceUniquePtr &surface) :
        CanvasImage(), ImplCanvasImage(surface.release()), CanvasModifiableImage() {}

      virtual ~ImplCanvasModifiableImage();

      virtual bool is_modifiable() const;

      virtual Canvas *canvas();
    };
    
    class ImplCanvasScalableImage : public ImplCanvasImage, public CanvasScalableImage
    {
      std::shared_ptr<::RsvgHandle> _M_handle;
      Point<double> _M_sp;
    public:
      explicit ImplCanvasScalableImage(::RsvgHandle *handle, const Point<double> &sp = Point<double>(1.0, 1.0)) :
        CanvasImage(), ImplCanvasImage(), CanvasScalableImage(),
        _M_handle(handle, RsvgHandleDelete()), _M_sp(1.0, 1.0) {}

      explicit ImplCanvasScalableImage(RsvgHandleUniquePtr &handle, const Point<double> &sp = Point<double>(1.0, 1.0)) :
        CanvasImage(), ImplCanvasImage(), CanvasScalableImage(),
        _M_handle(handle.release(), RsvgHandleDelete()), _M_sp(1.0, 1.0) {}

      explicit ImplCanvasScalableImage(const std::shared_ptr<::RsvgHandle> &handle, const Point<double> &sp = Point<double>(1.0, 1.0)) :
        CanvasImage(), ImplCanvasImage(), CanvasScalableImage(),
        _M_handle(handle), _M_sp(1.0, 1.0) {}

      virtual ~ImplCanvasScalableImage();

      virtual Dimension<int> size();

      virtual bool is_scalable() const;
      
      virtual CanvasImage *scale(const Point<double> &sp);

      virtual CanvasModifiableImage *modifiable_image();
    protected:
      virtual Native *native();

      void update_surface()
      { _M_surface = CairoSurfaceUniquePtr(new_surface()); }

      ::cairo_surface_t *new_surface();
    };

    class ImplCanvasPath : public CanvasPath
    {
      CairoPathUniquePtr _M_path;
    public:
      explicit ImplCanvasPath(::cairo_path_t *path) :
        _M_path(path) {}

      explicit ImplCanvasPath(CairoPathUniquePtr &path) :
        _M_path(path.release()) {}

      virtual ~ImplCanvasPath();
    protected:
      virtual Native *native();
    };

    class ImplCanvasTransformation : public CanvasTransformation
    {
      ::cairo_matrix_t _M_matrix;
    public:
      ImplCanvasTransformation() {}

      virtual ~ImplCanvasTransformation();
    protected:
      virtual Native *native();
    };

    class ImplCanvasFontFace : public CanvasFontFace
    {
      CairoFontFaceUniquePtr _M_font_face;
    public:
      explicit ImplCanvasFontFace(::cairo_font_face_t *font_face) :
        _M_font_face(font_face) {}

      explicit ImplCanvasFontFace(CairoFontFaceUniquePtr &font_face) :
        _M_font_face(font_face.release()) {}

      virtual ~ImplCanvasFontFace();
    protected:
      virtual Native *native();
    };

    void throw_canvas_exception_for_failure_status(::cairo_status_t status);

    class ImplCanvas : public Canvas
    {
      CairoUniquePtr _M_context;
    public:
      explicit ImplCanvas(::cairo_t *context) :
        _M_context(context) {}

      explicit ImplCanvas(CairoUniquePtr &context) :
        _M_context(context.release()) {}

      virtual ~ImplCanvas();

      virtual void save();

      virtual void restore();

      virtual CanvasPattern *pattern();

      virtual void set_pattern(CanvasPattern *pattern);

      virtual void set_color(Color color);
    private:
      template<typename _Iter>
      void set_linear_gradient(const Point<double> &p1, const Point<double> &p2, const _Iter &color_stop_begin, const _Iter &color_stop_end)
      {
        CairoPatternUniquePtr pattern(::cairo_pattern_create_linear(p1.x, p1.y, p2.x, p2.y));
        throw_canvas_exception_for_failure(pattern.get());
        for(_Iter iter = color_stop_begin; iter != color_stop_end; iter++) {
          ::cairo_pattern_add_color_stop_rgba(pattern.get(), iter->offset, iter->color.red(), iter->color.green(), iter->color.blue(), iter->color.alpha());
          throw_canvas_exception_for_failure(pattern.get());
        }
        ::cairo_set_source(_M_context.get(), pattern.get());
        throw_canvas_exception_for_failure(_M_context.get());
      }
    public:
      virtual void set_linear_gradient(const Point<double> &p1, const Point<double> &p2, std::initializer_list<ColorStop> color_stops);

      virtual void set_linear_gradient(const Point<double> &p1, const Point<double> &p2, const std::vector<ColorStop> &color_stops);
    private:
      template<typename _Iter>
      void set_radial_gradient(const Point<double> &p1, double radius1, const Point<double> &p2, double radius2, const _Iter &color_stop_begin, const _Iter &color_stop_end)
      {
        CairoPatternUniquePtr pattern(::cairo_pattern_create_radial(p1.x, p1.y, radius1, p2.x, p2.y, radius2));
        throw_canvas_exception_for_failure(pattern.get());
        for(_Iter iter = color_stop_begin; iter != color_stop_end; iter++) {
          ::cairo_pattern_add_color_stop_rgba(pattern.get(), iter->offset, iter->color.red(), iter->color.green(), iter->color.blue(), iter->color.alpha());
          throw_canvas_exception_for_failure(pattern.get());
        }
        ::cairo_set_source(_M_context.get(), pattern.get());
        throw_canvas_exception_for_failure(_M_context.get());
      }
    public:
      virtual void set_radial_gradient(const Point<double> &p1, double radius1, const Point<double> &p2, double radius2, std::initializer_list<ColorStop> color_stops);

      virtual void set_radial_gradient(const Point<double> &p1, double radius1, const Point<double> &p2, double radius2, const std::vector<ColorStop> &color_stops);

      virtual void set_image(CanvasImage *image, const Point<double> &p);
    
      virtual Antialias antialias();

      virtual void set_antialias(Antialias antialias);

      virtual void get_dash(std::vector<double> &dashes, double &offset);

      virtual void set_dash(std::initializer_list<double> dashes, double offset);

      virtual void set_dash(const std::vector<double> &dashes, double offset);

      virtual LineCap line_cap();

      virtual void set_line_cap(LineCap line_cap);

      virtual LineJoin line_join();

      virtual void set_line_join(LineJoin line_join);

      virtual double line_width();

      virtual void set_line_width(double width);

      virtual double miter_limit();

      virtual void set_miter_limit(double limit);

      virtual Operator op();

      virtual void set_op(Operator op);

      virtual void clip();

      virtual void reset_clip();

      virtual void fill();

      virtual void paint();

      virtual void paint(unsigned alpha);
    
      virtual void stroke();

      virtual CanvasPath *path();

      virtual CanvasPath *flat_path();

      virtual void append_path(CanvasPath *path);

      virtual bool has_point();

      virtual void get_point(Point<double> &p);

      virtual void new_path();

      virtual void close_path();

      virtual void arc(const Point<double> &p, double radius, double angle1, double angle2, bool is_negative);

      virtual void curve_to(const Point<double> &p1, const Point<double> &p2, const Point<double> &p3);

      virtual void line_to(const Point<double> &p);

      virtual void move_to(const Point<double> &p);

      virtual void rect(const Rectangle<double> &r);

      virtual void text_path(const char *utf8);

      virtual void text_path(const std::string &utf8);

      virtual void translate(const Point<double> &tp);

      virtual void scale(const Point<double> &sp);

      virtual void rotate(double angle);

      virtual CanvasTransformation *transformation();

      virtual void set_transformation(CanvasTransformation *transformation);

      virtual CanvasFontFace *font_face();

      virtual void set_font_face(const std::string &name, FontSlant slant, FontWeight weight);

      virtual void set_font_face(CanvasFontFace *font_face);

      virtual void set_font_size(double size);

      virtual void translate_font(const Point<double> &tp);

      virtual void scale_font(const Point<double> &sp);

      virtual void rotate_font(double angle);

      virtual CanvasTransformation *font_transformation();

      virtual void set_font_transformation(CanvasTransformation *transformation);

      virtual void show_text(const char *utf8);

      virtual void show_text(const std::string &utf8);

      virtual void get_font_matrics(FontMetrics &font_metrics);

      virtual void get_text_matrics(const char *utf8, TextMetrics &text_metrics);

      virtual void get_text_matrics(const std::string &utf8, TextMetrics &text_metrics);
    private:
      ::cairo_pattern_t *cairo_pattern(CanvasPattern *pattern) const
      { return reinterpret_cast<::cairo_pattern_t *>(native_pattern(pattern)); }

      ::cairo_surface_t *cairo_surface(CanvasImage *image) const
      { return reinterpret_cast<::cairo_surface_t *>(native_image(image)); }

      ::cairo_path_t *cairo_path(CanvasPath *path) const
      { return reinterpret_cast<::cairo_path_t *>(native_path(path)); }

      ::cairo_matrix_t *cairo_matrix(CanvasTransformation *transformation) const
      { return reinterpret_cast<::cairo_matrix_t *>(native_transformation(transformation)); }

      ::cairo_font_face_t *cairo_font_face(CanvasFontFace *font_face) const
      { return reinterpret_cast<::cairo_font_face_t *>(native_font_face(font_face)); }
    };

    inline void throw_canvas_exception_for_failure(::cairo_status_t status)
    {
      if(status != ::CAIRO_STATUS_SUCCESS)
        CanvasException(::cairo_status_to_string(status));
    }

    inline void throw_canvas_exception_for_failure(::cairo_t *context)
    { throw_canvas_exception_for_failure(::cairo_status(context)); }

    inline void throw_canvas_exception_for_failure(::cairo_pattern_t *pattern)
    { throw_canvas_exception_for_failure(cairo_pattern_status(pattern)); }

    inline void throw_canvas_exception_for_failure(::cairo_surface_t *surface)
    { throw_canvas_exception_for_failure(cairo_surface_status(surface)); }

    inline void throw_canvas_exception_for_failure(::cairo_path_t *path)
    { throw_canvas_exception_for_failure(path->status); }

    inline void throw_canvas_exception_for_failure(::cairo_font_face_t *font_face)
    { throw_canvas_exception_for_failure(::cairo_font_face_status(font_face)); }
  }
}

#endif
