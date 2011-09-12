/***************************************************************************
 FLJasperViewer.h
 -------------------
 begin                : 08/06/2010
 copyright            : (C) 2003-2010 by InfoSiAL S.L.
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

#ifndef FLJASPERVIEWER_H
#define FLJASPERVIEWER_H

#include <qobject.h>
#include <qstring.h>
#include <qdom.h>

class FLSqlQuery;
class FLSqlQueryInterface;
class FLDomNodeInterface;
class FLJasperViewerPrivate;

/**
\b OBSOLETA Permite lanzar el visor de informes nativo de JasperReports.

OBSOLETA. Utilizar FLJasperEngine junto con FLReportViewer::setReportPages.
          Ver FLJasperEngine.

@author InfoSiAL S.L.
*/
class FLJasperViewer : public QObject
{
  Q_OBJECT

public:

  FLJasperViewer(QObject *parent = 0, const char *name = 0);
  ~FLJasperViewer();

public slots:

  /**
  Establece los datos del informe a partir de una consulta

  @param q Objeto FLSqlQuery con la consulta de la que se toman los datos
  @return TRUE si todo ha ido bien
  */
  bool setReportData(FLSqlQuery *q);
  bool setReportData(FLSqlQueryInterface *q);

  /**
  Establece los datos del informe a partir de un documento XML

  @param d Objeto QDomNode con los datos
  @return TRUE si todo ha ido bien
  */
  bool setReportData(QDomNode n);
  bool setReportData(FLDomNodeInterface *n);

  /**
  Establece la plantilla para el informe.

  El nombre de la plantilla corresponde con el nombre del fichero con extesión ".jrxml".

  @param t Nombre dela plantilla
  @return TRUE si todo ha ido bien
  */
  bool setReportTemplate(const QString &t);

  /**
  Muestra el visor
  */
  bool exec();

private:

  FLJasperViewerPrivate *d;
};

#endif // FLJASPERVIEWER_H
