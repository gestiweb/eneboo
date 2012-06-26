/***************************************************************************
 AQZipWriter.h
 -------------------
 begin                : 12/12/2011
 copyright            : (C) 2003-2011 by InfoSiAL S.L.
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

#ifndef AQZIPWRITER_H
#define AQZIPWRITER_H

#include <qstring.h>
#include <qfile.h>
#include <qfileinfo.h>

class AQZipWriterPrivate;

class AQZipWriter
{
public:
  AQZipWriter(const QString &fileName, uint mode = (IO_WriteOnly | IO_Truncate));

  explicit AQZipWriter(QIODevice *device);
  ~AQZipWriter();

  QIODevice *device() const;

  bool isWritable() const;
  bool exists() const;

  enum Status {
    NoError,
    FileWriteError,
    FileOpenError,
    FilePermissionsError,
    FileError
  };

  Status status() const;

  enum CompressionPolicy {
    AlwaysCompress,
    NeverCompress,
    AutoCompress
  };

  void setCompressionPolicy(CompressionPolicy policy);
  CompressionPolicy compressionPolicy() const;

  void setCreationPermissions(QFileInfo::PermissionSpec permissions);
  QFileInfo::PermissionSpec creationPermissions() const;

  void addFile(const QString &fileName, const QByteArray &data);

  void addFile(const QString &fileName, QIODevice *device);

  void addDirectory(const QString &dirName);

  void addSymLink(const QString &fileName, const QString &destination);

  void close();
private:
  AQZipWriterPrivate *d;
  AQZipWriter(const AQZipWriter &);
  AQZipWriter &operator=(const AQZipWriter &);
};

#endif // AQZIPWRITER_H

/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

