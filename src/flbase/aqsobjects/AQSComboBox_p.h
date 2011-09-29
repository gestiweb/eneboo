/***************************************************************************
 AQSComboBox_p.h
 -------------------
 begin                : 29/01/2011
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

#ifndef AQSCOMBOBOX_P_H_
#define AQSCOMBOBOX_P_H_

#include "AQSWidget_p.h"
#include "AQSPixmap_p.h"
#include <qcombobox.h>

class AQSComboBox : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(ComboBox, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void insertStringList(const QStringList &, int = -1);
  void insertItem(QPixmap *, int = -1);
  void insertItem(AQSPixmap *, int = -1);
  void insertItem(QPixmap *, const QString &, int = -1);
  void insertItem(AQSPixmap *, const QString &, int = -1);
  void removeItem(int);
  void setCurrentItem(int);
  void setCurrentText(const QString &);
  QString text(int) const;
  const QPixmap *pixmap(int) const;
  void changeItem(const QString &, int);
  void changeItem(QPixmap *, int);
  void changeItem(AQSPixmap *, int);
  void changeItem(QPixmap *, const QString &, int);
  void changeItem(AQSPixmap *, const QString &, int);
  void setValidator(const QValidator *);
  const QValidator *validator() const;
  void setListBox(QListBox *);
  QListBox *listBox() const;
  void setLineEdit(QLineEdit *);
  QLineEdit *lineEdit() const;
  void popup();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[1].append(QString::fromLatin1("bool"));
    candidates[2].append(QString::fromLatin1("bool,QWidget*"));
    candidates[3].append(QString::fromLatin1("bool,QWidget*,QString"));
    candidates[0].append(QString::null);
    candidates[0].append(QString::null);
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QComboBox;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QComboBox(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QComboBox(argValue<QWidget *>(args[0]),
                           *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("bool"))
      return new QComboBox(args[0].variant().toBool());
    if (sgt == QString::fromLatin1("bool,QWidget*"))
      return new QComboBox(args[0].variant().toBool(),
                           argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("bool,QWidget*,QString"))
      return new QComboBox(args[0].variant().toBool(),
                           argValue<QWidget *>(args[1]),
                           *(argValue<QString *>(args[2])));
    if (sgt.isEmpty())
      return new QComboBox;
    if (sgt.isEmpty())
      return new QComboBox;
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[1].append(QString::fromLatin1("bool"));
    candidates[2].append(QString::fromLatin1("bool,QWidget*"));
    candidates[3].append(QString::fromLatin1("bool,QWidget*,QString"));
    candidates[0].append(QString::null);
    candidates[0].append(QString::null);
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSComboBox::insertStringList(const QStringList &arg0,  int arg1)
{
  AQ_CALL_VOID(insertStringList(arg0, arg1));
}
inline void AQSComboBox::insertItem(QPixmap *arg0,  int arg1)
{
  AQ_CALL_VOID(insertItem(*arg0, arg1));
}
inline void AQSComboBox::insertItem(AQSPixmap *arg0,  int arg1)
{
  AQ_CALL_VOID(insertItem(*arg0, arg1));
}
inline void AQSComboBox::insertItem(QPixmap *arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_VOID(insertItem(*arg0, arg1, arg2));
}
inline void AQSComboBox::insertItem(AQSPixmap *arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_VOID(insertItem(*arg0, arg1, arg2));
}
inline void AQSComboBox::removeItem(int arg0)
{
  AQ_CALL_VOID(removeItem(arg0));
}
inline void AQSComboBox::setCurrentItem(int arg0)
{
  AQ_CALL_VOID(setCurrentItem(arg0));
}
inline void AQSComboBox::setCurrentText(const QString &arg0)
{
  AQ_CALL_VOID(setCurrentText(arg0));
}
inline QString AQSComboBox::text(int arg0) const
{
  AQ_CALL_RET_V(text(arg0), QString);
}
inline const QPixmap *AQSComboBox::pixmap(int arg0) const
{
  AQ_CALL_RET(pixmap(arg0));
}
inline void AQSComboBox::changeItem(const QString &arg0,  int arg1)
{
  AQ_CALL_VOID(changeItem(arg0, arg1));
}
inline void AQSComboBox::changeItem(QPixmap *arg0,  int arg1)
{
  AQ_CALL_VOID(changeItem(*arg0, arg1));
}
inline void AQSComboBox::changeItem(AQSPixmap *arg0,  int arg1)
{
  AQ_CALL_VOID(changeItem(*arg0, arg1));
}
inline void AQSComboBox::changeItem(QPixmap *arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_VOID(changeItem(*arg0, arg1, arg2));
}
inline void AQSComboBox::changeItem(AQSPixmap *arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_VOID(changeItem(*arg0, arg1, arg2));
}
inline void AQSComboBox::setValidator(const QValidator *arg0)
{
  AQ_CALL_VOID(setValidator(arg0));
}
inline const QValidator *AQSComboBox::validator() const
{
  AQ_CALL_RET(validator());
}
inline void AQSComboBox::setListBox(QListBox *arg0)
{
  AQ_CALL_VOID(setListBox(arg0));
}
inline QListBox *AQSComboBox::listBox() const
{
  AQ_CALL_RET(listBox());
}
inline void AQSComboBox::setLineEdit(QLineEdit *arg0)
{
  AQ_CALL_VOID(setLineEdit(arg0));
}
inline QLineEdit *AQSComboBox::lineEdit() const
{
  AQ_CALL_RET(lineEdit());
}
inline void AQSComboBox::popup()
{
  AQ_CALL_VOID(popup());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCOMBOBOX_P_H_ */
// @AQSWIDGET@
