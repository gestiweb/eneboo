/***************************************************************************
 AQSSProject_p.h
 -------------------
 begin                : 07/02/2011
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

#ifndef AQSSPROJECT_P_H_
#define AQSSPROJECT_P_H_

#include "AQSObject_p.h"
#include <qsproject.h>
#include <qsinterpreter.h>
#include <qsworkbench.h>
#include <qtimer.h>

class AQSSProject : public AQSObject
{
  Q_OBJECT

  Q_ENUMS(ScriptState)

  Q_PROPERTY(QString entryFunction READ entryFunction)

  AQ_DECLARE_AQS_OBJECT(SProject, Object);

public:
  enum ScriptState {
    New,
    Changed,
    UnChanged
  };

  QString entryFunction() const {
    return entryFunction_;
  }

protected:
  void specializedInternalInit() {
    wb_ = 0;
    if (!o_)
      return;
    QObject::setName(o_->name());
  }

private:
  QString entryFunction_;
  QVariantList scriptInfos_;
  QSWorkbench *wb_;

private slots:
  void callEntryFunction() {
    if (entryFunction_.isEmpty())
      return;
    o_->interpreter()->call(entryFunction_);
    entryFunction_ = QString::null;
  }

  void evaluateScripts() {
    QVariantList::const_iterator it;
    for (it = scriptInfos_.begin(); it != scriptInfos_.end(); ++it) {
      const QVariantList scrInfo((*it).toList());
      if (scrInfo.size() < 4)
        continue;

      QString scrName(scrInfo[0].toString());
      QString scrCode(scrInfo[1].toString());
      ScriptState scrState = (ScriptState) scrInfo[2].toUInt();

      switch (scrState) {
        case New: {
          QSScript *scr = o_->createScript(scrName, scrCode);
          if (scr)
            scr->setFileName(scrInfo[3].toString());
          break;
        }
        case Changed: {
          QSScript *scr = o_->script(scrName);
          if (scr)
            scr->setCode(scrCode);
          break;
        }
      }
    }
    scriptInfos_.clear();
  }

public slots:
  void evaluate(const QString &entryFunction) {
    if (!o_)
      return;
    o_->commitEditorContents();
    entryFunction_ = entryFunction;
    QTimer::singleShot(0, o_, SLOT(evaluate()));
    QTimer::singleShot(0, this, SLOT(callEntryFunction()));
  }

  void evaluateScripts(const QValueList<QVariant> &scriptInfos,
                       const QString &entryFunction = QString::null) {
    if (!o_)
      return;
    scriptInfos_ = scriptInfos;
    QTimer::singleShot(0, this, SLOT(evaluateScripts()));
    evaluate(entryFunction);
  }

  QSWorkbench *workbench() {
    if (!wb_ && o_) {
      wb_ = new QSWorkbench(o_, 0, QObject::name());
      connect(o_, SIGNAL(destroyed()), wb_, SLOT(deleteLater()));
    }
    return wb_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool editorsModified() const;
  bool scriptsModified() const;
  QStringList scriptNames() const;
  QSScript *script(const QString &) const;
  QSScript *script(QObject *) const;
  QObject *object(const QString &) const;
  QObjectList objects() const;
  QSScript *createScript(QObject *, const QString& = QString::null);
  QSScript *createScript(const QString &, const QString& = QString::null);
  QSEditor *activeEditor() const;
  QSEditor *editor(QSScript *) const;
  QSEditor *createEditor(QSScript *, QWidget* = 0, const char* = 0);
  void addObject(QObject *);
  void removeObject(const QObject *);

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSSProject::editorsModified() const
{
  AQ_CALL_RET_V(editorsModified(), bool);
}
inline bool AQSSProject::scriptsModified() const
{
  AQ_CALL_RET_V(scriptsModified(), bool);
}
inline QStringList AQSSProject::scriptNames() const
{
  AQ_CALL_RET_V(scriptNames(), QStringList);
}
inline QSScript *AQSSProject::script(const QString &arg0) const
{
  AQ_CALL_RET(script(arg0));
}
inline QSScript *AQSSProject::script(QObject *arg0) const
{
  AQ_CALL_RET(script(arg0));
}
inline QObject *AQSSProject::object(const QString &arg0) const
{
  AQ_CALL_RET(object(arg0));
}
inline QObjectList AQSSProject::objects() const
{
  AQ_CALL_RET_V(objects(), QObjectList);
}
inline QSScript *AQSSProject::createScript(QObject *arg0,  const QString &arg1)
{
  AQ_CALL_RET(createScript(arg0, arg1));
}
inline QSScript *AQSSProject::createScript(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET(createScript(arg0, arg1));
}
inline QSEditor *AQSSProject::activeEditor() const
{
  AQ_CALL_RET(activeEditor());
}
inline QSEditor *AQSSProject::editor(QSScript *arg0) const
{
  AQ_CALL_RET(editor(arg0));
}
inline QSEditor *AQSSProject::createEditor(QSScript *arg0,  QWidget *arg1,  const char *arg2)
{
  AQ_CALL_RET(createEditor(arg0, arg1, arg2));
}
inline void AQSSProject::addObject(QObject *arg0)
{
  AQ_CALL_VOID(addObject(arg0));
}
inline void AQSSProject::removeObject(const QObject *arg0)
{
  AQ_CALL_VOID(removeObject(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSPROJECT_P_H_ */
// @AQSOBJECT@
