/***************************************************************************
AQAccessControlTable.cpp
-------------------
begin                : 28/01/2009
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

#include <AQCore/AQCore.h>
#include <AQCore/AQTableMD.h>
#include <AQCore/AQFieldMD.h>

#include "AQAccessControlTable.h"
#include "../../acl/AQAccessControl_p.h"

QString AQAccessControlTable::type() const
{
  return "table";
}

void AQAccessControlTable::processObject( void * obj ) const
{
  Q_ASSERT( obj );

  int maskPerm = 0;
  bool hasAcos = !d_->acosPerms_.isEmpty();

  if ( !d_->perm_.isEmpty() ) {
    if ( d_->perm_.left( 1 ) == "r" )
      maskPerm |= 2;
    if ( d_->perm_.right( 1 ) == "w" )
      maskPerm |= 1;
  } else if ( hasAcos )
    maskPerm = 8;
  else
    return ;

  QString fieldPerm;
  int maskFieldPerm = 0;

  AQFieldMDHash fieldList( static_cast<AQTableMD *>( obj ) ->fieldList() );
  AQFieldMD * field;

  for ( AQFieldMDHash::const_iterator it = fieldList.begin(); it != fieldList.end(); ++it ) {
    field = *it;
    maskFieldPerm = maskPerm;

    if ( hasAcos ) {
      fieldPerm = d_->acosPerms_.value( field->name() );

      if ( !fieldPerm.isEmpty() ) {
        maskFieldPerm = 0;
        if ( fieldPerm.left( 1 ) == "r" )
          maskFieldPerm |= 2;
        if ( fieldPerm.right( 1 ) == "w" )
          maskFieldPerm |= 1;
      }
    }

    switch ( maskFieldPerm ) {
    case 0:
      field->setVisible( false );
      field->setEditable( false );
      break;
    case 1:
      field->setVisible( false );
      field->setEditable( true );
      break;
    case 2:
      field->setVisible( true );
      field->setEditable( false );
      break;
    case 3:
      field->setVisible( true );
      field->setEditable( true );
      break;
    }
  }
}

void AQAccessControlTable::setFromObject( void * obj )
{
  Q_ASSERT( obj );

  d_->acosPerms_.clear();

  QChar permR, permW;
  AQFieldMDHash fieldList( static_cast<AQTableMD *>( obj ) ->fieldList() );
  AQFieldMD * field;

  for ( AQFieldMDHash::const_iterator it = fieldList.begin(); it != fieldList.end(); ++it ) {
    field = *it;
    permR = '-';
    permW = '-';

    if ( field->visible() )
      permR = 'r';
    if ( field->editable() )
      permW = 'w';

    d_->acosPerms_.insert( field->name(), QString( permR ) + permW );
  }
}
