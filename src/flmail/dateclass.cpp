/*
    Copyright (C) 2001-2002 by theKompany.com <www.thekompany.com>
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    By PLUGINS we mean the commercial plug-ins developed by theKompany.com
    for Aethera.

    In addition, as a special exception, theKompany.com gives permission
    to link the code of this program with PLUGINS (or with
    modified versions of PLUGINS that use the same license as PLUGINS),
    and distribute linked combinations including the two.  You must obey
    the GNU General Public License in all respects for all of the code used
    other than PLUGINS.  If you modify this file, you may extend this
    exception to your version of the file, but you are not obligated to do so.
    If you do not wish to do so, delete this exception statement from your
    version.

    This license grants you the ability to use PLUGINS with Aethera only
    and may not be used outside of Aethera.
    See also http://www.thekompany.com/products/license.txt for details.
*/
/*
 * This file is part of Magellan <http://www.kAlliance.org/Magellan>
 *
 * Copyright (c) 1998-2000 Teodor Mihai <teddy@ireland.com>
 * Copyright (c) 1998-2000 Laur Ivan <laur.ivan@ul.ie>
 * Copyright (c) 1999-2000 Virgil Palanciuc <vv@ulise.cs.pub.ro>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 * Also requires the KDE libraries, available at no cost at
 * http://www.kde.org/
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
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/***************************************************************************
                        dateclass.cpp - rfc822 date class
                             -------------------

  copyright : (C) by theKompany.com     <www.thekompany.com>
              (C) by Atol C&D           <www.atolcd.com>

  authors :   Eugen C.      <eug@thekompany.com>
              Fabien Armand <f.armand@atolcd.com>

 ***************************************************************************/


#include <dateclass.h>
#include <qtextstream.h>

DateClass::DateClass(const QCString &d)
        : day( 0L ),
        month( 0L ),
        year( 0L ),
        hour( 0L ),
        minute( 0L ),
        sec( 0L )
{
    if ( !d.isEmpty() ) {
        int pos;
        QTextStream txt(d, IO_ReadOnly);
        QCString t;
        char c;

        txt>>t;

        // check for the optional day name
        if ((pos=t.find(","))!=-1) {
            day_of_week=t.left(pos);
            txt>>t;
        }

        day=t.toInt();

        txt>>month_of_year;
        txt>>year;

        // Y2K stuff; should work until 2080 :)
        if (year<80)
            year=2000+year;
        else if (year<100)
            year=1900+year;

        txt>>hour;
        txt>>c;
        txt>>minute;
        txt>>c;

        if (c==':') {
            txt>>sec;
            txt>>timezone;
        } else {
            sec=0;
            txt>>timezone;
            timezone.prepend((const char *)(QCString)c);
        }

        // compute the month number
        t=month_of_year.lower();
        if (t=="jan")
            month=1;
        else if (t=="feb")
            month=2;
        else if (t=="mar")
            month=3;
        else if (t=="apr")
            month=4;
        else if (t=="may")
            month=5;
        else if (t=="jun")
            month=6;
        else if (t=="jul")
            month=7;
        else if (t=="aug")
            month=8;
        else if (t=="sep")
            month=9;
        else if (t=="oct")
            month=10;
        else if (t=="nov")
            month=11;
        else if (t=="dec")
            month=12;

        // compute the day name, if necessary
        if (day_of_week.isEmpty()) {
            QDate k(year, month, day);
            day_of_week=intToDay(k.dayOfWeek());
        }
    }
}

DateClass::DateClass(unsigned int date_t)
{
    QDateTime dateTime;
    if (date_t)
        dateTime.setTime_t(date_t);
    else
        dateTime=QDateTime::currentDateTime();

    QDate d=dateTime.date();
    QTime t=dateTime.time();

    day_of_week=intToDay(d.dayOfWeek());
    month_of_year=intToMonth(d.month());
    timezone=""; // oops
    day=d.day();
    month=d.month();
    year=d.year();
    hour=t.hour();
    minute=t.minute();
    sec=t.second();
}

DateClass::DateClass(const QDateTime &d)
{
    day=d.date().day();
    month=d.date().month();
    year=d.date().year();
    hour=d.time().hour();
    minute=d.time().minute();
    sec=d.time().second();
    timezone=""; // oops
    day_of_week=intToDay(d.date().dayOfWeek());
    month_of_year=intToMonth(month);
}

DateClass::~DateClass() {}

DateClass::operator QDate() const
{
    return QDate(year, month, day);
}

QDate DateClass::toQDate() const
{
    return operator QDate();
}

DateClass::operator QTime() const
{
    return QTime(hour, minute, sec);
}

QTime DateClass::toQTime() const
{
    return operator QTime();
}

DateClass::operator QCString() const
{
    QCString t;
    t.sprintf( "%s, %02d %s %04d %02d:%02d",
               (const char *)day_of_week,
               day,
               (const char *)month_of_year,
               year,
               hour,
               minute
             );
    return t;
}

QCString DateClass::toQCString() const
{
    QCString t;
    t.sprintf( "%s, %02d %s %04d %02d:%02d",
               (const char *)day_of_week,
               day,
               (const char *)month_of_year,
               year,
               hour,
               minute
             );
    return t;
}

QCString DateClass::toLocalDateQCString() const
{
    QCString t;
    t.sprintf( "%s, %02d %s %04d %02d:%02d",
               (const char *)QDate::shortDayName(QDate(year,month,day).dayOfWeek()),
               day,
               (const char *)QDate::shortMonthName(month),
               year,
               hour,
               minute
             );
    return t;
}

DateClass::operator QDateTime() const
{
    return QDateTime((QDate)(*this), (QTime)(*this));
}

QDateTime DateClass::toQDateTime() const
{
    return operator QDateTime();
}

QTextStream& operator<<(QTextStream &s, const DateClass &d)
{
    QCString t;
    t.sprintf("%s, %02d %s %04d %02d:%02d:%02d %s", (const char *)d.day_of_week, d.day, (const char *)d.month_of_year, d.year, d.hour,
              d.minute, d.sec, (const char *)d.timezone);
    s<<t;
    return s;
}

QDataStream& operator>>(QDataStream &s, DateClass &d)
{
    s>>d.day_of_week;
    s>>d.month_of_year;
    s>>d.timezone;
    s>>d.day;
    s>>d.month;
    s>>d.year;
    s>>d.hour;
    s>>d.minute;
    s>>d.sec;
    return s;
}

QDataStream& operator<<(QDataStream &s, const DateClass &d)
{
    s<<d.day_of_week;
    s<<d.month_of_year;
    s<<d.timezone;
    s<<d.day;
    s<<d.month;
    s<<d.year;
    s<<d.hour;
    s<<d.minute;
    s<<d.sec;
    return s;
}

QCString DateClass::intToMonth(const int & monthNumber) const
{
    switch (monthNumber) {
    case 1:
        return QCString("Jan");
        break;
    case 2:
        return QCString("Feb");
        break;
    case 3:
        return QCString("Mar");
        break;
    case 4:
        return QCString("Apr");
        break;
    case 5:
        return QCString("May");
        break;
    case 6:
        return QCString("Jun");
        break;
    case 7:
        return QCString("Jul");
        break;
    case 8:
        return QCString("Aug");
        break;
    case 9:
        return QCString("Sep");
        break;
    case 10:
        return QCString("Oct");
        break;
    case 11:
        return QCString("Nov");
        break;
    case 12:
        return QCString("Dec");
        break;
    default:
        return QCString("");
        break;
    }
}

QCString DateClass::intToDay(const int & dayNumber) const
{
    switch (dayNumber) {
    case 1:
        return QCString("Mon");
        break;
    case 2:
        return QCString("Tue");
        break;
    case 3:
        return QCString("Wed");
        break;
    case 4:
        return QCString("Thu");
        break;
    case 5:
        return QCString("Fri");
        break;
    case 6:
        return QCString("Sat");
        break;
    case 7:
        return QCString("Sun");
        break;
    default:
        return QCString("");
        break;
    }
}
