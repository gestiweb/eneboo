/***************************************************************************
  qsettingsextra.cpp
  -  Make Settings a bit more useful (save/load more types)

                             -------------------
    begin                : Thu Oct  4 22:59:52 BST 2001
    copyright            : (C) 2001 by theKompany.com
    author               : Rik Hemsley <rik@kde.org>
 ***************************************************************************/

#include <qsettingsextra.h>
#include "settings.h"

namespace QSettingsExtra
{

  // TODO:
  // We should use QFont::toString() and QFont::fromString()
  // May be it's better for future compatibility

    QFont
  readFontEntry
  (
   Settings        * settings,
   const QString    & key,
   const QFont      * def
  )
  {
    QFont f;

    bool ok = false;

#ifndef _QTE_
    QStringList l = settings->readListEntry(key, ',', &ok);
#else
    ok = true;
    QStringList l = settings->readListEntry(key, ',');
#endif

    if (!ok || l.count() != 6)
    {
      if (0 != def)
      {
        return *def;
      }
      else
      {
        return QFont();
      }
    }

    f.setFamily(l[0]);

    f.setPointSize(l[1].toInt());

    f.setStyleHint(QFont::StyleHint(l[2].toUInt()));

    // l[3] is charSet, but we just ignore it, as it's not needed for
    // Qt 3. We expect the entry to exist (may be blank) for compatibility
    // with KConfig.

    f.setWeight(l[4].toUInt());

    uint attributes = l[5].toUInt();

    f.setItalic     (attributes & 0x01);
    f.setUnderline  (attributes & 0x02);
    f.setStrikeOut  (attributes & 0x04);
    f.setFixedPitch (attributes & 0x08);
    f.setRawMode    (attributes & 0x20);

    return f;
  }

    QColor
  readColorEntry
  (
   Settings        * settings,
   const QString    & key,
   const QColor     * def
  )
  {
    QColor c;

    bool ok = false;

#ifndef _QTE_
    QString text = settings->readEntry(key, QString::null, &ok);
#else
    ok = true;
    QString text = settings->readEntry(key, QString::null);
#endif

    if (!ok)
    {
      if (0 != def)
      {
        return *def;
      }
      else
      {
        return QColor();
      }
    }

    if (text[0] == '#')
    {
      c.setNamedColor(text);
      return c;
    }

    QStringList l = QStringList::split(',', text);

    c.setRgb(l[0].toInt(), l[1].toInt(), l[2].toInt());

    return c;
  }

    QValueList<int>
  readIntListEntry
  (
   Settings        * settings,
   const QString    & key,
   const QValueList<int>  * def
  )
  {
    bool ok = false;

#ifndef _QTE_
    QStringList l = settings->readListEntry(key, ',', &ok);
#else
    ok = true;
    QStringList l = settings->readListEntry(key, ',');
#endif

    if (!ok)
    {
      if (0 != def)
      {
        return *def;
      }
      else
      {
        return QValueList<int>();
      }
    }

    QValueList<int> ret;

    for (QStringList::ConstIterator it(l.begin()); it != l.end(); ++it)
      ret << (*it).toInt();

    return ret;
  }

    uint
  readUnsignedNumEntry
  (
   Settings        * settings,
   const QString    & key,
   const uint       def
  )
  {
    return uint(settings->readNumEntry(key, int(def)));
  }

    unsigned long
  readUnsignedLongNumEntry
  (
   Settings            * settings,
   const QString        & key,
   const unsigned long  def
  )
  {
    return (unsigned long)(settings->readNumEntry(key, int(def)));
  }

    QSize
  readSizeEntry
  (
   Settings        * settings,
   const QString    & key,
   const QSize      * def
  )
  {
    bool ok = false;

#ifndef _QTE_
    QStringList l = settings->readListEntry(key, ',', &ok);
#else
    ok = true;
    QStringList l = settings->readListEntry(key, ',');
#endif

    if (!ok || l.count() != 2)
    {
      if (0 != def)
      {
        return *def;
      }
      else
      {
        return QSize();
      }
    }

    return QSize(l[0].toInt(), l[1].toInt());
  }

    QPoint
  readPointEntry
  (
   Settings        * settings,
   const QString    & key,
   const QPoint     * def
  )
  {
    bool ok = false;

#ifndef _QTE_
    QStringList l = settings->readListEntry(key, ',', &ok);
#else
    ok = true;
    QStringList l = settings->readListEntry(key, ',');
#endif

    if (!ok || l.count() != 2)
    {
      if (0 != def)
      {
        return *def;
      }
      else
      {
        return QPoint();
      }
    }

    return QPoint(l[0].toInt(), l[1].toInt());
  }

    QDateTime
  readDateTimeEntry
  (
   Settings        * settings,
   const QString    & key,
   const QDateTime  * def
  )
  {
    bool ok = false;

#ifndef _QTE_
    QStringList l = settings->readListEntry(key, ',', &ok);
#else
    ok = true;
    QStringList l = settings->readListEntry(key, ',');
#endif

    if (!ok || l.count() != 6)
    {
      if (0 != def)
      {
        return *def;
      }
      else
      {
        // Compatible with KConfig.
        return QDateTime::currentDateTime();
      }
    }

    QTime time;
    QDate date;

    date.setYMD
      (
       QString::fromLatin1(l[0]).toInt(),
       QString::fromLatin1(l[1]).toInt(),
       QString::fromLatin1(l[2]).toInt()
      );

    time.setHMS
      (
       QString::fromLatin1(l[3]).toInt(),
       QString::fromLatin1(l[4]).toInt(),
       QString::fromLatin1(l[5]).toInt()
      );

    QDateTime dt;

    dt.setDate(date);
    dt.setTime(time);

    return dt;
  }

    QRect
  readRectEntry
  (
   Settings        * settings,
   const QString    & key,
   const QRect      * def
  )
  {
    bool ok = false;

#ifndef _QTE_
    QStringList l = settings->readListEntry(key, ',', &ok);
#else
    ok = true;
    QStringList l = settings->readListEntry(key, ',');
#endif

    if (!ok || l.count() != 4)
    {
      if (0 != def)
      {
        return *def;
      }
      else
      {
        return QRect();
      }
    }

    return QRect(l[0].toInt(), l[1].toInt(), l[2].toInt(), l[3].toInt());
  }

    bool
  writeEntry
  (
   Settings        * settings,
   const QString    & key,
   const QFont      & f
  )
  {
    uint attributes = 0;

    attributes |= f.italic()      ? 0x01 : 0;
    attributes |= f.underline()   ? 0x02 : 0;
    attributes |= f.strikeOut()   ? 0x04 : 0;
    attributes |= f.fixedPitch()  ? 0x08 : 0;
    attributes |= f.rawMode()     ? 0x20 : 0;

    QStringList l;

    l << f.family()
      << QString::number(f.pointSize())
      << QString::number(uint(f.styleHint()))
      << " " // charSet -> unused in Qt 3, see readFontEntry comment.
      << QString::number(uint(f.weight()))
      << QString::number(attributes)
      ;

#ifndef _QTE_
    return settings->writeEntry(key, l, ',');
#else
    settings->writeEntry(key, l, ',');
    return true; 
#endif
  }

    bool
  writeEntry
  (
   Settings        * settings,
   const QString    & key,
   const QColor     & c
  )
  {
    QStringList l;

    l << QString::number(c.red())
      << QString::number(c.green())
      << QString::number(c.blue())
      ;

#ifndef _QTE_
    return settings->writeEntry(key, l, ',');
#else
    settings->writeEntry(key, l, ',');
    return true;
#endif

  }

    bool
  writeEntry
  (
   Settings        * settings,
   const QString    & key,
   const QValueList<int> & l
  )
  {
    QStringList sl;

    for (QValueList<int>::ConstIterator it(l.begin()); it != l.end(); ++it)
      sl << QString::number(*it);

#ifndef _QTE_
    return settings->writeEntry(key, sl, ',');
#else
    settings->writeEntry(key, sl, ',');
    return true;
#endif
  }

    bool
  writeEntry
  (
   Settings        * settings,
   const QString    & key,
   uint               i
  )
  {
#ifndef _QTE_
    return settings->writeEntry(key, int(i));
#else
    settings->writeEntry(key, int(i));
    return true;
#endif
  }

    bool
  writeEntry
  (
   Settings        * settings,
   const QString    & key,
   unsigned long      i
  )
  {
#ifndef _QTE_
    return settings->writeEntry(key, int(i));
#else
    settings->writeEntry(key, int(i));
    return true;
#endif
  }

    bool
  writeEntry
  (
   Settings        * settings,
   const QString    & key,
   const QSize      & s
  )
  {
    QString pattern("%1,%2");
#ifndef _QTE_
    return settings->writeEntry(key, pattern.arg(s.width()).arg(s.height()));
#else
    settings->writeEntry(key, pattern.arg(s.width()).arg(s.height()));
    return true;
#endif
  }

    bool
  writeEntry
  (
   Settings        * settings,
   const QString    & key,
   const QPoint     & p
  )
  {
    QString pattern("%1/%2");
#ifndef _QTE_
    return settings->writeEntry(key, pattern.arg(p.x()).arg(p.y()));
#else
    settings->writeEntry(key, pattern.arg(p.x()).arg(p.y()));
    return true;
#endif
  }

    bool
  writeEntry
  (
   Settings        * settings,
   const QString    & key,
   const QDateTime  & dt
  )
  {
    // Format: year,month,day,hour,minute,second

    QString pattern("%1,%2,%3,%4,%5,%6");
#ifndef _QTE_
    return settings->writeEntry
      (
       key,
       pattern
       .arg(dt.date().year())
       .arg(dt.date().month())
       .arg(dt.date().day())
       .arg(dt.time().hour())
       .arg(dt.time().minute())
       .arg(dt.time().second())
      );
#else
    settings->writeEntry
      (
       key,
       pattern
       .arg(dt.date().year())
       .arg(dt.date().month())
       .arg(dt.date().day())
       .arg(dt.time().hour())
       .arg(dt.time().minute())
       .arg(dt.time().second())
      );

    return true;
#endif
  }

    bool
  writeEntry
  (
   Settings        * settings,
   const QString    & key,
   const QRect      & r
  )
  {
    QString pattern("%1,%2,%3,%4");
#ifndef _QTE_
    return settings->writeEntry(key, pattern.arg(r.left()).arg(r.top()).arg(r.width()).arg(r.height()));
#else
    settings->writeEntry(key, pattern.arg(r.left()).arg(r.top()).arg(r.width()).arg(r.height()));
    return true; 
#endif
  }

} // End namespace SettingsExtra

// vim:ts=2:sw=2:tw=78:et
