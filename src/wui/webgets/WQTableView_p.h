/***************************************************************************
WQTableView_p.h
-------------------
begin                : 28/03/2009
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

#ifndef WQTABLEVIEW_P_H_
#define WQTABLEVIEW_P_H_

#include "AQUi/WQWidget_p.h"

class WQTableModel : public WAbstractTableModel
{
public:
  WQTableModel( WObject *parent = 0 ) :
      WAbstractTableModel( parent ), tbData_( 0 )
  {}

  int columnCount( const WModelIndex & = WModelIndex() ) const
  {
    if ( !tbData_ )
      return 3;

    return tbData_->numCols();
  }

  int rowCount( const WModelIndex & = WModelIndex() ) const
  {
    if ( !tbData_ )
      return 5;

    return tbData_->numRows();
  }

  boost::any data( const WModelIndex & index, int = Wt::DisplayRole ) const
  {
    if ( !tbData_ )
      return "AbanQ";

    return toWString( tbData_->text( index.row(), index.column() ) );
  }

  virtual boost::any headerData( int section, Wt::Orientation orientation = Wt::Horizontal, int = Wt::DisplayRole )
  {
    if ( !tbData_ )
      return section;

    if ( orientation == Wt::Vertical )
      return section;
    else
      return toWString( tbData_->horizontalHeader() ->label( section ) );
  }


  AQDataTable * tbData_;
};

class WQTableViewPrivate : public WQWidgetPrivate
{
  AQ_DECLARE_PUBLIC( WQTableView )

public :

  WQTableViewPrivate();
  ~WQTableViewPrivate();

  void init();

  Ext::TableView * tbv_;
  WQTableModel tbModel_;
};

#endif /*WQTABLEVIEW_P_H_*/
