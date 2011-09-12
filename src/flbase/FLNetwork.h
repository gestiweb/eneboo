/***************************************************************************
                           FLNetwork.h
                         -------------------
begin                : vie oct 9 2008
copyright            : (C) 2003-2008 by InfoSiAL, S.L.
email                : mail@infosial.com
***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 ***************************************************************************/
/***************************************************************************
   Este  programa es software libre. Puede redistribuirlo y/o modificarlo
   bajo  los  términos  de  la  Licencia  Pública General de GNU   en  su
   versión 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

#ifndef FLNETWORK_H
#define FLNETWORK_H

#include <qurloperator.h>

class FLNetwork : public QObject
{

  Q_OBJECT

public:

  FLNetwork(const QString &url);

public slots:

  void get(const QString &location);
  void put(const QString &data, const QString &location);
  void copy(const QString &fromLocation, const QString &toLocation);

private slots:

  void slotNetworkStart(QNetworkOperation *);
  void slotNetworkFinished(QNetworkOperation *);
  void slotNetworkData(const QByteArray &, QNetworkOperation *);
  void slotNetworkProgress(int, int, QNetworkOperation *);

signals:

  void finished();
  void start();
  void data(const QString &);
  void dataTransferProgress(int, int);

private:

  QUrlOperator networkOp_;
};

#endif
