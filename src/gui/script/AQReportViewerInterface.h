/***************************************************************************
AQReportViewerInterface.h
-------------------
begin                : 01/03/2009
copyright            : (C) 2003-2009 by InfoSiAL S.L.
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

#ifndef AQREPORTVIEWERINTERFACE_H_
#define AQREPORTVIEWERINTERFACE_H_

#include <AQScript/AQComponentDBInterface.h>
#include <AQUi/AQReportViewer.h>

#ifdef AQ_OBJ_FACTORY_IMPL
static void deleteSharedAQReportViewer( AQReportViewer * obj )
{
  if ( !obj->parent() )
    obj->deleteLater();
}
#endif

class AQ3Picture;
class AQSqlQueryInterface;
class AQDomNodeInterface;

class AQReportViewerInterfaceAQ2 : public AQComponentDBInterface
{
  Q_OBJECT

public:

  QSharedPointer<AQReportViewer> obj_;

public Q_SLOTS:

  bool renderReport( int = 0, int = 0 )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::renderReport( int = 0, int = 0 ) );
    return true;
  }
  bool setReportData( AQSqlQueryInterface * )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setReportData( AQSqlQueryInterface * ) );
    return true;
  }
  bool setReportData( AQDomNodeInterface * )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setReportData( AQDomNodeInterface * ) );
    return true;
  }
  bool setReportTemplate( const QString &, const QString & = QString() )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setReportTemplate( const QString &, const QString & = QString() ) );
    return true;
  }
  void exec()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::exec() );
  }
  void show()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::show() );
  }
  QString csvData()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::csvData() );
    return QString();
  }
  void printReport()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::printReport() );
  }
  void printReportToPS( const QString & )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::printReportToPS( const QString & ) );
  }
  void printReportToPDF( const QString & )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::printReportToPDF( const QString & ) );
  }
  void setNumCopies( int )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setNumCopies( int ) );
  }
  void setPrintToPos( bool )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setPrintToPos( bool ) );
  }
  void setPrinterName( const QString & )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setPrinterName( const QString & ) );
  }
  bool reportPrinted()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::reportPrinted() );
    return true;
  }
  void setAutoClose( bool )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setAutoClose( bool ) );
  }
  void setResolution( int )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setResolution( int ) );
  }
  void setPixel( int )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setPixel( int ) );
  }
  void setDefaults()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setDefaults() );
  }
  void updateReport()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::updateReport() );
  }
  void setStyleName( const QString & )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setStyleName( const QString & ) );
  }
  AQ3Picture * getCurrentPage()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::getCurrentPage() );
    return 0;
  }
  AQ3Picture * getFirstPage()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::getFirstPage() );
    return 0;
  }
  AQ3Picture * getPreviousPage()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::getPreviousPage() );
    return 0;
  }
  AQ3Picture * getNextPage()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::getNextPage() );
    return 0;
  }
  AQ3Picture * getLastPage()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::getLastPage() );
    return 0;
  }
  AQ3Picture * getPageAt( uint )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::getPageAt( uint ) );
    return 0;
  }
  void updateDisplay()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::updateDisplay() );
  }
  void clearPages()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::clearPages() );
  }
  void appendPage()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::appendPage() );
  }
  int getCurrentIndex()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::getCurrentIndex() );
    return 0;
  }
  void setCurrentPage( int )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setCurrentPage( int ) );
  }
  void setPageSize( int )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setPageSize( int ) );
  }
  void setPageOrientation( int )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setPageOrientation( int ) );
  }
  void setPageDimensions( QSize )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setPageDimensions( QSize ) );
  }
  int pageSize()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::pageSize() );
    return 0;
  }
  int pageOrientation()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::pageOrientation() );
    return 0;
  }
  QSize pageDimensions()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::pageDimensions() );
    return QSize();
  }
  int pageCount()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::pageCount() );
    return 0;
  }
  void setName( const QString & )
  {
    AQ_NO_PORT_AQ2( AQReportViewer::setName( const QString & ) );
  }
  AQReportViewer * obj()
  {
    AQ_NO_PORT_AQ2( AQReportViewer::obj() );
    return 0;
  }

protected:

  friend class AQUiObjectFactoryQSA;

  /**
  Constructor
  */
  AQReportViewerInterfaceAQ2( const AQScriptEngineQSA * eng );

  /**
  Constructor
  */
  AQReportViewerInterfaceAQ2( const AQScriptEngineQSA * eng,
                              QSharedPointer<AQReportViewer> aqReportViewer );

  /**
  Destructor
  */
  virtual ~AQReportViewerInterfaceAQ2();
};

#endif /*AQREPORTVIEWERINTERFACE_H_*/
