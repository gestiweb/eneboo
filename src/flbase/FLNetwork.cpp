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

#include "FLNetwork.h"
#include "FLApplication.h"

FLNetwork::FLNetwork(const QString &url)
  : QObject(0, url)
{
  networkOp_ = url;

  connect(&networkOp_, SIGNAL(start(QNetworkOperation *)),
          this, SLOT(slotNetworkStart(QNetworkOperation *)));
  connect(&networkOp_, SIGNAL(finished(QNetworkOperation *)), this,
          SLOT(slotNetworkFinished(QNetworkOperation *)));
  connect(&networkOp_, SIGNAL(data(const QByteArray &, QNetworkOperation *)),
          this, SLOT(slotNetworkData(const QByteArray &, QNetworkOperation *)));
  connect(&networkOp_, SIGNAL(dataTransferProgress(int, int, QNetworkOperation *)),
          this, SLOT(slotNetworkProgress(int, int, QNetworkOperation *)));
}

void FLNetwork::get(const QString &location)
{
  networkOp_.get(location)->state();
}

void FLNetwork::put(const QString &data, const QString &location)
{
  networkOp_.put(data.utf8(), location);
}

void FLNetwork::copy(const QString &fromLocation, const QString &toLocation)
{
  networkOp_.copy(fromLocation, toLocation);
}

void FLNetwork::slotNetworkStart(QNetworkOperation *)
{
  emit start();
}

void FLNetwork::slotNetworkFinished(QNetworkOperation *)
{
  emit finished();
}

void FLNetwork::slotNetworkData(const QByteArray &bytes, QNetworkOperation *)
{
  QString buffer = bytes;
  emit data(buffer);
}

void FLNetwork::slotNetworkProgress(int bytesDone, int bytesTotal, QNetworkOperation *)
{
  emit dataTransferProgress(bytesDone, bytesTotal);
}
