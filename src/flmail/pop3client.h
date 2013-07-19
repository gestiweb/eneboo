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

#ifndef POP3_CLIENT_H
#define POP3_CLIENT_H

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <qdict.h>
#include <qintdict.h>
#include <qptrlist.h>

class Account;
class POP3Private;

class POP3Client : public QObject
{
  Q_OBJECT

public:

  enum State {
    Idle,
    WaitingForConnection,
    WaitingForSTARTTLS,
    WaitingForAPOPResponse,
    WaitingForUserResponse,
    WaitingForPassResponse,
    WaitingForListResponse,
    WaitingForListItems,
    WaitingForUidlResponse,
    WaitingForUidlItems,
    WaitingForRetrResponse,
    WaitingForRetrData,
    WaitingForDeleResponse
  };

  POP3Client(Account *, QObject *parent = 0, const char *name = 0);
  virtual ~POP3Client();

  virtual void getMail();
  virtual void stop();

  Account *account();

protected slots:

  void slotHostFound();
  void slotConnected();
  void slotConnectionClosed();
  void slotReadyRead();
  void slotBytesWritten(int);
  void slotError(int);
  void slotIdle();
  void slotEmitFinished();

signals:
  void finished(POP3Client *);

protected:

  QString stateToString(State) const;
  void setState(State);

  bool isSingleLine();
  void setSingleLine(bool);

  void resetIdleTimer();

  virtual bool processLine(const QString &);
  virtual bool processMultiLine(const QByteArray &);

  virtual unsigned toMail(const QByteArray &);

  virtual bool replyOk(const QString &reply) const;
  virtual void writeLine(const QString &);
  virtual void retrieveDeleteOrQuit();

  void sendSTARTTLS();
  void sendAPOP(const QString &serverAPOPInfo);
  void sendUser();
  void sendPass();
  void sendList();
  void sendUidl();
  void sendRetr(uint index);
  void sendDele(uint index);
  void sendQuit();

  bool handleConnection(const QString &);
  bool handleSTARTTLS(const QString &);
  bool handleAPOPResponse(const QString &);
  bool handleUserResponse(const QString &);
  bool handlePassResponse(const QString &);
  bool handleListResponse(const QString &);
  bool handleListItem(const QString &);
  bool handleUidlResponse(const QString &);
  bool handleUidlItem(const QString &);
  bool handleRetrResponse(const QString &);
  bool handleRetrData(const QString &);
  bool handleRetrData(const QByteArray &);
  bool handleDeleResponse(const QString &);

private:

  POP3Private *d;

};

#endif // POP3_CLIENT_H
// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
