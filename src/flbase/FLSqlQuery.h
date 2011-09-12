/***************************************************************************
                              FLSqlQuery.h
                          -------------------
 begin                : sáb jun 22 2002
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

#ifndef FLSQLQUERY_H
#define FLSQLQUERY_H

#include "FLTableMetaData.h"

#include <qsqlquery.h>
#include <qvariant.h>
#include <qdict.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qinputdialog.h>
#include <qdict.h>

class FLParameterQuery;
class FLGroupByQuery;
class FLFieldMetaData;
class FLSqlDatabase;
class FLSqlQueryPrivate;

/**
Maneja consultas con características específicas para AbanQ, hereda de QSqlQuery.

Ofrece la funcionalidad para manejar consultas de QSqlQuery y además ofrece métodos
para trabajar con consultas parametrizadas y niveles de agrupamiento.

@author InfoSiAL S.L.
*/
class FL_EXPORT FLSqlQuery: public QObject
{

  Q_OBJECT

public:

  /**
  constructor
  */
  FLSqlQuery(QObject *parent = 0, const QString &connectionName = "default");

  /**
  destructor
  */
  ~FLSqlQuery();

  /**
  Ejecuta la consulta
  */
  bool exec();

  /**
  Añade la descripción parámetro al diccionario de parámetros.

  @param p Objeto FLParameterQuery con la descripción del parámetro a añadir
  */
  void addParameter(const FLParameterQuery *p);

  /**
  Añade la descripción de un grupo al diccionario de grupos.

  @param g Objeto FLGroupByQuery con la descripción del grupo a añadir
  */
  void addGroup(const FLGroupByQuery *g);

  /**
  Tipo de datos diccionario de parametros
  */
  typedef QDict<FLParameterQuery> FLParameterQueryDict;

  /**
  Tipo de datos diccionaro de grupos
  */
  typedef QDict<FLGroupByQuery> FLGroupByQueryDict;

  /**
  Para establecer el nombre de la consulta.

  @param n Nombre de la consulta
  */
  void setName(const QString &n);

  /**
  Para obtener el nombre de la consulta
  */
  QString name() const;

  /**
  Para obtener la parte SELECT de la sentencia SQL de la consulta
  */
  QString select() const;

  /**
  Para obtener la parte FROM de la sentencia SQL de la consulta
  */
  QString from() const;

  /**
  Para obtener la parte WHERE de la sentencia SQL de la consulta
  */
  QString where() const;

  /**
  Para obtener la parte ORDER BY de la sentencia SQL de la consulta
  */
  QString orderBy() const;

  /**
  Para establecer la parte SELECT de la sentencia SQL de la consulta.

  @param  s Cadena de texto con la parte SELECT de la sentencia SQL que
            genera la consulta. Esta cadena NO debe incluir la palabra reservada
            SELECT, ni tampoco el caracter '*' como comodín. Solo admite la lista
            de campos que deben aparecer en la consulta separados por la cadena
            indicada en el parámetro 'sep'
  @param  sep Cadena utilizada como separador en la lista de campos. Por defecto
              se utiliza la coma.
  */
  void setSelect(const QString &s, const QString &sep = QString::fromLatin1(","));

  /**
  Para establecer la parte FROM de la sentencia SQL de la consulta.

  @param f Cadena de texto con la parte FROM de la sentencia SQL que
       genera la consulta
  */
  void setFrom(const QString &f);

  /**
  Para establecer la parte WHERE de la sentencia SQL de la consulta.

  @param s Cadena de texto con la parte WHERE de la sentencia SQL que
       genera la consulta
  */
  void setWhere(const QString &w);

  /**
  Para establecer la parte ORDER BY de la sentencia SQL de la consulta.

  @param s Cadena de texto con la parte ORDER BY de la sentencia SQL que
       genera la consulta
  */
  void setOrderBy(const QString &w);

  /**
  Para obtener la sentencia completa SQL de la consulta.

  Este método une las tres partes de la consulta (SELECT, FROM Y WHERE),
  sustituye los parámetros por el valor que tienen en el diccionario y devuelve
  todo en una cadena de texto.

  @return Cadena de texto con la sentencia completa SQL que genera la consulta
  */
  QString sql();

  /**
  Para obtener los parametros de la consulta.

  @return Diccionario de parámetros
  */
  FLParameterQueryDict *parameterDict() const;

  /**
  Para obtener los niveles de agrupamiento de la consulta.

  @return Diccionario de niveles de agrupamiento
  */
  FLGroupByQueryDict *groupDict() const;

  /**
  Para obtener la lista de nombres de los campos.

  @return Lista de cadenas de texto con los nombres de los campos de la
      consulta
  */
  QStringList fieldList() const;

  /**
  Asigna un diccionario de parámetros, al diccionario de parámetros de la consulta.

  El diccionario de parámetros del tipo FLGroupByQueryDict , ya construido,
  es asignado como el nuevo diccionario de grupos de la consulta, en el caso de que
  ya exista un diccionario de grupos, este es destruido y sobreescrito por el nuevo.
  El diccionario pasado a este método pasa a ser propiedad de la consulta, y ella es la
  encargada de borrarlo. Si el diccionario que se pretende asignar es nulo o vacío este
  método no hace nada.

  @param gd Diccionario de parámetros
  */
  void setGroupDict(FLGroupByQueryDict *gd);

  /**
  Asigna un diccionario de grupos, al diccionario de grupos de la consulta.

  El diccionario de grupos del tipo FLParameterQueryDict , ya construido,
  es asignado como el nuevo diccionario de parámetros de la consulta, en el caso de que
  ya exista un diccionario de parámetros, este es destruido y sobreescrito por el nuevo.
  El diccionario pasado a este método pasa a ser propiedad de la consulta, y ella es la
  encargada de borrarlo. Si el diccionario que se pretende asignar es nulo o vacío este
  método no hace nada.

  @param pd Diccionario de parámetros
  */
  void setParameterDict(FLParameterQueryDict *pd);

  /**
  Este método muestra el contenido de la consulta, por la sálida estándar.

  Está pensado sólo para tareas de depuración
  */
  void showDebug();

  /**
  Obtiene el valor de un campo de la consulta.

  Dado un nombre de un campo de la consulta, este método devuelve un objeto QVariant
  con el valor de dicho campo. El nombre debe corresponder con el que se coloco en
  la parte SELECT de la sentenica SQL de la consulta.

  @param n Nombre del campo de la consulta
  @param raw Si TRUE y el valor del campo es una referencia a un valor grande
             (ver FLManager::storeLargeValue()) devuelve el valor de esa referencia,
             en vez de contenido al que apunta esa referencia
  */
  QVariant value(const QString &n, bool raw = false) const;
  
  
  /**
  Indica si un campo de la consulta es nulo o no

  Dado un nombre de un campo de la consulta, este método devuelve true si el campo de la consulta es nulo.
  El nombre debe corresponder con el que se coloco en
  la parte SELECT de la sentenica SQL de la consulta.

  @param n Nombre del campo de la consulta
  */
  bool isNull(const QString &n) const;

  /**
  Sobrecarga de FLSqlQuery::value(const QString &n, bool raw)

  @param i Posicion del campo
  */
  QVariant value(int i, bool raw = false) const;
  
  /**
  Devuelve el nombre de campo, dada su posicion en la consulta.

  @param p Posicion del campo en la consulta, empieza en cero y de izquierda
       a derecha
  @return Nombre del campo correspondiente. Si no existe el campo devuelve
      QString::null
  */
  QString posToFieldName(const int p) const;

  /**
  Devuelve la posición de una campo en la consulta, dado su nombre.

  @param n Nombre del campo
  @return Posicion del campo en la consulta. Si no existe el campo devuelve -1
  */
  int fieldNameToPos(const QString &n) const;

  /**
  Para obtener la lista de nombres de las tablas de la consulta.

  @return Lista de nombres de las tablas que entran a formar parte de la
      consulta
  */
  QStringList tablesList() const;

  /**
  Establece la lista de nombres de las tablas de la consulta

  @param tl Cadena de texto con los nombres de las tablas
        separados por comas, p.e. "tabla1,tabla2,tabla3"
  */
  void setTablesList(const QString &tl);

  /**
  Establece el valor de un parámetro.

  @param name Nombre del parámetro
  @param v Valor para el parámetros
  */
  void setValueParam(const QString &name, const QVariant &v);

  /**
  Obtiene el valor de un parámetro.

  @param name Nombre del parámetro.
  */
  QVariant valueParam(const QString &name) const;

  /**
  Redefinicion del método size() de QSqlQuery
  */
  int size() const;

  /**
  Para obtener la lista de definiciones de campos de la consulta

  @return Objeto con la lista de deficiones de campos de la consulta
  */
  FLTableMetaData::FLFieldMetaDataList *fieldMetaDataList();

#ifdef FL_DEBUG
  static long countRefQuery;
#endif

  /**
  Para obtener la base de datos sobre la que trabaja
  */
  FLSqlDatabase *db() const;

private:

  /**
  Privado
  */
  FLSqlQueryPrivate *d;

public:

  //
  // QSqlQuery
  //

  bool                isValid() const {
    return qry_.isValid();
  }
  bool                isActive() const {
    return qry_.isActive();
  }
  bool                isNull(int field) const {
    return qry_.isNull(field);
  }
  int                 at() const {
    return qry_.at();
  }
  QString             lastQuery() const {
    return qry_.lastQuery();
  }
  int                 numRowsAffected() const {
    return qry_.numRowsAffected();
  }
  QSqlError           lastError() const {
    return qry_.lastError();
  }
  bool                isSelect() const {
    return qry_.isSelect();
  }
  int                 QSqlQuery_size() const {
    return qry_.size();
  }
  const QSqlDriver   *driver() const {
    return qry_.driver();
  }
  const QSqlResult   *result() const {
    return qry_.result();
  }
  bool                isForwardOnly() const {
    return qry_.isForwardOnly();
  }
  void                setForwardOnly(bool forward) {
    qry_.setForwardOnly(forward);
  }

  bool        exec(const QString &query) {
    return qry_.exec(query);
  }
  QVariant    QSqlQuery_value(int i) const {
    return qry_.value(i);
  }

  bool        seek(int i, bool relative = FALSE) {
    return qry_.seek(i, relative);
  }
  bool        next() {
    return qry_.next();
  }
  bool        prev() {
    return qry_.prev();
  }
  bool        first() {
    return qry_.first();
  }
  bool        last() {
    return qry_.last();
  }

  // prepared query support
  bool                prepare(const QString &query) {
    return qry_.prepare(query);
  }
  void                bindValue(const QString &placeholder, const QVariant &val) {
    qry_.bindValue(placeholder, val);
  }
  void                bindValue(int pos, const QVariant &val) {
    qry_.bindValue(pos, val);
  }
  void                addBindValue(const QVariant &val) {
    qry_.addBindValue(val);
  }
  // remove these overloads in 4.0
  void                bindValue(const QString &placeholder, const QVariant &val, QSql::ParameterType type) {
    qry_.bindValue(placeholder, val, type);
  }
  void                bindValue(int pos, const QVariant &val, QSql::ParameterType type) {
    qry_.bindValue(pos, val, type);
  }
  void                addBindValue(const QVariant &val, QSql::ParameterType type) {
    qry_.addBindValue(val, type);
  }
  QVariant            boundValue(const QString &placeholder) const {
    return qry_.boundValue(placeholder);
  }
  QVariant            boundValue(int pos) const {
    return qry_.boundValue(pos);
  }
  QMap<QString, QVariant> boundValues() const {
    return qry_.boundValues();
  }
  QString             executedQuery() const {
    return qry_.executedQuery();
  }

private:

  QSqlQuery qry_;
};

class FLSqlQueryPrivate
{
public:

  FLSqlQueryPrivate();
  ~FLSqlQueryPrivate();

  /**
  Nombre de la consulta
  */
  QString name_;

  /**
  Parte SELECT de la consulta
  */
  QString select_;

  /**
  Parte FROM de la consulta
  */
  QString from_;

  /**
  Parte WHERE de la consulta
  */
  QString where_;

  /**
  Parte ORDER BY de la consulta
  */
  QString orderBy_;

  /**
  Lista de nombres de los campos
  */
  QStringList fieldList_;

  /**
  Lista de parámetros
  */
  FLSqlQuery::FLParameterQueryDict *parameterDict_;

  /**
  Lista de grupos
  */
  FLSqlQuery::FLGroupByQueryDict *groupDict_;

  /**
  Lista de nombres de las tablas que entran a formar
  parte en la consulta
  */
  QStringList tablesList_;

  /**
  Lista de con los metadatos de los campos de la consulta
  */
  FLTableMetaData::FLFieldMetaDataList *fieldMetaDataList_;

  /**
  Base de datos sobre la que trabaja
  */
  FLSqlDatabase *db_;
};

inline FLSqlDatabase *FLSqlQuery::db() const
{
  return d->db_;
}

inline void FLSqlQuery::setName(const QString &n)
{
  d->name_ = n;
}

inline QString FLSqlQuery::name() const
{
  return d->name_;
}

inline QString FLSqlQuery::select() const
{
  return d->select_;
}

inline QString FLSqlQuery::from() const
{
  return d->from_;
}

inline QString FLSqlQuery::where() const
{
  return d->where_;
}

inline QString FLSqlQuery::orderBy() const
{
  return d->orderBy_;
}

inline FLSqlQuery::FLParameterQueryDict *FLSqlQuery::parameterDict() const
{
  return d->parameterDict_;
}

inline FLSqlQuery::FLGroupByQueryDict *FLSqlQuery::groupDict() const
{
  return d->groupDict_;
}

inline QStringList FLSqlQuery::fieldList() const
{
  return d->fieldList_;
}

inline QStringList FLSqlQuery::tablesList() const
{
  return d->tablesList_;
}

#endif
