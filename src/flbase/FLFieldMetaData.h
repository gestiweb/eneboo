/***************************************************************************
                           FLFieldMetaData.h
                          -------------------
 begin                : Mon Jul 2 2001
 copyright            : (C) 2001-2005 by InfoSiAL S.L.
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

#ifndef FLFIELDMETADATA_H
#define FLFIELDMETADATA_H

#include <qvariant.h>
#include <qstring.h>
#include <qptrlist.h>

#include "AQGlobal.h"

class FLTableMetaData;
class FLRelationMetaData;
class FLFieldMetaDataPrivate;

/**
Mantiene la descripcion/metadatos de una campo de una tabla.

Esta clase solo tiene sentido cuando es parte de un objeto FLTableMetaData

@author InfoSiAL S.L.
*/
class AQ_EXPORT FLFieldMetaData : public QShared
{
public:

  /**
  Constantes de tipos especiales no contemplados en QVariant
  */
  enum specialType {
    /**
    Tipo SERIAL o autoincremento
    */
    Serial = 100,
    /**
    Tipo de campo unlock: por Andrés Otón Urbano
    */
    Unlock = 200,
    /**
    Tipo selector de registro
    */
    Check = 300
  };

#ifdef FL_DEBUG
  static long count_;
#endif

  /**
  constructor.

  @param n Nombre del campo
  @param a Alias del campo, utilizado en etiquetas de los formularios
  @param aN TRUE si permite nulos (NULL), FALSE si los permite (NOT NULL)
  @param iPK TRUE si es clave primaria, FALSE si no es clave primaria, ser
        clave primaria implica ser Indice y Único
  @param t Tipo del campo
  @param l Longitud del campo en caracteres, siempre que se de tipo cadena
       de caracteres
  @param c Indica si el campo es calculado
  @param v Indica si el campo es visible
  @param ed Indica si el campo es editable
  @param pI Indica el número de dígitos de la parte entera
  @param pD Indica el número de decimales
  @param iNX TRUE si el campo es índice
  @param uNI TRUE si el campo determina registros únicos
  @param coun Indica si es un contador. Para referencias automáticas
  @param defValue Valor por defecto para el campo
  @param oT Indica si las modificaciones en el campo son fuera de transaccion
  @param rX Expresion regular utilizada como mascara de validacion
  @param vG Indica si el campo es visible en la rejilla de la tabla
  @param gen Indica si el campo es generado.
  @param iCK Indica si es clave compuesta
  */
  FLFieldMetaData(const QString &n, const QString &a, bool aN, bool iPK, int t,
                  int l = 0, bool c = false, bool v = true, bool ed = false,
                  int pI = 4, int pD = 0, bool iNX = false, bool uNI = false,
                  bool coun = false, const QVariant &defValue = QVariant(), bool oT = false,
                  const QString &rX = QString::null, bool vG = true, bool gen = true, bool iCK = false);
  FLFieldMetaData(const FLFieldMetaData *other);

  /**
  desctructor
  */
  ~FLFieldMetaData();

  /**
  Obtiene el nombre del campo.

  @return Nombre del campo
  */
  QString name() const;

  /**
  Establece el nombre para el campo

  @param n Nombre del campo
  */
  void setName(const QString &n);

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
  void setIsPrimaryKey(bool b);

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

  @param c Valor TRUE si se quiere poner el campo como calculado, FALSE en caso contrario
  */
  void setCalculated(bool c);

  /**
  Obtiene si el campo es editable.

  @return TRUE si el campo es editable, FALSE en caso contrario
  */
  bool editable() const;

  /**
  Establece si el campo es editable.

  @param ed Valor TRUE si se quiere que el campo sea editable, FALSE
        en caso contrario
  */
  void setEditable(bool ed);

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

  @param v Valor TRUE si se quiere poner el campo como visible, FALSE
       en caso contrario
  */
  void setVisible(bool v);

  /**
  Establece si el campo es visible en la rejilla de la tabla.

  @param v Valor TRUE si se quiere poner el campo como visible, FALSE
       en caso contrario
  */
  void setVisibleGrid(bool v);

  /**
  Obtiene el número de dígitos de la parte entera.

  @return El número de dígitos de la parte entera del campo
  */
  int partInteger() const;

  /**
  Obtiene el número de dígitos de la parte decimal.

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
  Tipo de datos lista de relaciones
  */
  typedef QPtrList<FLRelationMetaData> FLRelationMetaDataList;

  /**
  Añade una relacion con otra tabla para este campo.

  Añade un nuevo objeto FLRelationMetaData, a la lista
  de relaciones para este campo.

  Hay que notar que para un campo solo puede existir una
  sola relacion del tipo M1 (muchos a uno), por lo que en
  el caso de que se quieran añadir varias relaciones de
  este tipo para el campo solo se tendrá en cuenta la primera.
  Relaciones del tipo 1M (uno a muchos) pueden existir todas
  las que sean necesarias. Ver FLRelationMetaData::Cardinality.

  @param r Objeto FlRelationMetaData con la definicion de la
       relacion a añadir */
  void addRelationMD(FLRelationMetaData *r);

  /**
  Para obtener la lista de definiciones de las relaciones.

     No incluye la relacion M1

  @return Objeto con la lista de deficiones de la relaciones del campo
  */
  const FLRelationMetaDataList *relationList() const;

  /**
  Para obtener la relacion muchos a uno para este campo.

     No incluida en relationList()

  @return Objeto FLRelationMetaData con la descripcion de la relacion
      muchos a uno para este campo
  */
  const FLRelationMetaData *relationM1() const;

  /**
  Establece un campo asociado para este campo, y el nombre
  del campo de la tabla foránea que se debe utilizar para filtrar según
  el valor del campo asociado.

  Ver FLFieldMetaData::associatedField_
  Ver FLFieldMetaData::associatedFieldFilterTo_

  @param r Objeto FLFieldMetaData que define el campo que se quiere asociar a este
  @param f Nombre del campo a aplicar el filtro
  */
  void setAssociatedField(FLFieldMetaData *r, const QString &f);

  /**
  Sobrecargada por conveniencia

  @param r Nombre del campo que se quiere asociar a este
  @param f Nombre del campo a aplicar el filtro
  */
  void setAssociatedField(const QString &rName, const QString &f);

  /**
  Devuelve el campo asociado para este campo.

  Ver FLFieldMetaData::associatedField_

  @return Objeto FLFieldMetaData que define el campo asociado a este, o 0
      si no hay campo asociado
  */
  FLFieldMetaData *associatedField() const;

  /**
  Devuelve el nombre del campo que hay que filtrar según el campo asociado.

  Ver FLFieldMetaData::associatedFieldFilterTo_

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
  QVariant defaultValue();

  /**
  Devuelve si el campo se modifica fuera de transaccion,
  ver FLFieldMetaData::outTransaction_.

  @return TRUE si el campo se modifica fuera de transaccion, FALSE en caso contrario
  */
  bool outTransaction();

  /**
  Devuelve la expresion regular que sirve como mascara de validacion para el campo.

  @return Cadena de caracteres que contiene una expresion regular, utilizada como
      mascara para validar los valores introducidos en el campo
  */
  QString regExpValidator();

  /**
  Devuelve la lista de opciones para el campo

  @return Lista de opciones del campo
  */
  QStringList optionsList();

  /**
  Establece la lista de opciones para el campo

  @param ol Cadena de texto con la opciones para el campo
        separada por comas, p.e. "opcion1,opcion2,opcion3"
  */
  void setOptionsList(const QString &ol);

  /**
  Obtiene si el campo es de tipo Check
  */
  bool isCheck() const;

  /**
  Obtiene si el campo tiene lista de opciones
  */
  bool hasOptionsList() const;

  /**
  Ver FLFieldMetaData::fullyCaclulated_
  */
  bool fullyCalculated() const;
  void setFullyCalculated(bool c);

  /**
  Ver FLFieldMetaData::trimmed_
  */
  bool trimed() const;
  void setTrimed(bool t);

  /**
  Establece el objeto FLTableMetaData al que pertenece
  */
  void setMetadata(FLTableMetaData *mtd);

  /**
  Obtiene el objeto FLTableMetaData al que pertenece
  */
  FLTableMetaData *metadata() const;

  /**
  Obtiene el tipo del campo convertido a un tipo equivalente de la clase QVariant
  */
  static QVariant::Type flDecodeType(int fltype);

private:

  /**
  Privado
  */
  FLFieldMetaDataPrivate *d;

  void copy(const FLFieldMetaData *other);

  friend class FLTableMetaData;
  friend class FLTableMetaDataPrivate;
};

class FLFieldMetaDataPrivate
{
public:

  FLFieldMetaDataPrivate(const QString &n, const QString &a, bool aN, bool iPK, int t,
                         int l, bool c, bool v, bool ed, int pI, int pD,
                         bool iNX, bool uNI, bool coun, const QVariant &defValue, bool oT,
                         const QString &rX, bool vG, bool gen, bool iCK);
  FLFieldMetaDataPrivate();

  ~FLFieldMetaDataPrivate();

  /**
  Limpia la lista de definiciones de relaciones
  */
  void clearRelationList();

  /**
  Nombre del campo en la tabla
  */
  QString fieldName_;

  /**
  Alias o mote para el campo, usado como
  etiqueta de campos en los formularios
  */
  QString alias_;

  /**
  Almacena si el campo permite ser nulo
  */
  bool allowNull_;

  /**
  Almacena si el campo es clave primaria
  */
  bool isPrimaryKey_;

  /**
  Tipo del campo
  */
  int type_;

  /**
  Longitud del campo
  */
  int length_;

  /**
  Indica si el campo es calculado de forma diferida.
  Esto indica que el campo se calcula al editar o insertar un registro, en el commit.
  */
  bool calculated_;

  /**
  Indica si el campo es totalmente calculado.
  Esto indica que el valor campo del campo es dinámico y se calcula en cada refresco.
  Un campo totalmente calculado implica que es generado.
  */
  bool fullyCalculated_;

  /**
  Indica que al leer el campo de la base de datos los espacios mas a la derecha
  son eliminados.
  */
  bool trimmed_;

  /**
  Indica si el campo es visible
  */
  bool visible_;

  /**
  Indica si el campo es editable
  */
  bool editable_;

  /**
  Indica el número de dígitos de la parte entera
  */
  int partInteger_;

  /**
  Indica el númeor de dígitos de la parte decimal
  */
  int partDecimal_;

  /**
  Indica si el campo es índice
  */
  bool isIndex_;

  /**
  Indica si el campo es único
  */
  bool isUnique_;

  /**
  Indica si el campo es un contador de referencia y abanq en el
  momento de insertar un registro debe intentar calcular cual sería el
  siguiente numero.

  @author Andrés Otón Urbano (andresoton@eresmas.com)
  */
  bool contador_;

  /**
  Lista de relaciones para este campo
  */
  FLFieldMetaData::FLRelationMetaDataList *relationList_;

  /**
  Mantiene, si procede, la relación M1 (muchos a uno)
  para el campo (solo puede haber una relacion de este tipo para un campo)
  */
  FLRelationMetaData *relationM1_;

  /**
  Asocia este campo con otro, para efectuar filtros en búsquedas.

  El campo que se asocia a este debe tener una relación M-1.
  Este campo también debe tener una relación M-1. Al asociar un campo a este,
  las búsquedas mediante los botones de búsqueda en los formularios de edición
  de registros vendrán condicionadas por el valor del campo asociado en el
  momento de realizar dicha búsqueda. Cuando se realiza una búsqueda para
  este campo la tabla relacionada con él (M-1) será mostrada para elegir un
  registro de todos los posibles, en el caso normal se muestran todos los registros,
  pero cuando se asocia un campo sólo se muestran aquellos registros que satisfagan el
  valor del campo asociado. Ejemplo : En la tabla albaranes asociamos el campo
  'codemporig' al campo 'codalmorig' (NO es lo mismo que asociar 'codalmorig'
  a 'codemporig') cuando abrimos el formulario de albaranes elegimos una empresa
  origen (codemporig), cuando vayamos a elegir un almacen origen (codalmorig) sólo
  se podrá elegir entre los almacenes que son de la empresa origen , ya que el formulario
  de búsqueda sólo se mostrarán los almacenes cuyo código de empresa
  (ver FLFieldMetaData::associatedFieldFilterTo_) sea igual al valor de la empresa origen
  elegida (codemporig)
  */
  FLFieldMetaData *associatedField_;
  QString associatedFieldName_;

  /**
  Nombre del campo que se debe filtra según el campo asociado.

  Esta propiedad sólo tiene sentido cuando hay un campo asociado a este,
  ver FLFieldMetaData ::associatedField_ , y si ese campo tiene una relacion M-1. Indica
  el nombre del campo de la tabla foránea en la relación M-1, que se debe utilizar para filtrar
  los registros según el valor del campo asociado. Ejemplo : En la tabla albaranes asociamos el campo
  'codemporig' al campo 'codalmorig' (NO es lo mismo que asociar 'codalmorig'
  a 'codemporig'), e indicamos que el campo de filtro es 'codempresa' de la tabla relacionada M-1 con el
  campo 'codalmorig' (Almacenes) . Cuando abrimos el formulario de albaranes elegimos una empresa
  origen (codemporig), cuando vayamos a elegir un almacen origen (codalmorig) sólo se podrá elegir
  entre los almacenes que son de la empresa origen, ya que el formulario de búsqueda sólo se mostrarán
  los almacenes cuyo código de empresa (el campo indicado de filtro ) sea igual al valor de la empresa
  origen elegida (codemporig)
  */
  QString associatedFieldFilterTo_;

  /**
  Valor por defecto para el campo
  */
  QVariant defaultValue_;

  /**
  Lista de opciones para el campo
  */
  QStringList optionsList_;

  /**
  Indica si las modificaciones del campo se hacen fuera de cualquier transaccion.

  Al estar activado este flag, todos los cambios en el valor de este campo se
  realizan fuera de la transaccion y de forma exclusiva. Es decir los cambios
  realizados en el campo son inmediatamente reflejados en la tabla sin esperar a
  que se termine transaccion, y de forma exclusiva (bloqueando el registro al que
  pertenece el campo mientras se modifica). Esto permite en el acto hacer visibles
  para todas las demas conexiones de la base de datos los cambios realizados en un campo.
  Hay que tener en cuenta que al tener el campo esta caracteristica especial de modificarse
  fuera de la transaccion, el "rollback" no tendra efecto sobre los cambios realizados
  en el y siempre permanecera en la base de datos la ultima modificacion efectuada en
  el campo.
  */
  bool outTransaction_;

  /**
  Almacena la expresion regular que sirve como mascara de validacion para el campo.
  */
  QString regExpValidator_;

  /**
  Indica si el campo debe ser visible en la rejilla de la tabla.
  */
  bool visibleGrid_;

  /**
  Indica si el campo es generado, es decir, se incluye en las consultas
  */
  bool generated_;

  /**
  Almacena si el campo es clave compuesta
  */
  bool isCompoundKey_;

  /**
  Indica si el campo toma su valor de una lista de opciones
  */
  bool hasOptionsList_;

  /**
  Objeto FLTableMetaData al que pertenece
  */
  FLTableMetaData *mtd_;
};

inline QString FLFieldMetaData::name() const
{
  return d->fieldName_;
}

inline QString FLFieldMetaData::alias() const
{
  return d->alias_;
}

inline bool FLFieldMetaData::allowNull() const
{
  return d->allowNull_;
}

inline bool FLFieldMetaData::isPrimaryKey() const
{
  return d->isPrimaryKey_;
}

inline void FLFieldMetaData::setIsPrimaryKey(bool b)
{
  d->isPrimaryKey_ = b;
}

inline int FLFieldMetaData::type() const
{
  return (d->type_ == FLFieldMetaData::Check ? QVariant::Bool : d->type_);
}

inline int FLFieldMetaData::length() const
{
  return d->length_;
}

inline bool FLFieldMetaData::calculated() const
{
  return d->calculated_;
}

inline void FLFieldMetaData::setCalculated(bool c)
{
  d->calculated_ = c;
}

inline bool FLFieldMetaData::editable() const
{
  return d->editable_;
}

inline void FLFieldMetaData::setEditable(bool ed)
{
  d->editable_ = ed;
}

inline bool FLFieldMetaData::visible() const
{
  return d->visible_;
}

inline bool FLFieldMetaData::visibleGrid() const
{
  return d->visibleGrid_;
}

inline bool FLFieldMetaData::generated() const
{
  return d->generated_;
}

inline void FLFieldMetaData::setVisible(bool v)
{
  d->visible_ = v;
}

inline void FLFieldMetaData::setVisibleGrid(bool v)
{
  d->visibleGrid_ = v;
}

inline int FLFieldMetaData::partInteger() const
{
  return d->partInteger_;
}

inline int FLFieldMetaData::partDecimal() const
{
  return d->partDecimal_;
}

inline bool FLFieldMetaData::isCounter() const
{
  return d->contador_;
}

inline bool FLFieldMetaData::isIndex() const
{
  return d->isIndex_;
}

inline bool FLFieldMetaData::isUnique() const
{
  return d->isUnique_;
}

inline const FLFieldMetaData::FLRelationMetaDataList *FLFieldMetaData::relationList() const
{
  return d->relationList_;
}

inline const FLRelationMetaData *FLFieldMetaData::relationM1() const
{
  return d->relationM1_;
}

inline void FLFieldMetaData::setAssociatedField(FLFieldMetaData *r, const QString &f)
{
  d->associatedField_ = r;
  d->associatedFieldFilterTo_ = f;
}

inline void FLFieldMetaData::setAssociatedField(const QString &rName, const QString &f)
{
  d->associatedFieldName_ = rName;
  d->associatedFieldFilterTo_ = f;
}

inline FLFieldMetaData *FLFieldMetaData::associatedField() const
{
  return d->associatedField_;
}

inline QString FLFieldMetaData::associatedFieldFilterTo() const
{
  return d->associatedFieldFilterTo_;
}

inline QString FLFieldMetaData::associatedFieldName() const
{
  return d->associatedFieldName_;
}

inline QVariant FLFieldMetaData::defaultValue()
{
  return d->defaultValue_;
}

inline bool FLFieldMetaData::outTransaction()
{
  return d->outTransaction_;
}

inline QString FLFieldMetaData::regExpValidator()
{
  return d->regExpValidator_;
}

inline QStringList FLFieldMetaData::optionsList()
{
  return d->optionsList_;
}

inline bool FLFieldMetaData::isCompoundKey() const
{
  return d->isCompoundKey_;
}

inline bool FLFieldMetaData::isCheck() const
{
  return (d->type_ == FLFieldMetaData::Check);
}

inline bool FLFieldMetaData::hasOptionsList() const
{
  return d->hasOptionsList_;
}

inline bool FLFieldMetaData::fullyCalculated() const
{
  return d->fullyCalculated_;
}

inline void FLFieldMetaData::setFullyCalculated(bool c)
{
  d->fullyCalculated_ = c;
  if (c)
    d->generated_ = true;
}

inline bool FLFieldMetaData::trimed() const
{
  return d->trimmed_;
}

inline void FLFieldMetaData::setTrimed(bool t)
{
  d->trimmed_ = t;
}

inline void FLFieldMetaData::setMetadata(FLTableMetaData *mtd)
{
  d->mtd_ = mtd;
}

inline FLTableMetaData *FLFieldMetaData::metadata() const
{
  return d->mtd_;
}

#endif
