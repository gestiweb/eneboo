/***************************************************************************
 AQSScrollView_p.h
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

#ifndef AQSSCROLLVIEW_P_H_
#define AQSSCROLLVIEW_P_H_

#include "AQSFrame_p.h"
#include <qscrollview.h>

class AQSScrollView : public AQSFrame
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(ScrollView, Frame);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void addChild(QWidget *, int = 0, int = 0);
  void moveChild(QWidget *, int, int);
  int childX(QWidget *);
  int childY(QWidget *);
  QWidget *cornerWidget() const;
  void setCornerWidget(QWidget *);
  QScrollBar *horizontalScrollBar() const;
  QScrollBar *verticalScrollBar() const;
  QWidget *viewport() const;
  QWidget *clipper() const;
  void updateContents(int, int, int, int);
  void updateContents(const QRect &);
  void updateContents();
  void repaintContents(int, int, int, int, bool = TRUE);
  void repaintContents(const QRect &, bool = TRUE);
  void repaintContents(bool = TRUE);
  void contentsToViewport(int, int, int &, int &) const;
  void viewportToContents(int, int, int &, int &) const;
  QPoint contentsToViewport(const QPoint &) const;
  QPoint viewportToContents(const QPoint &) const;
  void enableClipper(bool);
  void setStaticBackground(bool);
  bool hasStaticBackground() const;
  QSize viewportSize(int, int) const;
  bool isHorizontalSliderPressed();
  bool isVerticalSliderPressed();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QScrollView;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QScrollView(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QScrollView(argValue<QWidget *>(args[0]),
                             *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new QScrollView(argValue<QWidget *>(args[0]),
                             *(argValue<QString *>(args[1])),
                             static_cast<WFlags>(args[2].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSScrollView::addChild(QWidget *arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(addChild(arg0, arg1, arg2));
}
inline void AQSScrollView::moveChild(QWidget *arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(moveChild(arg0, arg1, arg2));
}
inline int AQSScrollView::childX(QWidget *arg0)
{
  AQ_CALL_RET_V(childX(arg0), int);
}
inline int AQSScrollView::childY(QWidget *arg0)
{
  AQ_CALL_RET_V(childY(arg0), int);
}
inline QWidget *AQSScrollView::cornerWidget() const
{
  AQ_CALL_RET(cornerWidget());
}
inline void AQSScrollView::setCornerWidget(QWidget *arg0)
{
  AQ_CALL_VOID(setCornerWidget(arg0));
}
inline QScrollBar *AQSScrollView::horizontalScrollBar() const
{
  AQ_CALL_RET(horizontalScrollBar());
}
inline QScrollBar *AQSScrollView::verticalScrollBar() const
{
  AQ_CALL_RET(verticalScrollBar());
}
inline QWidget *AQSScrollView::viewport() const
{
  AQ_CALL_RET(viewport());
}
inline QWidget *AQSScrollView::clipper() const
{
  AQ_CALL_RET(clipper());
}
inline void AQSScrollView::updateContents(int arg0,  int arg1,  int arg2,  int arg3)
{
  AQ_CALL_VOID(updateContents(arg0, arg1, arg2, arg3));
}
inline void AQSScrollView::updateContents(const QRect &arg0)
{
  AQ_CALL_VOID(updateContents(arg0));
}
inline void AQSScrollView::updateContents()
{
  AQ_CALL_VOID(updateContents());
}
inline void AQSScrollView::repaintContents(int arg0,  int arg1,  int arg2,  int arg3,  bool arg4)
{
  AQ_CALL_VOID(repaintContents(arg0, arg1, arg2, arg3, arg4));
}
inline void AQSScrollView::repaintContents(const QRect &arg0,  bool arg1)
{
  AQ_CALL_VOID(repaintContents(arg0, arg1));
}
inline void AQSScrollView::repaintContents(bool arg0)
{
  AQ_CALL_VOID(repaintContents(arg0));
}
inline void AQSScrollView::contentsToViewport(int arg0,  int arg1,  int &arg2,  int &arg3) const
{
  AQ_CALL_VOID(contentsToViewport(arg0, arg1, arg2, arg3));
}
inline void AQSScrollView::viewportToContents(int arg0,  int arg1,  int &arg2,  int &arg3) const
{
  AQ_CALL_VOID(viewportToContents(arg0, arg1, arg2, arg3));
}
inline QPoint AQSScrollView::contentsToViewport(const QPoint &arg0) const
{
  AQ_CALL_RET_V(contentsToViewport(arg0), QPoint);
}
inline QPoint AQSScrollView::viewportToContents(const QPoint &arg0) const
{
  AQ_CALL_RET_V(viewportToContents(arg0), QPoint);
}
inline void AQSScrollView::enableClipper(bool arg0)
{
  AQ_CALL_VOID(enableClipper(arg0));
}
inline void AQSScrollView::setStaticBackground(bool arg0)
{
  AQ_CALL_VOID(setStaticBackground(arg0));
}
inline bool AQSScrollView::hasStaticBackground() const
{
  AQ_CALL_RET_V(hasStaticBackground(), bool);
}
inline QSize AQSScrollView::viewportSize(int arg0,  int arg1) const
{
  AQ_CALL_RET_V(viewportSize(arg0, arg1), QSize);
}
inline bool AQSScrollView::isHorizontalSliderPressed()
{
  AQ_CALL_RET_V(isHorizontalSliderPressed(), bool);
}
inline bool AQSScrollView::isVerticalSliderPressed()
{
  AQ_CALL_RET_V(isVerticalSliderPressed(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSCROLLVIEW_P_H_ */
// @AQSWIDGET@
