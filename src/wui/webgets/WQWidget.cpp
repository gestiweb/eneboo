/***************************************************************************
WQWidget.cpp
-------------------
begin                : 21/06/2008
copyright            : (C) 2003-2008 by InfoSiAL S.L.
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

#include <WContainerWidget>
#include <WBoxLayout>

#include <QEvent>
#include <QWidget>
#include <QLayout>
#include <QVariant>
#include <QAtomicInt>

#include <AQUi/AQComponentDB.h>

#include "../AQApplication.h"
#include "../wtqt/WQApplication.h"
#include "WQTabWidget.h"
#include "WQGroupBox.h"
#include "WQFrame.h"
#include "WQBoxLayout.h"
#include "WQGridLayout.h"
#include "WQLineEdit.h"
#include "WQTableView.h"
#include "WQLabel.h"
#include "WQPushButton.h"
#include "WQToolButton.h"
#include "WQRadioButton.h"
#include "WQCheckBox.h"
#include "WQSpacer.h"
#include "WQComboBox.h"
#include "WQWidget.h"
#include "WQWidget_p.h"

static void dumpTreeGUI( QLayoutItem * item, int level )
{
  if ( !item )
    return ;

  if ( item->widget() ) {
    QWidget * wid = item->widget();
    aqWarn( QString( 2 * level + 1, ' ' ) << wid << wid->geometry() );

    if ( !wid->layout() ) {
      QObjectList list( wid->children() );
      for ( QObjectList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        if ( ( *it ) ->isWidgetType() ) {
          QLayoutItem * widItem = new QWidgetItem( static_cast<QWidget *>( *it ) );

          dumpTreeGUI( widItem, level + 1 );
          delete widItem;
        }
      }
    } else
      dumpTreeGUI( wid->layout(), level + 1 );

  } else if ( item->layout() ) {
    QLayout * lay = item->layout();
    aqWarn( QString( 2 * level + 1, ' ' ) << lay << lay->geometry() );

    int layCount = ( QString( lay->metaObject() ->className() ) == "QMainWindowLayout" ? 5 : lay->count() );

    for ( int idx = 0; idx < layCount; ++idx )
      dumpTreeGUI( lay->itemAt( idx ), level + 1 );

  } else if ( item->spacerItem() ) {
    aqWarn( QString( 2 * level + 1, ' ' ) << "QSpacerItem" << item->geometry() );
  } else
    aqWarn( QString( 2 * level + 1, ' ' ) << "???????" );
}

static void dumpTreeWUI( WQWidget * wq, int level )
{
  if ( !wq )
    return ;

  if ( wq->isWidget() ) {
    aqWarn( QString( 2 * level + 1, ' ' ) << wq << dynamic_cast<WQWidget *>( wq->WObject::parent() ) );

    if ( !wq->widgetLayout() ) {
      WQWidgetList list( wq->widgets() );
      for ( WQWidgetList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        dumpTreeWUI( *it, level + 1 );
      }
    } else
      dumpTreeWUI( wq->widgetLayout(), level + 1 );

  } else if ( wq->isLayout() ) {
    aqWarn( QString( 2 * level + 1, ' ' ) << wq << dynamic_cast<WQWidget *>( wq->WObject::parent() ) << wq->parentWidget() );

    WQWidgetList list( wq->items() );
    for ( WQWidgetList::const_iterator it = list.begin(); it != list.end(); ++it ) {
      dumpTreeWUI( *it, level + 1 );
    }

  } else if ( wq->isSpacer() ) {
    aqWarn( QString( 2 * level + 1, ' ' ) << "QSpacerItem" );

  } else
    aqWarn( QString( 2 * level + 1, ' ' ) << "???????" );
}

static WQWidget * loadWQWidget( QLayoutItem * item, WQWidget * wqLayout )
{
  if ( !item )
    return 0;

  WQWidget * w = 0;

  if ( item->widget() ) {
    QWidget * wid = item->widget();
    QString className( wid->metaObject() ->className() );
    WQWidget * wqParent = ( WQWidget::isWebget( wid->parent() ) ? WQWidget::webget( wid->parent() ) : 0 );

    if ( WQWidget::isWebget( wid ) )
      w = WQWidget::webget( wid );

    if ( !w ) {
      if ( className == "Q3Frame" || className == "QFrame" ) {
        w = new WQFrame( wqParent );
      } else if ( className == "Q3GroupBox" || className == "QGroupBox" ) {
        w = new WQGroupBox( wqParent );
      } else if ( className == "QTabWidget" ) {
        w = new WQTabWidget( wqParent );
      } else if ( className == "QLineEdit" || className == "AQLineEdit" ) {
        w = new WQLineEdit( wqParent );
      } else if ( className == "AQDataTable" ) {
        w = new WQFrame( wqParent );
        w->d_func() ->canContain_ = false;
        //w = new WQTableView( wqParent );
      } else if ( className == "QLabel" ) {
        w = new WQLabel( wqParent );
      } else if ( className == "QPushButton" ) {
        w = new WQPushButton( wqParent );
      } else if ( className == "QToolButton" ) {
        w = new WQToolButton( wqParent );
      } else if ( className == "QRadioButton" ) {
        w = new WQRadioButton( wqParent );
      } else if ( className == "QComboBox" ) {
        w = new WQComboBox( wqParent );
      } else if ( className == "QCheckBox" ) {
        w = new WQCheckBox( wqParent );
      } else if ( className == "QWidget" || className == "AQFieldDB" || className == "FLFieldDB" ||
                  className == "AQTableDB" || className == "FLTableDB" || className == "AQFormDB" ) {
        w = new WQWidget( wqParent );
      } else {
        w = new WQWidget( static_cast<WObject *>( wqParent ) );
        w->d_func() ->isWidget_ = false;
        w->d_func() ->isProxy_ = true;
        //aqWarn( "WQWidget::create !widget: " << className << w << wqParent );
      }

      w->setQLayItem( item );
      w->setQWidget( wid );
    }

    if ( !w->isProxy() ) {
      if ( wid->layout() )
        loadWQWidget( wid->layout() );

      if ( wqLayout )
        wqLayout->addWQWidget( w );
      else if ( wqParent )
        wqParent->addWQWidget( w );
    }

    if ( !wid->layout() ) {
      QObjectList list( wid->children() );
      for ( QObjectList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        if ( ( *it ) ->isWidgetType() ) {
          QLayoutItem * widItem = new QWidgetItem( static_cast<QWidget *>( *it ) );
          loadWQWidget( widItem );
          delete widItem;
        }
      }
    } else
      loadWQWidget( wid->layout() );

  } else if ( item->layout() ) {
    QLayout * lay = item->layout();
    QString className( lay->metaObject() ->className() );
    int layCount = 0;
    WQWidget * wqParent = ( WQWidget::isWebget( lay->parent() ) ? WQWidget::webget( lay->parent() ) : 0 );

    if ( WQWidget::isWebget( lay ) )
      w = WQWidget::webget( lay );

    if ( !w ) {
      if ( className == "QBoxLayout" || className == "QVBoxLayout" || className == "QHBoxLayout" ) {
        QBoxLayout * qLay = qobject_cast<QBoxLayout *> ( lay );
        w = new WQBoxLayout( ( WBoxLayout::Direction ) qLay->direction(), wqParent );
      } else if ( className == "QGridLayout" ) {
        w = new WQGridLayout( wqParent );
      } else if ( className == "QMainWindowLayout" ) {
        w = new WQBoxLayout( WBoxLayout::TopToBottom, wqParent );
      } else {
        w = new WQWidget( static_cast<WObject *>( wqParent ) );
        w->d_func() ->isWidget_ = false;
        w->d_func() ->isProxy_ = true;
        //aqWarn( "WQWidget::create !layout: " << className << w << wqParent );
      }

      w->setQLayItem( item );
    }

    layCount = ( ( className == "QMainWindowLayout" ) ? 5 : lay->count() );

    if ( !w->isProxy() ) {
      if ( wqLayout )
        wqLayout->addWQWidget( w );
      else if ( wqParent ) {
        if ( wqParent->isWidget() ) {
          WQWidget * wqParentLay = wqParent->widgetLayout();
          if ( !wqParentLay ) {
            wqParent->addWQWidget( w );
            return w;
          }
        } else
          wqParent->addWQWidget( w );
      }
    }

    for ( int idx = 0; idx < layCount; ++idx ) {
      if ( !loadWQWidget( lay->itemAt( idx ), w ) )
        break;
    }

  } else if ( item->spacerItem() ) {
    if ( wqLayout ) {
      w = new WQSpacer( wqLayout->parentWidget() );
      w->setQLayItem( item );
      wqLayout->addWQWidget( w );
    }
  }

  return w;
}

WQWidgetPrivate::WQWidgetPrivate() :
    AQObjectPrivate(), wc_( 0 ), layout_( 0 ), qWidget_( 0 ),
    qLayItem_( 0 ), canContain_( true ), isSpacer_( false ),
    isWidget_( true ), isLayout_( false ), component_( 0 ),
    componentInitialized_( false ), invalidated_( true ),
    isProxy_( false )
{}

WQWidgetPrivate::~WQWidgetPrivate()
{}

void WQWidgetPrivate::init()
{
  if ( wc_ ) {
    wc_->setMinimumSize( qWidget_->minimumWidth(), WLength() );
    if ( !q_func() ->WObject::parent() )
      wc_->resize( qWidget_->size().width(), qWidget_->size().height() );
  }
}

int WQWidgetPrivate::fetchAndAddGrandChildren( int value )
{
  AQ_Q( WQWidget );

  int ret = grandChildrenAdded_.fetchAndAddRelaxed( value );

  WQWidget * wqParent = q->parentWidget();
  if ( wqParent )
    wqParent->d_func() ->fetchAndAddGrandChildren( value );

  if ( !invalidated_ )
    invalidated_ = ( childrenAdded_ != 0 || grandChildrenAdded_ != 0 );

  return ret;
}

int WQWidgetPrivate::fetchAndAddChildren( int value )
{
  AQ_Q( WQWidget );

  int ret = childrenAdded_.fetchAndAddRelaxed( value );

  WQWidget * wqParent = q->parentWidget();
  if ( wqParent )
    wqParent->d_func() ->fetchAndAddGrandChildren( value );

  if ( !invalidated_ )
    invalidated_ = ( childrenAdded_ != 0 || grandChildrenAdded_ != 0 );

  return ret;
}

bool WQWidgetPrivate::isDirty() const
{
  AQ_Q( const WQWidget );

  WQWidget * wqParent;
  return ( invalidated_ || childrenAdded_ != 0 || grandChildrenAdded_ != 0 ||
           ( ( wqParent = q->parentWidget() ) && wqParent->d_func() ->isDirty() ) );
}

void WQWidgetPrivate::processChildrenAdded()
{
  AQ_Q( WQWidget );

  if ( childrenAdded_ != 0 ) {
    QMutexLocker lock( &mutexChildObjectsAdded_ );
    int childrenSubtracted = 0;

    while ( !childObjectsAdded_.isEmpty() ) {
      QObject * childObj = childObjectsAdded_.dequeue();

      --childrenSubtracted;

      if ( WQWidget::isWebget( childObj ) )
        continue;

      QWidget * wid = qobject_cast<QWidget *>( childObj );
      QLayout * lay;

      if ( wid ) {
        QLayoutItem * widItem = new QWidgetItem( wid );
        loadWQWidget( widItem );
        delete widItem;
      } else if ( ( lay = qobject_cast<QLayout *>( childObj ) ) ) {
        loadWQWidget( lay );
      } else {
        aqWarn( "WQWidget::update() !childObj " << childObj );
      }
    }

    if ( childrenSubtracted < 0 ) {
      fetchAndAddChildren( childrenSubtracted );
      if ( q->widgetLayout() )
        q->widgetLayout() ->setInvalidated();
      return ;
    }
  }
}

WFlags<AlignmentFlag> WQWidgetPrivate::calcAlign( WQWidget * wq ) const
{
  QWidget * wid = ( wq->isLayout() ? wq->parentWidget() ->qWidget() : wq->qWidget() );

  QSizePolicy sizeP( wid->sizePolicy() );
  QSizePolicy::Policy vP( sizeP.verticalPolicy() );
  QSizePolicy::Policy hP( sizeP.horizontalPolicy() );
  Qt::Alignment qAlig = wq->qLayItem() ->alignment();

  if ( !qAlig )
    qAlig = ( Qt::AlignVCenter | Qt::AlignLeft );

  if ( vP == QSizePolicy::Expanding || vP == QSizePolicy::MinimumExpanding ||
       vP == QSizePolicy::Ignored )
    qAlig &= Qt::AlignHorizontal_Mask;

  if ( hP == QSizePolicy::Expanding || hP == QSizePolicy::MinimumExpanding ||
       hP == QSizePolicy::Ignored || hP == QSizePolicy::Minimum ||
       hP == QSizePolicy::Preferred )
    qAlig &= Qt::AlignVertical_Mask;

  return toWAlig( qAlig );
}

WQWidget::WQWidget( WQWidget * parent ) :
    WObject( static_cast<WObject *> ( parent ) ),
    AQObject( *new WQWidgetPrivate, 0 )
{
  AQ_D( WQWidget );
  d->wc_ = new WContainerWidget;
  d->wc_->setStyleClass( "WQWidget" );
  d->wc_->setMargin( 0 );
}

WQWidget::WQWidget( WObject * parent ) :
    WObject( parent ), AQObject( *new WQWidgetPrivate, 0 )
{}

WQWidget::WQWidget( WQWidgetPrivate & dd, WObject * parent ) :
    WObject( parent ), AQObject( dd, 0 )
{}

WQWidget::~WQWidget()
{}

void WQWidget::qWidgetEvent( QEvent * ev )
{
  AQ_D( WQWidget );

  QEvent::Type evType = ev->type();

  switch ( evType ) {

    case AQCoreEvent::LayoutRequest: {
        //aqWarn( this << "LayoutRequest" );
        if ( !d->invalidated_ )
          return ;

        if ( d->isLayout_ ) {
          WQWidgetList list( d->items_ );
          for ( WQWidgetList::const_iterator it = list.begin(); it != list.end(); ++it ) {
            if ( ( *it ) ->isLayout() ) {
              AQ_SENDEVENT( *it, LayoutRequest );
            }
          }

          AQ_POSTEVENT( this, UpdateRequest );
        } else if ( d->layout_ ) {
          AQ_POSTEVENT( d->layout_, LayoutRequest );
        }
      }
      break;

    case QEvent::ChildAdded: {
        //aqWarn( this << "ChildAdded" );
        QMutexLocker lock( &d->mutexChildObjectsAdded_ );

        QObject * childObj = static_cast<QChildEvent *>( ev ) ->child();

        if ( !isWebget( childObj ) ) {
          d->childObjectsAdded_.enqueue( childObj );
          if ( !d->invalidated_ ) {
            d->fetchAndAddChildren( 1 );
            AQ_POSTEVENT( this, UpdateRequest );
          } else
            d->fetchAndAddChildren( 1 );
        }
      }
      break;

    case AQCoreEvent::UpdateNow: {
        //aqWarn( this << "UpdateNow" );
        if ( !d->isDirty() ) {
          //aqWarn( "Updated" << this );
          this->update();

          if ( d->isWidget_ ) {
            WQWidgetList list( d->widgets_ );
            for ( WQWidgetList::const_iterator it = list.begin(); it != list.end(); ++it ) {
              AQ_SENDEVENT( ( *it ), UpdateNow );
            }
            if ( d->layout_ ) {
              AQ_SENDEVENT( d->layout_, LayoutRequest );
            }
          }

          return ;
        }

        if ( d->isWidget_ ) {
          WQWidgetList list( d->widgets_ );
          for ( WQWidgetList::const_iterator it = list.begin(); it != list.end(); ++it ) {
            AQ_SENDEVENT( *it, UpdateNow );
          }

          if ( d->component_ && !d->componentInitialized_ ) {
            d->componentInitialized_ = true;

            QEvent ev( ( QEvent::Type ) AQCoreEvent::InitNow );
            d->invalidated_ = d->component_->eventFilter( this, &ev );

            if ( d->invalidated_ )
              return ;
          }

          if ( d->invalidated_ )
            d->processChildrenAdded();
        }

        d->invalidated_ = false;

        if ( d->isWidget_ && d->isDirty() ) {
          AQ_POSTEVENT( this, UpdateRequest );
        } else if ( !WObject::parent() && !d->childrenAdded_ && !d->grandChildrenAdded_ ) {
          AQ_POSTEVENT( this, UpdateNow );
        }
      }
      break;

    case AQCoreEvent::UpdateRequest: {
        //aqWarn( this << "UpdateRequest" );
        if ( d->invalidated_ || d->childrenAdded_ != 0 || d->grandChildrenAdded_ != 0 ) {
          d->invalidated_ = true;
          AQ_POSTEVENT( this, UpdateNow );
        } else if ( d->isWidget_ && d->isDirty() ) {
          WQWidget * wqParent = parentWidget();
          if ( wqParent && !wqParent->invalidated() ) {
            wqParent->d_func() ->invalidated_ = true;
            AQ_POSTEVENT( wqParent, UpdateNow );
          }
        }
      }
      break;

    default: {
        //aqWarn( this << evType );
      }
      break;
  }
}

void WQWidget::update()
{}

void WQWidget::init()
{
  AQ_D( WQWidget );
  d->init();
}

void WQWidget::finish()
{
  AQ_D( WQWidget );
  d->finish();
}

WWidget * WQWidget::ww()
{
  AQ_D( WQWidget );
  return d->wc_;
}

WLayout * WQWidget::wl()
{
  return 0;
}

void WQWidget::addWQWidget( WQWidget * w )
{
  if ( !w || w->isProxy() )
    return ;

  AQ_D( WQWidget );

  if ( d->isProxy_ ) {
    WQWidget * wqParent = parentWidget();
    if ( wqParent )
      wqParent->addWQWidget( w );
    return ;
  }

  if ( !d->wc_ || !d->canContain_ )
    return ;

  if ( !d->layout_ ) {
    if ( w->isLayout() ) {
      d->layout_ = w;
      if ( w->wl() )
        d->wc_->setLayout( w->wl(), d->calcAlign( w ) );
    } else {
      WQWidget * wqParent = parentWidget();
      if ( wqParent )
        wqParent->addWQWidget( w );
    }
  } else
    d->layout_->addWQWidget( w );
}

WQWidgetList WQWidget::widgets() const
{
  AQ_D( const WQWidget );
  return d->widgets_;
}

WQWidgetList WQWidget::items() const
{
  AQ_D( const WQWidget );
  return d->items_;
}

WQWidget * WQWidget::parentWidget() const
{
  WQWidget * wqParent = dynamic_cast<WQWidget *>( WObject::parent() );

  if ( wqParent && ( !wqParent->isWidget() || wqParent->isProxy() ) )
    return wqParent->parentWidget();

  return wqParent;
}

WQWidget * WQWidget::widgetLayout() const
{
  AQ_D( const WQWidget );
  return d->layout_;
}

void WQWidget::setQWidget( QWidget * qWidget )
{
  if ( !qWidget )
    return ;

  AQ_D( WQWidget );

  d->qWidget_ = qWidget;
  d->component_ = qobject_cast<AQComponentDB *>( qWidget );

  this->QObject::setObjectName( qWidget->objectName() );
  qWidget->setProperty( "webget", QVariant::fromValue<QObject *>( this ) );
  qWidget->setProperty( "isWebget", true );

  init();
}

void WQWidget::setQLayItem( QLayoutItem * qLayItem )
{
  if ( !qLayItem )
    return ;

  AQ_D( WQWidget );

  d->qLayItem_ = qLayItem;
  QLayout * lay;

  if ( d->isLayout_ && ( lay = qLayItem->layout() ) ) {
    this->QObject::setObjectName( lay->objectName() );
    lay->setProperty( "webget", QVariant::fromValue<QObject *>( this ) );
    lay->setProperty( "isWebget", true );

    init();
  }
}

QWidget * WQWidget::qWidget() const
{
  AQ_D( const WQWidget );
  return d->qWidget_;
}

QLayoutItem * WQWidget::qLayItem() const
{
  AQ_D( const WQWidget );
  return d->qLayItem_;
}

void WQWidget::addAction( QAction * action )
{
  insertAction( 0, action );
}

void WQWidget::removeAction( QAction * action, bool up )
{
  if ( !action )
    return ;

  AQ_D( WQWidget );
  d->actions_.removeAll( action );
  if ( up )
    this->updateActions();
}

void WQWidget::addActions( QList<QAction *> actions )
{
  for ( int i = 0; i < actions.size(); i++ )
    insertAction( 0, actions.at( i ), false );
  this->updateActions();
}

void WQWidget::insertAction( QAction * before, QAction * action, bool up )
{
  if ( !action )
    return ;

  AQ_D( WQWidget );

  if ( d->actions_.contains( action ) )
    removeAction( action, false );

  int pos = d->actions_.indexOf( before );
  if ( pos < 0 ) {
    before = 0;
    pos = d->actions_.size();
  }

  d->actions_.insert( pos, action );
  if ( up )
    this->updateActions();
}

void WQWidget::insertActions( QAction * before, QList<QAction*> actions )
{
  for ( int i = 0; i < actions.size(); ++i )
    insertAction( before, actions.at( i ), false );
  this->updateActions();
}

QList<QAction *> WQWidget::actions() const
{
  AQ_D( const WQWidget );
  return d->actions_;
}

void WQWidget::updateActions()
{}

bool WQWidget::canContain() const
{
  AQ_D( const WQWidget );
  return d->canContain_;
}

bool WQWidget::isSpacer() const
{
  AQ_D( const WQWidget );
  return d->isSpacer_;
}

bool WQWidget::isWidget() const
{
  AQ_D( const WQWidget );
  return d->isWidget_;
}

bool WQWidget::isLayout() const
{
  AQ_D( const WQWidget );
  return d->isLayout_;
}

bool WQWidget::isProxy() const
{
  AQ_D( const WQWidget );
  return d->isProxy_;
}

bool WQWidget::invalidated() const
{
  AQ_D( const WQWidget );
  return d->invalidated_;
}
bool WQWidget::setInvalidated( bool inv )
{
  AQ_D( WQWidget );
  d->invalidated_ = inv;
}

WQWidget * WQWidget::create( QWidget * widget )
{
  widget->setAttribute( Qt::WA_DontShowOnScreen );
  widget->setAttribute( Qt::WA_OpaquePaintEvent );
  widget->setAttribute( Qt::WA_ForceUpdatesDisabled );

  QList<AQComponentDB *> cos( widget->findChildren<AQComponentDB *>() );
  for ( QList<AQComponentDB *>::const_iterator it = cos.begin(); it != cos.end(); ++it ) {
    QEvent ev( ( QEvent::Type ) AQCoreEvent::InitNow );
    ( *it ) ->eventFilter( widget, &ev );
  }

  QList<QLayout *> lays( widget->findChildren<QLayout *>() );
  for ( QList<QLayout *>::const_iterator it = lays.begin(); it != lays.end(); ++it ) {
    ( *it ) ->activate();
  }

  widget->ensurePolished();
  QCoreApplication::sendPostedEvents();

  QLayoutItem * widItem = new QWidgetItem( widget );

  //dumpTreeGUI( widItem, 0 );

  WQWidget * wq = loadWQWidget( widItem );

  AQ_SENDEVENT( wq, UpdateRequest );

  //dumpTreeWUI( wq, 0 );

  delete widItem;

  return wq;
}

WQWidget * WQWidget::webget( QObject * obj )
{
  Q_ASSERT( isWebget( obj ) );

  return static_cast<WQWidget *>( obj->property( "webget" ).value<QObject *>() );
}

bool WQWidget::isWebget( QObject * obj )
{
  return ( obj && ( obj->isWidgetType() || qobject_cast<QLayout *>( obj ) ) && obj->property( "isWebget" ).isValid() );
}

void WQWidget::addChildWidget( WQWidget * w )
{
  if ( !w || !w->isWidget() )
    return ;

  AQ_D( WQWidget );

  if ( d->isProxy_ ) {
    WQWidget * wqParent = parentWidget();
    if ( wqParent ) {
      wqParent->addChildWidget( w );
      return ;
    }
  }

  d->widgets_.append( w );
}
