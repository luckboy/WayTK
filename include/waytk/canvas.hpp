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

  ///
  /// An enumeration of antialiasing mode.
  ///
  enum class Antialias
  {
    DEFAULT,                    ///< Default antialiasing.
    NONE,                       ///< No antialiasing.
    GRAY,                       ///< Gray antialiasing.
    SUBPIXEL                    ///< Subpixel antialising.
  };

  ///
  /// An enumeration of line cap style.
  ///
  enum class LineCap
  {
    BUTT,                       ///< Flatten line end.
    ROUND,                      ///< Rounded line end.
    SQUARE                      ///< Square line end.
  };

  ///
  /// An enumeration of line join styte.
  ///
  enum class LineJoin
  {
    MITER,                      ///< Sharp corner.
    ROUND,                      ///< Rounded corner.
    BEVEL                       ///< Beveled corner.
  };

  ///
  /// An operator enumeration.
  ///
  enum class Operator
  {
    CLEAR,                      ///< Clears a destination.
    SOURCE,                     ///< Replaces a destination by a source.
    OVER,                       ///< Draws a source on a destination.
    IN,                         ///< Removes a destination and draws a source
                                ///  where was the destination.
    OUT,                        ///< Removes a destination and draws a source
                                ///  where wasn't the destination.
    ATOP,                       ///< Draws a source where is a destination.
    DESTINATION,                ///< Doesn't draw a source.
    DESTINATION_OVER,           ///< Draws a source and redraws a destination on
                                ///  the source.
    DESTINATION_IN,             ///< Removes a destination where won't be the
                                ///  source and redraws the destination where
                                ///  will be the source.
    DESTINATION_OUT,            ///< Redraws a destination where won't be a
                                ///  source.
    DESTINATION_ATOP,           ///< Redraws a destination where is a source.
    XOR,                        ///< Uses exclusive disjuction operation.
    ADD,                        ///< Uses addition operation.
    SATURATE                    ///< Draws a source on a destination but
                                ///  assuming the source and the destination are
                                ///  disjoint geometries.
  };

  ///
  /// An enumeration of font slant.
  ///
  enum class FontSlant
  {
    NORMAL,                     ///< Upright font.
    ITALIC,                     ///< Italic font.
    OBLIQUE                     ///< Oblique font.
  };

  ///
  /// An enumeration of font weight.
  ///
  enum class FontWeight
  {
    NORMAL,                     ///< Normal font weight.
    BOLD                        ///< Bold font weight.
  };

  ///
  /// A color class.
  ///
  class Color
  {
    std::uint32_t _M_value;
  public:
    /// Default constructor.
    Color() {}

    /// Creates a new color from 32-bit integer number that is ARGB color value.
    explicit Color(std::uint32_t value) :
      _M_value(value) {}

    /// Creates a new color from a red component, a green component, and a blue
    /// component.
    Color(unsigned red, unsigned green, unsigned blue) :
      _M_value((0xff << 24) | ((red & 0xff) << 16) | ((green & 0xff) << 8) | (blue & 0xff)) {}

    /// Creates a new color from a red component, a green component, a blue
    /// component, and a alpha component.
    Color(unsigned red, unsigned green, unsigned blue, unsigned alpha) :
      _M_value(((alpha & 0xff) << 24) | ((red & 0xff) << 16) | ((green & 0xff) << 8) | (blue & 0xff)) {}

    /// Returns \c true if the color is equal to \p color, otherwise \c false.
    bool operator==(Color color) const
    { return _M_value == color._M_value; }

    /// Returns \c true if the color isn't equal to \p color, otherwise
    /// \c false.
    bool operator!=(Color color) const
    { return _M_value != color._M_value; }

    /// Returns the red component of the color.
    unsigned red() const
    { return (_M_value >> 16) & 0xff; }

    /// Returns the green component of the color.
    unsigned green() const
    { return (_M_value >> 8) & 0xff; }

    /// Returns the blue component of the color.
    unsigned blue() const
    { return _M_value & 0xff; }

    /// Returns the alpha component of the color.
    unsigned alpha() const
    { return (_M_value >> 24) & 0xff; }

    /// Retruns 32-bit integer number of the color that is ARGB color value.
    std::uint32_t value() const
    { return _M_value; }
  };

  ///
  /// A structure of color stop.
  ///
  struct ColorStop
  {
    double offset;              ///< The offset of the color stop.
    Color color;                ///< The color of the color stop.

    /// Default constructor.
    ColorStop() {}

    /// Constructor.
    ColorStop(double offset, unsigned red, unsigned green, unsigned blue) :
      offset(offset), color(Color(red, green, blue)) {}

    /// Constructor.
    ColorStop(double offset, unsigned red, unsigned green, unsigned blue, unsigned alpha) :
      offset(offset), color(Color(red, green, blue, alpha)) {}

    /// Constructor.
    ColorStop(double offset, Color color) :
      offset(offset), color(color) {}
  };

  ///
  /// A structure of font metrics.
  ///
  struct FontMetrics
  {
    double ascent;              ///< The font ascent.
    double descent;             ///< The font descent. 
    double height;              ///< The font height.
    double max_x_advance;       ///< The maximal X advance.
    double max_y_advance;       ///< The maximal Y advance.

    /// Default constructor.
    FontMetrics() {}

    /// Constructor.
    FontMetrics(double ascent, double descent, double height, double max_x_advance, double max_y_advance) :
      ascent(ascent), descent(descent), height(height), max_x_advance(max_x_advance), max_y_advance(max_y_advance) {}
  };

  ///
  /// A structure of text metrics.
  ///
  struct TextMetrics
  {
    double x_bearing;           ///< The X bearing of the text.
    double y_bearing;           ///< The Y bearing of the text.
    double width;               ///< The text width.
    double height;              ///< The text height.
    double x_advance;           ///< The X advance of the text.
    double y_advance;           ///< The Y advance of the text.

    /// Default constructor.
    TextMetrics() {}

    /// Constructor.
    TextMetrics(double x_bearing, double y_bearing, double width, double height, double x_advance, double y_advance) :
      x_bearing(x_bearing), y_bearing(y_bearing), width(width), height(height), x_advance(x_advance), y_advance(y_advance) {}
  };

  ///
  /// A class of canvas pattern.
  ///
  class CanvasPattern
  {
  protected:
    /// A class of native pattern that interally used by a canvas.
    class Native { char _M_pad; Native() {} };

    /// Default constructor.
    CanvasPattern() {}
  public:
    /// Destructor.
    virtual ~CanvasPattern();
  protected:
    /// Returns the native pattern.
    virtual Native *native() = 0;

    friend class Canvas;
  };

  ///
  /// A class of canvas image.
  ///
  class CanvasImage
  {
  protected:
    /// A class of native image that interally used by a canvas.
    class Native { char _M_pad; Native() {} };

    /// Default constructor.
    CanvasImage() {}
  public:
    /// Destructor.
    virtual ~CanvasImage();

    /// Returns the size of the canvas image.
    virtual Dimension<int> size() = 0;

    /// Returns \c true if the canvas image is modifiable, otherwise \c false.
    virtual bool is_modifiable() const;

    /// Creates a new canvas.
    virtual Canvas *canvas();

    /// Returns \c true if the canvas image is scalable, otherwise \c false.
    virtual bool is_scalable() const;

    /// Creates a new image that is a scaled copy of the canvas image.
    CanvasImage *scale(double x)
    { return scale(Point<double>(x, x)); }

    /// \copydoc scale(double x)
    CanvasImage *scale(double x, double y)
    { return scale(Point<double>(x, y)); }

    /// \copydoc scale(double x)
    virtual CanvasImage *scale(const Point<double> &sp);

    /// Creates a new image that is modifiable copy of the canvas image. 
    virtual CanvasModifiableImage *modifiable_image();
  protected:
    /// Returns the native image.
    virtual Native *native() = 0;

    friend class Canvas;
  };

  ///
  /// A canvas image that is modifiable.
  ///
  class CanvasModifiableImage : public virtual CanvasImage
  {
  protected:
    /// Defualt constructor.
    CanvasModifiableImage() {}
  public:
    /// Destructor.
    virtual ~CanvasModifiableImage();

    virtual bool is_modifiable() const;
  };

  ///
  /// A canvas image that is scalable.
  ///
  class CanvasScalableImage : public virtual CanvasImage
  {
  protected:
    /// Default constructor.
    CanvasScalableImage() {}
  public:
    /// Destructor.
    virtual ~CanvasScalableImage();

    virtual bool is_scalable() const;
  };

  ///
  /// A class of canvas path.
  ///
  class CanvasPath
  {
  protected:
    /// A class of native path that interally used by a canvas.
    class Native { char _M_pad; Native() {} };

    /// Dwefault constructor.
    CanvasPath() {}
  public:
    /// Destructor.
    virtual ~CanvasPath();
  protected:
    /// Returns the native path.
    virtual Native *native() = 0;

    friend class Canvas;
  };

  ///
  /// A class of canvas transformation.
  ///
  class CanvasTransformation
  {
  protected:
    /// A class of native transformation that interally used by a canvas.
    class Native { char _M_pad; Native() {} };

    /// Default constructor.
    CanvasTransformation() {}
  public:
    /// Destructor.
    virtual ~CanvasTransformation();
  protected:
    /// Returns the native transformation.
    virtual Native *native() = 0;

    friend class Canvas;
  };

  ///
  /// A class of canvas font face.
  ///
  class CanvasFontFace
  {
  protected:
    /// A class of native font face that interally used by a canvas.
    class Native { char _M_pad; Native() {} };

    /// Default constructor.
    CanvasFontFace() {}
  public:
    /// Destructor.
    virtual ~CanvasFontFace();
  protected:
    /// Returns the native font face.
    virtual Native *native() = 0;

    friend class Canvas;
  };

  ///
  /// A canvas class that allows to draw on a surface or an image.
  ///
  class Canvas
  {
  protected:
    /// Defualt constructor.
    Canvas() {}
  public:
    /// Destructor.
    virtual ~Canvas();

    /// Saves the current drawing state to an internal stack.
    virtual void save() = 0;

    /// Restores the saved drawing state from an internal stack.
    virtual void restore() = 0;

    /// Returns the pattern that is used for drawing.
    virtual CanvasPattern *pattern() = 0;

    /// Sets the pattern that is used for drawing.
    virtual void set_pattern(CanvasPattern *pattern) = 0;

    /// Sets the drawing color that is used for drawing.
    void set_color(unsigned red, unsigned green, unsigned blue)
    { set_color(Color(red, green, blue)); }

    /// \copydoc set_color(unsigned red, unsigned green, unsigned blue)
    void set_color(unsigned red, unsigned green, unsigned blue, unsigned alpha)
    { set_color(Color(red, green, blue, alpha)); }

    /// \copydoc set_color(unsigned red, unsigned green, unsigned blue)
    virtual void set_color(Color color) = 0;

    /// Sets the linear gradient that is used for drawing.
    void set_linear_gradient(double x1, double y1, double x2, double y2, std::initializer_list<ColorStop> color_stops)
    { set_linear_gradient(Point<double>(x1, y1), Point<double>(x2, y2), color_stops); }

    /// \copydoc set_linear_gradient(double x1, double y1, double x2, double y2, std::initializer_list<ColorStop> color_stops)
    virtual void set_linear_gradient(const Point<double> &p1, const Point<double> &p2, std::initializer_list<ColorStop> color_stops) = 0;

    /// \copydoc set_linear_gradient(double x1, double y1, double x2, double y2, std::initializer_list<ColorStop> color_stops)
    void set_linear_gradient(double x1, double y1, double x2, double y2, const std::vector<ColorStop> &color_stops)
    { set_linear_gradient(Point<double>(x1, y1), Point<double>(x2, y2), color_stops); }

    /// \copydoc set_linear_gradient(double x1, double y1, double x2, double y2, std::initializer_list<ColorStop> color_stops)
    virtual void set_linear_gradient(const Point<double> &p1, const Point<double> &p2, const std::vector<ColorStop> &color_stops) = 0;

    /// Sets the radial gradient that is used for drawing.
    void set_radial_gradient(double x1, double y1, double radius1, double x2, double y2, double radius2, std::initializer_list<ColorStop> color_stops)
    { set_radial_gradient(Point<double>(x1, y1), radius1, Point<double>(x2, y2), radius2, color_stops); }

    /// \copydoc set_radial_gradient(double x1, double y1, double radius1, double x2, double y2, double radius2, std::initializer_list<ColorStop> color_stops)
    virtual void set_radial_gradient(const Point<double> &p1, double radius1, const Point<double> &p2, double radius2, std::initializer_list<ColorStop> color_stops) = 0;

    /// \copydoc set_radial_gradient(double x1, double y1, double radius1, double x2, double y2, double radius2, std::initializer_list<ColorStop> color_stops)
    void set_radial_gradient(double x1, double y1, double radius1, double x2, double y2, double radius2, const std::vector<ColorStop> &color_stops)
    { set_radial_gradient(Point<double>(x1, y1), radius1, Point<double>(x2, y2), radius2, color_stops); }

    /// \copydoc set_radial_gradient(double x1, double y1, double radius1, double x2, double y2, double radius2, std::initializer_list<ColorStop> color_stops)
    virtual void set_radial_gradient(const Point<double> &p1, double radius1, const Point<double> &p2, double radius2, const std::vector<ColorStop> &color_stops) = 0;

    /// Sets the image that is used for drawing.
    void set_image(CanvasImage *image, double x, double y)
    { set_image(image, x, y); }

    /// \copydoc set_image(CanvasImage *image, double x, double y)
    virtual void set_image(CanvasImage *image, const Point<double> &p) = 0;

    /// Returns the antialiasing mode.
    virtual Antialias antialias() = 0;

    /// Sets the antialiasing mode.
    virtual void set_antialias(Antialias antialias) = 0;

    /// Gets the dash array and the dash offset.
    virtual void get_dash(std::vector<double> &dashes, double &offset) = 0;

    ///
    /// Sets the dash array and the dash offset.
    ///
    /// If length of the dash array is \c 0, dashes aren't drawn.
    ///
    virtual void set_dash(std::initializer_list<double> dashes, double offset) = 0;

    /// \copydoc set_dash(std::initializer_list<double> dashes, double offset)
    virtual void set_dash(const std::vector<double> &dashes, double offset) = 0;

    /// Returns the line cap style.
    virtual LineCap line_cap() = 0;

    /// Sets the line cap style.
    virtual void set_line_cap(LineCap line_cap) = 0;

    /// Returns the line join style.
    virtual LineJoin line_join() = 0;

    /// Sets the line join style.
    virtual void set_line_join(LineJoin line_join) = 0;

    /// Returns the line width.
    virtual double line_width() = 0;

    /// Sets the line width.
    virtual void set_line_width(double width) = 0;

    /// Returns the miter limit.
    virtual double miter_limit() = 0;

    /// Sets the miter limit.
    virtual void set_miter_limit(double limit) = 0;

    /// Returns the operator that is used for drawing.
    virtual Operator op() = 0;

    /// Sets the operator that is used for drawing.
    virtual void set_op(Operator op) = 0;

    ///
    /// Sets the clip region as intersection of the current clip region with the
    /// current path.
    ///
    /// The current path is cleared after setting the clip region.
    ///
    virtual void clip() = 0;

    /// Resets the clip redion to an initial clip region.
    virtual void reset_clip() = 0;

    ///
    /// Fills the current path.
    ///
    /// The current path is cleared after filling this path.
    ///
    virtual void fill() = 0;

    /// Fills all clip region. 
    virtual void paint() = 0;

    /// Fills all clip region but with the specified alpha instead of the alpha
    /// of the current color.
    virtual void paint(unsigned alpha) = 0;

    ///
    /// Draws the current path.
    ///
    /// The current paths is cleared after drawing this path.
    ///
    virtual void stroke() = 0;

    /// Returns a copy of the current path.
    virtual CanvasPath *path() = 0;

    /// Returns a flattened copy of the current path.
    virtual CanvasPath *flat_path() = 0;

    /// Appends a path onto the current path.
    virtual void append_path(CanvasPath *path) = 0;

    /// Returns \c true if the canvas has the current point, otherwise \c false.
    virtual bool has_point() = 0;

    /// Returns the current point.
    Point<double> point()
    {
      Point<double> p;
      get_point(p);
      return p;
    }

    /// Gets the current point.
    virtual void get_point(Point<double> &p) = 0;

    /// Clears the current path.
    virtual void new_path() = 0;

    /// Closes the subpath.
    virtual void close_path() = 0;

    /// Adds an arc to the current path.
    void arc(double x, double y, double radius, double angle1, double angle2, bool is_negative = false)
    { arc(Point<double>(x, y), radius, angle1, angle2, is_negative); }

    /// \copydoc arc(double x, double y, double radius, double angle1, double angle2, bool is_negative)
    virtual void arc(const Point<double> &p, double radius, double angle1, double angle2, bool is_negative = false) = 0;

    /// Adds a curve to the current path. 
    void curve_to(double x1, double y1, double x2, double y2, double x3, double y3)
    { curve_to(Point<double>(x1, y1), Point<double>(x2, y2), Point<double>(x3, y3)); }

    /// \copydoc curve_to(double x1, double y1, double x2, double y2, double x3, double y3)
    virtual void curve_to(const Point<double> &p1, const Point<double> &p2, const Point<double> &p3) = 0;

    /// Adds a line segment to the current path.
    void line_to(double x, double y)
    { line_to(Point<double>(x, y)); }

    /// \copydoc line_to(double x, double y)
    virtual void line_to(const Point<double> &p) = 0;

    /// Creates a subpath and changes the current point.
    void move_to(double x, double y)
    { move_to(Point<double>(x, y)); }

    /// \copydoc move_to(double x, double y)
    virtual void move_to(const Point<double> &p) = 0;

    /// Adds a rectangle to the current path.
    void rect(double x, double y, double width, double height)
    { rect(Rectangle<double>(x, y, width, height)); }

    /// \copydoc rect(double x, double y, double width, double height)
    void rect(const Point<double> &p, const Dimension<double> &d)
    { rect(Rectangle<double>(p, d)); }

    /// \copydoc rect(double x, double y, double width, double height)
    virtual void rect(const Rectangle<double> &r) = 0;

    /// Adds a text path to the current path.
    virtual void text_path(const char *utf8) = 0;

    /// \copydoc text_path(const char *utf8)
    virtual void text_path(const std::string &utf8) = 0;

    /// Traslates drawing.
    void translate(double tx, double ty)
    { translate(Point<double>(tx, ty)); }

    /// \copydoc translate(double tx, double ty)
    virtual void translate(const Point<double> &tp) = 0;

    /// Scales drawing.
    void scale(double sx, double sy)
    { scale(Point<double>(sx, sy)); }

    /// \copydoc scale(double sx, double sy)
    virtual void scale(const Point<double> &sp) = 0;

    /// Rotates drawing.
    virtual void rotate(double angle) = 0;

    /// Returns the current transformation that used for drawing.
    virtual CanvasTransformation *transformation() = 0;

    /// Sets the current transformation that used for drawing.
    virtual void set_transformation(CanvasTransformation *transformation) = 0;

    /// Returns the current font face.
    virtual CanvasFontFace *font_face() = 0;

    /// Sets the current font face.
    void set_font_face(const std::string &name)
    { set_font_face(name, FontSlant::NORMAL, FontWeight::NORMAL); }

    /// \copydoc set_font_face(const std::string &name)
    virtual void set_font_face(const std::string &name, FontSlant slant, FontWeight weight) = 0;

    /// \copydoc set_font_face(const std::string &name)
    virtual void set_font_face(CanvasFontFace *font_face);

    /// Sets the current font size.
    virtual void set_font_size(double size) = 0;

    /// Translates the current font.
    void translate_font(double tx, double ty)
    { translate_font(Point<double>(tx, ty)); }

    /// \copydoc translate_font(double tx, double ty)
    virtual void translate_font(const Point<double> &tp) = 0;

    /// Scales the current font.
    void scale_font(double sx, double sy)
    { scale_font(Point<double>(sx, sy)); }

    /// \copydoc scale_font(double sx, double sy)
    virtual void scale_font(const Point<double> &sp) = 0;

    /// Rotates the current font.
    virtual void rotate_font(double angle) = 0;

    /// Returns the current font transformation.
    virtual CanvasTransformation *font_transformation() = 0;

    /// Sets the current font transformation.
    virtual void set_font_transformation(CanvasTransformation *transformation) = 0;

    /// Draws a text.
    virtual void show_text(const char *utf8) = 0;

    /// \copydoc show_text(const char *utf8)
    virtual void show_text(const std::string &utf8) = 0;

    /// Returns font metrics for the current font.
    FontMetrics font_metrics()
    {
      FontMetrics font_metrics;
      get_font_matrics(font_metrics);
      return font_metrics;
    }

    /// Gets font metrics for the current font.
    virtual void get_font_matrics(FontMetrics &font_metrics) = 0;

    /// Returns text metrics for a text and the current font.
    TextMetrics text_metrics(const char *utf8)
    {
      TextMetrics text_metrics;
      get_text_matrics(utf8, text_metrics);
      return text_metrics;
    }

    /// \copydoc text_metrics(const char *utf8)
    TextMetrics text_metrics(const std::string &utf8)
    {
      TextMetrics text_metrics;
      get_text_matrics(utf8, text_metrics);
      return text_metrics;
    }

    /// Gets text metrics for a text and the current font.
    virtual void get_text_matrics(const char *utf8, TextMetrics &text_metrics) = 0;

    /// \copydoc get_text_matrics(const char *utf8, TextMetrics &text_metrics)
    virtual void get_text_matrics(const std::string &utf8, TextMetrics &text_metrics) = 0;
  protected:
    /// Returns a native pattern.
    CanvasPattern::Native *native_pattern(CanvasPattern *pattern) const
    { return pattern->native(); }

    /// Returns a native image.
    CanvasImage::Native *native_image(CanvasImage *image) const
    { return image->native(); }

    /// Returns a native path.
    CanvasPath::Native *native_path(CanvasPath *path) const
    { return path->native(); }

    /// Returns a native transformation.
    CanvasTransformation::Native *native_transformation(CanvasTransformation *transformation) const
    { return transformation->native(); }

    /// Returns a native font face.
    CanvasFontFace::Native *native_font_face(CanvasFontFace *font_face) const
    { return font_face->native(); }
  };

  /// Creates a new canvas image that is modifiable.
  CanvasModifiableImage *new_canvas_modifiable_image(const Dimension<int> &size);

  /// \copydoc new_canvas_modifiable_image(const Dimension<int> &size)
  inline CanvasModifiableImage *new_canvas_modifiable_image(int width, int height)
  { return new_canvas_modifiable_image(Dimension<int>(width, height)); }

  /// \copydoc new_canvas_modifiable_image(const Dimension<int> &size)
  CanvasModifiableImage *new_canvas_modifiable_image(const Dimension<int> &size, int stride, void *data);

  /// \copydoc new_canvas_modifiable_image(const Dimension<int> &size)
  inline CanvasModifiableImage *new_canvas_modifiable_image(int width, int height, int stride, void *data)
  { return new_canvas_modifiable_image(Dimension<int>(width, height), stride, data); }

  /// Loads a canvas image from a file.
  CanvasImage *load_canvas_image(const std::string &file_name);
}

#endif
