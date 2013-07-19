//
// C++ Interface: tkcsocket
//
// Description:
//
//
// Author: Eugen Constantinescu <eug@thekompany.com>
//
// Copyright: theKompany.com, (C) 2004
//
//
#ifndef SSLSOCKET_H
#define SSLSOCKET_H

/****************************************************************************
** $Id: sslsocket.h,v 1.11 2004/10/16 07:09:05 eug Exp $
**
** Definition of SSLSocket class.
**
** Created : 970521
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the network module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "ssldevice.h"

#include <qsocket.h>
#include <qhostaddress.h> // int->QHostAddress conversion

#define _TKCSSL_

// #define OWN_QSOCKET
// #ifdef OWN_QSOCKET
// namespace tkcSSL
// {
// #endif

class SSLSocketPrivate;
class QSocketDevice;

class TKCSSL_API SSLSocket : public QSocket
{
  Q_OBJECT
public:

  SSLSocket(QObject *parent = 0, const char *name = 0);
  virtual ~SSLSocket();

  State state() const;

  int     socket() const;
  virtual void setSocket(int);

  QSocketDevice *socketDevice();
  virtual void setSocketDevice(QSocketDevice *);
#ifdef _TKCSSL_
  virtual void setSocketDevice(QSocketDevice * , bool);
  virtual bool startTLS();
#endif

  virtual void connectToHost(const QString &host, Q_UINT16 port);
  QString   peerName() const;

  // Implementation of QIODevice abstract virtual functions
  bool   open(int mode);
  void   close();
  void   flush();
#if QT_VERSION < 300
  uint   size() const;
#else
  Offset size() const;
#endif
  Offset at() const;
  bool   at(Offset);
  bool   atEnd() const;

  Q_ULONG   bytesAvailable() const; // ### QIODevice::Offset instead?
  Q_ULONG   waitForMore(int msecs, bool *timeout) const;
  Q_ULONG   waitForMore(int msecs) const;   // ### Qt 4.0: merge the two overloads
  Q_ULONG   bytesToWrite() const;
  void   clearPendingData();

  Q_LONG   readBlock(char *data, Q_ULONG maxlen);
  Q_LONG   writeBlock(const char *data, Q_ULONG len);
  Q_LONG   readLine(char *data, Q_ULONG maxlen);

  int     getch();
  int     putch(int);
  int     ungetch(int);

  bool   canReadLine() const;
  virtual   QString readLine();

  Q_UINT16   port() const;
  Q_UINT16   peerPort() const;
  QHostAddress address() const;
  QHostAddress peerAddress() const;

  void   setReadBufferSize(Q_ULONG);
  Q_ULONG   readBufferSize() const;

protected slots:
#if QT_VERSION < 300
  virtual void sn_read();
  virtual void sn_read(bool force);
#else
  virtual void sn_read(bool force = FALSE);
#endif
  virtual void sn_write();

private slots:
  void  tryConnecting();
  void  emitErrorConnectionRefused();

  // #ifdef OWN_QSOCKET
  // signals:
  //     void   hostFound();
  //     void   connected();
  //     void   connectionClosed();
  //     void   delayedCloseFinished();
  //     void   readyRead();
  //     void   bytesWritten( int nbytes );
  //     void   error( int );
  // #endif

private:
  SSLSocketPrivate *d;

  bool   consumeWriteBuf(Q_ULONG nbytes);
  void   tryConnection();
  void         setSocketIntern(int socket);

private:  // Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
  SSLSocket(const SSLSocket &);
  SSLSocket &operator=(const SSLSocket &);
#endif
};

// #ifdef OWN_QSOCKET
// }
// #endif

#endif
