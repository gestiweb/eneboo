/***************************************************************************
 AQSLineEdit_p.h
 -------------------
 begin                : 30/01/2011
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

#ifndef AQSLINEEDIT_P_H_
#define AQSLINEEDIT_P_H_

#include "AQSFrame_p.h"
#include <qlineedit.h>

class AQSLineEdit : public AQSFrame
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(LineEdit, Frame);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void cursorForward(bool, int = 1);
  void cursorBackward(bool, int = 1);
  void cursorWordForward(bool);
  void cursorWordBackward(bool);
  void backspace();
  void del();
  void home(bool);
  void end(bool);
  int selectionStart() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QString,QString,QWidget*"));
    candidates[4].append(QString::fromLatin1("QString,QString,QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QLineEdit(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QLineEdit(argValue<QWidget *>(args[0]),
                           *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QString,QWidget*"))
      return new QLineEdit(*(argValue<QString *>(args[0])),
                           argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("QString,QWidget*,QString"))
      return new QLineEdit(*(argValue<QString *>(args[0])),
                           argValue<QWidget *>(args[1]),
                           *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QString,QString,QWidget*"))
      return new QLineEdit(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           argValue<QWidget *>(args[2]));
    if (sgt == QString::fromLatin1("QString,QString,QWidget*,QString"))
      return new QLineEdit(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           argValue<QWidget *>(args[2]),
                           *(argValue<QString *>(args[3])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QString,QString,QWidget*"));
    candidates[4].append(QString::fromLatin1("QString,QString,QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSLineEdit::cursorForward(bool arg0,  int arg1)
{
  AQ_CALL_VOID(cursorForward(arg0, arg1));
}
inline void AQSLineEdit::cursorBackward(bool arg0,  int arg1)
{
  AQ_CALL_VOID(cursorBackward(arg0, arg1));
}
inline void AQSLineEdit::cursorWordForward(bool arg0)
{
  AQ_CALL_VOID(cursorWordForward(arg0));
}
inline void AQSLineEdit::cursorWordBackward(bool arg0)
{
  AQ_CALL_VOID(cursorWordBackward(arg0));
}
inline void AQSLineEdit::backspace()
{
  AQ_CALL_VOID(backspace());
}
inline void AQSLineEdit::del()
{
  AQ_CALL_VOID(del());
}
inline void AQSLineEdit::home(bool arg0)
{
  AQ_CALL_VOID(home(arg0));
}
inline void AQSLineEdit::end(bool arg0)
{
  AQ_CALL_VOID(end(arg0));
}
inline int AQSLineEdit::selectionStart() const
{
  AQ_CALL_RET_V(selectionStart(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSLINEEDIT_P_H_ */
// @AQSWIDGET@
