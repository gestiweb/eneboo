/***************************************************************************
 AQSManagerModules_p.h
 -------------------
 begin                : 29/03/2011
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

#ifndef AQSMANAGERMODULES_P_H_
#define AQSMANAGERMODULES_P_H_

#include "AQSByteArray_p.h"
#include "AQObjects.h"

class AQSManagerModules : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(ManagerModules, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QString content(const QString &);
  QString byteCodeToStr(QByteArray *) const;
  QString byteCodeToStr(AQSByteArray *) const;
  QString contentCode(const QString &);
  QString contentCached(const QString &, QString* = 0);
  void setContent(const QString &, const QString &, const QString &);
  QWidget *createUI(const QString &, QObject* = 0, QWidget* = 0, const char* = 0);
  QWidget *createForm(const AQActionMD *, QObject* = 0, QWidget* = 0, const char* = 0);
  QWidget *createFormRecord(const AQActionMD *, QObject* = 0, QWidget* = 0, const char* = 0);
  void setActiveIdModule(const QString &);
  QString activeIdArea() const;
  QString activeIdModule() const;
  QStringList listIdAreas();
  QStringList listIdModules(const QString &);
  QStringList listAllIdModules();
  QString idAreaToDescription(const QString &);
  QString idModuleToDescription(const QString &);
  QPixmap *iconModule(const QString &);
  QString versionModule(const QString &);
  QString shaLocal();
  QString shaGlobal();
  void setShaLocalFromGlobal();
  QString shaOfFile(const QString &);
  void loadKeyFiles();
  void loadAllIdModules();
  void loadIdAreas();
  QString idModuleOfFile(const QString &);
  QString contentFS(const QString &);

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSManagerModules::content(const QString &arg0)
{
  AQ_CALL_RET_V(content(arg0), QString);
}
inline QString AQSManagerModules::byteCodeToStr(QByteArray *arg0) const
{
  AQ_CALL_RET_V(byteCodeToStr(*arg0), QString);
}
inline QString AQSManagerModules::byteCodeToStr(AQSByteArray *arg0) const
{
  AQ_CALL_RET_V(byteCodeToStr(*arg0), QString);
}
inline QString AQSManagerModules::contentCode(const QString &arg0)
{
  AQ_CALL_RET_V(contentCode(arg0), QString);
}
inline QString AQSManagerModules::contentCached(const QString &arg0,  QString *arg1)
{
  AQ_CALL_RET_V(contentCached(arg0, arg1), QString);
}
inline void AQSManagerModules::setContent(const QString &arg0,  const QString &arg1,  const QString &arg2)
{
  AQ_CALL_VOID(setContent(arg0, arg1, arg2));
}
inline QWidget *AQSManagerModules::createUI(const QString &arg0,  QObject *arg1,  QWidget *arg2,  const char *arg3)
{
  AQ_CALL_RET(createUI(arg0, arg1, arg2, arg3));
}
inline QWidget *AQSManagerModules::createForm(const AQActionMD *arg0,  QObject *arg1,  QWidget *arg2,  const char *arg3)
{
  AQ_CALL_RET(createForm(arg0, arg1, arg2, arg3));
}
inline QWidget *AQSManagerModules::createFormRecord(const AQActionMD *arg0,  QObject *arg1,  QWidget *arg2,  const char *arg3)
{
  AQ_CALL_RET(createFormRecord(arg0, arg1, arg2, arg3));
}
inline void AQSManagerModules::setActiveIdModule(const QString &arg0)
{
  AQ_CALL_VOID(setActiveIdModule(arg0));
}
inline QString AQSManagerModules::activeIdArea() const
{
  AQ_CALL_RET_V(activeIdArea(), QString);
}
inline QString AQSManagerModules::activeIdModule() const
{
  AQ_CALL_RET_V(activeIdModule(), QString);
}
inline QStringList AQSManagerModules::listIdAreas()
{
  AQ_CALL_RET_V(listIdAreas(), QStringList);
}
inline QStringList AQSManagerModules::listIdModules(const QString &arg0)
{
  AQ_CALL_RET_V(listIdModules(arg0), QStringList);
}
inline QStringList AQSManagerModules::listAllIdModules()
{
  AQ_CALL_RET_V(listAllIdModules(), QStringList);
}
inline QString AQSManagerModules::idAreaToDescription(const QString &arg0)
{
  AQ_CALL_RET_V(idAreaToDescription(arg0), QString);
}
inline QString AQSManagerModules::idModuleToDescription(const QString &arg0)
{
  AQ_CALL_RET_V(idModuleToDescription(arg0), QString);
}
inline QPixmap *AQSManagerModules::iconModule(const QString &arg0)
{
  AQ_CALL_RET_PTR(iconModule(arg0), QPixmap);
}
inline QString AQSManagerModules::versionModule(const QString &arg0)
{
  AQ_CALL_RET_V(versionModule(arg0), QString);
}
inline QString AQSManagerModules::shaLocal()
{
  AQ_CALL_RET_V(shaLocal(), QString);
}
inline QString AQSManagerModules::shaGlobal()
{
  AQ_CALL_RET_V(shaGlobal(), QString);
}
inline void AQSManagerModules::setShaLocalFromGlobal()
{
  AQ_CALL_VOID(setShaLocalFromGlobal());
}
inline QString AQSManagerModules::shaOfFile(const QString &arg0)
{
  AQ_CALL_RET_V(shaOfFile(arg0), QString);
}
inline void AQSManagerModules::loadKeyFiles()
{
  AQ_CALL_VOID(loadKeyFiles());
}
inline void AQSManagerModules::loadAllIdModules()
{
  AQ_CALL_VOID(loadAllIdModules());
}
inline void AQSManagerModules::loadIdAreas()
{
  AQ_CALL_VOID(loadIdAreas());
}
inline QString AQSManagerModules::idModuleOfFile(const QString &arg0)
{
  AQ_CALL_RET_V(idModuleOfFile(arg0), QString);
}
inline QString AQSManagerModules::contentFS(const QString &arg0)
{
  AQ_CALL_RET_V(contentFS(arg0), QString);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSMANAGERMODULES_P_H_ */
// @AQOBJECT_VOID@
