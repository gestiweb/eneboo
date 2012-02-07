/***************************************************************************
 AQSReportViewer_p.h
 -------------------
 begin                : 02/11/2011
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

#ifndef AQSREPORTVIEWER_P_H_
#define AQSREPORTVIEWER_P_H_

#include "AQSWidget_p.h"
#include "AQObjects.h"
#include "AQSDomNode_p.h"

class AQSReportViewer : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_AQOBJECT(ReportViewer, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void exec();
  bool renderReport(int = 0, int = 0, uint = MReportViewer::Display);
  bool setReportData(FLSqlQuery *);
  bool setReportData(FLSqlCursor *);
  bool setReportData(QDomNode *);
  bool setReportData(AQSDomNode *);
  bool setReportTemplate(const QString &, const QString& = QString::null);
  bool setReportTemplate(QDomNode *, const QString& = QString::null);
  bool setReportTemplate(AQSDomNode *, const QString& = QString::null);
  QSize sizeHint() const;
  void setNumCopies(const int);
  void setPrintToPos(bool);
  void setPrinterName(const QString &);
  bool reportPrinted();
  void setStyleName(const QString &);
  MReportViewer *rptViewer() const;
  FLReportEngine *rptEngine() const;
  void rptViewerEmbedInParent(QWidget *);
  void rptViewerReparent(QWidget *);
  QString csvData();
  void setReportPages(FLReportPages *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,bool"));
    candidates[4].append(QString::fromLatin1("QWidget*,QString,bool,FLReportEngine*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQReportViewer;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new AQReportViewer(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new AQReportViewer(argValue<QWidget *>(args[0]),
                                *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,bool"))
      return new AQReportViewer(argValue<QWidget *>(args[0]),
                                *(argValue<QString *>(args[1])),
                                args[2].variant().toBool());
    if (sgt == QString::fromLatin1("QWidget*,QString,bool,FLReportEngine*"))
      return new AQReportViewer(argValue<QWidget *>(args[0]),
                                *(argValue<QString *>(args[1])),
                                args[2].variant().toBool(),
                                argValue<FLReportEngine *>(args[3]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,bool"));
    candidates[4].append(QString::fromLatin1("QWidget*,QString,bool,FLReportEngine*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSReportViewer::exec()
{
  AQ_CALL_VOID(exec());
}
inline bool AQSReportViewer::renderReport(int arg0,  int arg1,  uint arg2)
{
  AQ_CALL_RET_V(renderReport(arg0, arg1, arg2), bool);
}
inline bool AQSReportViewer::setReportData(FLSqlQuery *arg0)
{
  AQ_CALL_RET_V(setReportData(arg0), bool);
}
inline bool AQSReportViewer::setReportData(FLSqlCursor *arg0)
{
  AQ_CALL_RET_V(setReportData(arg0), bool);
}
inline bool AQSReportViewer::setReportData(QDomNode *arg0)
{
  AQ_CALL_RET_V(setReportData(*arg0), bool);
}
inline bool AQSReportViewer::setReportData(AQSDomNode *arg0)
{
  AQ_CALL_RET_V(setReportData(*arg0), bool);
}
inline bool AQSReportViewer::setReportTemplate(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(setReportTemplate(arg0, arg1), bool);
}
inline bool AQSReportViewer::setReportTemplate(QDomNode *arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(setReportTemplate(*arg0, arg1), bool);
}
inline bool AQSReportViewer::setReportTemplate(AQSDomNode *arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(setReportTemplate(*arg0, arg1), bool);
}
inline QSize AQSReportViewer::sizeHint() const
{
  AQ_CALL_RET_V(sizeHint(), QSize);
}
inline void AQSReportViewer::setNumCopies(const int arg0)
{
  AQ_CALL_VOID(setNumCopies(arg0));
}
inline void AQSReportViewer::setPrintToPos(bool arg0)
{
  AQ_CALL_VOID(setPrintToPos(arg0));
}
inline void AQSReportViewer::setPrinterName(const QString &arg0)
{
  AQ_CALL_VOID(setPrinterName(arg0));
}
inline bool AQSReportViewer::reportPrinted()
{
  AQ_CALL_RET_V(reportPrinted(), bool);
}
inline void AQSReportViewer::setStyleName(const QString &arg0)
{
  AQ_CALL_VOID(setStyleName(arg0));
}
inline MReportViewer *AQSReportViewer::rptViewer() const
{
  AQ_CALL_RET(rptViewer());
}
inline FLReportEngine *AQSReportViewer::rptEngine() const
{
  AQ_CALL_RET(rptEngine());
}
inline void AQSReportViewer::rptViewerEmbedInParent(QWidget *arg0)
{
  AQ_CALL_VOID(rptViewerEmbedInParent(arg0));
}
inline void AQSReportViewer::rptViewerReparent(QWidget *arg0)
{
  AQ_CALL_VOID(rptViewerReparent(arg0));
}
inline QString AQSReportViewer::csvData()
{
  AQ_CALL_RET_V(csvData(), QString);
}
inline void AQSReportViewer::setReportPages(FLReportPages *arg0)
{
  AQ_CALL_VOID(setReportPages(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSREPORTVIEWER_P_H_ */
// @AQWIDGET@
