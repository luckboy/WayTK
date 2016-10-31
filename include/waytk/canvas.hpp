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
#ifndef _WAYTK_CANVAS_HPP
#define _WAYTK_CANVAS_HPP

#include <cstdint>
#include <initializer_list>
#include <string>
#include <vector>
#include <waytk/structs.hpp>

namespace waytk
{
  class Canvas;
  class CanvasModifiableImage;

  enum class Antialias
  {
    DEFAULT,
    NONE,
    GRAY,
    SUBPIXEL
  };
  
  enum class LineCap
  {
    BUTT,
    ROUND,
    SQUARE
  };

  enum class LineJoin
  {
    MITER,
    ROUND,
    BEVEL
  };

  enum class Operator
  {
    CLEAR,
    SOURCE,
    OVER,
    IN,
    OUT,
    ATOP,
    DESTINATION,
    DESTINATION_OVER,
    DESTINATION_IN,
    DESTINATION_OUT,
    DESTINATION_ATOP,
    XOR,
    ADD,
    SATURATE
  };

  enum class FontSlant
  {
    NORMAL,
    ITALIC,
    OBLIQUE
  };

  enum class FontWeight
  {
    NORMAL,
    BOLD
  };

  class Color
  {
    std::uint32_t _M_value;
  public:
    Color() {}

    explicit Color(std::uint32_t value) :
      _M_value(value) {}

    Color(unsigned red, unsigned green, unsigned blue) :
      _M_value((0xff << 24) | ((red & 0xff) << 16) | ((green & 0xff) << 8) | (blue & 0xff)) {}

    Color(unsigned red, unsigned green, unsigned blue, unsigned alpha) :
      _M_value(((alpha & 0xff) << 24) | ((red & 0xff) << 16) | ((green & 0xff) << 8) | (blue & 0xff)) {}

    bool operator==(Color color) const
    { return _M_value == color._M_value; }

    bool operator!=(Color color) const
    { return _M_value != color._M_value; }

    unsigned red() const
    { return (_M_value >> 16) & 0xff; }

    unsigned green() const
    { return (_M_value >> 8) & 0xff; }

    unsigned blue() const
    { return _M_value & 0xff; }

    unsigned alpha() const
    { return (_M_value >> 24) & 0xff; }

    std::uint32_t value() const
    { return _M_value; }
  };

  struct ColorStop
  {
    double offset;
    Color color;

    ColorStop() {}

    ColorStop(double offset, unsigned red, unsigned green, unsigned blue) :
      offset(offset), color(Color(red, green, blue)) {}

    ColorStop(double offset, unsigned red, unsigned green, unsigned blue, unsigned alpha) :
      offset(offset), color(Color(red, green, blue, alpha)) {}

    ColorStop(double offset, Color color) :
      offset(offset), color(color) {}
  };

  struct FontMetrics
  {
    double ascent, descent;
    double height;
    double max_x_advance, max_y_advance;

    FontMetrics() {}

    FontMetrics(double ascent, double descent, double height, double max_x_advance, double max_y_advance) :
      ascent(ascent), descent(descent), height(height), max_x_advance(max_x_advance), max_y_advance(max_y_advance) {}
  };

  struct TextMetrics
  {
    double x_bearing, y_bearing;
    double width, heihgt;
    double x_advance, y_advance;

    TextMetrics() {}

    TextMetrics(double x_bearing, double y_bearing, double width, double height, double x_advance, double y_advance) :
      x_bearing(x_bearing), y_bearing(y_bearing), width(width), heihgt(height), x_advance(x_advance), y_advance(y_advance) {}
  };

  class CanvasPattern
  {
  protected:
    class Native { char _M_pad; Native() {} };

    CanvasPattern() {}
  public:
    virtual ~CanvasPattern();
  protected:
    virtual Native *native() = 0;

    friend class Canvas;
  };

  class CanvasImage
  {
  protected:
    class Native { char _M_pad; Native() {} };

    CanvasImage() {}
  public:
    virtual ~CanvasImage();

    virtual Dimension<int> size() = 0;

    virtual bool is_modifiable() const;

    virtual Canvas *canvas();

    virtual bool is_scalable() const;
    
    CanvasImage *scale(double x)
    { return scale(Point<double>(x, x)); }

    CanvasImage *scale(double x, double y)
    { return scale(Point<double>(x, y)); }

    virtual CanvasImage *scale(const Point<double> &sp);

    virtual CanvasModifiableImage *modifiable_image();
  protected:
    virtual Native *native() = 0;

    friend class Canvas;
  };

  class CanvasModifiableImage : public virtual CanvasImage
  {
  protected:
    CanvasModifiableImage() {}
  public:
    virtual ~CanvasModifiableImage();

    virtual bool is_modifiable() const;
  };

  class CanvasScalableImage : public virtual CanvasImage
  {
  protected:
    CanvasScalableImage() {}
  public:
    virtual ~CanvasScalableImage();

    virtual bool is_scalable() const;
  };

  class CanvasPath
  {
  protected:
    class Native { char _M_pad; Native() {} };

    CanvasPath() {}
  public:
    virtual ~CanvasPath();
  protected:
    virtual Native *native() = 0;

    friend class Canvas;
  };

  class CanvasTransformation
  {
  protected:
    class Native { char _M_pad; Native() {} };

    CanvasTransformation() {}
  public:
    virtual ~CanvasTransformation();
  protected:
    virtual Native *native() = 0;

    friend class Canvas;
  };

  class CanvasFontFace
  {
  protected:
    class Native { char _M_pad; Native() {} };

    CanvasFontFace() {}
  public:
    virtual ~CanvasFontFace();
  protected:
    virtual Native *native() = 0;

    friend class Canvas;
  };

  class Canvas
  {
  protected:
    class Native { char _M_pad; Native() {} };

    Canvas() {}
  public:
    virtual ~Canvas();

    virtual void save() = 0;

    virtual void restore() = 0;

    virtual CanvasPattern *pattern() = 0;

    virtual void set_pattern(CanvasPattern *pattern) = 0;

    void set_color(unsigned red, unsigned green, unsigned blue)
    { set_color(Color(red, green, blue)); }

    void set_color(unsigned red, unsigned green, unsigned blue, unsigned alpha)
    { set_color(Color(red, green, blue, alpha)); }

    virtual void set_color(Color color) = 0;

    void set_linear_gradient(double x1, double y1, double x2, double y2, std::initializer_list<ColorStop> color_stops)
    { set_linear_gradient(Point<double>(x1, y1), Point<double>(x2, y2), color_stops); }

    virtual void set_linear_gradient(const Point<double> &p1, const Point<double> &p2, std::initializer_list<ColorStop> color_stops) = 0;

    void set_linear_gradient(double x1, double y1, double x2, double y2, const std::vector<ColorStop> &color_stops)
    { set_linear_gradient(Point<double>(x1, y1), Point<double>(x2, y2), color_stops); }

    virtual void set_linear_gradient(const Point<double> &p1, const Point<double> &p2, const std::vector<ColorStop> &color_stops) = 0;

    void set_radial_gradient(double x1, double y1, double radius1, double x2, double y2, double radius2, std::initializer_list<ColorStop> color_stops)
    { set_radial_gradient(Point<double>(x1, y1), radius1, Point<double>(x2, y2), radius2, color_stops); }

    virtual void set_radial_gradient(const Point<double> &p1, double radius1, const Point<double> &p2, double radius2, std::initializer_list<ColorStop> color_stops) = 0;

    void set_radial_gradient(double x1, double y1, double radius1, double x2, double y2, double radius2, const std::vector<ColorStop> &color_stops)
    { set_radial_gradient(Point<double>(x1, y1), radius1, Point<double>(x2, y2), radius2, color_stops); }

    virtual void set_radial_gradient(const Point<double> &p1, double radius1, const Point<double> &p2, double radius2, const std::vector<ColorStop> &color_stops) = 0;

    void set_image(CanvasImage *image, double x, double y)
    { set_image(image, x, y); }

    virtual void set_image(CanvasImage *image, const Point<double> &p) = 0;
    
    virtual Antialias antialias() = 0;

    virtual void set_antialias(Antialias antialias) = 0;
    
    virtual void get_dash(std::vector<double> &dashes, double &offset) = 0;

    virtual void set_dash(std::initializer_list<double> dashes, double offset) = 0;

    virtual void set_dash(const std::vector<double> &dashes, double offset) = 0;

    virtual LineCap line_cap() = 0;

    virtual void set_line_cap(LineCap line_cap) = 0;

    virtual LineJoin line_join() = 0;

    virtual void set_line_join(LineJoin line_join) = 0;

    virtual double line_width() = 0;

    virtual void set_line_width(double width) = 0;

    virtual double miter_limit() = 0;

    virtual void set_miter_limit(double limit) = 0;
    
    virtual Operator op() = 0;

    virtual void set_op(Operator op) = 0;

    virtual void clip() = 0;

    virtual void reset_clip() = 0;

    virtual void fill() = 0;

    virtual void paint() = 0;

    virtual void paint(unsigned alpha) = 0;
    
    virtual void stroke() = 0;

    virtual CanvasPath *path() = 0;

    virtual CanvasPath *flat_path() = 0;

    virtual void append_path(CanvasPath *path) = 0;

    virtual bool has_point() = 0;

    Point<double> point()
    {
      Point<double> p;
      get_point(p);
      return p;
    }

    virtual void get_point(Point<double> &p) = 0;

    virtual void new_path() = 0;

    virtual void close_path() = 0;

    void arc(double x, double y, double radius, double angle1, double angle2, bool is_negative = false)
    { arc(Point<double>(x, y), radius, angle1, angle2, is_negative); }

    virtual void arc(const Point<double> &p, double radius, double angle1, double angle2, bool is_negative = false) = 0;

    void curve_to(double x1, double y1, double x2, double y2, double x3, double y3)
    { curve_to(Point<double>(x1, y1), Point<double>(x2, y2), Point<double>(x3, y3)); }

    virtual void curve_to(const Point<double> &p1, const Point<double> &p2, const Point<double> &p3) = 0;

    void line_to(double x, double y)
    { line_to(Point<double>(x, y)); }

    virtual void line_to(const Point<double> &p) = 0;

    void move_to(double x, double y)
    { move_to(Point<double>(x, y)); }

    virtual void move_to(const Point<double> &p) = 0;

    void rect(double x, double y, double width, double height)
    { rect(Rectangle<double>(x, y, width, height)); }

    void rect(const Point<double> &p, const Dimension<double> &d)
    { rect(Rectangle<double>(p.x, p.y, d.width, d.height)); }

    virtual void rect(const Rectangle<double> &r) = 0;

    virtual void text_path(const char *utf8) = 0;

    virtual void text_path(const std::string &utf8) = 0;

    void translate(double tx, double ty)
    { translate(Point<double>(tx, ty)); }

    virtual void translate(const Point<double> &tp) = 0;

    void scale(double sx, double sy)
    { scale(Point<double>(sx, sy)); }

    virtual void scale(const Point<double> &sp) = 0;

    virtual void rotate(double angle) = 0;

    virtual CanvasTransformation *transformation() = 0;

    virtual void set_transformation(CanvasTransformation *transformation) = 0;

    virtual CanvasFontFace *font_face() = 0;

    void set_font_face(const std::string &name)
    { set_font_face(name, FontSlant::NORMAL, FontWeight::NORMAL); }

    virtual void set_font_face(const std::string &name, FontSlant slant, FontWeight weight) = 0;

    virtual void set_font_face(CanvasFontFace *font_face);

    virtual void set_font_size(double size) = 0;

    void translate_font(double tx, double ty)
    { translate_font(Point<double>(tx, ty)); }

    virtual void translate_font(const Point<double> &tp) = 0;

    void scale_font(double sx, double sy)
    { scale_font(Point<double>(sx, sy)); }

    virtual void scale_font(const Point<double> &sp) = 0;

    virtual void rotate_font(double angle) = 0;

    virtual CanvasTransformation *font_transformation() = 0;

    virtual void set_font_transformation(CanvasTransformation *transformation) = 0;

    virtual void show_text(const char *utf8) = 0;

    virtual void show_text(const std::string &utf8) = 0;

    FontMetrics font_metrics()
    {
      FontMetrics font_metrics;
      get_font_matrics(font_metrics);
      return font_metrics;
    }

    virtual void get_font_matrics(FontMetrics &font_metrics) = 0;

    TextMetrics text_metrics(const char *utf8)
    {
      TextMetrics text_metrics;
      get_text_matrics(utf8, text_metrics);
      return text_metrics;
    }

    TextMetrics text_metrics(const std::string &utf8)
    {
      TextMetrics text_metrics;
      get_text_matrics(utf8, text_metrics);
      return text_metrics;
    }

    virtual void get_text_matrics(const char *utf8, TextMetrics &text_metrics) = 0;

    virtual void get_text_matrics(const std::string &utf8, TextMetrics &text_metrics) = 0;
  protected:
    CanvasPattern::Native *native_pattern(CanvasPattern *pattern) const
    { return pattern->native(); }

    CanvasImage::Native *native_image(CanvasImage *image) const
    { return image->native(); }

    CanvasPath::Native *native_path(CanvasPath *path) const
    { return path->native(); }

    CanvasTransformation::Native *native_transformation(CanvasTransformation *transformation) const
    { return transformation->native(); }

    CanvasFontFace::Native *native_font_face(CanvasFontFace *font_face) const
    { return font_face->native(); }
  };

  CanvasModifiableImage *new_canvas_modifiable_image(const Dimension<int> &size);

  inline CanvasModifiableImage *new_canvas_modifiable_image(int width, int height)
  { return new_canvas_modifiable_image(Dimension<int>(width, height)); }

  CanvasModifiableImage *new_canvas_modifiable_image(const Dimension<int> &size, int stride, void *data);

  inline CanvasModifiableImage *new_canvas_modifiable_image(int width, int height, int stride, void *data)
  { return new_canvas_modifiable_image(Dimension<int>(width, height), stride, data); }

  CanvasImage *load_canvas_image(const std::string &file_name);
}

#endif
