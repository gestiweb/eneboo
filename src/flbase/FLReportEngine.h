/***************************************************************************
                           FLReportEngine.h
                          -------------------
 begin                : jue jun 27 2002
 copyright            : (C) 2002-2005 by InfoSiAL S.L.
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

#ifndef FLREPORTENGINE_H
#define FLREPORTENGINE_H

#include <qdom.h>

#include "mreportengine.h"
#include "mreportviewer.h"

class FLSqlCursor;
class FLSqlQuery;
class FLSqlQueryInterface;
class FLDomNodeInterface;
class FLPicture;
class FLReportEngine;
class FLReportPages;
class FLReportEnginePrivate;

/**
Motor de generación de informes para Kugar. Hereda de MReportEngine.

Extiende la funcionalidad de MReportEngine, para crear el origen
de datos (una estructura XML) de Kugar a partir de una tabla o una consulta.
El origen de una tabla o una consulta son objetos del tipo FLSqlCursor para tablas
y FLSqlQuery para consultas. El formato de los informes se sigue tomando de un
fichero XML según el formato prefijado por Kugar.

@author InfoSiAL S.L.
*/
class FLReportEngine: public MReportEngine
{
  Q_OBJECT

public:

  /**
  constructor
  */
  FLReportEngine(QObject *parent = 0);

  /**
  destructor
  */
  ~FLReportEngine();

  /**
  Establece los datos del informe a partir de una consulta

  @param q Objeto FLSqlQuery con la consulta de la que se toman los datos
  @return TRUE si todo ha ido bien
  */
  bool setReportData(FLSqlQuery *q);

  /**
  Establece los datos del informe a partir de un documento XML

  @param d Objeto QDomNode con el conjunto de registros de la que se toman los datos
  @return TRUE si todo ha ido bien
  */
  bool setFLReportData(QDomNode n);

  /**
  Establece los datos del informe a partir de una tabla

  @param t Objeto FLSqlCursor con  la tabla de la que se toman los datos
  @return TRUE si todo ha ido bien
  */
  bool setReportData(FLSqlCursor *t);

  /**
  Establece la plantilla para el informe.

  El nombre de la plantilla corresponde con el nombre del fichero con extesión ".kut".

  @param t Nombre dela plantilla
  @return TRUE si todo ha ido bien
  */
  bool setFLReportTemplate(const QString &t);

  /**
  Establece la plantilla para el informe.

  La plantilla viene definida por un nodo KugarTemplate

  @param d Nodo KugarTemplate
  @return TRUE si todo ha ido bien
  */
  bool setFLReportTemplate(QDomNode n);

  /**
  @return La consulta de origen con los datos para el informe. Puede ser cero si lo datos del informe
          se establecieron directamente mediante un documento XML.
          Se debe llamar despues de establecer los datos para el informe con set*ReportData, de
          lo contrario el resultado será indefinido.
  */
  FLSqlQuery *rptQueryData() const;

  /**
  @return Nombre del fichero ".kut" con la plantilla.
  */
  QString rptNameTemplate() const;

  /**
  @return Los datos de origen del informe en un documento XML con el formato usado por Kugar.
          Se debe llamar despues de establecer los datos para el informe con set*ReportData, de
          lo contrario el resultado será indefinido.
  */
  QDomDocument *rptXmlData() const {
    return rd;
  }

  /**
  @return Plantilla del informe en un documento XML con el formato usado por Kugar.
          Se debe llamar despues de establecer los datos para el informe con set*ReportTemplate, de
          lo contrario el resultado será indefinido.
  */
  QDomDocument *rptXmlTemplate() const {
    return rt;
  }

public slots:

  // Intefaz scripts
  bool setReportData(FLSqlQueryInterface *q);
  bool setReportData(FLDomNodeInterface *n);
  bool setReportTemplate(const QString &t);
  bool setReportTemplate(FLDomNodeInterface *n);
  FLDomNodeInterface *reportData() const;
  FLDomNodeInterface *reportTemplate() const;

  /**
  Imprime el informe a imagenes que devuelve como una colección de paginas

  @param  initRow     Fila de inicio cuando se imprimen etiquetas
  @param  initCol     Columna de inicio cuando se imprimen etiquetas
  @param  pages       Aqui se puede indicar una coleccion de paginas a las
                      que se añadirán las nuevas paginas generadas
  @param  fillRecords Si es TRUE se añadiran al XML interno de datos los campos
                      calculados y su valor. Posteriormente se puede obtener el conjunto
                      de registros actualizado en XML mediante el método reportData().

  Nota: La coleccion de paginas se puede visualizarla en un FLReportViewer
        mediante FLReportViewer::setReportPages()

  @return La colección de paginas generada
  */
  FLReportPages *renderReport(int initRow = 0,
                              int initCol = 0,
                              bool fillRecords = false,
                              FLReportPages *pages = 0);
  // Intefaz scripts

private:

  FLReportEnginePrivate *d;
};

#endif
