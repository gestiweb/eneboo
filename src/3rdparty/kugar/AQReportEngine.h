/***************************************************************************
                           AQReportEngine.h
                       -------------------
begin                : 09/08/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
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

#ifndef AQREPORTENGINE_H_
#define AQREPORTENGINE_H_

#define AQ_MAXGROUPLEVEL 10

#include <AQCore/AQConfig.h>

#include <QObject>
#include <QString>
#include <QDomDocumentFragment>

//#include "mreportengine.h"

class AQSqlCursor;
class AQSqlQuery;

/**
Motor de generación de informes. Hereda de MReportEngine.
 
Extiende la funcionalidad de MReportEngine, para crear el origen
de datos (una estructura XML) de Kugar a partir de una tabla o una consulta.
El origen de una tabla o una consulta son objetos del tipo AQSqlCursor para tablas
y AQSqlQuery para consultas. El formato de los informes se sigue tomando de un
fichero XML según el formato prefijado por Kugar.
 
@author InfoSiAL S.L.
*/
class AQReportEngine /*: public MReportEngine*/
{

    /*Q_OBJECT*/

public:

    /**
    constructor
    */
    AQReportEngine( QObject * parent = 0 );

    /**
    desctructor
    */
    ~AQReportEngine();

    /**
    Establece los datos del informe a partir de una consulta

    @param q Objeto AQSqlQuery con la consulta de la que se toman los datos
    @return TRUE si todo ha ido bien
    */
    bool setReportData( AQSqlQuery * q );

    /**
    Establece los datos del informe a partir de una tabla

    @param t Objeto AQSqlCursor con  la tabla de la que se toman los datos
    @return TRUE si todo ha ido bien
    */
    bool setReportData( AQSqlCursor * t );

    /**
    Establece la plantilla para el informe.

    El nombre de la plantilla corresponde con el nombre del fichero con extesión ".kut".

    @param t Nombre dela plantilla
    @return TRUE si todo ha ido bien
    */
    bool setAQReportTemplate( const QString & t );

private:

    /**
    Añade los datos del registro activo de la consulta dada al origen de datos del informe.

    Todos los campos del registro son agregados a la estructura XML con los datos de origen
    del informe, en un elemento tipo "Row", indicando a que nivel pertenece el registro según
    el parámetro l. La consulta debe pasarse en un estado válido, es decir, con un registro activo.

    @param q Consulta de la que tomar su registro activo para agregar datos al informe
    @param l Nivel al  que pertenece el registro
    */
    void addRowToReportData( AQSqlQuery * q, const int l );

    /**
    Agrupa los registros de la consulta recursivamente según el nivel de agrupamiento, y los
    va sacando al objeo raiz XML, en el orden adecuado.

    La consulta debe pasarse en un estado válido, es decir, con un registro activo
    */
    void groupBy( int levelMax, AQSqlQuery * q, QString vA[ AQ_MAXGROUPLEVEL ] );

    /**
    Registros de la consulta en formato de nodos XML para Kugar
    */
    QDomDocumentFragment * rows;

    /**
    Guarda la consulta origen
    */
    AQSqlQuery * qry_;
};

#endif /*AQREPORTENGINE_H_*/
