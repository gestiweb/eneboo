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

#include <messagefactory.h>
#include <qcache.h>
#include <indexclass.h>
#include <messagedevice.h>
#include <mailfolder.h>

MessageFactory *MessageFactory::_instance = 0;

MessageFactory *MessageFactory::thisInstance()
{
    return _instance?_instance:_instance=new MessageFactory();
}

MessageFactory::MessageFactory():_messageCache(1000, 1009)
{
    _messageCache.setAutoDelete(true);
}

MessageDevice *MessageFactory::getMessage(IndexClass *index)
{
    Q_ASSERT(0 != index);

    MessageDevice *dev=_messageCache[index->getID()];

    if (!dev) {
        dev=new MessageDevice(index);
        _messageCache.insert(index->getID(), dev);
    }

    return dev;
}

void MessageFactory::releaseMessage(const QString &indexID)
{
    _messageCache.remove(indexID);
}
