/***************************************************************************
AQTableMD.h
-------------------
begin                : 06/08/2007
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

#ifndef AQTABLEMD_H_
#define AQTABLEMD_H_

#include <AQCore/AQCore.h>

#include <QString>
#include <QVariant>

class AQTableMDPrivate;
class AQCompKeyMD;

/**
Mantiene la meta-información de una tabla de la base de datos.
 
Esta clase mantienen la definicion de
ciertas caracteristicas de una tabla de la base
de datos.
 
Adicionalmente puede ser utilizada para la definición de
los metadatos de una consulta, ver AQTableMD::query().
 
@author InfoSiAL S.L.
*/
class AQTableMD : public QObject
{
  Q_OBJECT

public:

  /**
  constructor

  @param n Nombre de la tabla a definir
  @param a Alias de la tabla, utilizado en formularios
  @param q Nombre de la consulta de la que define sus metadatos
  */
  AQTableMD( const QString & n, const QString & a, const QString & q = QString() );

  /**
  destructor
  */
  ~AQTableMD();

  /**
  Obtiene el nombre de la tabla

  @return El nombre de la tabla que se describe
  */
  QString name() const;

  /**
  Establece el nombre de la tabla

  @param n Nombre de la tabla
  */
  void setName( const QString & n );

  /**
  Obtiene el alias asociado a la tabla
  */
  QString alias() const;

  /**
  Obtiene el nombre de la consulta de la que define sus metadatos.

  El nombre corresponderá a la definición de una consulta mediante
  (fichero .qry). Si el nombre de la consulta está definido entonces
  el nombre de la tabla correponderá a la tabla principal de la consulta
  cuando esta referencie a varias tablas.
  */
  QString query() const;

  /**
  Obtiene si define los metadatos de una consulta
  */
  bool isQuery() const;

  /**
  Añade la descripción de un campo a lista de descripciones de campos, de esta descripcion de tabla.

  El objeto 'f' es toma en propiedad por lo que no debe ser borrado

  @param f Objeto AQFieldMD con la descripción del campo a añadir
  */
  void addFieldMD( AQFieldMD * f );

  /**
  Elimina la descripción de un campo de la lista de descripciones de campos.

  @param fN Nombre del campo a eliminar
  */
  void removeFieldMD( const QString & fN );

  /**
  Establece la clave compuesta de esta tabla.

  @param cK Objeto AQCompKeyMD con la descripción de la clave compuesta a añadir
  */
  void setCompoundKey( const AQCompKeyMD & cK );

  /**
  Obtiene el nombre del campo que es clave primaria para esta tabla.

  @param prefixTable Si es TRUE se añade un prefijo con el nombre de la tabla; nombretabla.nombrecampo
  */
  QString primaryKey( bool prefixTable = false ) const;

  /**
  Obtiene el alias de un campo a partir de su nombre.

  @param fN Nombre del campo
  */
  QString fieldNameToAlias( const QString & fN ) const;

  /**
  Obtiene el nombre de un campo a partir de su alias.

  @param aN Nombre del alias del campo
  */
  QString fieldAliasToName( const QString & aN ) const;

  /**
  Obtiene el tipo de un campo a partir de su nombre.

  @param fN Nombre del campo
  */
  int fieldType( const QString & fN ) const;

  /**
  Obtiene si un campo es clave primaria partir de su nombre.

  @param fN Nombre del campo
  */
  bool fieldIsPrimaryKey( const QString & fN ) const;

  /**
  Obtiene si un campo es índice a partir de su nombre.

  @param fN Nombre del campo
  */
  bool fieldIsIndex( const QString & fN ) const;

  /**
  Obtiene si un campo es contador.

  @param fN Nombre del campo
  */
  bool fieldIsCounter( const QString & fN ) const;

  /**
  Obtiene si un campo puede ser nulo

  @param fN Nombre del campo
  */
  bool fieldAllowNull( const QString & fN ) const;

  /**
  Obtiene si un campo es único a partir de su nombre.

  @param fN Nombre del campo
  */
  bool fieldIsUnique( const QString & fN ) const;

  /**
  Obtiene el nombre de la tabla foránea relacionada con un campo de esta tabla mediante
  una relacion M1 (muchos a uno).

  @param  fN  Campo de la relacion M1 de esta tabla, que se supone que esta relacionado
              con otro campo de otra tabla
  @return El nombre de la tabla relacionada M1, si hay relacion para el campo, o una cadena
          vacia sin el campo no está relacionado
  */
  QString fieldTableM1( const QString & fN ) const;

  /**
  Obtiene el nombre del campo de la tabla foránea relacionado con el indicado mediante
  una relacion M1 (muchos auno).

  @param  fN  Campo de la relacion M1 de esta tabla, que se supone que esta relacionado
              con otro campo de otra tabla
  @return El nombre del campo foráneo relacionado con el indicado
  */
  QString fieldForeignFieldM1( const QString & fN ) const;

  /**
  Obtiene el objeto relación que definen dos campos.

  @param  fN    Nombre del campo de esta tabla que forma parte de la relación
  @param  fFN   Nombre del campo foráneo a esta tabla que forma parte de la relación
  @param  fTN   Nombre de la tabla foránea
  @return Devuelve un objeto FLRelationMetaData con la información de la relación, siempre y
          cuando esta exista. Si no existe devuelve 0
  */
  AQRelationMD * relation( const QString & fN, const QString & fFN, const QString & fTN ) const;

  /**
  Obtiene la longitud de un campo a partir de su nombre.

  @param fN Nombre del campo
  */
  int fieldLength( const QString & fN ) const;

  /**
  Obtiene el número de dígitos de la parte entera de un campo a partir de su nombre.

  @param fN Nombre del campo
  */
  int fieldPartInteger( const QString & fN ) const;

  /**
  Obtiene el número de dígitos de la parte decimal de un campo a partir de su nombre.

  @param fN Nombre del campo
  */
  int fieldPartDecimal( const QString & fN ) const;

  /**
  Obtiene si un campo es calculado.

  @param fN Nombre del campo
  */
  bool fieldCalculated( const QString & fN ) const;

  /**
  Obtiene si un campo es visible.

  @param fN Nombre del campo
  */
  bool fieldVisible( const QString & fN ) const;

  /** Obtiene los metadatos de un campo.

  @param fN Nombre del campo
  @return Un objeto AQFieldMD con lainformación o metadatos de un campo dado
  */
  AQFieldMD * field( const QString & fN ) const;

  /**
  Para obtener la lista de definiciones de campos.

  @return Objeto con la lista de deficiones de campos de la tabla
  */
  AQFieldMDHash fieldList() const;

  /**
  Para obtener una cadena con los nombres de los campos separados por comas.

  @param prefixTable  Si es TRUE se añade un prefijo a cada campo con el nombre de la tabla; 
                      nombretabla.nombrecampo
  @return Cadena de caracteres con los nombres de los campos separados por comas
  */
  QString fieldList( bool prefixTable ) const;

  /**
  Obtiene la lista de campos de una clave compuesta, a partir del nombre de
  un campo del que se quiere averiguar si está en esa clave compuesta.

  @param  fN  Nombre del campo del que se quiere averiguar si pertenece a una clave compuesta.
  @return Si el campo pertenece a una clave compuesta, devuelve la lista de campos
          que forman dicha clave compuesta, incluido el campo consultado. En el caso
          que el campo consultado no pertenezca a ninguna clave compuesta devuelve una
          lista vacia
  */
  AQFieldMDHash fieldListOfCompoundKey( const QString & fN ) const;

  /**
  Lista de nombres de campos de la tabla que son del tipo AQCore::Unlock
  */
  QStringList fieldsNamesUnlock() const;

private:

  /**
  Privado
  */
  AQTableMDPrivate * d;
};

#endif /*AQTABLEMD_H_*/
