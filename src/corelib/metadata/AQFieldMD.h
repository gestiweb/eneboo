/***************************************************************************
AQFieldMD.h
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

#ifndef AQFIELDMD_H_
#define AQFIELDMD_H_

#include <AQCore/AQCore.h>

#include <QString>
#include <QVariant>
#include <QStringList>

class AQFieldMDPrivate;
class AQTableMD;
class AQTableMDPrivate;

/**
Contiene la meta-información de campos de la base de dato.
 
Solo tiene sentido cuando forma parte de un objeto AQTableMD
 
@author InfoSiAL S.L.
*/
class AQFieldMD
{
  friend class AQTableMD;
  friend class AQTableMDPrivate;

public:

  /**
  constructor.

  @param  n         Nombre del campo
  @param  a         Alias del campo, utilizado en etiquetas de los formularios
  @param  aN        TRUE si permite nulos (NULL), FALSE si los permite (NOT NULL)
  @param  iPK       TRUE si es clave primaria, FALSE si no es clave primaria, ser
                    clave primaria implica ser Indice y Único
  @param  t         Tipo del campo
  @param  l         Longitud del campo en caracteres, siempre que se de tipo cadena
                    de caracteres
  @param  c         Indica si el campo es calculado
  @param  v         Indica si el campo es visible
  @param  ed        Indica si el campo es editable
  @param  pI        Indica el número de dígitos de la parte entera
  @param  pD        Indica el número de decimales
  @param  iNX       TRUE si el campo es índice
  @param  uNI       TRUE si el campo determina registros únicos
  @param  coun      Indica si es un contador. Para referencias automáticas
  @param  defValue  Valor por defecto para el campo
  @param  oT        Indica si las modificaciones en el campo son fuera de transaccion
  @param  rX        Expresion regular utilizada como mascara de validacion
  @param  vG        Indica si el campo es visible en la rejilla de la tabla
  @param  gen       Indica si el campo es generado.
  @param  iCK       Indica si es clave compuesta
  */
  AQFieldMD( const QString & n, const QString & a, bool aN, bool iPK,
             int t, int l = 0, bool c = false, bool v = true,
             bool ed = false, int pI = 4, int pD = 0, bool iNX = false,
             bool uNI = false, bool coun = false, const QVariant & defValue = QVariant(),
             bool oT = false, const QString & rX = QString(), bool vG = true,
             bool gen = true, bool iCK = false );

  /**
  desctructor
  */
  ~AQFieldMD();

  /**
  Obtiene el nombre del campo.

  @return Nombre del campo
  */
  QString name() const;

  /**
  Obtiene el alias del campo.

  @return Alias del campo
  */
  QString alias() const;

  /**
  Obtiene si permite nulos.

  @return TRUE si permite nulos, FALSE en caso contrario
  */
  bool allowNull() const;

  /**
  Obtiene si es clave primaria.

  @return TRUE si es clave primaria, FALSE en caso contrario
  */
  bool isPrimaryKey() const;

  /**
  Obtiene si es clave compuesta.

  @return TRUE si es clave compuesta, FALSE en caso contrario
  */
  bool isCompoundKey() const;

  /**
  Obtiene el tipo del campo.

  @return El tipo del campo
  */
  int type() const;

  /**
  Obtiene la longitud del campo.

  @return La longitud del campo
  */
  int length() const;

  /**
  Obtiene si el campo es calculado.

  @return TRUE si el campo es calculado, FALSE en caso contrario
  */
  bool calculated() const;

  /**
  Establece si el campo es calculado.

  @param  c Valor TRUE si se quiere poner el campo como calculado, FALSE en caso contrario
  */
  void setCalculated( bool c ) const;

  /**
  Obtiene si el campo es editable.

  @return TRUE si el campo es editable, FALSE en caso contrario
  */
  bool editable() const;

  /**
  Establece si el campo es editable.

  @param  ed  Valor TRUE si se quiere que el campo sea editable, FALSE
              en caso contrario
  */
  void setEditable( bool ed ) const;

  /**
  Obtiene si el campo es visible.

  @return TRUE si el campo es visible, FALSE en caso contrario
  */
  bool visible() const;

  /**
  Obtiene si el campo es visible en la rejilla de la tabla.

  @return TRUE si el campo es visible en la rejilla de la tabla, FALSE en caso contrario
  */
  bool visibleGrid() const;

  /**
  @return TRUE si el campo es generado, es decir, se incluye en las consultas
  */
  bool generated() const;

  /**
  Establece si el campo es visible.

  @param  v Valor TRUE si se quiere poner el campo como visible, FALSE
            en caso contrario
  */
  void setVisible( bool v ) const;

  /**
  Obtiene el número de dígitos de la parte entera.

  @return El número de dígitos de la parte entera del campo
  */
  int partInteger() const;

  /**
  Obtiene si el número de dígitos de la parte decimal.

  @return El número de dígitos de la parte decimal del campo
  */
  int partDecimal() const;

  /**
  Obtiene si el campo es contador.

  @return TRUE si el campo es una referencia con contador
  */
  bool isCounter() const;

  /**
  Obtiene si el campo es índice.

  @return TRUE si el campo es índice, FALSE en caso contrario
  */
  bool isIndex() const;

  /**
  Obtiene si el campo determina registros únicos.

  @return TRUE si el campo determina registros únicos, FALSE en caso contrario
  */
  bool isUnique() const;

  /**
  Añade una relacion con otra tabla para este campo.

  Añade un nuevo objeto AQRelationMD, a la lista
  de relaciones para este campo.

  Hay que notar que para un campo solo puede existir una
  sola relacion del tipo M1 (muchos a uno), por lo que en
  el caso de que se quieran añadir varias relaciones de
  este tipo para el campo solo se tendrá en cuenta la primera.
  Relaciones del tipo 1M (uno a muchos) pueden existir todas
  las que sean necesarias. Ver AQRelationMD::Cardinality.

  @param  r Objeto AQRelationMD con la definicion de la
            relacion a añadir. Es tomado en propieda por lo que no debe ser borrado */
  void addRelationMD( AQRelationMD * r ) const;

  /**
  Para obtener la lista de definiciones de las relaciones.

  @return Objeto con la lista de deficiones de la relaciones del campo
  */
  AQRelationMDList relationList() const;

  /**
  Para obtener la relacion muchos a uno para este campo.

  @return Objeto  AQRelationMD con la descripcion de la relacion
                  muchos a uno para este campo
  */
  AQRelationMD * relationM1() const;

  /**
  Establece un campo asociado para este campo, y el nombre
  del campo de la tabla foránea que se debe utilizar para filtrar según
  el valor del campo asociado.

  Ver AQFieldMD::associatedField_
  Ver AQFieldMD::associatedFieldFilterTo_

  @param  r Objeto AQFieldMD que define el campo que se quiere asociar a este
  @param  f Nombre del campo a aplicar el filtro
  */
  void setAssociatedField( AQFieldMD * r, const QString & f ) const;

  /**
  Sobrecargada por conveniencia

  @param  r Nombre del campo que se quiere asociar a este
  @param  f Nombre del campo a aplicar el filtro
  */
  void setAssociatedField( const QString & rName, const QString & f ) const;

  /**
  Devuelve el campo asociado para este campo.

  Ver AQFieldMD::associatedField_

  @return Objeto AQFieldMD que define el campo asociado a este, o 0
          si no hay campo asociado
  */
  AQFieldMD * associatedField() const;

  /**
  Devuelve el nombre del campo que hay que filtrar según el campo asociado.

  Ver AQFieldMD::associatedFieldFilterTo_

  @return Nombre del campo de la tabla foránea M-1, al que hay que aplicar el filtro
          según el valor del campo asociado
  */
  QString associatedFieldFilterTo() const;

  /**
  Devuelve el nombre del campo asociado este.

  Ver FLFieldMetaData::associatedField_

  @return Nombre del campo asociado
  */
  QString associatedFieldName() const;

  /**
  Devuelve el valor por defecto para el campo.

  @return Valor que se asigna por defecto al campo
  */
  QVariant defaultValue() const;

  /**
  Devuelve si el campo se modifica fuera de transaccion,
  ver AQFieldMD::outTransaction_.

  @return TRUE si el campo se modifica fuera de transaccion, FALSE en caso contrario
  */
  bool outTransaction() const;

  /**
  Devuelve la expresion regular que sirve como mascara de validacion para el campo.

  @return Cadena de caracteres que contiene una expresion regular, utilizada como
          mascara para validar los valores introducidos en el campo
  */
  QString regExpValidator() const;

  /**
  Devuelve la lista de opciones para el campo

  @return Lista de opciones del campo
  */
  QStringList optionsList() const;

  /**
  Establece la lista de opciones para el campo

  @param  ol  Cadena de texto con la opciones para el campo
              separada por comas, p.e. "opcion1,opcion2,opcion3"
  */
  void setOptionsList( const QString & ol ) const;

  /**
   Establece el objeto FLTableMetaData al que pertenece
   */
  void setMetadata( AQTableMD * mtd );

  /**
  Obtiene el objeto FLTableMetaData al que pertenece
  */
  AQTableMD * metadata() const;

  /**
  Obtiene el tipo del campo convertido a un tipo equivalente de la clase QVariant
  */
  static QVariant::Type aqDecodeType( int aqtype );

private:

  /**
  Privado
  */
  AQFieldMDPrivate * d;
};

#endif /*AQFIELDMD_H_*/
