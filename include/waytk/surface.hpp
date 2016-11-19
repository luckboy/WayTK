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
#ifndef _WAYTK_SURFACE_HPP
#define _WAYTK_SURFACE_HPP

#include <functional>
#include <memory>
#include <string>
#include <waytk/structs.hpp>

namespace waytk
{
  class Surface;
  class Widget;

  ///
  /// An enumeration of surface state.
  ///
  enum class SurfaceState
  {
    TOPLEVEL,                   ///< Toplevel surface.
    MAXIMIZED,                  ///< Maximized toplevel surface.
    MINIMIZED,                  ///< Minimized toplevel surface.
    TRANSIENT,                  ///< Transient surface.
    POPUP                       ///< Popup surface.
  };

  ///
  /// A listener type for changes of the surface size.
  ///
  /// \param surface is the surface.
  /// \param size is the surface size.
  ///
  typedef std::function<void (const std::shared_ptr<Surface> &surface, const Dimension<int> &size)> OnSizeChangeListener;

  ///
  /// A listener type for touch cancellations.
  ///
  /// \param surface is the surface.
  ///
  typedef std::function<void (const std::shared_ptr<Surface> &surface)> OnTouchCancelListener;

  ///
  /// A surface class.
  ///
  /// A surface is displayed on the screen and can be resized, maximized, and
  /// minimized by an user. When a size change of the surface or a touch
  /// cancellation occurs, an appropriate listener is invoked. Also, the
  /// listener of size change is invoked if the surface size is changed during
  /// surface maximization. The surface has a root widget that with descendants
  /// are drawn on the surface when the surface is displayed and they can be
  /// displayed. The drawn widgets handle user actions for the keyboard, the
  /// touch screen, and the pointer.
  ///
  class Surface
  {
    std::string _M_title;
    std::unique_ptr<Widget> _M_root_widget;
    bool _M_is_modal;
    SurfaceState _M_state;
    std::shared_ptr<Surface> _M_parent;
    Point<int> _M_point_in_parent;
    bool _M_is_resizable;
    bool _M_is_visible;
    Dimension<int> _M_size;
    OnSizeChangeListener _M_on_size_change_listener;
    OnTouchCancelListener _M_on_touch_cancel_listener;
  public:
    /// Creates a new toplevel surface without a title.
    Surface(Widget *widget);

    /// Creates a new toplevel surface with a title.
    Surface(const std::string &title, Widget *widget);

    /// Creates a new transient surface with a parent, a title, and an initial
    /// top left point in the parent.
    Surface(const std::shared_ptr<Surface> &parent, const std::string &title, Widget *widget, const Point<int> &point);

    /// Creates a new popup surface with a parent surface, a title, and an
    /// initial top left point in the parent surface.
    Surface(const std::shared_ptr<Surface> &parent, Widget *widget, const Point<int> &point);

    /// Destructor.
    virtual ~Surface();

    /// Returns the surface title.
    const std::string &title() const
    { return _M_title; }

    /// Sets the surface title.
    void set_title(const std::string &title)
    { _M_title = title; }

    /// Returns the root widget of the surface.
    Widget *root_widget() const
    { return _M_root_widget.get(); }

    /// Sets the root widget of the surface.
    void set_root_widget(Widget *widget)
    { _M_root_widget = std::unique_ptr<Widget>(widget); }

    ///
    /// Returns \c true if the surface is active, otherwise \c false.
    ///
    /// An active surface is a current surface with the keyboard focus.
    ///
    bool is_active() const;

    /// Returns \c true if the surface is modal, otherwise \c false.
    bool is_modal() const
    { return _M_is_modal; }

    ///
    /// Sets the surface as modal if \p is_modal is \c true, otherwise sets
    /// the surface as non-modal.
    ///
    /// If some surface is modal, other surface can't react on an user actions.
    /// By default, each surface isn't modal.
    ///
    bool set_modal(bool is_modal);

    /// Returns the surface state.
    SurfaceState state() const
    { return _M_state; }

    /// Returns the surface parent.
    const std::shared_ptr<Surface> &parent() const
    { return _M_parent; }

    /// Returns the initial top left point in the surface parent.
    const Point<int> point_in_parent() const
    { return _M_point_in_parent; }

    ///
    /// Sets the surface as a toplevel surface.
    ///
    /// The toplevel surface can be maximized and minimized by an user.
    ///
    void set_toplevel()
    {
      _M_state = SurfaceState::TOPLEVEL;
      _M_parent = std::shared_ptr<Surface>();
      _M_point_in_parent = Point<int>(0, 0);
    }

    /// Sets the surface as a toplevel surface is maximized.
    void set_maximized()
    {
      _M_state = SurfaceState::MAXIMIZED;
      _M_parent = std::shared_ptr<Surface>();
      _M_point_in_parent = Point<int>(0, 0);
    }

    /// Sets the surface as a toplevel surface is minimized.
    void set_minimized()
    {
      _M_state = SurfaceState::MINIMIZED;
      _M_parent = std::shared_ptr<Surface>();
      _M_point_in_parent = Point<int>(0, 0);
    }

    ///
    /// Sets the surface as a transient surface.
    ///
    /// The transient surface can't be maximixed and is minimized with own
    /// minimized parent.
    ///
    void set_transient(const std::shared_ptr<Surface> &parent, const Point<int> &point)
    {
      _M_state = SurfaceState::TRANSIENT;
      _M_parent = parent;
      _M_point_in_parent = point;
    }

    ///
    /// Sets the surface as a popup surface.
    ///
    /// The popup surface can't be maximixed and is minimized with own minimized
    /// parent.
    ///
    void set_popup(const std::shared_ptr<Surface> &parent, const Point<int> &point)
    {
      _M_state = SurfaceState::POPUP;
      _M_parent = parent;
      _M_point_in_parent = point;
    }

    /// Returns \c true if the surface is resizable, otherwise \c false.
    bool is_resizable() const
    { return _M_is_resizable; }

    ///
    /// Sets the surface as resizable if \p is_resizable is \c true, otherwise
    /// sets the surface as unresizable.
    ///
    /// The resizable surface can be resized by an user. In case of the surface
    /// isn't resizable, the user can't change the surface size but an
    /// application can change it. By default, each surface is resizable.
    ///
    void set_resizable(bool is_resizable)
    { _M_is_resizable = is_resizable; }

    /// Returns \c true if the surface is visible, otherwise \c false.
    bool is_visible() const
    { return _M_is_visible; }

    ///
    /// Sets the surface as visible if \p is_visible is \c true, otherwise sets
    /// the surface as invisible.
    ///
    /// If the surface is visible and added, it is displayed on the screen,
    /// otherwise it doesn't display. In case of the surface has a parent, the
    /// parent also have to be visible and added so that the surface is
    /// displayed. By default, each surface is invisible.
    ///
    bool set_visible(bool is_visible);

    /// Returns the surface size.
    const Dimension<int> &size() const
    { return _M_size; }

    ///
    /// Sets the surface size.
    ///
    /// If the surface width and/or the surface height are/is the maximal
    /// integer number, they/it are/is calculated after updating sizes of
    /// widgets. By default, a size of each surface is calculated after
    /// updating sizes of widgets.
    ///
    void set_size(const Dimension<int> &size);

    /// Returns the listener for changes of the surface size.
    const OnSizeChangeListener &on_size_change_listener() const
    { return _M_on_size_change_listener; }

    /// Sets the listener for changes of the surface size.
    void set_on_size_change_listener(const OnSizeChangeListener &listener)
    { _M_on_size_change_listener = listener; }

    /// Returns the listener for touch cancellations.
    const OnTouchCancelListener &on_touch_cancel_listener() const
    { return _M_on_touch_cancel_listener; }

    /// Sets the listener for touch cancellations.
    void set_on_touch_cancel_listener(const OnTouchCancelListener &listener)
    { _M_on_touch_cancel_listener = listener; }

    /// This method is invoked when the surface size is changed.
    virtual void on_size_change(const std::shared_ptr<Surface> &surface, const Dimension<int> &size);

    /// This method is invoked when a touch is canceled.
    virtual void on_touch_cancel(const std::shared_ptr<Surface> &surface);
  };
}

#endif
