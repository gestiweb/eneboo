/***************************************************************************
 AQSDir_p.h
 -------------------
 begin                : 20/11/2011
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

#ifndef AQSDIR_P_H_
#define AQSDIR_P_H_

#include "AQSVoidPtr_p.h"
#include <qdir.h>

// @AQ_PTR_INDIRECTION@

class AQSDir : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(Dir, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual void setPath(const QString &);
  virtual QString path() const;
  virtual QString absPath() const;
  virtual QString canonicalPath() const;
  virtual QString dirName() const;
  virtual QString filePath(const QString &, bool = TRUE) const;
  virtual QString absFilePath(const QString &, bool = TRUE) const;
  virtual bool cd(const QString &, bool = TRUE);
  virtual bool cdUp();
  QString nameFilter() const;
  virtual void setNameFilter(const QString &);
  uint filter() const;
  virtual void setFilter(int);
  uint sorting() const;
  virtual void setSorting(int);
  bool matchAllDirs() const;
  virtual void setMatchAllDirs(bool);
  uint count() const;
  virtual QStringList entryList(int = QDir::DefaultFilter, int = QDir::DefaultSort) const;
  virtual QStringList entryList(const QString &, int = QDir::DefaultFilter, int = QDir::DefaultSort) const;
  virtual bool mkdir(const QString &, bool = TRUE) const;
  virtual bool rmdir(const QString &, bool = TRUE) const;
  virtual bool mkpath(const QString &) const;
  virtual bool rmpath(const QString &) const;
  virtual bool isReadable() const;
  virtual bool exists() const;
  virtual bool isRoot() const;
  virtual bool isRelative() const;
  virtual void convertToAbs();
  virtual bool remove(const QString &, bool = TRUE);
  virtual bool rename(const QString &, const QString &, bool = TRUE);
  virtual bool exists(const QString &, bool = TRUE);
  void refresh() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QString"));
    candidates[1].append(QString::fromLatin1("QDir*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDir;
    if (sgt == QString::fromLatin1("QString"))
      return new QDir(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,QString"))
      return new QDir(*(argValue<QString *>(args[0])),
                      *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QDir*"))
      return new QDir(*argValue<QDir *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QString"));
    candidates[1].append(QString::fromLatin1("QDir*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSDir::setPath(const QString &arg0)
{
  AQ_CALL_VOID(setPath(arg0));
}
inline QString AQSDir::path() const
{
  AQ_CALL_RET_V(path(), QString);
}
inline QString AQSDir::absPath() const
{
  AQ_CALL_RET_V(absPath(), QString);
}
inline QString AQSDir::canonicalPath() const
{
  AQ_CALL_RET_V(canonicalPath(), QString);
}
inline QString AQSDir::dirName() const
{
  AQ_CALL_RET_V(dirName(), QString);
}
inline QString AQSDir::filePath(const QString &arg0,  bool arg1) const
{
  AQ_CALL_RET_V(filePath(arg0, arg1), QString);
}
inline QString AQSDir::absFilePath(const QString &arg0,  bool arg1) const
{
  AQ_CALL_RET_V(absFilePath(arg0, arg1), QString);
}
inline bool AQSDir::cd(const QString &arg0,  bool arg1)
{
  AQ_CALL_RET_V(cd(arg0, arg1), bool);
}
inline bool AQSDir::cdUp()
{
  AQ_CALL_RET_V(cdUp(), bool);
}
inline QString AQSDir::nameFilter() const
{
  AQ_CALL_RET_V(nameFilter(), QString);
}
inline void AQSDir::setNameFilter(const QString &arg0)
{
  AQ_CALL_VOID(setNameFilter(arg0));
}
inline uint AQSDir::filter() const
{
  AQ_CALL_RET_V(filter(), uint);
}
inline void AQSDir::setFilter(int arg0)
{
  AQ_CALL_VOID(setFilter(arg0));
}
inline uint AQSDir::sorting() const
{
  AQ_CALL_RET_V(sorting(), uint);
}
inline void AQSDir::setSorting(int arg0)
{
  AQ_CALL_VOID(setSorting(arg0));
}
inline bool AQSDir::matchAllDirs() const
{
  AQ_CALL_RET_V(matchAllDirs(), bool);
}
inline void AQSDir::setMatchAllDirs(bool arg0)
{
  AQ_CALL_VOID(setMatchAllDirs(arg0));
}
inline uint AQSDir::count() const
{
  AQ_CALL_RET_V(count(), uint);
}
inline QStringList AQSDir::entryList(int arg0,  int arg1) const
{
  AQ_CALL_RET_V(entryList(arg0, arg1), QStringList);
}
inline QStringList AQSDir::entryList(const QString &arg0,  int arg1,  int arg2) const
{
  AQ_CALL_RET_V(entryList(arg0, arg1, arg2), QStringList);
}
inline bool AQSDir::mkdir(const QString &arg0,  bool arg1) const
{
  AQ_CALL_RET_V(mkdir(arg0, arg1), bool);
}
inline bool AQSDir::rmdir(const QString &arg0,  bool arg1) const
{
  AQ_CALL_RET_V(rmdir(arg0, arg1), bool);
}
inline bool AQSDir::mkpath(const QString &arg0) const
{
  AQ_CALL_RET_V(mkpath(arg0), bool);
}
inline bool AQSDir::rmpath(const QString &arg0) const
{
  AQ_CALL_RET_V(rmpath(arg0), bool);
}
inline bool AQSDir::isReadable() const
{
  AQ_CALL_RET_V(isReadable(), bool);
}
inline bool AQSDir::exists() const
{
  AQ_CALL_RET_V(exists(), bool);
}
inline bool AQSDir::isRoot() const
{
  AQ_CALL_RET_V(isRoot(), bool);
}
inline bool AQSDir::isRelative() const
{
  AQ_CALL_RET_V(isRelative(), bool);
}
inline void AQSDir::convertToAbs()
{
  AQ_CALL_VOID(convertToAbs());
}
inline bool AQSDir::remove(const QString &arg0,  bool arg1)
{
  AQ_CALL_RET_V(remove(arg0, arg1), bool);
}
inline bool AQSDir::rename(const QString &arg0,  const QString &arg1,  bool arg2)
{
  AQ_CALL_RET_V(rename(arg0, arg1, arg2), bool);
}
inline bool AQSDir::exists(const QString &arg0,  bool arg1)
{
  AQ_CALL_RET_V(exists(arg0, arg1), bool);
}
inline void AQSDir::refresh() const
{
  AQ_CALL_VOID(refresh());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDIR_P_H_ */
// @AQSVOIDPTR@
