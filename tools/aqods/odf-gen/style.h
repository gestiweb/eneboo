/*
odf-gen: Simple API to generate OpenDocument documents.
    Copyright (C) 2009  Pablo Jorge, FuDePAN

    This file is part of the odf-gen project.

    odf-gen is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    odf-gen is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with odf-gen.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef STYLE_H
#define STYLE_H

#include <iostream>

class Style
{
public:
  enum StyleFlags {
    NONE           = 0,
    BORDER_BOTTOM  = (1 <<  0),
    BORDER_LEFT    = (1 <<  1),
    BORDER_RIGHT   = (1 <<  2),
    BORDER_TOP     = (1 <<  3),
    ALIGN_LEFT     = (1 <<  4),
    ALIGN_CENTER   = (1 <<  5),
    ALIGN_RIGHT    = (1 <<  6),
    TEXT_BOLD      = (1 <<  7),
    TEXT_ITALIC    = (1 <<  8),
    TEXT_UNDERLINE = (1 <<  9),
  };

  Style(StyleFlags flags = NONE)
    : _flags(flags)
  {}

  Style &operator = (StyleFlags flags) {
    _flags = flags;
    return *this;
  }

  Style &operator |= (const Style &other) {
    _flags = (Style::StyleFlags)(_flags | other._flags);
    return *this;
  }

  bool operator !() const {
    return !_flags;
  }

  bool operator & (StyleFlags flags) const {
    return (_flags & flags);
  }

  std::ostream &operator << (std::ostream &ostream) const {
    if (this->operator & (BORDER_BOTTOM))
      ostream << "border-bottom,";
    if (this->operator & (BORDER_LEFT))
      ostream << "border-left,";
    if (this->operator & (BORDER_RIGHT))
      ostream << "border-right,";
    if (this->operator & (BORDER_TOP))
      ostream << "border-top,";
    if (this->operator & (ALIGN_LEFT))
      ostream << "align-left,";
    if (this->operator & (ALIGN_CENTER))
      ostream << "align-center,";
    if (this->operator & (ALIGN_RIGHT))
      ostream << "align-right,";
    if (this->operator & (TEXT_BOLD))
      ostream << "text-bold,";
    if (this->operator & (TEXT_ITALIC))
      ostream << "text-italic,";
    if (this->operator & (TEXT_UNDERLINE))
      ostream << "text-underline,";

    return ostream;
  }

private:
  StyleFlags _flags;
};

inline
std::ostream &operator << (std::ostream &ostream,
                           const Style &style)
{
  return style.operator << (ostream);
}

#endif // STYLE_H
