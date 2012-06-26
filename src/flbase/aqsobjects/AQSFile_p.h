/***************************************************************************
 AQSFile_p.h
 -------------------
 begin                : 05/11/2011
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

#ifndef AQSFILE_P_H_
#define AQSFILE_P_H_

#include "AQSIODevice_p.h"
#include <qfile.h>

class AQSFile : public AQSIODevice
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(File, IODevice);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

public slots:
  QIODevice *ioDevice() const {
    return static_cast<QIODevice *>(o_);
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QString name() const;
  void setName(const QString &);
  bool exists() const;
  bool remove();
  virtual bool open(int);
  bool open(int, int);
  virtual void close();
  virtual void flush();
  virtual uint size() const;
  virtual bool atEnd() const;
  virtual long readLine(char *, ulong);
  long readLine(QString &, ulong);
  virtual int getch();
  virtual int putch(int);
  virtual int ungetch(int);
  int handle() const;
  QString errorString() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QFile;
    if (sgt == QString::fromLatin1("QString"))
      return new QFile(*(argValue<QString *>(args[0])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSFile::name() const
{
  AQ_CALL_RET_V(name(), QString);
}
inline void AQSFile::setName(const QString &arg0)
{
  AQ_CALL_VOID(setName(arg0));
}
inline bool AQSFile::exists() const
{
  AQ_CALL_RET_V(exists(), bool);
}
inline bool AQSFile::remove()
{
  AQ_CALL_RET_V(remove(), bool);
}
inline bool AQSFile::open(int arg0)
{
  AQ_CALL_RET_V(open(arg0), bool);
}
inline bool AQSFile::open(int arg0,  int arg1)
{
  AQ_CALL_RET_V(open(arg0, arg1), bool);
}
inline void AQSFile::close()
{
  AQ_CALL_VOID(close());
}
inline void AQSFile::flush()
{
  AQ_CALL_VOID(flush());
}
inline uint AQSFile::size() const
{
  AQ_CALL_RET_V(size(), uint);
}
inline bool AQSFile::atEnd() const
{
  AQ_CALL_RET_V(atEnd(), bool);
}
inline long AQSFile::readLine(char *arg0,  ulong arg1)
{
  AQ_CALL_RET_V(readLine(arg0, arg1), long);
}
inline long AQSFile::readLine(QString &arg0,  ulong arg1)
{
  AQ_CALL_RET_V(readLine(arg0, arg1), long);
}
inline int AQSFile::getch()
{
  AQ_CALL_RET_V(getch(), int);
}
inline int AQSFile::putch(int arg0)
{
  AQ_CALL_RET_V(putch(arg0), int);
}
inline int AQSFile::ungetch(int arg0)
{
  AQ_CALL_RET_V(ungetch(arg0), int);
}
inline int AQSFile::handle() const
{
  AQ_CALL_RET_V(handle(), int);
}
inline QString AQSFile::errorString() const
{
  AQ_CALL_RET_V(errorString(), QString);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSFILE_P_H_ */
// @AQSVOIDPTR@
