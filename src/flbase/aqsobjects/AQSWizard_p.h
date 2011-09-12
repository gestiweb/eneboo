/***************************************************************************
 AQSWizard_p.h
 -------------------
 begin                : 09/02/2011
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

#ifndef AQSWIZARD_P_H_
#define AQSWIZARD_P_H_

#include "AQSDialog_p.h"
#include <qwizard.h>

class AQSWizard : public AQSDialog
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Wizard, Dialog);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void addPage(QWidget *, const QString &);
  void insertPage(QWidget *, const QString &, int);
  void removePage(QWidget *);
  QString title(QWidget *) const;
  void setTitle(QWidget *, const QString &);
  void showPage(QWidget *);
  QWidget *currentPage() const;
  QWidget *page(int) const;
  int pageCount() const;
  int indexOf(QWidget *) const;
  bool appropriate(QWidget *) const;
  void setAppropriate(QWidget *, bool);
  QPushButton *backButton() const;
  QPushButton *nextButton() const;
  QPushButton *finishButton() const;
  QPushButton *cancelButton() const;
  QPushButton *helpButton() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,bool"));
    candidates[4].append(QString::fromLatin1("QWidget*,QString,bool,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QWizard;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QWizard(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QWizard(argValue<QWidget *>(args[0]),
                         *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,bool"))
      return new QWizard(argValue<QWidget *>(args[0]),
                         *(argValue<QString *>(args[1])),
                         args[2].variant().toBool());
    if (sgt == QString::fromLatin1("QWidget*,QString,bool,WFlags"))
      return new QWizard(argValue<QWidget *>(args[0]),
                         *(argValue<QString *>(args[1])),
                         args[2].variant().toBool(),
                         static_cast<WFlags>(args[3].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,bool"));
    candidates[4].append(QString::fromLatin1("QWidget*,QString,bool,WFlags"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSWizard::addPage(QWidget *arg0,  const QString &arg1)
{
  AQ_CALL_VOID(addPage(arg0, arg1));
}
inline void AQSWizard::insertPage(QWidget *arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_VOID(insertPage(arg0, arg1, arg2));
}
inline void AQSWizard::removePage(QWidget *arg0)
{
  AQ_CALL_VOID(removePage(arg0));
}
inline QString AQSWizard::title(QWidget *arg0) const
{
  AQ_CALL_RET_V(title(arg0), QString);
}
inline void AQSWizard::setTitle(QWidget *arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setTitle(arg0, arg1));
}
inline void AQSWizard::showPage(QWidget *arg0)
{
  AQ_CALL_VOID(showPage(arg0));
}
inline QWidget *AQSWizard::currentPage() const
{
  AQ_CALL_RET(currentPage());
}
inline QWidget *AQSWizard::page(int arg0) const
{
  AQ_CALL_RET(page(arg0));
}
inline int AQSWizard::pageCount() const
{
  AQ_CALL_RET_V(pageCount(), int);
}
inline int AQSWizard::indexOf(QWidget *arg0) const
{
  AQ_CALL_RET_V(indexOf(arg0), int);
}
inline bool AQSWizard::appropriate(QWidget *arg0) const
{
  AQ_CALL_RET_V(appropriate(arg0), bool);
}
inline void AQSWizard::setAppropriate(QWidget *arg0,  bool arg1)
{
  AQ_CALL_VOID(setAppropriate(arg0, arg1));
}
inline QPushButton *AQSWizard::backButton() const
{
  AQ_CALL_RET(backButton());
}
inline QPushButton *AQSWizard::nextButton() const
{
  AQ_CALL_RET(nextButton());
}
inline QPushButton *AQSWizard::finishButton() const
{
  AQ_CALL_RET(finishButton());
}
inline QPushButton *AQSWizard::cancelButton() const
{
  AQ_CALL_RET(cancelButton());
}
inline QPushButton *AQSWizard::helpButton() const
{
  AQ_CALL_RET(helpButton());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSWIZARD_P_H_ */
// @AQSWIDGET@
