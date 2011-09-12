/***************************************************************************
 AQSSEditor_p.h
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

#ifndef AQSSEDITOR_P_H_
#define AQSSEDITOR_P_H_

#include "AQSWidget_p.h"
#include <qseditor.h>

class AQSSEditor : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(SEditor, Widget);

protected:
  static void *specializedConstruct(void *co) {
    if (co)
      static_cast<QSEditor *>(co)->setInterpreter(globalAQSInterpreter);
    return co;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setText(const QString &);
  QString text() const;
  bool isModified() const;
  void commit();
  void revert();
  bool isActive() const;
  bool isUndoAvailable() const;
  bool isRedoAvailable() const;
  void undo();
  void redo();
  void cut();
  void copy();
  void paste();
  void selectAll();
  bool find(const QString &, bool, bool, bool, bool);
  bool replace(const QString &, const QString &, bool, bool, bool, bool, bool);
  void gotoLine(int);
  void indent();
  void readSettings();
  void setErrorMark(int);
  void removeErrorMark();
  QTextEdit *textEdit() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QSEditor(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QSEditor(argValue<QWidget *>(args[0]),
                          *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSSEditor::setText(const QString &arg0)
{
  AQ_CALL_VOID(setText(arg0));
}
inline QString AQSSEditor::text() const
{
  AQ_CALL_RET_V(text(), QString);
}
inline bool AQSSEditor::isModified() const
{
  AQ_CALL_RET_V(isModified(), bool);
}
inline void AQSSEditor::commit()
{
  AQ_CALL_VOID(commit());
}
inline void AQSSEditor::revert()
{
  AQ_CALL_VOID(revert());
}
inline bool AQSSEditor::isActive() const
{
  AQ_CALL_RET_V(isActive(), bool);
}
inline bool AQSSEditor::isUndoAvailable() const
{
  AQ_CALL_RET_V(isUndoAvailable(), bool);
}
inline bool AQSSEditor::isRedoAvailable() const
{
  AQ_CALL_RET_V(isRedoAvailable(), bool);
}
inline void AQSSEditor::undo()
{
  AQ_CALL_VOID(undo());
}
inline void AQSSEditor::redo()
{
  AQ_CALL_VOID(redo());
}
inline void AQSSEditor::cut()
{
  AQ_CALL_VOID(cut());
}
inline void AQSSEditor::copy()
{
  AQ_CALL_VOID(copy());
}
inline void AQSSEditor::paste()
{
  AQ_CALL_VOID(paste());
}
inline void AQSSEditor::selectAll()
{
  AQ_CALL_VOID(selectAll());
}
inline bool AQSSEditor::find(const QString &arg0,  bool arg1,  bool arg2,  bool arg3,  bool arg4)
{
  AQ_CALL_RET_V(find(arg0, arg1, arg2, arg3, arg4), bool);
}
inline bool AQSSEditor::replace(const QString &arg0,  const QString &arg1,  bool arg2,  bool arg3,  bool arg4,  bool arg5,  bool arg6)
{
  AQ_CALL_RET_V(replace(arg0, arg1, arg2, arg3, arg4, arg5, arg6), bool);
}
inline void AQSSEditor::gotoLine(int arg0)
{
  AQ_CALL_VOID(gotoLine(arg0));
}
inline void AQSSEditor::indent()
{
  AQ_CALL_VOID(indent());
}
inline void AQSSEditor::readSettings()
{
  AQ_CALL_VOID(readSettings());
}
inline void AQSSEditor::setErrorMark(int arg0)
{
  AQ_CALL_VOID(setErrorMark(arg0));
}
inline void AQSSEditor::removeErrorMark()
{
  AQ_CALL_VOID(removeErrorMark());
}
inline QTextEdit *AQSSEditor::textEdit() const
{
  AQ_CALL_RET(textEdit());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSEDITOR_P_H_ */
// @AQSWIDGET@
