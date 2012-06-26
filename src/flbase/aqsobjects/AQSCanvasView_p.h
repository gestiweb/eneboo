/***************************************************************************
 AQSCanvasView_p.h
 -------------------
 begin                : 06/10/2011
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

#ifndef AQSCANVASVIEW_P_H_
#define AQSCANVASVIEW_P_H_

#include "AQSScrollView_p.h"
#include "AQSWMatrix_p.h"
#include <qcanvas.h>

class AQSCanvasView : public AQSScrollView
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(CanvasView, ScrollView);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QCanvas *canvas() const;
  void setCanvas(QCanvas *);
  QWMatrix *worldMatrix() const;
  QWMatrix *inverseWorldMatrix() const;
  bool setWorldMatrix(QWMatrix *);
  bool setWorldMatrix(AQSWMatrix *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    candidates[1].append(QString::fromLatin1("QCanvas*"));
    candidates[2].append(QString::fromLatin1("QCanvas*,QWidget*"));
    candidates[3].append(QString::fromLatin1("QCanvas*,QWidget*,QString"));
    candidates[4].append(QString::fromLatin1("QCanvas*,QWidget*,QString,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QCanvasView;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QCanvasView(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QCanvasView(argValue<QWidget *>(args[0]),
                             *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new QCanvasView(argValue<QWidget *>(args[0]),
                             *(argValue<QString *>(args[1])),
                             static_cast<WFlags>(args[2].variant().toUInt()));
    if (sgt == QString::fromLatin1("QCanvas*"))
      return new QCanvasView(argValue<QCanvas *>(args[0]));
    if (sgt == QString::fromLatin1("QCanvas*,QWidget*"))
      return new QCanvasView(argValue<QCanvas *>(args[0]),
                             argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("QCanvas*,QWidget*,QString"))
      return new QCanvasView(argValue<QCanvas *>(args[0]),
                             argValue<QWidget *>(args[1]),
                             *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QCanvas*,QWidget*,QString,WFlags"))
      return new QCanvasView(argValue<QCanvas *>(args[0]),
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
    candidates[1].append(QString::fromLatin1("QCanvas*"));
    candidates[2].append(QString::fromLatin1("QCanvas*,QWidget*"));
    candidates[3].append(QString::fromLatin1("QCanvas*,QWidget*,QString"));
    candidates[4].append(QString::fromLatin1("QCanvas*,QWidget*,QString,WFlags"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QCanvas *AQSCanvasView::canvas() const
{
  AQ_CALL_RET(canvas());
}
inline void AQSCanvasView::setCanvas(QCanvas *arg0)
{
  AQ_CALL_VOID(setCanvas(arg0));
}
inline QWMatrix *AQSCanvasView::worldMatrix() const
{
  AQ_CALL_RET_PTR(worldMatrix(), QWMatrix);
}
inline QWMatrix *AQSCanvasView::inverseWorldMatrix() const
{
  AQ_CALL_RET_PTR(inverseWorldMatrix(), QWMatrix);
}
inline bool AQSCanvasView::setWorldMatrix(QWMatrix *arg0)
{
  AQ_CALL_RET_V(setWorldMatrix(*arg0), bool);
}
inline bool AQSCanvasView::setWorldMatrix(AQSWMatrix *arg0)
{
  AQ_CALL_RET_V(setWorldMatrix(*arg0), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCANVASVIEW_P_H_ */
// @AQSWidget@
