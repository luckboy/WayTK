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
#ifndef _WAYTK_WIDGETS_HPP
#define _WAYTK_WIDGETS_HPP

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <waytk/adapters.hpp>
#include <waytk/callback.hpp>
#include <waytk/canvas.hpp>
#include <waytk/modifiers.hpp>
#include <waytk/structs.hpp>
#include <waytk/styles.hpp>
#include <waytk/text_buffer.hpp>
#include <waytk/util.hpp>

namespace waytk
{
  class ComboBox;
  class MenuItem;
  class RadioButton;
  class Surface;
  class Widget;

  ///
  /// An enumeration of horizontal alignment.
  ///
  enum class HAlignment
  {
    LEFT,                       ///< Aligns to left.
    CENTER,                     ///< Aligns to horizontal center.
    RIGHT,                      ///< Aligns to right.
    FILL                        ///< Aligns to left and right.
  };

  ///
  /// An enumeration of vertical alignment.
  ///
  enum class VAlignment
  {
    TOP,                        ///< Aligns to top.
    CENTER,                     ///< Aligns to vertical center.
    BOTTOM,                     ///< Aligns to bottom.
    FILL                        ///< Aligns to top and bottom.
  };

  ///
  /// An enumeration of touch state.
  ///
  enum class TouchState
  {
    DOWN,                       ///< A touch start or a pointer button is
                                ///  pressed.
    MOTION,                     ///< A touch motion or a pointer motion with
                                ///  pressed button.
    UP                          ///< A touch end or a pointer button is
                                ///  released.
  };

  ///
  /// An enumeration of axis.
  ///
  enum class Axis
  {
    HORIZONTAL_SCROLL,          ///< Horizontal scroll wheel.
    VERTICAL_SCROLL             ///< Vertical scroll wheel.
  };

  ///
  /// An enumeration of key state.
  ///
  enum class KeyState
  {
    PRESSED,                    ///< Pressed key.
    REPEATED,                   ///< Pepeated key.
    RELEASED                    ///< Released key.
  };

  ///
  /// An enumeration of input type.
  ///
  enum class InputType
  {
    SINGLE_LINE,                ///< Single-line text.
    MULTI_LINE,                 ///< Multi-line text.
    PASSWORD                    ///< Password.
  };

  ///
  /// An orientation enumeration.
  ///
  enum class Orientation
  {
    HORIZONTAL,                 ///< Horizontal orientation.
    VERTICAL                    ///< Vertical orientation.
  };

  ///
  /// An enumeration of selection mode.
  ///
  enum class SelectionMode
  {
    SINGLE,                     ///< Just one of items can be selected.
    MULTI                       ///< More than one item can be selected.
  };

  ///
  /// An enumeration of icon size.
  ///
  enum class IconSize
  {
    SMALL,                      ///< Small icon.
    MEDIUM,                     ///< Medium icon.
    LARGE                       ///< Large icon.
  };

  ///
  /// A touch pointer class.
  ///
  class Pointer
  {
    bool _M_is_touch;
    std::uint32_t _M_touch_id;
  public:
    /// Creates a new touch pointer.
    Pointer() : _M_is_touch(false) {}

    /// Creates a new touch pointer with a touch identifier.
    explicit Pointer(std::uint32_t touch_id) : _M_is_touch(true), _M_touch_id(touch_id) {}

    /// Returns \c true if the touch pointer concers a touch, otherwise
    /// \c false.
    bool is_touch() const { return _M_is_touch; }

    /// Returns the touch identifier.
    std::uint32_t touch_id() const { return _M_touch_id; }

    bool operator==(const Pointer &pointer) const
    { return (_M_is_touch && pointer._M_is_touch) ? _M_touch_id == pointer._M_touch_id : _M_is_touch == pointer._M_is_touch; }

    bool operator!=(const Pointer &pointer) const
    { return !(*this == pointer); }
  };

  ///
  /// An icon class.
  ///
  class Icon
  {
    std::string _M_name;
  public:
    /// Default constructor.
    Icon() {}

    /// Creates an icon with a name.
    explicit Icon(const std::string &name) :
      _M_name(name) {}

    /// Returns \c true if the icon is equal to \p icon, otherwise \c false.
    bool operator==(const Icon &icon) const
    { return _M_name == icon._M_name; }

    /// Returns \c true if the icon isn't equal to \p icon, otherwise \c false.
    bool operator!=(const Icon &icon) const
    { return !(*this == icon); }

    /// Returns the icon name.
    const std::string &name() const
    { return _M_name; }

    /// Returns the icon image.
    std::shared_ptr<CanvasImage> image(IconSize size) const;
  };

  ///
  /// A structure to comparing two table positions.
  ///
  struct TablePositionCompare
  {
    bool operator()(const TablePosition &pos1, const TablePosition &pos2) const
    { return pos1.row == pos2.row ? pos1.column < pos2.column : pos1.column < pos2.column; }
  };

  ///
  /// A structure to comparing two tree paths.
  ///
  struct TreePathCompare
  { bool operator()(const TreePath &path1, const TreePath &path2) const; };

  ///
  /// A viewport class that used in a scroll widget.
  ///
  /// A viewport object is used by the scroll widget to store information about
  /// a widget bounds at the viewport and a scroll slider. The part of this
  /// information is modified by the scroll widget so that the scroll widget
  /// inform a widget motion at viewport.
  ///
  class Viewport
  {
  protected:
    /// Default constructor.
    Viewport() {}
  public:
    /// Destructor.
    virtual ~Viewport();

    /// Returns the widget viewport at the viewport.
    virtual Rectangle<int> bounds() const = 0;

    /// Returns the X offset of the horizontal scroll slider.
    virtual int h_scroll_slider_x(int width) const = 0;

    /// Returns the step of the X offset of the horizontal scroll slider.
    virtual int h_scroll_slider_x_step(int width) const = 0;
    
    /// Sets the X offset of the horizontal scroll slider.
    virtual void set_h_scroll_slider_x(int x, int width) = 0; 

    /// Returns the width of the horizontal scroll slider.
    virtual int h_scroll_slider_width(int width) const = 0;

    /// Returns the Y offset of the vertical scroll slider.
    virtual int v_scroll_slider_y(int height) const = 0;

    /// Returns the step of the Y offset of the vertical scroll slider.
    virtual int v_scroll_slider_y_step(int height) const = 0;

    /// Sets the Y offset of the vertical scroll slider.
    virtual void set_v_scroll_slider_y(int y, int height) = 0; 

    /// Returns the height of the vertical scroll slider.
    virtual int v_scroll_slider_height(int height) const = 0;
  };

  ///
  /// A listener type for touch events.
  ///
  /// \param widget is the widget.
  /// \param pointer is the pointer that identifies pointer or touch.
  /// \param point is the point.
  /// \param state is the touch state.
  ///
  typedef std::function<void (Widget *widget, const Pointer &pointer, const Point<double> &point, TouchState state)> OnTouchListener;

  
  ///
  /// A listener type for touch leaves.
  ///
  /// \param widget is the widget.
  /// \param pointer is the pointer that identifies pointer or touch.
  ///
  typedef std::function<void (Widget *widget, const Pointer &pointer)> OnTouchLeaveListener;
  
  ///
  /// A listener type for pointer motions.
  ///
  /// \param widget is the widget.
  /// \param point is the point.
  ///
  typedef std::function<void (Widget *widget, const Point<double> &point)> OnPointerMotionListener;

  ///
  /// A listener type for pointer leaves.
  ///
  /// \param widget is the widget.
  ///
  typedef std::function<void (Widget *widget)> OnPointerLeaveListener;

  ///
  /// A listener type for pointer axis events.
  ///
  /// \param widget is the widget.
  /// \param axis is the axis of the pointer wheel.
  /// \param value is the value.
  ///
  typedef std::function<void (Widget *widget, Axis axis, double value)> OnPointerAxisListener;

  ///
  /// A listener type for key events.
  ///
  /// \param widget is the widget.
  /// \param key_sym is the key symbol.
  /// \param modifiers are the modifiers.
  /// \param utf8 is the UTF-8 character sequence.
  /// \param state is the key state.
  ///
  typedef std::function<void (Widget *widget, std::uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state)> OnKeyListener;
  
  ///
  /// A listener type for the widget scrolling.
  ///
  /// \param widget is the widget.
  /// \param viewport is the widget viewport.
  ///
  typedef std::function<void (Widget *widget, Viewport *viewport)> OnScrollListener;

  ///
  /// A listener type for clicks.
  ///
  /// \param widget is the widget.
  ///
  typedef std::function<void (Widget *widget)> OnClickListener;

  ///
  /// A listener type for text changes.
  ///
  /// \param widget is the widget.
  /// \param range is the iterator ranges of the text.
  ///
  typedef std::function<void (Widget *widget, const Range<TextCharIterator> &range)> OnTextChangeListener;

  ///
  /// A listener type for cursor changes.
  ///
  /// \param widget is the widget.
  /// \param iter is the cursor iterator.
  /// \param pos is the cursor position.
  ///
  typedef std::function<void (Widget *widget, const TextCharIterator &iter, const TextPosition &pos)> OnCursorChangeListener;

  ///
  /// A listener type for text selection changes.
  ///
  /// \param widget is the widget.
  /// \param range is the iterator range of the selected text.
  ///
  typedef std::function<void (Widget *widget, const Range<TextCharIterator> &range)> OnTextSelectionListener;

  ///
  /// A listener type for changes of the check box state.
  ///
  /// \param widget is the widget.
  /// \param is_checked is \c true if the check box is checked, otherwise
  ///        \c false.
  ///
  typedef std::function<void (Widget *widget, bool is_checked)> OnCheckListener;

  ///
  /// A listener type for selection changes.
  ///
  /// \param widget is the widget.
  /// \param pos is the position of the selected item.
  ///
  typedef std::function<void (Widget *widget, std::size_t pos)> OnSelectionListener;

  ///
  /// A listener type for selection changes of list widget.
  ///
  /// \param widget is the widget.
  /// \param poses are the positions of the selected items.
  ///
  typedef std::function<void (Widget *widget, const std::set<std::size_t> &poses)> OnListSelectionListener;

  ///
  /// A listener type for selection changes of table widget.
  ///
  /// \param widget is the widget.
  /// \param poses are the positions of the selected cells.
  ///
  typedef std::function<void (Widget *widget, const std::set<TablePosition, TablePositionCompare> &poses)> OnTableSelectionListener;

  ///
  /// A listener type for selection changes of tree widget.
  ///
  /// \param widget is the widget.
  /// \param paths are the paths of the selected nodes.
  ///
  typedef std::function<void (Widget *widget, const std::set<TreePath, TreePathCompare> &paths)> OnTreeSelectionListener;

  /// A callback type for touch events.
  typedef Callback<Widget *, const Pointer &, const Point<double> &, TouchState> OnTouchCallback;

  /// A callback type for touch leaves.
  typedef Callback<Widget *, const Pointer &> OnTouchLeaveCallback;
  
  /// A callback type for pointer motions.
  typedef Callback<Widget *, const Point<double> &> OnPointerMotionCallback;

  /// A callback type for pointer leaves.
  typedef Callback<Widget *> OnPointerLeaveCallback;

  /// A callback type for pointer axis events.
  typedef Callback<Widget *, Axis, double> OnPointerAxisCallback;

  /// A callback type for key events.
  typedef Callback<Widget *, std::uint32_t, Modifiers, const char *, KeyState> OnKeyCallback;

  /// A callback type for the widget scrolling.
  typedef Callback<Widget *, Viewport *> OnScrollCallback;

  /// A callback type for clicks.
  typedef Callback<Widget *> OnClickCallback;

  /// A callback type for text changes.
  typedef Callback<Widget *, const Range<TextCharIterator> &> OnTextChangeCallback;

  /// A callback type for cursor changes.
  typedef Callback<Widget *, const TextCharIterator &, const TextPosition &> OnCursorChangeCallback;

  /// A callback type for text selection changes.
  typedef Callback<Widget *, const Range<TextCharIterator> &> OnTextSelectionCallback;

  /// A callback type for changes of the check box state.
  typedef Callback<Widget *, bool> OnCheckCallback;

  /// A callback type for selection changes.
  typedef Callback<Widget *, std::size_t> OnSelectionCallback;

  /// A callback type for selection changes of list widget.
  typedef Callback<Widget *, const std::set<std::size_t> &> OnListSelectionCallback;

  /// A callback type for selection changes of table widget.
  typedef Callback<Widget *, const std::set<TablePosition, TablePositionCompare> &> OnTableSelectionCallback;

  /// A callback type for selection changes of tree widget.
  typedef Callback<Widget *, const std::set<TreePath, TreePathCompare> &> OnTreeSelectionCallback;

  ///
  /// A widget class that is base for all widgets.
  ///
  /// Widgets are drawn on a surface and react on an user actions. The widget
  /// display and the widget behavior are specified by properties of this class
  /// The widget reactions can be specified by setting the listeners of the
  /// widget. The listeners of the widgets are invoked when an appropriate event
  /// occurs. For example, the widget events are: key press, touch, and press of
  /// pointer button. Each widget mustn't be used as child in more then one other
  /// widget. The widget must be deleted from own parent if the widget will be
  /// added onto other parent.
  ///
  class Widget
  {
  protected:
    /// A class that is used as an unused argument at empty constructors.
    class Unused { char _M_pad; };
  private:
    bool _M_is_enabled;
    bool _M_has_focus;
    PseudoClasses _M_pseudo_classes;
    bool _M_is_visible;
    HAlignment _M_h_align;
    VAlignment _M_v_align;
    int _M_max_width;
    int _M_max_height;
    int _M_min_width;
    int _M_min_height;
    int _M_weight;
    Rectangle<int> _M_bounds;
    std::weak_ptr<Surface> _M_surface;
    Widget *_M_parent;
    const char *_M_style_name;
    Styles *_M_styles;
    Dimension<int> _M_content_size;
    OnTouchCallback _M_on_touch_callback;
    OnTouchLeaveCallback _M_on_touch_leave_callback;
    OnPointerMotionCallback _M_on_pointer_motion_callback;
    OnPointerLeaveCallback _M_on_pointer_leave_callback;
    OnPointerAxisCallback _M_on_pointer_axis_callback;
    OnKeyCallback _M_on_key_callback;
    OnScrollCallback _M_on_scroll_callback;
  public:
    /// Default constructor.
    Widget();

    /// Destructor.
    virtual ~Widget();

    ///
    /// Returns \c true if the widget is enabled, otherwise \c false.
    ///
    /// If the widget isn't enabled, it doesn't react on user actions. By
    /// default, each widget of WayTK is enabled.
    ///
    bool is_enabled() const
    { return _M_is_enabled; }

    /// Enables the widget if \p is_enabled is \c true, otherwise disables the
    /// widget.
    void set_enabled(bool is_enabled);

    ///
    /// Returns \c true if the widget has focus, otherwise \c false.
    ///
    /// If the widget has focus, it also can react on key events; otherwise it
    /// just can react touch events and pointer.
    ///
    bool has_focus() const
    { return _M_has_focus; }

    /// Sets the widget focus if \p has_focus is \c true, otherwise unsets the
    /// widget focus.
    void set_focus(bool has_focus);

    ///
    /// Returns the pseudo classe of the widget.
    ///
    /// The pseudo classes of the widget affects on determining the widget
    /// style.
    ///
    PseudoClasses pseudo_classes() const
    { return _M_pseudo_classes; }

    /// Sets the pseudo classe of the widget.
    void set_pseudo_classes(PseudoClasses pseudo_classes)
    { _M_pseudo_classes = pseudo_classes; }

    /// Returns the pseudo classes of the widget with a dropback pseudo class
    /// if the widget surface is active, the pseudo class of the widget.
    PseudoClasses real_pseudo_classes();
    
    ///
    /// Returns \c true if the widget is visible, otherwise \c false.
    ///
    /// If the widget isn't visible, it isn't displayed. Also, descendants of a
    /// widget aren't displayed in case of their ascendants are invisible. By
    /// default, each widget of WayTK is visible.
    ///
    bool is_visible() const
    { return _M_is_visible; }

    /// Sets the widget as visible if \p is_visible is \c true, otherwise sets
    /// the widget as invisible.
    void set_visibale(bool is_visible)
    { _M_is_visible = is_visible; }

    ///
    /// Returns the horizontal alignment of the widget.
    ///
    /// By default, each widget of WayTK is horizontally aligned to left except
    /// buttons.
    ///
    HAlignment h_align() const
    { return _M_h_align; }

    /// Sets the horizontal alignment of the widget.
    void set_h_align(HAlignment align)
    { _M_h_align = align; }

    ///
    /// Returns the vertical alignment of the widget.
    ///
    /// By default, each widget of WayTK is vertically aligned to center.
    ///
    VAlignment v_align() const
    { return _M_v_align; }

    /// Sets the vertical alignment of the widget.
    void set_v_align(VAlignment align)
    { _M_v_align = align; }

    ///
    /// Returns the maximal width of the widget.
    ///
    /// By default, the maximal width has maximal value of integer.
    ///
    int max_width() const
    { return _M_max_width; }

    /// Sets the maximal width of the widget. 
    void set_max_width(int max_width)
    { _M_max_width = (max_width >= 1 ? max_width : 1); }

    ///
    /// Returns the maximal height of the widget.
    ///
    /// By default, the maximal height has maximal value of an integer number.
    ///
    int max_height() const
    { return _M_max_height; }

    /// Sets the maximal height of the widget.
    void set_max_height(int max_height)
    { _M_max_height = (max_height >= 1 ? max_height : 1); }

    ///
    /// Returns the minimal width of the widget.
    ///
    /// By default, the minimal width is \c 0.
    ///
    int min_width() const
    { return _M_min_width; }

    /// Sets the minimal width of the widget.
    void set_min_width(int min_width)
    { _M_min_width = (min_width >= 0 ? min_width : 0); }

    ///
    /// Returns the minimal height of the widget.
    ///
    /// By default, the minimal height is \c 0.
    ///
    int min_height() const
    { return _M_min_height; }

    /// Sets the minimal height of the widget.
    void set_min_height(int min_height)
    { _M_min_height = (min_height >= 0 ? min_height : 0); }

    ///
    /// Returns the widget weidght.
    ///
    /// By default, the weight is \c 0.
    ///
    int weight() const
    { return _M_weight; }

    /// Sets the widget weidght.
    void set_weight(int weight)
    { _M_weight = (weight >= 0 ? weight : 0); }

    ///
    /// Returns the widget bounds.
    ///
    /// The widget is drawn on the ractangle that is specified by the widget
    /// bounds. The widget just react on touch and pointer action at this
    /// bounds.
    ///
    const Rectangle<int> &bounds() const
    { return _M_bounds; }
  protected:
    /// Sets the widget bounds.
    void set_bounds(int x, int y, int width, int height)
    { set_bounds(Rectangle<int>(x, y, width, height)); }

    /// \copydoc set_bounds(int x, int y, int width, int height)
    void set_bounds(const Point<int> point, const Dimension<int> size)
    { set_bounds(Rectangle<int>(point.x, point.y, size.width, size.height)); }

    /// \copydoc set_bounds(int x, int y, int width, int height)
    void set_bounds(const Rectangle<int> &bounds)
    { _M_bounds = bounds; }
  public:
    /// Returns the widget surface.
    const std::weak_ptr<Surface> &surface();

    /// Returns the widget parent.
    Widget *parent() const
    { return _M_parent; }
  protected:
    /// Sets this widget as a new parent of \p widget.
    void set_this_as_widget_parent(Widget *widget)
    { widget->_M_parent = this; }

    /// Sets the parent of \p widget as \c nullptr.
    void unset_this_as_widget_parent(Widget *widget)
    { widget->_M_parent = nullptr; }
  protected:
    /// Returns \c true if a touch pointer is in the widget, otherwise \c false.
    bool has_pointer(const Pointer &pointer) const;
  private:
    void add_pointer(const Pointer &pointer);

    bool delete_pointer(const Pointer &pointer);
  public:
    /// Returns the widget styles.
    Styles *styles();
    
    /// Returns the widget margin.
    Edges<int> margin()
    { return styles()->margin(real_pseudo_classes()); }

    /// Returns \c true if the widget can be adjacent to other widget, otherwise
    /// \c false.
    bool has_adjacency_to(Widget *widget)
    { return styles()->has_adjacency_to(); }
  protected:
    /// Returns the content size of the widget.
    const Dimension<int> &content_size() const
    { return _M_content_size; }

    /// Sets the content size of the widget.
    void set_content_size(const Dimension<int> &size)
    { _M_content_size = size; }
  public:
    /// Returns the listener for touch events.
    const OnTouchListener &on_touch_listener() const
    { return _M_on_touch_callback.listener(); }

    ///
    /// Sets the listener for touch events.
    ///
    /// A touch event occurs when:
    ///
    /// \li an user starts the widget touch or presses a pointer button (touch
    ///     state is \ref TouchState::DOWN),
    /// \li an user moves figer or moves the pointer with a pressed button
    ///     (touch state is \ref TouchState::MOTION),
    /// \li an user ends the widget touch or releases a pointer button (touch
    ///     state is \ref TouchState::UP).
    ///
    /// It is possible that more touches can be at same time. Each touch is
    /// identified by the touch pointer with the touch identifier. If the touch
    /// pointer hasn't a touch identifier, the widget isn't touched but is
    /// clicked by the pointer.
    ///
    void set_on_touch_listener(const OnTouchListener &listener)
    { _M_on_touch_callback.set_listener(listener); }

    /// Returns the listener for touch leaves.
    const OnTouchLeaveListener &on_touch_leave_listener() const
    { return _M_on_touch_leave_callback.listener(); }

    /// Sets the listener for touch leaves.
    void set_on_touch_leave_listener(const OnTouchLeaveListener &listener)
    { _M_on_touch_leave_callback.set_listener(listener); }

    /// Returns the listener for pointer motions.
    const OnPointerMotionListener &on_pointer_motion_listener() const
    { return _M_on_pointer_motion_callback.listener(); }

    /// Sets the listener for pointer motions.
    void set_on_pointer_motion_listener(const OnPointerMotionListener &listener)
    { _M_on_pointer_motion_callback.set_listener(listener); }

    /// Returns the listener for pointer leaves.
    const OnPointerLeaveListener &on_pointer_leave_listener() const
    { return _M_on_pointer_leave_callback.listener(); }

    /// Sets the listener for pointer leaves.
    void set_on_pointer_leave_listener(const OnPointerLeaveListener &listener)
    { _M_on_pointer_leave_callback.set_listener(listener); }

    /// Returns the listener for pointer axis events.
    const OnPointerAxisListener &on_pointer_axis_listener() const
    { return _M_on_pointer_axis_callback.listener(); }

    ///
    /// Sets the listener for pointer axis events.
    ///
    /// A pointer axis event occurs when the scroll wheel of the pointer is
    /// turned.
    ///
    void set_on_pointer_axis_listener(const OnPointerAxisListener &listener)
    { _M_on_pointer_axis_callback.set_listener(listener); }

    /// Returns the listener for key events.
    const OnKeyListener &on_key_listener() const
    { return _M_on_key_callback.listener(); }

    ///
    /// Sets the listener for key events.
    ///
    /// A key event occurs when:
    ///
    /// \li an user presses a key (key state is \ref KeyState::PRESSED),
    /// \li an user repeated a key (key stata is \ref KeyState::REPEATED),
    /// \li an user released a key (key state is \ref KeyState::RELEASED).
    ///
    void set_on_key_listener(const OnKeyListener &listener)
    { _M_on_key_callback.set_listener(listener); }

    /// Returns the listener for the widget scrolling.
    const OnScrollListener &on_scroll_listener() const
    { return _M_on_scroll_callback.listener(); }

    /// Sets the listener for the widget scrolling.
    void set_on_scroll_listener(const OnScrollListener &listener)
    { _M_on_scroll_callback.set_listener(listener); }

    /// Returns the widget name.
    virtual const char *name() const;

    /// Updates the top left point of the widget.
    virtual void update_point(const Rectangle<int> &area_bounds, const HAlignment *h_align = nullptr, const VAlignment *v_align = nullptr);
  protected:
    /// Updates the top left points of the widget children.
    virtual void update_child_points(const Rectangle<int> &area_bounds);
  public:
    /// Updates the widget size.
    virtual void update_size(Canvas *canvas, const Dimension<int> &area_size, const HAlignment *h_align = nullptr, const VAlignment *v_align = nullptr);
  protected:
    /// Updates the sizes of the widget children.
    virtual void update_child_sizes(Canvas *canvas, const Dimension<int> &area_size);

    /// Updates the content size of the widget.
    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);
  public:
    /// Returns \c true if the widget width can be again updated, \c false.
    virtual bool can_again_update_width() const;

    /// Returns \c true if the widget height can be again updated, \c false.
    virtual bool can_again_update_height() const;

    /// Draws the widget.
    virtual void draw(Canvas *canvas);
  protected:
    /// Draws the widget content.
    virtual void draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds);

    /// Draws the widget children.
    virtual void draw_children(Canvas *canvas, const Rectangle<int> &inner_bounds);
  public:
    /// Creates a new viewport of the widget.
    virtual Viewport *viewport();

    ///
    /// This method that is invoked when a touch event occurs.
    ///
    /// If this method returns \c false, this method is invoked for the widget
    /// parent.
    ///
    virtual bool on_touch(const Pointer &pointer, const Point<double> &point, TouchState state);

    /// This method that is invoked when a touch leaves from the widget.
    virtual void on_touch_leave(const Pointer &pointer);

    ///
    /// This method that is invoked when a pointer moves.
    ///
    /// If this method returns \c false, this method is invoked for the widget
    /// parent.
    ///
    virtual bool on_pointer_motion(const Point<double> &point);

    /// This method that is invoked when a pointer leaves from the widget.
    virtual void on_pointer_leave();

    ///
    /// This method that is invoked when a pointer axis occurs.
    ///
    /// If this method returns \c false, this method is invoked for the widget
    /// parent.
    ///
    virtual bool on_pointer_axis(Axis axis, double value);

    ///
    /// This method that is invoked when a key event occurs.
    ///
    /// If this method returns \c false, this method is invoked for the widget
    /// parent.
    ///
    virtual bool on_key(std::uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state);

    /// This method that is invoked when the widget is scrolled.
    virtual void on_scoll(Viewport *viewport);
  protected:
    /// Returns a margin box size of a block.
    Dimension<int> block_margin_box_size(const char *name, PseudoClasses pseudo_classes, const Dimension<int> &content_size);

    /// Draws a block.
    void draw_block(const char *name, PseudoClasses pseudo_classes, Canvas *canvas, const Point<int> &margin_box_point, const Dimension<int> &margin_box_size)
    {
      Styles *styles;
      Rectangle<int> inner_bounds;
      draw_block(name, pseudo_classes, canvas, margin_box_point, margin_box_size, inner_bounds, styles);
    }

    /// \copydoc draw_block(const char *name, PseudoClasses pseudo_classes, Canvas *canvas, const Point<int> &margin_box_point, const Dimension<int> &margin_box_size)
    void draw_block(const char *name, PseudoClasses pseudo_classes, Canvas *canvas, const Point<int> &margin_box_point, const Dimension<int> &margin_box_size, Rectangle<int> &inner_bounds)
    {
      Styles *styles;
      draw_block(name, pseudo_classes, canvas, margin_box_point, margin_box_size, inner_bounds, styles);
    }

    /// \copydoc draw_block(const char *name, PseudoClasses pseudo_classes, Canvas *canvas, const Point<int> &margin_box_point, const Dimension<int> &margin_box_size)
    void draw_block(const char *name, PseudoClasses pseudo_classes, Canvas *canvas, const Point<int> &margin_box_point, const Dimension<int> &margin_box_size, Rectangle<int> &inner_bounds, Styles *&styles);

    /// Converts an area size to a inner area size.
    Dimension<int> area_size_to_inner_area_size(const Dimension<int> &size);

    /// Converts an area bounds to a inner area bounds.
    Rectangle<int> area_bounds_to_inner_area_bounds(const Rectangle<int> &bounds);
  };

  ///
  /// A container class is base for all containers.
  ///
  /// A container is widget that contains other widgets.
  ///
  class Container : public Widget
  {
    std::list<std::unique_ptr<Widget>> _M_widgets;
  protected:
    /// Default constructor.
    Container() {}
  public:
    /// Destreuctor.
    virtual ~Container();
  protected:
    /// Initializes the container.
    void initialize(std::initializer_list<Widget *> widgets);
  public:
    /// Returns the widgets of the container.
    const std::list<std::unique_ptr<Widget>> &widgets() const
    { return _M_widgets; }

    /// Adds a new widget.
    void add_widget(Widget *widget);

    /// Tries delete the widget and returns \c true if the widget is deleted,
    /// otherwise \c false.
    bool delete_widget(Widget *widget);

    /// Deletes all widgets of the container.
    void delete_all_widgets();
  };

  ///
  /// A label is a widget that displays a text.
  ///
  class Label : public Widget
  {
    std::string _M_text;
  protected:
    /// Default constructor that doesn't invoke an \ref initialize method.
    Label() {}
  public:
    /// Constructor.
    explicit Label(const std::string &text)
    { initialize(text); }

    /// Destructor.
    virtual ~Label();
  protected:
    /// Initializes the label.
    void initialize(const std::string &text);
  public:
    /// Returns the label text.
    const std::string &text() const
    { return _M_text; }

    /// Sets the label text.
    void set_text(const std::string &text);

    virtual const char *name() const;
  protected:
    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual void draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds);
  };

  ///
  /// A button is a widget that can be clicked by an user.
  ///
  /// The button can have an icon and/or a label which are displayed on the
  /// button. The reaction of the button on a click can be specified by setting
  /// the listener.
  ///
  class Button : public Widget
  {
    Icon _M_icon;
    std::string _M_label;
    OnClickCallback _M_on_click_callback;
    std::size_t _M_touch_count;
    Dimension<int> _M_icon_margin_box_size;
    Dimension<int> _M_label_margin_box_size;
  protected:
    /// Default constructor that doesn't invoke an \ref initialize method.
    Button() {}
  public:
    /// Creates a new button with a label.
    explicit Button(const std::string &label)
    { initialize(Icon(), label, [](Widget *widget) {}, true); }

    /// Creates a new button with a label and a specified action on a click.
    Button(const std::string &label, const OnClickListener &listener)
    { initialize(Icon(), label, listener, true); }

    /// Creates a new button with an icon.
    explicit Button(const Icon &icon)
    { initialize(icon, "", [](Widget *widget) {}, true); }

    /// Creates a new button with an icon and a specified action on a click.
    Button(const Icon &icon, const OnClickListener &listener)
    { initialize(icon, "", listener, true); }

    /// Creates a new button with an icon and a label.
    Button(const Icon &icon, const std::string &label)
    { initialize(icon, label, [](Widget *widget) {}, true); }

    /// Creates a new button with an icon, a label, and a specified action on a
    /// click.
    Button(const Icon &icon, const std::string &label, const OnClickListener &listener)
    { initialize(icon, label, listener, true); }

    /// Destructor.
    virtual ~Button();
  protected:
    void initialize(const Icon &icon, const std::string &label, const OnClickListener &listener, bool is_button_h_align = false);
  public:
    /// Returns the button icon.
    const Icon icon() const
    { return _M_icon; }

    /// Sets the icon.
    void set_icon(const Icon &icon)
    { _M_icon = icon; }

    /// Returns the label text of the button.
    const std::string &label() const
    { return _M_label; }

    /// Sets the label text.
    void set_label(const std::string &label);

    /// Returns the listener for clicks.
    const OnClickListener &on_click_listener() const
    { return _M_on_click_callback.listener(); }

    /// Sets the listener for clicks.
    void set_on_click_listener(const OnClickListener &listener)
    { _M_on_click_callback.set_listener(listener); }
  public:
    virtual const char *name() const;
  protected:
    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual void draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds);
  public:
    virtual bool on_touch(const Pointer &pointer, const Point<double> &point, TouchState state);

    virtual void on_touch_leave(const Pointer &pointer);

    virtual bool on_key(std::uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state);
  protected:
    /// This method that is invoked when a click occurs.
    virtual void on_click();

    /// Returns the name of the button icon.
    virtual const char *icon_name() const;

    /// Returns the size of th button icon.
    virtual IconSize icon_size() const;

    /// Returns the margin box size of the button icon.
    const Dimension<int> &icon_margin_box_size() const
    { return _M_icon_margin_box_size; }

    /// Sets the margin box size of the button icon.
    void set_icon_margin_box_size(const Dimension<int> &size)
    { _M_icon_margin_box_size = size; }

    /// Updates the margin box size of the button icon.
    virtual void update_icon_margin_box_size(Canvas *canvas);

    /// Draws the button icon.
    virtual void draw_icon(Canvas *canvas, const Point<int> &margin_box_point);

    /// Returns the name of the button label.
    virtual const char *label_name() const;

    /// Returns the margin box size of the button label.
    const Dimension<int> &label_margin_box_size() const
    { return _M_label_margin_box_size; }

     /// Sets the margin box size of the button label.
    void label_margin_box_size(const Dimension<int> &size)
    { _M_label_margin_box_size = size; }

    /// Updates the margin box size of the label button.
    virtual void update_label_margin_box_size(Canvas *canvas);

    /// Draws the button label.
    virtual void draw_label(Canvas *canvas, const Point<int> &margin_box_point);
  };

  ///
  /// A text widget that allows to display and edit a text.
  ///
  /// The text widget can be a text field or a text area or a password field,
  /// but a text area just allows to edit a multi-line text. The text of this
  /// widget can be selected, copied to a clipboard, and a text from clipboard
  /// can be inserted to this text. This widget also supports basic key
  /// shortcuts for editing text.
  ///
  class Text : public Widget
  {
    InputType _M_input_type;
    std::unique_ptr<TextBuffer> _M_buffer;
    TextLineIterator _M_first_visible_iter;
    TextLineIterator _M_last_visible_iter;
    std::size_t _M_max_length;
    bool _M_has_line_wrap;
    bool _M_has_word_wrap;
    bool _M_is_editable;
    OnTextChangeCallback _M_on_text_change_callback;
    OnCursorChangeCallback _M_on_cursor_change_callback;
    OnTextSelectionCallback _M_on_text_selection_callback;
  protected:
    /// Constructor that doesn't invoke the \ref initialize method.
    Text(Unused unused) {}
  public:
    /// Creates a new text widget.
    Text()
    { initialize(InputType::SINGLE_LINE, std::string()); }

    /// Creates a new text widget with a text.
    explicit Text(const std::string &text)
    { initialize(InputType::SINGLE_LINE, text); }

    /// Creates a new text widget with a specified input type.
    explicit Text(InputType input_type)
    { initialize(input_type, std::string()); }

    /// Creates a new text widget with a specified input type and a text.
    Text(InputType input_type, const std::string &text)
    { initialize(input_type, text); }

    /// Destructor.
    virtual ~Text();
  protected:
    /// Initializes the text widget.
    void initialize(InputType input_type, const std::string &text);
  public:
    ///
    /// Return the input type of the text widget.
    ///
    /// The input type determines whether the text widget is a text field or a
    /// text area or a password field. By default, the text of the text widget
    /// is the text field.
    ///
    InputType input_type() const
    { return _M_input_type; }

    /// Sets the input type of the text widget.
    void set_input_type(InputType input_type)
    { _M_input_type = input_type; }

    /// Returns the text of the text widget.
    const std::string text() const
    { return _M_buffer->text(); }

    /// Sets the text of the text widget.
    void set_text(const std::string &text);

    ///
    /// Returns the text buffer of the text widget.
    ///
    /// The text widget uses the text buffer to store the text, the cursor
    /// position, and the selection range.
    ///
    const TextBuffer *buffer() const
    { return _M_buffer.get(); }

    /// \copydoc buffer() const
    TextBuffer *buffer()
    { return _M_buffer.get(); }

    /// Returns the text length of the text widget.
    std::size_t length() const
    { return _M_buffer->length(); }

    /// Returns the cursor iterator of the text widget.
    TextCharIterator cursor_iter() const
    { return _M_buffer->cursor_iter(); }

    /// Sets the cursor iterator of the text widget.
    void set_cursor_iter(const TextCharIterator &iter);

    /// Returns the cursor position of the text widget.
    TextPosition cursor_pos() const
    { return _M_buffer->cursor_pos(); }

    /// Returns the selection range of the text widget.
    Range<TextCharIterator> selection_range() const
    { return _M_buffer->selection_range(); }
 
    /// Sets the selection range of the text widget.
    void set_selection_range(const TextCharIterator &begin, const TextCharIterator &end)
    { set_selection_range(Range<TextCharIterator>(begin, end)); }

    /// \copydoc set_selection_range(const TextCharIterator &, const TextCharIterator &)
    void set_selection_range(const Range<TextCharIterator> &range);

    ///
    /// Inserts a new text into the text of the text widget.
    ///
    /// The new text is inserted after the cursor. The cursor is moved after the
    /// last inserted character.
    ///
    void insert_string(const std::string &str);

    ///
    /// Replaces the text fragment of the text widget.
    ///
    /// Indeed, this method deletes characters after the cursor, and then a new
    /// text is inserted at this same position.
    /// 
    void replace_string(std::size_t count, const std::string &str);

    ///
    /// Deletes characters from the text of the text widget.
    ///
    /// The characters are deleted from the text of the text widget after the
    /// cursor.
    ///
    void delete_chars(std::size_t count);

    ///
    /// Appends a new text to the text of the text widget at end of the text of
    /// the text widget.
    ///
    void append_string(const std::string &str);

    /// Returns the maximal text length of the text widget.
    std::size_t max_length() const
    { return _M_max_length; }

    /// Sets the maximal text length of the text widget.
    void set_max_length(std::size_t max_length)
    { _M_max_length = max_length; }

    ///
    /// Returns \c true if the line wrapping is enabled, otherwise \c false.
    ///
    /// The line wrapping breaks a text line if this line is too long to
    /// display. By default, the line wrapping is disabled.
    ///
    bool has_line_wrap() const
    { return _M_has_line_wrap; }

    /// Enables the line wrapping if \p has_line_wrap is \c true, otherwise
    /// disables the line wrapping.
    void set_line_wrap(bool has_line_wrap)
    { _M_has_line_wrap = has_line_wrap; }

    ///
    /// Returns \c true if the word wrapping, otherwise \c false.
    ///
    /// If the word wrapping and the line wraping are enabled, the words can be
    /// breaked. By default, the word wrapping is disabled.
    ///
    bool has_word_wrap() const
    { return _M_has_word_wrap; }

    /// Enables the word wrapping if \p has_line_wrap is \c true, otherwise
    /// disables the word wrapping.
    void set_word_wrap(bool has_word_wrap)
    { _M_has_word_wrap = has_word_wrap; }

    /// Returns \c true if the text widget is editable, otherwise \c false.
    bool is_editable() const
    { return _M_is_editable; }

    /// Sets the text widget as editable if \p is_editable is \c true, otherwise
    /// the widget as uneditable.
    void set_editable(bool is_editable)
    { _M_is_editable = is_editable; }

    /// Returns the selected text of the text widget.
    std::string selected_text() const
    { return _M_buffer->selected_text(); }

    /// Selects all text.
    void all_select()
    { select(_M_buffer->char_begin(), _M_buffer->char_end()); }

    /// Selects the text fragment that is specified by \p begin and \p end.
    void select(const TextCharIterator &begin, const TextCharIterator &end)
    { set_selection_range(Range<TextCharIterator>(begin, end)); }

    /// Clears the text selection.
    void clear_selection()
    { set_selection_range(Range<TextCharIterator>(_M_buffer->char_begin(), _M_buffer->char_begin())); }

    /// Copies a selected text to a clipboard.
    void copy();

    /// Inserts a text from a clipboard.
    void paste();

    /// Returns the listener for text changes.
    const OnTextChangeListener &on_text_change_listener() const
    { return _M_on_text_change_callback.listener(); }

    /// Sets the listener for text changes.
    void set_on_text_change_listener(const OnTextChangeListener &listener)
    { _M_on_text_change_callback.set_listener(listener); }

    /// Returns the listener for cursor position changes.
    const OnCursorChangeListener &on_cursor_change_listener() const
    { return _M_on_cursor_change_callback.listener(); }

    /// Sets the listener for cursor position changes.
    void set_on_cursor_change_listener(const OnCursorChangeListener &listener)
    { _M_on_cursor_change_callback.set_listener(listener); }

    /// Returns the listener for text selection changes.
    const OnTextSelectionListener &on_text_selection_listener() const
    { return _M_on_text_selection_callback.listener(); }

    /// Sets the listener for text selection changes.
    void set_on_text_selection_listener(const OnTextSelectionListener &listener)
    { _M_on_text_selection_callback.set_listener(listener); }

    virtual const char *name() const;

    virtual void draw(Canvas *canvas);

    virtual Viewport *viewport();

    virtual bool on_touch(const Pointer &pointer, const Point<double> &point, TouchState state);

    virtual void on_touch_leave(const Pointer &pointer);

    virtual bool on_key(std::uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state);
  protected:
    /// This method is invoked when the text is changed.
    virtual void on_text_change(const Range<TextCharIterator> &range);

    /// This method is invoked when the cursor position is changed.
    virtual void on_cursor_change(const TextCharIterator &iter, const TextPosition &pos);

    /// The method is invoked when the text selection is changed.
    virtual void on_text_selection(const Range<TextCharIterator> &range);
  };

  ///
  /// A check box is a toggle button.
  ///
  /// The check box has a state that is changed by clicking. This state is
  /// displayd after change.
  ///
  class CheckBox : public virtual Button
  {
    bool _M_is_checked;
    OnCheckCallback _M_on_check_callback;
    Dimension<int> _M_check_margin_box_size;
  protected:
    /// Constructor that doesn't invoke the \ref initialize method.
    CheckBox(Unused unused) {}
  public:
    /// Creates a new check box.
    CheckBox()
    { initialize(Icon(), std::string(), false); }

    /// Creates a new check box with a specified state.
    explicit CheckBox(bool is_checked)
    { initialize(Icon(), std::string(), is_checked); }

    /// Creates a new check box with a label.
    explicit CheckBox(const std::string &label)
    { initialize(Icon(), label, false); }

    /// Creates a new check box with a label and a specified state.
    CheckBox(const std::string &label, bool is_checked)
    { initialize(Icon(), label, is_checked); }

    /// Destructor.
    virtual ~CheckBox();
  protected:
    /// Initializes the check box.
    void initialize(const Icon &icon, const std::string &label, bool is_checked);
  public:
    /// Returns \c true if the check box is checked, otherwise \c false.
    bool is_checked() const
    { return _M_is_checked; }

    /// Sets the check box as checked if \p is_checked is \c true, otherwise
    /// sets the checked box as unchecked.
    void set_checked(bool is_checked);

    /// Returns the listener for changes of the check box state.
    const OnCheckListener &on_check_listener() const
    { return _M_on_check_callback.listener(); }

    /// Sets the listener for chenges of the check box state.
    void set_on_check_listener(const OnCheckListener &listener)
    { _M_on_check_callback.set_listener(listener); }

    virtual const char *name() const;
  protected:
    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual void draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds);

    virtual void on_click();

    // This method is inovked when the check box state is changed.
    virtual void on_check(bool is_checked);

    /// Returns \c true if the check box can draw an icon, otherwise \c false.
    virtual bool can_draw_icon() const;

    virtual const char *label_name() const;

    /// Returns the name of the check of the check box.
    virtual const char *check_name() const;
    
    /// Returns the margin box size of the check of the check box.
    const Dimension<int> &check_margin_box_size() const
    { return _M_check_margin_box_size; }

    /// Sets the margin box size of the check of the check box.
    void set_check_margin_box_size(const Dimension<int> &size)
    { _M_check_margin_box_size = size; }

    /// Updates the margin box size of the check of the check box.
    virtual void update_check_margin_box_size(Canvas *canvas);

    /// Draws the check of the check box.
    virtual void draw_check(Canvas *canvas, const Point<int> &margin_box_point);
  };

  ///
  /// A class that is used for grouped radio buttons.
  ///
  class RadioGroup
  {
    std::list<RadioButton *> _M_radio_buttons;
  public:
    /// Default constructor.
    RadioGroup() {}

    /// Returns the radio buttons which are grouped.
    const std::list<RadioButton *> &radio_buttons() const
    { return _M_radio_buttons; } 

    /// Adds a new radio button to the group.
    void add_radio_button(RadioButton *radio_button);
  };

  ///
  /// A radio button is a toggle button that is a option to choose.
  ///
  /// Radio buttons can be grouped and if they are grouped, just one of them can
  /// be checked. If the radio buttons aren't grouped, they behave as check
  /// boxes.
  ///
  class RadioButton : public CheckBox
  {
    std::shared_ptr<RadioGroup> _M_group;
  protected:
    /// Constructor that doesn't invoke the \ref initialize method.
    RadioButton(Unused unused) {}
  public:
    /// Creates a new radio button.
    RadioButton() :
      Button(), CheckBox(Unused())
    { initialize(Icon(), std::string(), false, std::shared_ptr<RadioGroup>()); }

    /// Creates a new radio button with a specified state.
    explicit RadioButton(bool is_checked) :
      Button(), CheckBox(Unused())
    { initialize(Icon(), std::string(), is_checked, std::shared_ptr<RadioGroup>()); }

    /// Creates a new radio button that belongs to a specified group.
    explicit RadioButton(const std::shared_ptr<RadioGroup> &group) :
      Button(), CheckBox(Unused())
    { initialize(Icon(), std::string(), false, group); }

    /// Creates a new radio button with a specified state that belongs to a
    /// specified group.
    RadioButton(bool is_checked, const std::shared_ptr<RadioGroup> &group) :
      Button(), CheckBox(Unused())
    { initialize(Icon(), std::string(), is_checked, group); }

    /// Creates a new radio button with a label.
    explicit RadioButton(const std::string &label) :
      Button(), CheckBox(Unused())
    { initialize(Icon(), label, false, std::shared_ptr<RadioGroup>()); }

    /// Creates a new radio button with a label and a specified state.
    RadioButton(const std::string &label, bool is_checked) :
      Button(), CheckBox(Unused())
    { initialize(Icon(), label, is_checked, std::shared_ptr<RadioGroup>()); }

    /// Creates a new radio button with a label that belongs to a specified
    /// group.
    RadioButton(const std::string &label, const std::shared_ptr<RadioGroup> &group) :
      Button(), CheckBox(Unused())
    { initialize(Icon(), label, false, group); }

    /// Creates a new radio button with a label and a specified stat that
    /// belongs to a specified group.
    RadioButton(const std::string &label, bool is_checked, const std::shared_ptr<RadioGroup> &group) :
      Button(), CheckBox(Unused())
    { initialize(Icon(), label, is_checked, group); }

    /// Destructor.
    virtual ~RadioButton();
  protected:
    /// Initializes the radio button.
    void initialize(const Icon &icon, const std::string &label, bool is_checked, const std::shared_ptr<RadioGroup> &group);
  public:
    /// Returns the group of the radio button.
    const std::shared_ptr<RadioGroup> &group() const
    { return _M_group; }

    /// Sets the group of the radio button.
    void set_group(const std::shared_ptr<RadioGroup> &group);

    virtual const char *name() const;
  protected:
    virtual void on_check(bool is_checked);

    virtual const char *label_name() const;

    virtual const char *check_name() const;
  };

  namespace priv
  {
    class ComboBoxListBase;
    
    class ComboBoxFields
    {
      std::size_t _M_selected_pos;
      std::shared_ptr<ComboBoxAdapter> _M_adapter;
      OnSelectionCallback _M_on_selection_callback;
      std::shared_ptr<Surface> _M_popup_surface;
      int _M_selected_item_y;
      std::function<void (std::size_t)> _M_on_selection;

      friend class ComboBoxListBase;
      friend class waytk::ComboBox;
    };

    class ComboBoxListBase
    {
    protected:
      std::shared_ptr<ComboBoxFields> _M_fields;

      ComboBoxListBase(const std::shared_ptr<ComboBoxFields> &fields) :
        _M_fields(fields) {}
    public:
      virtual ~ComboBoxListBase();
    protected:
      std::size_t &selected_pos()
      { return _M_fields->_M_selected_pos; }

      std::shared_ptr<ComboBoxAdapter> &adapter()
      { return _M_fields->_M_adapter; }

      OnSelectionCallback &on_selection_callback()
      { return _M_fields->_M_on_selection_callback; }

      std::shared_ptr<Surface> &popup_surface()
      { return _M_fields->_M_popup_surface; }
      
      int &selected_item_y()
      { return _M_fields->_M_selected_item_y; }

      void on_selection(std::size_t pos)
      { return _M_fields->_M_on_selection(pos); }
    };
  }

  ///
  /// A combo box is a button with drop-down list.
  ///
  /// When this button is clicked, the drop-down list is displayed. This list
  /// has options, one of them can be selected. The selected option is displayed
  /// on the combo box.
  ///
  class ComboBox : public Button
  {
    std::shared_ptr<priv::ComboBoxFields> _M_fields;
  protected:
    /// Constructor that doesn't invoke the \ref initialize method.
    ComboBox(Unused unused) {}
  public:
    /// Creates a new combo box.
    ComboBox()
    { initialize(std::shared_ptr<ComboBoxAdapter>(new StringComboBoxAdapter())); }

    /// Creates a new combo box with items.
    explicit ComboBox(std::initializer_list<std::string> items)
    { initialize(std::shared_ptr<ComboBoxAdapter>(new StringComboBoxAdapter(items))); }

    /// Creates a new combo box with an adapter.
    explicit ComboBox(ComboBoxAdapter *adapter)
    { initialize(std::shared_ptr<ComboBoxAdapter>(adapter)); }

    /// \copydoc ComboBox(ComboBoxAdapter *adapter)
    explicit ComboBox(const std::shared_ptr<ComboBoxAdapter> &adapter)
    { initialize(adapter); }

    /// Destructor.
    virtual ~ComboBox();
  protected:
    /// Initializes the combo box.
    void initialize(const std::shared_ptr<ComboBoxAdapter> &adapter);
  public:
    /// Returns the position of the selected item.
    std::size_t selected_pos() const
    { return _M_fields->_M_selected_pos; }

    /// Sets the position of the selcted itme.
    void set_selected_pos(std::size_t pos);

    /// Returns the adatper of the combo box.
    const std::shared_ptr<ComboBoxAdapter> &adapter() const
    { return _M_fields->_M_adapter; }

    /// Sets the adapter of the combo box.
    void set_adapter(ComboBoxAdapter *adapter)
    { _M_fields->_M_adapter = std::shared_ptr<ComboBoxAdapter>(adapter); }

    /// \copydoc set_adapter
    void set_adapter(const std::shared_ptr<ComboBoxAdapter> &adapter)
    { _M_fields->_M_adapter = adapter; }

    /// Selects the one item of the combo box.
    void select(std::size_t pos)
    { set_selected_pos(pos); }

    /// Returns the listener for selection changes.
    const OnSelectionListener &on_selection_listener() const
    { return _M_fields->_M_on_selection_callback.listener(); }

    /// Sets the listener for selection changes.
    void set_selection_listener(const OnSelectionListener &listener)
    { _M_fields->_M_on_selection_callback.set_listener(listener); }

    virtual const char *name() const;

    virtual void draw(Canvas &canvas);
  protected:
    virtual void on_click();
  public:
    /// This method is invoked when the selection is changed.
    virtual void on_selection(std::size_t pos);
  };

  ///
  /// A progress bar displays a progress.
  ///
  class ProgressBar : public Widget
  {
    int _M_max_value;
    int _M_value;
  protected:
    /// Constructor that doesn't invoke the \ref initialize method.
    ProgressBar(Unused unused) {}
  public:
    /// Creates a new progress bar with a maximal value that is \c 100.
    ProgressBar()
    { initialize(100); }

    /// Creates a new progress bar with a specified maximal value.
    explicit ProgressBar(int max_value)
    { initialize(max_value); }

    /// Destructor.
    virtual ~ProgressBar();
  protected:
    /// Initializes the progress bar.
    void initialize(int max_value);
  public:
    /// Returns the maximal value of the progress bar.
    int max_value() const
    { return _M_max_value; }

    /// Sets the maximal value of the progress bar.
    void set_max_value(int max_value)
    { _M_max_value = max_value; }

    ///
    /// Returns the value of the progress bar.
    ///
    /// The value of the progress bar indicates a progress. By default, the
    /// value is \c 0.
    ///
    int value() const
    { return _M_value; }

    /// Sets the value of the progress bar.
    void set_value(int value)
    { _M_value = (value < _M_max_value ? value : _M_max_value); }

    virtual const char *name() const;
  protected:
    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual void draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds);

    /// Returns the name of the progess of the progress bar.
    virtual const char *progress_name() const;

    /// Draws the progress of the progess bar.
    virtual void draw_progress(Canvas *canvas, const Point<int> &margin_box_point, const Dimension<int> &margin_box_size);
  };

  ///
  /// An image widget displays an image.
  ///
  class Image : public Widget
  {
    std::shared_ptr<CanvasImage> _M_image;
  protected:
    /// Default constructor that doesn't invoke the \ref initialize method.
    Image() {}
  public:
    /// Creates a new image widget.
    Image(int width, int height)
    { initialize(std::shared_ptr<CanvasImage>(new_canvas_modifiable_image(width, height))); }

    /// \copydoc Image(int, int)
    explicit Image(Dimension<int> size)
    { initialize(std::shared_ptr<CanvasImage>(new_canvas_modifiable_image(size))); }

    /// Creates a new image widget with an image from \p data.
    Image(int width, int height, int stride, void *data)
    { initialize(std::shared_ptr<CanvasImage>(new_canvas_modifiable_image(width, height, stride, data))); }

    /// \copydoc Image(int, int, int, void *)
    Image(Dimension<int> size, int stride, void *data)
    { initialize(std::shared_ptr<CanvasImage>(new_canvas_modifiable_image(size, stride, data))); }

    /// Creates a new image widget with a image from the file.
    explicit Image(const std::string &file_name)
    { initialize(std::shared_ptr<CanvasImage>(load_canvas_image(file_name)));}

    /// Creates a new image widget with an image.
    explicit Image(CanvasImage *image)
    { initialize(std::shared_ptr<CanvasImage>(image)); }

    /// \copydoc Image(CanvasImage *)
    explicit Image(const std::shared_ptr<CanvasImage> &image)
    { initialize(image); }

    /// Destructor.
    virtual ~Image();
  protected:
    /// Initializes the image widget.
    void initialize(const std::shared_ptr<CanvasImage> &image);
  public:
    /// Returns the image of the image widget.
    const std::shared_ptr<CanvasImage> &image() const
    { return _M_image; }

    /// Sets an empty image.
    void set_image(int width, int height)
    { set_image(new_canvas_modifiable_image(width, height)); }

    /// \copydoc set_image(int, int)
    void set_image(Dimension<int> size)
    { set_image(new_canvas_modifiable_image(size)); }

    /// Sets an image from \p data.
    void set_image(int width, int height, int stride, void *data)
    { set_image(new_canvas_modifiable_image(width, height)); }

    /// \copydoc set_image(int, int, int, void *)
    void set_image(Dimension<int> size, int stride, void *data)
    { set_image(new_canvas_modifiable_image(size)); }

    /// Sets the image of the image widget.
    void set_image(CanvasImage *image)
    { set_image(std::shared_ptr<CanvasImage>(image)); }

    /// \copydoc set_image(CanvasImage *)
    void set_image(const std::shared_ptr<CanvasImage> &image);

    /// Loads an image from the file.
    void load(const std::string &file_name)
    { set_image(std::shared_ptr<CanvasImage>(load_canvas_image(file_name))); }

    virtual const char *name() const;
  protected:
    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual void draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds, Styles *style);
  };

  ///
  /// A panel is container that displays widgets.
  ///
  class Panel : public Container
  {
  protected:
    /// Default constructor.
    Panel() {}
  public:
    /// Destructor.
    virtual ~Panel();
  protected:
    virtual void draw_children(Canvas *canvas, const Rectangle<int> &inner_bounds);
  };

  ///
  /// A linear panel displays widgets in one line.
  ///
  class LinearPanel : public Panel
  {
    Orientation _M_orientation;
    int _M_weight_sum;
    union
    {
      int _M_item_height;       ///< \private
      int _M_item_width;        ///< \private
    };
  protected:
    /// Constructor that doesn't invoke the \ref initialize method.
    LinearPanel(Unused unused) {}
  public:
    /// Creates a new horizontal linear panel.
    LinearPanel()
    { initialize(Orientation::HORIZONTAL, {}); }

    /// Creates a new horizontal linear panel with widgets.
    explicit LinearPanel(std::initializer_list<Widget *> widgets)
    { initialize(Orientation::HORIZONTAL, widgets); }

    /// Creates a new linear panel with a specified orientation.
    explicit LinearPanel(Orientation orientation)
    { initialize(orientation, {}); }

    /// Creates a new linear panel with a specified orientation and widgets.
    LinearPanel(Orientation orientation, std::initializer_list<Widget *> widgets)
    { initialize(orientation, widgets); }

    /// Destructor.
    virtual ~LinearPanel();
  protected:
    /// Initializes the linear panel.
    void initialize(Orientation orientation, std::initializer_list<Widget *> widgets);
  public:
    ///
    /// Returns the orientation of the linear panel.
    ///
    /// The orientation of the linear panel determines whether widgets are
    /// displayed in horizontal line or vertical line.
    ///
    Orientation orientation() const
    { return _M_orientation; }

    /// Sets the orientation of the linear panel.
    void set_orientation(Orientation orientation)
    { _M_orientation = orientation; }

    int weight_sum() const
    { return _M_weight_sum; }

    void set_weight_sum(int weight_sum)
    { _M_weight_sum = (weight_sum >= 0 ? weight_sum : 0); }
    
    virtual const char *name() const;
  protected:
    virtual void update_child_points(const Rectangle<int> &area_bounds);

    virtual void update_child_sizes(Canvas *canvas, const Dimension<int> &area_size);

    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual bool can_again_update_width() const;

    virtual bool can_again_update_height() const;
  private:
    int min_item_width(int inner_area_width, double &float_width);

    int min_item_height(int inner_area_height, double &float_height);
  };

  ///
  /// A grid panel displays widgets in grid cells.
  ///
  class GridPanel : public Panel
  {
    int _M_column_count;
    Dimension<int> _M_cell_size;
  protected:
    /// Default constructor that doesn't invoke the \ref initialize method.
    GridPanel() {}
  public:
    /// Creates a new grid panel with a specified number of columns.
    explicit GridPanel(int column_count)
    { initialize(column_count, {}); }

    /// Creates a new grid panel with a specified number of columns and widgets.
    GridPanel(int column_count, std::initializer_list<Widget *> widgets)
    { initialize(column_count, widgets); }

    /// Destructor.
    virtual ~GridPanel();
  protected:
    /// Initializes the grid panel.
    void initialize(int column_count, std::initializer_list<Widget *> widgets);
  public:
    /// Returns the number of columns.
    int column_count() const
    { return _M_column_count; }

    /// Sets the number of columns.
    void set_column_count(int column_count)
    { _M_column_count = (column_count >= 1 ? column_count : 1); }

    virtual const char *name() const;
  protected:
    virtual void update_child_points(const Rectangle<int> &area_bounds);

    virtual void update_child_sizes(Canvas *canvas, const Dimension<int> &area_size);

    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual bool can_again_update_width() const;

    virtual bool can_again_update_height() const;
  };

  ///
  /// A split pane contains two widgets.
  ///
  /// Two widgets of the split pane display as left and right or top and bottom
  /// Widths of two widgets or heights of two widgets can be adjusted by an
  /// user.
  ///
  class SplitPane : public Widget
  {
    Orientation _M_orientation;
    std::unique_ptr<Widget> _M_first_widget;
    std::unique_ptr<Widget> _M_second_widget;
  protected:
    /// Default constructor that doesn't invoke the \ref initialize method.
    SplitPane() {}
  public:
    /// Creates a new horizontal split pane with two widgets.
    SplitPane(Widget *first_widget, Widget *second_widget)
    { initialize(Orientation::HORIZONTAL, first_widget, second_widget); }

    /// Creates a new split pane with two widgets and a specified orientation.
    SplitPane(Orientation orientation, Widget *first_widget, Widget *second_widget)
    { initialize(orientation, first_widget, second_widget); }

    /// Destructor.
    virtual ~SplitPane();
  protected:
    /// Initializes the split pane.
    void initialize(Orientation orientation, Widget *first_widget, Widget *second_widget);
  public:
    ///
    /// Returns the orientation of the split pane.
    ///
    /// The orientation of the split pnae determines whether two widget of the
    /// split pane are left and right or top and bottom.
    ///
    Orientation orientation() const
    { return _M_orientation; }

    /// Sets the orientation of the split pane.
    void set_orientation(Orientation orientation)
    { _M_orientation = orientation; }

    /// Returns the first widget of the split pane.
    Widget *first_widget() const
    { return _M_first_widget.get(); }

    /// Sets the first widget of the split pane.
    void set_first_widget(Widget *widget);
    
    /// Returns the second widget of the split pane.
    Widget *second_widget() const
    { return _M_second_widget.get(); }

    /// Sets the second widget of the split pane.
    void set_second_widget(Widget *widget);

    virtual const char *name() const;
  protected:
    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual void draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds, Styles *style);
  };

  ///
  /// A list widget displays items of a list.
  ///
  /// The list widget allows to select a single item or more items. If more than
  /// one item can be selected, an user can use shortcuts for a selection. Each
  /// item is displayed as a new widget that was created by a list adapter of
  /// the list widget. The item widgets are created for visible items and
  /// destroyed if these items are invisible.
  ///
  class List : public Widget
  {
    SelectionMode _M_selection_mode;
    std::set<std::size_t> _M_selected_poses;
    std::shared_ptr<ListAdapter> _M_adapter;
    OnListSelectionCallback _M_on_list_selection_callback;
  protected:
    /// Constructor that doesn't invoke the \ref initialize method.
    List(Unused unused) {}
  public:
    /// Creates a new list widget.
    List()
    { initialize(SelectionMode::SINGLE, std::shared_ptr<ListAdapter>(new StringListAdapter())); }

    /// Creates a new list widget with items.
    explicit List(std::initializer_list<std::string> items)
    { initialize(SelectionMode::SINGLE, std::shared_ptr<ListAdapter>(new StringListAdapter(items))); }

    /// Creates a new list widget with an adapter.
    explicit List(ListAdapter *adapter)
    { initialize(SelectionMode::SINGLE, std::shared_ptr<ListAdapter>(adapter)); }

    /// \copydoc List(ListAdapter *adapter)
    explicit List(const std::shared_ptr<ListAdapter> &adapter)
    { initialize(SelectionMode::SINGLE, adapter); }

    /// Creates a new list widget with a specified mode of selection.
    explicit List(SelectionMode mode)
    { initialize(mode, std::shared_ptr<ListAdapter>(new StringListAdapter())); }

    /// Creates a new list widget with a specified mode of selection and items.
    List(SelectionMode mode, std::initializer_list<std::string> items)
    { initialize(mode, std::shared_ptr<ListAdapter>(new StringListAdapter(items))); }

    /// Creates a new list widget with a specified mode of selection and an
    /// adapter.
    List(SelectionMode mode, ListAdapter *adapter)
    { initialize(mode, std::shared_ptr<ListAdapter>(adapter)); }

    /// \copydoc List(SelectionMode mode, ListAdapter *adapter)
    List(SelectionMode mode, const std::shared_ptr<ListAdapter> &adapter)
    { initialize(mode, adapter); }

    /// Destructor.
    virtual ~List();
  protected:
    /// Initializes the list widget.
    void initialize(SelectionMode mode, const std::shared_ptr<ListAdapter> &adapter);
  public:
    ///
    /// Returns the selection mode of the list widget.
    ///
    /// The selection mode of the list widget is determines whether a single
    /// item or more items can be selected. By default, a single item just can
    /// be selected.
    ///
    SelectionMode selection_mode() const
    { return _M_selection_mode; }

    /// Sets the selection mode of the list widget.
    void set_selection_mode(SelectionMode mode)
    { _M_selection_mode = mode; }

    /// Returns the positions of the selected items.
    const std::set<std::size_t> &selected_poses() const
    { return _M_selected_poses; }

    /// Sets the positions of the selected items.
    void set_selected_poses(std::initializer_list<std::size_t> poses)
    { set_selected_poses(std::set<std::size_t>(poses)); }

    /// \copydoc set_selected_poses(std::initializer_list<std::size_t> poses)
    void set_selected_poses(const std::list<std::size_t> &poses)
    { set_selected_poses(std::set<std::size_t>(poses.begin(), poses.end())); }

    /// \copydoc set_selected_poses(std::initializer_list<std::size_t> poses)
    void set_selected_poses(const std::vector<std::size_t> &poses)
    { set_selected_poses(std::set<std::size_t>(poses.begin(), poses.end())); }

    /// \copydoc set_selected_poses(std::initializer_list<std::size_t> poses)
    void set_selected_poses(const std::set<std::size_t> &poses);

    /// Returns the list adapter of the list widget.
    const std::shared_ptr<ListAdapter> &adapter() const
    { return _M_adapter; }

    /// Sets the list adapter of the list widget.
    void set_adapter(ListAdapter *adapter)
    { _M_adapter = std::shared_ptr<ListAdapter>(adapter); }

    /// \copydoc set_adapter(ListAdapter *adapter)
    void set_adapter(const std::shared_ptr<ListAdapter> &adapter)
    { _M_adapter = adapter; }

    /// Selects all items of the list widget.
    void select_all();

    /// Selects the item if this item is deselected, otherwise deselects the
    /// item.
    void change_selection(std::size_t pos)
    { change_selection(Range<std::size_t>(pos, pos + 1)); }

    /// Selects the deslected items and deselects the selected items.
    void change_selection(const Range<std::size_t> &range);

    /// Clears the item selection.
    void clear_selection();

    /// Returns the listener for the selection changes.
    const OnListSelectionListener &on_list_selection_listener() const
    { return _M_on_list_selection_callback.listener(); }

    /// Sets the listener for the selection changes.
    void set_on_list_selection_listener(const OnListSelectionListener &listener)
    { _M_on_list_selection_callback.set_listener(listener); }

    virtual const char *name() const;
  protected:
    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual void draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds, Styles *style);
  public:
    virtual Viewport *viewport();

    virtual bool on_touch(const Pointer &pointer, const Point<double> &point, TouchState state);

    virtual void on_touch_leave(const Pointer &pointer);

    virtual bool on_key(std::uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state);
  protected:
    /// This method is invoked when the selection is changes.
    virtual void on_list_selection(const std::set<std::size_t> &poses);
  };

  ///
  /// A table widget displays cells of a table.
  ///
  /// The table widget displays cells on grid. Also, headers with labels can be
  /// displayed on top of the table widget. One of the cells or more cells can
  /// be selected. The table widget allows to use shortcuts for a selection if
  /// more than one cells can be selected. The table adapter of the table widget
  /// creates cell widgets which are creates for visible cells.
  ///
  class Table : public Widget
  {
    SelectionMode _M_selection_mode;
    std::vector<std::string> _M_header_labels;
    std::set<TablePosition, TablePositionCompare> _M_selected_poses;
    std::shared_ptr<TableAdapter> _M_adapter;
    OnTableSelectionCallback _M_on_table_selection_callback;
  protected:
    /// Constructor that doesn't invoke the \ref initialize method.
    Table(Unused unused) {}
  public:
    /// Creates a new table widget with a specified number of columns.
    explicit Table(std::size_t column_count)
    { initialize(SelectionMode::SINGLE, {}, std::shared_ptr<TableAdapter>(new StringTableAdapter(column_count))); }

    /// Creates a new table widget with cells.
    explicit Table(std::initializer_list<std::initializer_list<std::string>> cells)
    { initialize(SelectionMode::SINGLE, {}, std::shared_ptr<TableAdapter>(new StringTableAdapter(cells))); }

    /// Creates a new table widget with an adapter.
    explicit Table(TableAdapter *adapter)
    { initialize(SelectionMode::SINGLE, {}, std::shared_ptr<TableAdapter>(adapter)); }

    /// \copydoc Table(TableAdapter *adapter)
    explicit Table(const std::shared_ptr<TableAdapter> &adapter)
    { initialize(SelectionMode::SINGLE, {}, adapter); }

    /// Creates a new table widget with headers.
    explicit Table(std::initializer_list<std::string> header_labels)
    { initialize(SelectionMode::SINGLE, header_labels, std::shared_ptr<TableAdapter>(new StringTableAdapter(header_labels.size()))); }

    /// Creates a new table widget with headers and cells.
    Table(std::initializer_list<std::string> header_labels, std::initializer_list<std::initializer_list<std::string>> cells)
    { initialize(SelectionMode::SINGLE, header_labels, std::shared_ptr<TableAdapter>(new StringTableAdapter(cells))); }

    /// Creates a new table widget with headers and an adapter.
    Table(std::initializer_list<std::string> header_labels, TableAdapter *adapter)
    { initialize(SelectionMode::SINGLE, header_labels, std::shared_ptr<TableAdapter>(adapter)); }

    /// \copydoc Table(std::initializer_list<std::string> header_labels, TableAdapter *adapter)
    Table(std::initializer_list<std::string> header_labels, const std::shared_ptr<TableAdapter> &adapter)
    { initialize(SelectionMode::SINGLE, header_labels, adapter); }

    /// Creates a new table widget with a selection mode and a specified number
    /// of columns.
    Table(SelectionMode mode, std::size_t column_count)
    { initialize(mode, {}, std::shared_ptr<TableAdapter>(new StringTableAdapter(column_count))); }

    /// Creates a new table widget with a selection mode and cells.
    Table(SelectionMode mode, std::initializer_list<std::initializer_list<std::string>> cells)
    { initialize(mode, {}, std::shared_ptr<TableAdapter>(new StringTableAdapter(cells))); }

    /// Creates a new table widget with a selection mode and an adatper.
    Table(SelectionMode mode, TableAdapter *adapter)
    { initialize(mode, {}, std::shared_ptr<TableAdapter>(adapter)); }

    /// \copydoc Table(SelectionMode mode, TableAdapter *adapter)
    Table(SelectionMode mode, const std::shared_ptr<TableAdapter> &adapter)
    { initialize(mode, {}, adapter); }

    /// Creates a new table widget with a selection mode and headers.
    Table(SelectionMode mode, std::initializer_list<std::string> header_labels)
    { initialize(mode, header_labels, std::shared_ptr<TableAdapter>(new StringTableAdapter(header_labels.size()))); }

    /// Creates a new table widget with a selection mode, headers, and cells.
    Table(SelectionMode mode, std::initializer_list<std::string> header_labels, std::initializer_list<std::initializer_list<std::string>> cells)
    { initialize(mode, header_labels, std::shared_ptr<TableAdapter>(new StringTableAdapter(cells))); }

    /// Creates a new table widget with a selection mode, headers, and an
    /// adapter.
    Table(SelectionMode mode, std::initializer_list<std::string> header_labels, TableAdapter *adapter)
    { initialize(mode, header_labels, std::shared_ptr<TableAdapter>(adapter)); }

    /// \copydoc Table(SelectionMode mode, std::initializer_list<std::string> header_labels, TableAdapter *adapter)
    Table(SelectionMode mode, std::initializer_list<std::string> header_labels, const std::shared_ptr<TableAdapter> &adapter)
    { initialize(mode, header_labels, adapter); }

    /// Destructor.
    virtual ~Table();
  protected:
    /// Initializes the table widget.
    void initialize(SelectionMode selection_mode, std::initializer_list<std::string> header_labels, const std::shared_ptr<TableAdapter> &adapter);
  public:
    ///
    /// Returns the selection mode of the table widget.
    ///
    /// The selection mode of the table widget is determines whether a single
    /// item or more items can be selected. By default, a single item just can
    /// be selected.
    ///
    SelectionMode selection_mode() const
    { return _M_selection_mode; }

    /// Sets the selection mode of the table widget.
    void set_selection_mode(SelectionMode mode)
    { _M_selection_mode = mode; }

    /// Returns the header labels of the table widget.
    const std::vector<std::string> &header_labels() const
    { return _M_header_labels; }

    /// Sets the header labels of the table widget.
    void set_header_labels(std::initializer_list<std::string> header_labels)
    { set_header_labels(std::vector<std::string>(header_labels)); }

    /// \copydoc set_header_labels(std::initializer_list<std::string> header_labels)
    void set_header_labels(const std::list<std::string> &header_labels)
    { set_header_labels(std::vector<std::string>(header_labels.begin(), header_labels.end())); }

    /// \copydoc set_header_labels(std::initializer_list<std::string> header_labels)
    void set_header_labels(const std::vector<std::string> &header_labels)
    { _M_header_labels = header_labels; }

    /// Returns the position of the selected cells.
    const std::set<TablePosition, TablePositionCompare> &selected_poses() const
    { return _M_selected_poses; }

    /// Sets the position of the selected cells.
    void set_selected_poses(std::initializer_list<TablePosition> poses)
    { set_selected_poses(std::set<TablePosition, TablePositionCompare>(poses)); }

    /// \copydoc set_selected_poses(std::initializer_list<TablePosition> poses)
    void set_selected_poses(const std::list<TablePosition> &poses)
    { set_selected_poses(std::set<TablePosition, TablePositionCompare>(poses.begin(), poses.end())); }
    
    /// \copydoc set_selected_poses(std::initializer_list<TablePosition> poses)
    void set_selected_poses(const std::vector<TablePosition> &poses)
    { set_selected_poses(std::set<TablePosition, TablePositionCompare>(poses.begin(), poses.end())); }

    /// \copydoc set_selected_poses(std::initializer_list<TablePosition> poses)
    void set_selected_poses(const std::set<TablePosition, TablePositionCompare> &poses);

    /// Returns the table adapter of the table widget.
    const std::shared_ptr<TableAdapter> &adapter() const
    { return _M_adapter; }

    /// Sets the table adapter of the table widget.
    void set_adapter(TableAdapter *adapter)
    { _M_adapter = std::shared_ptr<TableAdapter>(adapter); }

    /// \copydoc set_adapter(TableAdapter *adapter)
    void set_adapter(const std::shared_ptr<TableAdapter> &adapter)
    { _M_adapter = adapter; }

    /// Selects all cells.
    void select_all();

    /// Selects the cell if this cell is deselected, otherwise deselects the
    /// cell.
    void change_selection(std::size_t row, std::size_t column)
    { change_selection(Range<std::size_t>(row, row + 1), Range<std::size_t>(column, column + 1)); }

    /// Selects the deslected cells and deselects the selected cells.
    void change_selection(const Range<std::size_t> &row_range, const Range<std::size_t> &column_range);

    /// Clears the item selection.
    void clear_selection();

    /// Returns the listener for the selection changes.
    const OnTableSelectionListener &on_table_selection_listener() const
    { return _M_on_table_selection_callback.listener(); }

    /// Sets the listener for the selection changes.
    void set_on_table_selection_listener(const OnTableSelectionListener &listener)
    { _M_on_table_selection_callback.set_listener(listener); }

    virtual const char *name() const;
  protected:
    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual void draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds, Styles *style);
  public:
    virtual Viewport *viewport();

    virtual bool on_touch(const Pointer &pointer, const Point<double> &point, TouchState state);

    virtual void on_touch_leave(const Pointer &pointer);

    virtual bool on_key(std::uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state);
  protected:
    /// This method is invoked when the selection is changes.
    virtual void on_table_selection(const std::set<TablePosition, TablePositionCompare> &poses);
  };

  ///
  /// A tree widget displays trees.
  ///
  /// Branch of trees can be expanded or unexpended. Ascendants of a branch are
  /// displayed if the branch is expanded, otherwise is hidden. One of the nodes
  /// of tree or more notes of tree can be selected by an user. If more than on
  /// node of the tree widget can be selected, an user can use shortcuts for a
  /// selection. Nodes of trees are displayed by displaying the widgets and
  /// these widgets are which are creates by the tree adapter.
  ///
  class Tree : public Widget
  {
    struct ExtendedNode
    {
      std::size_t index;
      std::list<ExtendedNode> extended_children;
    };

    SelectionMode _M_selection_mode;
    std::set<TreePath, TreePathCompare> _M_selected_paths;
    std::shared_ptr<TreeAdapter> _M_adapter;
    std::list<ExtendedNode> _M_extended_nodes;
    std::size_t _M_item_count;
    OnTreeSelectionCallback _M_on_tree_selection_callback;
  protected:
    /// Constructor that doesn't invoke the \ref initialize method.
    Tree(Unused unused) {}
  public:
    /// Creates a new tree widget.
    Tree()
    { initialize(SelectionMode::SINGLE, std::shared_ptr<TreeAdapter>(new StringTreeAdapter())); }

    /// Creates a new tree widget with nodes.
    explicit Tree(std::initializer_list<StringTreeNode> nodes)
    { initialize(SelectionMode::SINGLE, std::shared_ptr<TreeAdapter>(new StringTreeAdapter(nodes))); }

    /// Creates a new tree widget with an adapter.
    explicit Tree(TreeAdapter *adapter)
    { initialize(SelectionMode::SINGLE, std::shared_ptr<TreeAdapter>(adapter)); }

    /// \copydoc Tree(TreeAdapter *adapter)
    explicit Tree(const std::shared_ptr<TreeAdapter> &adapter)
    { initialize(SelectionMode::SINGLE, adapter); }

    /// Creates a new tree widget with a seletion mode.
    explicit Tree(SelectionMode selection_mode)
    { initialize(selection_mode, std::shared_ptr<TreeAdapter>(new StringTreeAdapter())); }

    /// Creates a new tree widget with a selection mode with nodes.
    Tree(SelectionMode selection_mode, std::initializer_list<StringTreeNode> nodes)
    { initialize(selection_mode, std::shared_ptr<TreeAdapter>(new StringTreeAdapter(nodes))); }

    /// Creates a new tree widget with a selection mode with an adapter.
    Tree(SelectionMode selection_mode, TreeAdapter *adapter)
    { initialize(selection_mode, std::shared_ptr<TreeAdapter>(adapter)); }

    /// \copydoc Tree(SelectionMode selection_mode, TreeAdapter *adapter)
    Tree(SelectionMode selection_mode, const std::shared_ptr<TreeAdapter> &adapter)
    { initialize(selection_mode, adapter); }

    /// Destructor.
    virtual ~Tree();
  protected:
    /// Initializes the tree widget.
    void initialize(SelectionMode selection_mode, const std::shared_ptr<TreeAdapter> &adapter);
  public:
    /// Returns the selection mode of the tree widget.
    SelectionMode selection_mode() const
    { return _M_selection_mode; }

    /// Sets the selection mode of the tree widget.
    void set_selection_mode(SelectionMode mode)
    { _M_selection_mode = mode; }

    /// Returns the selected paths of the tree widget.
    const std::set<TreePath, TreePathCompare> &selected_paths() const
    { return _M_selected_paths; }

    /// Sets the selected paths of the tree widget.
    void set_selected_paths(std::initializer_list<TreePath> paths)
    { set_selected_paths(std::set<TreePath, TreePathCompare>(paths)); }

    /// \copydoc set_selected_paths(std::initializer_list<TreePath> paths)
    void set_selected_paths(const std::list<TreePath> &paths)
    { set_selected_paths(std::set<TreePath, TreePathCompare>(paths.begin(), paths.end())); }
    
    /// \copydoc set_selected_paths(std::initializer_list<TreePath> paths)
    void set_selected_paths(const std::vector<TreePath> &paths)
    { set_selected_paths(std::set<TreePath, TreePathCompare>(paths.begin(), paths.end())); }

    /// \copydoc set_selected_paths(std::initializer_list<TreePath> paths)
    void set_selected_paths(const std::set<TreePath, TreePathCompare> &paths);

    /// Returns the adapter of the tree widget.
    const std::shared_ptr<TreeAdapter> &adapter() const
    { return _M_adapter; }

    /// Sets the adapter of the tree widget.
    void set_adapter(TreeAdapter *adapter)
    { _M_adapter = std::shared_ptr<TreeAdapter>(adapter); }

    /// \copydoc set_adapter(TreeAdapter *adapter)
    void set_adapter(const std::shared_ptr<TreeAdapter> &adapter)
    { _M_adapter = adapter; }

    /// Selects all nodes of the list widget.
    void select_all();

    /// Selects the node if this node is deselected, otherwise deselects the
    /// node.
    void change_selection(std::size_t item_pos)
    { change_selection(Range<std::size_t>(item_pos, item_pos + 1)); }

    /// Selects the deslected nodes and deselects the selected nodes.
    void change_selection(const Range<std::size_t> &item_pos_range);

    /// Clears the node selection.
    void clear_selection();
  private:
    bool select_branch(TreePath &path);

    void change_selection_for_branch(const Range<std::size_t> &item_pos_range, const ExtendedNode &extended_node, TreePath &path, std::size_t &pos);
  public:
    /// Returns the listener for the node selection.
    const OnTreeSelectionListener &on_tree_selection_listener() const
    { return _M_on_tree_selection_callback.listener(); }

    /// Sets the listener for the node selection.
    void set_on_tree_selection_listener(const OnTreeSelectionListener &listener)
    { _M_on_tree_selection_callback.set_listener(listener); }

    virtual const char *name() const;
  protected:
    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual void draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds, Styles *style);
  public:
    virtual Viewport *viewport();

    virtual bool on_touch(const Pointer &pointer, const Point<double> &point, TouchState state);

    virtual void on_touch_leave(const Pointer &pointer);

    virtual bool on_key(std::uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state);
  protected:
    /// This method is invoked when the selection is changes.
    virtual void on_tree_selection(const std::set<TreePath, TreePathCompare> &paths);
  };

  ///
  /// A scroll widget allows to move a widget at inside.
  ///
  /// The scroll widget uses a viewport object of the widget at inside to get
  /// and set offset at the viewport. This offset is used for displaying this
  /// widget at inside.
  ///
  class Scroll : public Widget
  {
    std::unique_ptr<Widget> _M_widget;
    bool _M_has_h_scroll_bar;
    bool _M_has_auto_h_scroll_bar;
    bool _M_has_v_scroll_bar;
    bool _M_has_auto_v_scroll_bar;
  protected:
    /// Default constructor that doesn't invoke the \ref initialize method.
    Scroll() {}
  public:
    /// Creates a new scroll widget with a widget.
    explicit Scroll(Widget *widget);

    /// Destructor.
    virtual ~Scroll();
  protected:
    /// Initializes the scroll widget.
    void initialize(Widget *widget);
  public:
    /// Returns the widget of the scroll widget.
    Widget *widget() const
    { return _M_widget.get(); }

    /// Sets the widget of the scroll widget.
    void set_widget(Widget *widget);

    /// Returns \c true if the horizontal scroll bar is enable, otherwise
    /// \c false.
    bool has_h_scroll_bar() const
    { return _M_has_h_scroll_bar; }

    /// Enables the horizontal scroll bar if \p has_scroll_bar is \c true,
    /// otherwise disable the horizontal scroll bar.
    void set_h_scroll_bar(bool has_scroll_bar)
    { _M_has_h_scroll_bar = has_scroll_bar; }

    /// Returns \c true if the horizontal scroll bar can be automatically
    /// enabled, otherwise \c false.
    bool has_auto_h_scroll_bar() const
    { return _M_has_auto_h_scroll_bar; }

    /// Allows to automatically enable the horizontal scroll bar if
    /// \c has_auto_h_scroll_bar is \c true, otherwise denies to automatically
    /// enable the horizontal scroll bar.
    void set_auto_h_scroll_bar(bool has_auto_h_scroll_bar)
    { _M_has_auto_h_scroll_bar = has_auto_h_scroll_bar; }
    
    /// Returns \c true if the vertical scroll bar is enable, otherwise
    /// \c false.
    bool has_v_scroll_bar() const
    { return _M_has_v_scroll_bar; }

    /// Enables the vertical scroll bar if \p has_scroll_bar is \c true,
    /// otherwise disable the veritical scroll bar.
    void set_v_scroll_bar(bool has_scroll_bar)
    { _M_has_v_scroll_bar = has_scroll_bar; }

    /// Returns \c true if the vertical scroll bar can be automatically
    /// enabled, otherwise \c false.
    bool has_auto_v_scroll_bar() const
    { return _M_has_auto_v_scroll_bar; }

    /// Allows to automatically enable the vertical scroll bar if
    /// \c has_auto_v_scroll_bar is \c true, otherwise denies to automatically
    /// enable the vertical scroll bar.
    void set_auto_v_scroll_bar(bool has_auto_v_scroll_bar)
    { _M_has_auto_v_scroll_bar = has_auto_v_scroll_bar; }

    virtual const char *name() const;
  protected:
    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual void draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds, Styles *style);
  public:
    virtual bool on_touch(const Pointer &pointer, const Point<double> &point, TouchState state);

    virtual void on_touch_leave(const Pointer &pointer);

    virtual bool on_pointer_axis(Axis axis, double value);

    virtual bool on_key(std::uint32_t key_sym, Modifiers modifiers, const char *utf8, KeyState state);
  };

  ///
  /// A menu item is a similar widget to button for a menu.
  ///
  /// The menu item like the button can be clicked by an user and reacts this
  /// clicks.
  ///
  class MenuItem : public virtual Button
  {
  protected:
    /// Default constructor that doesn't invoke the \ref initialize method.
    MenuItem() {}
  public:
    /// Creates a new menu item with a label.
    explicit MenuItem(const std::string &label)
    { initialize(Icon(), label, [](Widget *widget) {}); } 

    /// Creates a new menu item with a label and a specified action on a click.
    MenuItem(const std::string &label, const OnClickListener &listener)
    { initialize(Icon(), label, listener); }
    
    /// Creates a new menu item with an icon and a label.
    MenuItem(const Icon &icon, const std::string &label)
    { initialize(icon, label, [](Widget *widget) {}); } 

    /// Creates a new menu item with an icon, a label, and a specified action
    /// on a click.
    MenuItem(const Icon &icon, const std::string &label, const OnClickListener &listener)
    { initialize(icon, label, listener); }

    /// Destructor.
    virtual ~MenuItem();

    virtual const char *name() const;
  protected:
    virtual void draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds);

    virtual const char *icon_name() const;

    virtual IconSize icon_size() const;

    virtual const char *label_name() const;
  };

  ///
  /// A check menu item is a check box for a menu.
  ///
  class CheckMenuItem : public MenuItem, public CheckBox
  {
  protected:
    /// Default constructor that doesn't invoke the \ref initialize method.
    CheckMenuItem() :
      Button(), MenuItem(), CheckBox(Unused()) {}
  public:
    /// Creates a new check menu item with a label.
    explicit CheckMenuItem(const std::string &label) :
      Button(), MenuItem(), CheckBox(Unused())
    { this->CheckBox::initialize(Icon(), label, false); }

    /// Creates a new check menu item with a label and a specified state.
    CheckMenuItem(const std::string &label, bool is_checked) :
      Button(), MenuItem(), CheckBox(Unused())
    { this->CheckBox::initialize(Icon(), label, is_checked); }

    /// Creates a new check menu item with an icon and a label.
    CheckMenuItem(const Icon &icon, const std::string &label) :
      Button(), MenuItem(), CheckBox(Unused())
    { this->CheckBox::initialize(icon, label, false); }

    /// Creates a new check menu item with an icon, a label, and a specified
    /// state.
    CheckMenuItem(const Icon &icon, const std::string &label, bool is_checked) :
      Button(), MenuItem(), CheckBox(Unused())
    { this->CheckBox::initialize(icon, label, is_checked); }

    /// Destructor.
    virtual ~CheckMenuItem();

    virtual const char *name() const;
  protected:
    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual void draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds);

    virtual const char *icon_name() const;

    virtual IconSize icon_size() const;

    virtual bool can_draw_icon() const;

    virtual const char *label_name() const;

    virtual const char *check_name() const;
  };

  ///
  /// A radio menu item is a radio button for a menu.
  ///
  class RadioMenuItem : public MenuItem, public RadioButton
  {
  protected:
    /// Default constructor that doesn't invoke the \ref initialize method.
    RadioMenuItem() :
      Button(), MenuItem(), RadioButton(Unused()) {}
  public:
    /// Creates a new radio menu item with a label.
    explicit RadioMenuItem(const std::string &label) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(Icon(), label, false, std::shared_ptr<RadioGroup>()); }

    /// Creates a new radio menu item with a label and a specified state.
    RadioMenuItem(const std::string &label, bool is_checked) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(Icon(), label, is_checked, std::shared_ptr<RadioGroup>()); }

    /// Creates a new radio menu item with a label and a specified state.
    RadioMenuItem(const std::string &label, const std::shared_ptr<RadioGroup> &group) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(Icon(), label, false, group); }

    /// Creates a new radio menu item with a label and a specified state that
    /// belongs a specified group.
    RadioMenuItem(const std::string &label, bool is_checked, const std::shared_ptr<RadioGroup> &group) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(Icon(), label, false, group); }

    /// Creates a new radio menu item with an icon and a label.
    RadioMenuItem(const Icon &icon, const std::string &label) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(icon, label, false, std::shared_ptr<RadioGroup>()); }

    /// Creates a new radio menu item with an icon, a label, and a specified
    /// state.
    RadioMenuItem(const Icon &icon, const std::string &label, bool is_checked) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(icon, label, is_checked, std::shared_ptr<RadioGroup>()); }

    /// Creates a new radio menu item with an icon and a label that belongs to a
    /// specified group.
    RadioMenuItem(const Icon &icon, const std::string &label, const std::shared_ptr<RadioGroup> &group) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(icon,label, false, group); }

    /// Creates a new radio menu item with an icon, a label, a specified state
    /// that belongs to a specified group.
    RadioMenuItem(const Icon &icon, const std::string &label, bool is_checked, const std::shared_ptr<RadioGroup> &group) :
      Button(), MenuItem(), RadioButton(Unused())
    { this->RadioButton::initialize(icon, label, false, group); }

    /// Destructor.
    virtual ~RadioMenuItem();

    virtual const char *name() const;
  protected:
    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual void draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds);

    virtual const char *icon_name() const;

    virtual IconSize icon_size() const;

    virtual bool can_draw_icon() const;

    virtual const char *label_name() const;

    virtual const char *check_name() const;
  };

  ///
  /// A separator menu item  is a separator for a menu.
  ///
  class SeparatorMenuItem : public MenuItem
  {
  protected:
    /// Default constructor that doesn't invoke the \ref initialize method.
    SeparatorMenuItem(Unused unused) :
      Button(), MenuItem() {}
  public:
    /// Creates a new sepeperator menu item.
    SeparatorMenuItem() :
      Button(), MenuItem()
    { initialize(Icon(), "", [](Widget *widget) {}); }

    /// Destructor.
    virtual ~SeparatorMenuItem();

    virtual const char *name() const;
  protected:
    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual void draw_content(Canvas *canvas, const Rectangle<int> &inner_bounds, Styles *style);
  };

  ///
  /// A menu is a menu item that contains menu items.
  ///
  /// The menu is a widget that is clickeble and if it is clicked, a popup
  /// surface pops up with menu items. The menu can be added to other menu or a
  /// menu bar. If the menu is added onto other menu, the menu is displayed as a
  /// menu item of other menu.
  ///
  class Menu : public MenuItem
  {
    std::list<std::unique_ptr<MenuItem>> _M_menu_items;
  protected:
    /// Default constructor that doesn't invoke the \ref initialize method.
    Menu() :
      Button(), MenuItem() {}
  public:
    /// Creates a new menu with a lebel.
    explicit Menu(const std::string &label) :
      Button(), MenuItem()
    { initialize(Icon(), label, {}); }

    /// Creates a new menu with a label and menu items.
    Menu(const std::string &label, std::initializer_list<MenuItem *> menu_items) :
      Button(), MenuItem()
    { initialize(Icon(), label, menu_items); }

    /// Creates a new menu with an icon and a label.
    Menu(const Icon &icon, const std::string &label) :
      Button(), MenuItem()
    { initialize(icon, label, {}); }

    /// Creates a new menu with a label, menu items, and menu items.
    Menu(const Icon &icon, const std::string &label, std::initializer_list<MenuItem *> menu_items) :
      Button(), MenuItem()
    { initialize(icon, label, menu_items); }

    /// Destructor.
    virtual ~Menu();
  protected:
    /// Initializes the menu.
    void initialize(const Icon &icon, const std::string &label, std::initializer_list<MenuItem *> menu_items);
  public:
    /// Returns the menu items.
    const std::list<std::unique_ptr<MenuItem>> &menu_items() const
    { return _M_menu_items; }

    /// Adds a new menu item.
    void add_menu_item(MenuItem *menu_item);

    /// Tries delete the menu item and returns \c true if the menu item is
    /// deleted, otherwise \c false.
    bool delete_menu_item(MenuItem *menu_item);

    /// Deletes all menu items.
    void delete_all_menu_items();

    virtual const char *name() const;

    virtual const char *icon_name() const;

    virtual const char *label_name() const;
  };

  ///
  /// A menu bar contains menus. 
  ///
  class MenuBar : public Widget
  {
    std::list<std::unique_ptr<Menu>> _M_menus;
    int _M_menu_height;
  protected:
    /// Constructor that doesn't invoke the \ref initialize method.
    MenuBar(Unused unused) {}
  public:
    /// Creates a new menu bar.
    MenuBar()
    { initialize({}); }

    /// Creates a new menu bar with menus.
    explicit MenuBar(std::initializer_list<Menu *> menus)
    { initialize(menus); }

    /// Destructor.
    virtual ~MenuBar();
  protected:
    /// Initializes the menu bar.
    void initialize(std::initializer_list<Menu *> menus);
  public:
    /// Returns the menus of the menu var.
    const std::list<std::unique_ptr<Menu>> &menus() const
    { return _M_menus; }

    /// Adds a new menu.
    void add_menu(Menu *menu);

    /// Tries delete the menu and returns \c true if the menu is deleted,
    /// otherwise \c false.
    bool delete_menu(Menu *menu);

    /// Deletes all menus.
    void delete_all_menus();

    virtual const char *name() const;
  protected:
    virtual void update_child_points(const Rectangle<int> &area_bounds);

    virtual void update_child_sizes(Canvas *canvas, const Dimension<int> &area_size);

    virtual void update_content_size(Canvas *canvas, const Dimension<int> &area_size);

    virtual void draw_children(Canvas *canvas, const Rectangle<int> &inner_bounds);
  };

  /// Converts an icon size to a dimension.
  Dimension<int> icon_size_to_dimension(IconSize size);
}

#endif
