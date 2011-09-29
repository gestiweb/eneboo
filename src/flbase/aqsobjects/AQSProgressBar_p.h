/***************************************************************************
 AQSProgressBar_p.h
 -------------------
 begin                : 19/02/2011
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

#ifndef AQSPROGRESSBAR_P_H_
#define AQSPROGRESSBAR_P_H_

#include "AQSFrame_p.h"
#include <qprogressbar.h>

class AQSProgressBar : public AQSFrame
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(ProgressBar, Frame);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int totalSteps() const;
  int progress() const;
  const QString &progressString() const;
  void setCenterIndicator(bool);
  bool centerIndicator() const;
  void setIndicatorFollowsStyle(bool);
  bool indicatorFollowsStyle() const;
  bool percentageVisible() const;
  void setPercentageVisible(bool);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    candidates[1].append(QString::fromLatin1("int"));
    candidates[2].append(QString::fromLatin1("int,QWidget*"));
    candidates[3].append(QString::fromLatin1("int,QWidget*,QString"));
    candidates[4].append(QString::fromLatin1("int,QWidget*,QString,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QProgressBar;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QProgressBar(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QProgressBar(argValue<QWidget *>(args[0]),
                              *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new QProgressBar(argValue<QWidget *>(args[0]),
                              *(argValue<QString *>(args[1])),
                              static_cast<WFlags>(args[2].variant().toUInt()));
    if (sgt == QString::fromLatin1("int"))
      return new QProgressBar(args[0].variant().toInt());
    if (sgt == QString::fromLatin1("int,QWidget*"))
      return new QProgressBar(args[0].variant().toInt(),
                              argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("int,QWidget*,QString"))
      return new QProgressBar(args[0].variant().toInt(),
                              argValue<QWidget *>(args[1]),
                              *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("int,QWidget*,QString,WFlags"))
      return new QProgressBar(args[0].variant().toInt(),
                              argValue<QWidget *>(args[1]),
                              *(argValue<QString *>(args[2])),
                              static_cast<WFlags>(args[3].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    candidates[1].append(QString::fromLatin1("int"));
    candidates[2].append(QString::fromLatin1("int,QWidget*"));
    candidates[3].append(QString::fromLatin1("int,QWidget*,QString"));
    candidates[4].append(QString::fromLatin1("int,QWidget*,QString,WFlags"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSProgressBar::totalSteps() const
{
  AQ_CALL_RET_V(totalSteps(), int);
}
inline int AQSProgressBar::progress() const
{
  AQ_CALL_RET_V(progress(), int);
}
inline const QString &AQSProgressBar::progressString() const
{
  AQ_CALL_RET_V(progressString(), QString);
}
inline void AQSProgressBar::setCenterIndicator(bool arg0)
{
  AQ_CALL_VOID(setCenterIndicator(arg0));
}
inline bool AQSProgressBar::centerIndicator() const
{
  AQ_CALL_RET_V(centerIndicator(), bool);
}
inline void AQSProgressBar::setIndicatorFollowsStyle(bool arg0)
{
  AQ_CALL_VOID(setIndicatorFollowsStyle(arg0));
}
inline bool AQSProgressBar::indicatorFollowsStyle() const
{
  AQ_CALL_RET_V(indicatorFollowsStyle(), bool);
}
inline bool AQSProgressBar::percentageVisible() const
{
  AQ_CALL_RET_V(percentageVisible(), bool);
}
inline void AQSProgressBar::setPercentageVisible(bool arg0)
{
  AQ_CALL_VOID(setPercentageVisible(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSPROGRESSBAR_P_H_ */
// @AQSWIDGET@
