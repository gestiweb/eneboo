/***************************************************************************
WQTableView.cpp
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

#include <WAbstractTableModel>

#include <WContainerWidget>
#include <Ext/TableView>
#include <QLayout>

#include <AQUi/AQDataTable.h>

#include "../wtqt/WQApplication.h"
#include "WQTableView.h"
#include "WQTableView_p.h"

WQTableViewPrivate::WQTableViewPrivate() :
    WQWidgetPrivate()
{
  canContain_ = false;
}

WQTableViewPrivate::~WQTableViewPrivate()
{}

void WQTableViewPrivate::init()
{
  if ( !tbModel_.tbData_ )
    tbModel_.tbData_ = qobject_cast<AQDataTable *>( qWidget_ );
}

WQTableView::WQTableView( WQWidget * parent ) :
    WQWidget( *new WQTableViewPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( WQTableView );

  d->wc_ = new WContainerWidget;
  d->tbv_ = new Ext::TableView( d->wc_ );
  d->tbv_->setModel( &d->tbModel_ );
  d->tbv_->setDataLocation( Ext::ServerSide );
  d->tbv_->setSelectionMode( Wt::SingleSelection );
  d->tbv_->setSelectionBehavior( Wt::SelectRows );
  d->tbv_->setPageSize( 30 );
  d->tbv_->setColumnsMovable( false );
  d->tbv_->setAlternatingRowColors( true );
  d->tbv_->setBottomToolBar( d->tbv_->createPagingToolBar() );
}

void WQTableView::update()
{
  AQ_D( WQTableView );

  AQDataTable * data = d->tbModel_.tbData_;

  for ( int i = 0; i < data->numCols(); ++i ) {
    d->tbv_->setColumnHidden( i, data->isColumnHidden( i ) );
    d->tbv_->setColumnWidth( i, data->columnWidth( i ) );
  }

  if ( data->currentRow() > -1 )
    d->tbv_->setCurrentCell( data->currentRow() , 1 );
}
