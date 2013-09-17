/***************************************************************************
AQRelationMD.h
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

#ifndef AQRELATIONMD_H_
#define AQRELATIONMD_H_

#include <AQCore/AQCore.h>

#include <QString>

class AQRelationMDPrivate;

/**
Mantiene la  meta-información de las relaciones entre campos.
 
Esta clase solo tiene sentido cuando es parte de un objeto AQFieldMD.
 
 IMPORTANTE:  Aunque las relaciones gráficamente sólo se dibujan en un sentido,
              ya que el otro se obvia, cuando haya que definirlas con código es preciso
              indicar la relación en ambos sentidos.
 
 Ejemplo:
    Si tenemos ALMACENES <-M----1> PAISES, tenemos que especificar
    que ALMACENES está relacionado M1 con PAISES, y también que PAISES esta
    relacionado 1M con ALMACENES, en código quedaría así:
 
    - Para la tabla ALMACENES
    \code
    ....
    AQTableMD *tmd = new AQTableMD (n, tr ("Almacenes"), "almacenes.ui");
    AQFieldMD *f;
    AQRelationMD *r;
 
    f = new AQFieldMD ("codpais", tr ("Pais"), true, false, QVariant::String, 3);
    r = new AQRelationMD ("flpaises", "codpais", AQRelationMD::RELATION_M1);
    f->addRelationMD (r);
    tmd->addFieldMD (f);
    ....
    \endcode
 
    - Para la tabla PAISES
    \code
    ....
    AQTableMD *tmd = new AQTableMD (n, tr ("Paises"), "paises.ui");
    AQFieldMD *f;
    AQRelationMD *r;
 
    f = new AQFieldMD ("codpais", tr ("Código"), false, true, QVariant::String, 20);
    r = new AQRelationMD ("flalmacenes", "codpais", AQRelationMD::RELATION_1M);
    f->addRelationMD (r);
    tmd->addFieldMD (f);
    ....
    \endcode
 
@author InfoSiAL S.L.
*/
class AQRelationMD
{

public:

  /**
  constructor

  @param fT Tabla foránea relacionada
  @param fF Campo foráneo relacionado
  @param rC Cardinalidad de la relacion
  @param dC Borrado en cascada, sólo se tiene en cuenta en cardinalidades M1
  @param uC Actualizaciones en cascada, sólo se tiene en cuenta en cardinalidades M1
  @param cI Chequeos de integridad sobre la relacion
  */
  AQRelationMD( const QString & fT, const QString & fF, int rC,
                bool dC = false, bool uC = false, bool cI = true );

  /**
  destructor
  */
  ~AQRelationMD();

  /**
  Establece el nombre del campo relacionado.

  @param fN Nombre del campo relacionado
  */
  void setField( const QString & fN ) const;

  /**
  Obtiene en el nombre del campo de la relacion.

  @return Devuelve el nombre del campo relacionado
  */
  QString field() const;

  /**
  Obtiene el nombre de la tabla foránea.

  @return Devuelve el nombre de la tabla de la base de datos con la que se está relacionada
  */
  QString foreignTable() const;

  /**
  Obtiene el nombre de la campo foráneo.

  @return Devuelve el nombre del campo de la tabla foránea con la que está relacionada
  */
  QString foreignField() const;

  /**
  Obtiene la cardinalidad de la relacion.

  @return Devuelve la cardinalidad de la relacion, mirando desde la tabla donde se
          define este objeto hacia la foránea
  */
  int cardinality() const;

  /**
  Obtiene si la relación implica borrados en cascada, sólo se tiene en cuenta en cardinalidades M1.

  @return Devuelve TRUE si la relacion implica borrados en cascada, FALSE en caso contrario
  */
  bool deleteCascade() const;

  /**
  Obtiene si la relación implica modificaciones en cascada, sólo se tiene en cuenta en cardinalidades M1.

  @return Devuelve TRUE si la relacion implica modificaciones en cascada, FALSE en caso contrario
  */
  bool updateCascade() const;

  /**
  Obtiene si se deben aplicar la reglas de integridad sobre la relación
  */
  bool checkIn() const;

private:

  /**
  Privado
  */
  AQRelationMDPrivate * d;
};

#endif /*AQRELATIONMD_H_*/
