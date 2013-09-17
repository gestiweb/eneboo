/***************************************************************************
AQFieldMD_p.h
-------------------
begin                : 21/01/2009
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

#ifndef AQFIELDMD_P_H_
#define AQFIELDMD_P_H_

class AQFieldMDPrivate
{
public:

  AQFieldMDPrivate( const QString & n, const QString & a, bool aN,
                    bool iPK, int t, int l, bool c,
                    bool v, bool ed, int pI, int pD,
                    bool iNX, bool uNI, bool coun, const QVariant & defValue,
                    bool oT, const QString & rX, bool vG, bool gen, bool iCK );
  ~AQFieldMDPrivate();

  /**
  Limpiar
  */
  void clear();

  /**
  Nombre del campo en la tabla
  */
  QString name_;

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
  Indica si el campo es calculado
  */
  bool calculated_;

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
  Indica si el campo es un contador
  */
  bool isCounter_;

  /**
  Lista de relaciones para este campo
  */
  AQRelationMDList relationList_;

  /**
  Mantiene, si procede, la relación M1 (muchos a uno)
  para el campo (solo puede haber una relacion de este tipo para un campo)
  */
  AQRelationMD * relationM1_;

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
  (ver AQFieldMD::associatedFieldFilterTo_) sea igual al valor de la empresa origen
  elegida (codemporig)
  */
  AQFieldMD * associatedField_;
  QString associatedFieldName_;

  /**
  Nombre del campo que se debe filtra según el campo asociado.

  Esta propiedad sólo tiene sentido cuando hay un campo asociado a este,
  ver AQFieldMD ::associatedField_ , y si ese campo tiene una relacion M-1. Indica
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
  Objeto FLTableMetaData al que pertenece
  */
  AQTableMD * mtd_;
};

#endif /*AQFIELDMD_P_H_*/
