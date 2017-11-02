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
#ifndef _WAYTK_MODIFIERS_HPP
#define _WAYTK_MODIFIERS_HPP

namespace waytk
{
  ///
  /// An enumeration of key modifiers.
  ///
  enum class Modifiers
  {
    NONE = 0,                   ///< No key modifiers.
    SHIFT = 1,                  ///< Shift key modifier.
    CAPS = 2,                   ///< Caps key modifier.
    CTRL = 4,                   ///< Ctrl key modifier.
    ALT = 8,                    ///< Alt key modifier.
    NUM = 16,                   ///< Num Lock key modifier.
    LOGO = 32                   ///< Logo key modifier.
  };

  /// Returns negation of \p modifiers.
  inline Modifiers operator~(Modifiers modifiers)
  { return static_cast<Modifiers>(static_cast<int>(modifiers) ^ 63); }

  /// Returns conjuction of \p modifiers1 with \p modifiers2.
  inline Modifiers operator&(Modifiers modifiers1, Modifiers modifiers2)
  { return static_cast<Modifiers>(static_cast<int>(modifiers1) & static_cast<int>(modifiers2)); }

  /// Assigns conjuction of \p modifiers1 with \p modifiers2 to \p modifiers1.
  inline Modifiers operator&=(Modifiers &modifiers1, Modifiers modifiers2)
  { modifiers1 = modifiers1 & modifiers2; return modifiers1; }

  /// Returns disjuction of \p modifiers1 with \p modifiers2.
  inline Modifiers operator|(Modifiers modifiers1, Modifiers modifiers2)
  { return static_cast<Modifiers>(static_cast<int>(modifiers1) | static_cast<int>(modifiers2)); }

  /// Assigns disjuction of \p modifiers1 with \p modifiers2 to \p modifiers1.
  inline Modifiers operator|=(Modifiers &modifiers1, Modifiers modifiers2)
  { modifiers1 = modifiers1 | modifiers2; return modifiers1; }

  /// Returns exclusive disjuction of \p modifiers1 with \p modifiers2.
  inline Modifiers operator^(Modifiers modifiers1, Modifiers modifiers2)
  { return static_cast<Modifiers>(static_cast<int>(modifiers1) ^ static_cast<int>(modifiers2)); }

  /// Assigns exclusive disjuction of \p modifiers1 with \p modifiers2 to
  /// \p modifiers1.
  inline Modifiers operator^=(Modifiers &modifiers1, Modifiers modifiers2)
  { modifiers1 = modifiers1 ^ modifiers2; return modifiers1; }
}

#endif
