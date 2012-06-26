/***************************************************************************
 AQZipReader.h
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

#ifndef AQZIPREADER_H
#define AQZIPREADER_H

#include <qdatetime.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <qvaluelist.h>

class AQZipReaderPrivate;

class AQZipReader
{
public:
  AQZipReader(const QString &fileName, uint mode = IO_ReadOnly);

  explicit AQZipReader(QIODevice *device);
  ~AQZipReader();

  QIODevice *device() const;

  bool isReadable() const;
  bool exists() const;

  struct FileInfo {
    FileInfo();
    FileInfo(const FileInfo &other);
    ~FileInfo();
    FileInfo &operator=(const FileInfo &other);
    bool isValid() const;
    QString filePath;
    uint isDir : 1;
    uint isFile : 1;
    uint isSymLink : 1;
    QFileInfo::PermissionSpec permissions;
    uint crc32;
    Q_UINT64 size;
    QDateTime lastModified;
    void *d;
  };

  QValueList<FileInfo> fileInfoList() const;
  int count() const;

  FileInfo entryInfoAt(int index) const;
  QByteArray fileData(const QString &fileName) const;
  bool extractAll(const QString &destinationDir) const;

  enum Status {
    NoError,
    FileReadError,
    FileOpenError,
    FilePermissionsError,
    FileError
  };

  Status status() const;

  void close();

private:
  AQZipReaderPrivate *d;
  AQZipReader(const AQZipReader &);
  AQZipReader &operator=(const AQZipReader &);
};

#endif // AQZIPREADER_H

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
