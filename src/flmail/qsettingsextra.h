/***************************************************************************
  qsettingsextra.h
  -  Make Settings a bit more useful (save/load more types)

                             -------------------
    begin                : Thu Oct  4 22:59:52 BST 2001
    copyright            : (C) 2001 by theKompany.com
    author               : Rik Hemsley <rik@kde.org>
 ***************************************************************************/

#ifndef QSETTINGS_EXTRA
#define QSETTINGS_EXTRA

class Settings;

#include <qfont.h>
#include <qcolor.h>
#include <qvaluelist.h>
#include <qsize.h>
#include <qpoint.h>
#include <qdatetime.h>
#include <qstring.h>
#include <qrect.h>

#include <defines.h>

#ifdef Q_WS_WIN
#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class TKCBASE_API QValueList<int>;
// MOC_SKIP_END
#endif
#endif

namespace QSettingsExtra
{
  TKCBASE_API
  QFont readFontEntry
    (
     Settings        *,
     const QString    & key,
     const QFont      * def = 0
    );

  TKCBASE_API
  QColor readColorEntry
    (
     Settings        *,
     const QString    & key,
     const QColor     * def = 0
    );

  TKCBASE_API
  QValueList<int> readIntListEntry
    (
     Settings        *,
     const QString    & key,
     const QValueList<int>  * def = 0
    );

  TKCBASE_API
  uint readUnsignedNumEntry
    (
     Settings        *,
     const QString    & key,
     const uint       def = 0
    );

  TKCBASE_API
  unsigned long readUnsignedLongNumEntry
    (
     Settings            *,
     const QString        & key,
     const unsigned long  def = 0
    );

  TKCBASE_API
  QSize readSizeEntry
    (
     Settings        *,
     const QString    & key,
     const QSize      * def = 0
    );

  TKCBASE_API
  QPoint readPointEntry
    (
     Settings        *,
     const QString    & key,
     const QPoint     * def = 0
    );

  TKCBASE_API
  QDateTime readDateTimeEntry
    (
     Settings        *,
     const QString    & key,
     const QDateTime  * def = 0
    );

  TKCBASE_API
  QRect readRectEntry
    (
     Settings        *,
     const QString    & key,
     const QRect  * def = 0
    );

  TKCBASE_API
  bool writeEntry
    (
     Settings        *,
     const QString    & key,
     const QFont      &
    );

  TKCBASE_API
  bool writeEntry
    (
     Settings        *,
     const QString    & key,
     const QColor     &
    );

  TKCBASE_API
  bool writeEntry
    (
     Settings        *,
     const QString    & key,
     const QValueList<int>  &
    );

  TKCBASE_API
  bool writeEntry
    (
     Settings        *,
     const QString    & key,
     uint
    );

  TKCBASE_API
  bool writeEntry
    (
     Settings        *,
     const QString    & key,
     unsigned long
    );

  TKCBASE_API
  bool writeEntry
    (
     Settings        *,
     const QString    & key,
     const QSize      &
    );

  TKCBASE_API
  bool writeEntry
    (
     Settings        *,
     const QString    & key,
     const QPoint     &
    );

  TKCBASE_API
  bool writeEntry
    (
     Settings        *,
     const QString    & key,
     const QDateTime  &
    );

  TKCBASE_API
  bool writeEntry
    (
     Settings        *,
     const QString    & key,
     const QRect      &
    );

} // End namespace QSettingsExtra

#endif // QSETTINGS_EXTRA
// vim:ts=2:sw=2:tw=78:et
