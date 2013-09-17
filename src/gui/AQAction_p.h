/***************************************************************************
AQAction_p.h
-------------------
 begin                : 30/01/2009
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

#ifndef AQACTION_P_H_
#define AQACTION_P_H_

#include "AQCore/AQObject_p.h"

class AQActionPrivate : public AQObjectPrivate
{
  AQ_DECLARE_PUBLIC( AQAction )

public:

  AQActionPrivate();
  ~AQActionPrivate();

  QString connName_;
  QString actName_;

  AQSharedAction actMaster_;
  AQRelationMD * relMasterToDetail_;

  AQActionMD * action_;
  AQSqlCursor * cursor_;

  AQSqlCursor * cursorField_;
  QString fieldName_;
  QString defaultSearchFieldName_;
  bool modal_;
  
  QHash<QString, AQSharedAction> hashActionsField_;

  QWeakPointer<AQAction> weakRef_;
  
  void init();
  bool showWarnings( int cursorModeAccess ) const;
};

#endif /*AQACTION_P_H_*/
