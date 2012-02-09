/***************************************************************************
                       FLAccessControlFactory.cpp
                          -------------------
 begin                : Fri Oct 10 2005
 copyright            : (C) 2004-2005 by InfoSiAL S.L.
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

#include <qmainwindow.h>
#include <qdict.h>
#include <qstringlist.h>
#include <qaction.h>
#include <qobjectlist.h>
#include <qapplication.h>

#include "FLAccessControlFactory.h"
#include "FLAccessControl.h"
#include "FLFormDB.h"
#include "FLTableMetaData.h"
#include "FLFieldMetaData.h"

/**
Control de Acceso para objetos de alto nivel del tipo "mainwindow"

@author InfoSiAL S.L.
*/
class FLAccessControlMainWindow : public FLAccessControl
{

public:

  FLAccessControlMainWindow() : FLAccessControl() {}
  ~FLAccessControlMainWindow() {}

  /**
  @return El tipo del que se encarga; "mainwindow".
  */
  QString type();

  /**
  Procesa objetos que heredan de la clase QMainWindow.

  Sólo controla los hijos del objeto que son de la clase QAction, y sólo
  permite hacerlos no visibles. Los permisos que acepta son :

  - "-w" o "--" (no_lectura/escritura o no_lectura/no_escritura) -> no visible

  Esto permite eliminar opciones de las barras de menús y barras
  de herramientas de las ventanas principales, así como opciones de
  activación de módulos de la caja de herramientas principal, haciéndolas
  no visibles para el usuario.
  */
  void processObject(QObject *obj);

  /** No implementado */
  void setFromObject(QObject *);
};

QString FLAccessControlMainWindow::type()
{
  return "mainwindow";
}

void FLAccessControlMainWindow::processObject(QObject *obj)
{
  QMainWindow *mw = ::qt_cast<QMainWindow *>(obj);
  if (!mw || !acosPerms_)
    return;

  if (!perm_.isEmpty()) {
    QObjectList *l = mw->queryList("QAction");
    QObjectListIt ito(*l);
    QAction *a;
    while ((a = ::qt_cast<QAction *>(ito.current())) != 0) {
      ++ito;
      if ((*acosPerms_)[a->name()])
        continue;
      if (perm_ == "-w" || perm_ == "--")
        a->setVisible(false);
    }
    delete l;
  }

  QDictIterator < QString > it(*acosPerms_);
  for (; it.current(); ++it) {
    QAction *a = ::qt_cast<QAction *>(mw->child(it.currentKey(), "QAction"));
    if (a) {
      QString perm = *(*it);
      if (perm == "-w" || perm == "--")
        a->setVisible(false);
    }
  }
}

void FLAccessControlMainWindow::setFromObject(QObject *)
{
#ifdef FL_DEBUG
  qWarning("FLAccessControlMainWindow::setFromObject " + QApplication::tr("No implementado todavía."));
#endif
}

/**
Control de Acceso para objetos de alto nivel del tipo "form"

@author InfoSiAL S.L.
*/
class FLAccessControlForm : public FLAccessControl
{

public:

  FLAccessControlForm();
  ~FLAccessControlForm() {}

  /**
  @return El tipo del que se encarga; "form".
  */
  QString type();

  /**
  Procesa objetos que son de la clase FLFormDB.

  Sólo controla los hijos del objeto que son de la clase QWidget,y sólo
  permite hacerlos no visibles o no editables. En realidad hacerlos
  no visibles significa que sean no editables y modficando la paleta para
  que toda la región del componente sea mostrada en color negro. Los permisos
  que acepta son :

  - "-w" o "--" (no_lectura/escritura o no_lectura/no_escritura) -> no visible
  - "r-" (lectura/no_escritura) -> no editable

  Esto permite que cualquier componente de un formulario de AbanQ ( FLFormDB,
  FLFormRecordDB y FLFormSearchDB) se pueda hacer no visible o no editable a conveniencia.
  */
  void processObject(QObject *obj);

  /** No implementado */
  void setFromObject(QObject *);

private:

  QPalette pal;
};

FLAccessControlForm::FLAccessControlForm() : FLAccessControl()
{
  QColorGroup cg;
  QColor bg = qApp->palette().color(QPalette::Active, QColorGroup::Background);
  cg.setColor(QColorGroup::Foreground, bg);
  cg.setColor(QColorGroup::Text, bg);
  cg.setColor(QColorGroup::ButtonText, bg);
  cg.setColor(QColorGroup::Base, bg);
  cg.setColor(QColorGroup::Background, bg);
  pal.setDisabled(cg);
}

QString FLAccessControlForm::type()
{
  return "form";
}

void FLAccessControlForm::processObject(QObject *obj)
{
  FLFormDB *fm = ::qt_cast<FLFormDB *>(obj);
  if (!fm || !acosPerms_)
    return ;

  if (!perm_.isEmpty()) {
    QObjectList *l = fm->queryList("QWidget");
    QObjectListIt ito(*l);
    QWidget *w;
    while ((w = ::qt_cast<QWidget *>(ito.current())) != 0) {
      ++ito;
      if ((*acosPerms_)[ w->name()])
        continue;
      if (perm_ == "-w" || perm_ == "--") {
        w->setPalette(pal);
        w->setDisabled(true);
        w->hide();
        continue;
      }
      if (perm_ == "r-")
        w->setDisabled(true);
    }
    delete l;
  }

  QDictIterator < QString > it(*acosPerms_);
  for (; it.current(); ++it) {
    QWidget *w = ::qt_cast<QWidget * >(fm->child(it.currentKey(), "QWidget"));
    if (w) {
      QString perm = *(*it);
      if (perm == "-w" || perm == "--") {
        w->setPalette(pal);
        w->setDisabled(true);
        w->hide();
        continue;
      }
      if (perm == "r-")
        w->setDisabled(true);
    }
  }
}

void FLAccessControlForm::setFromObject(QObject *)
{
#ifdef FL_DEBUG
  qWarning("FLAccessControlForm::setFromObject " + QApplication::tr("No implementado todavía."));
#endif
}

/**
Control de Acceso para objetos de alto nivel del tipo "table"

@author InfoSiAL S.L.
*/
class FLAccessControlTable : public FLAccessControl
{

public:

  FLAccessControlTable() : FLAccessControl() {}
  ~FLAccessControlTable() {}

  /**
  @return El tipo del que se encarga; "table".
  */
  QString type();

  /**
  Procesa objetos que son de la clase FLTableMetaData.

  Sólo controla los objetos que pertenecen a él de la clase FLFieldMetaData
  y sólo permite hacerlos no visibles o no editables. Esto en la práctica
  significa que los componentes de los formularios de la clase FLFieldDB y
  FLTableDB utilizarán los metadatos para autoconfigurarse y serán no visibles
  o no editables si así lo establecen los objetos FLTableMetaData y FLFieldMetaData
  que definen esos metadatos.

  Los permisos que acepta son :

  - "-w" o "--" (no_lectura/escritura o no_lectura/no_escritura) -> no visible
  - "r-" (lectura/no_escritura) -> no editable

  Esto permite que cualquier componente de un formulario de AbanQ ( FLFormDB,
  FLFormRecordDB y FLFormSearchDB) del tipo FLFieldDB o FLTableDB sea no visible
  o no editable a partir de los metadatos que utiliza para autoconfigurarse,
  independientemente del formulario en que se encuentre. Por ejemplo, si definimos
  que el campo "nombre" de la tabla "clientes" es no visible, implicará que cada vez
  que un componente tipo FLFieldDB forme parte de un formulario y esté enlazando con el campo
  "nombre" de "clientes" su contenido sea no visible para el usuario.
  */
  void processObject(QObject *obj);

  /**
  Crea la regla la regla de control de acceso a partir de objetos de la clase FLTableMetaData.

  Al igual que en FLAccessControlTable::processObject() sólo tiene en cuenta
  las propiedades 'visible' y 'editable' de los campos, a partir de las cuales
  crea la regla.
  */
  void setFromObject(QObject *obj);
};

QString FLAccessControlTable::type()
{
  return "table";
}

void FLAccessControlTable::processObject(QObject *obj)
{
  if (!obj || obj->aqWasDeleted())
    return;
  FLTableMetaData *tm = ::qt_cast<FLTableMetaData *>(obj);
  if (!tm)
    return;

  int maskPerm = 0;
  bool hasAcos = (acosPerms_ && !acosPerms_->isEmpty());

  if (!perm_.isEmpty()) {
    if (perm_.left(1) == "r")
      maskPerm |= 2;
    if (perm_.right(1) == "w")
      maskPerm |= 1;
  } else if (hasAcos)
    maskPerm = 8;
  else
    return;

  QString fieldPerm;
  QString *fieldPermPtr;
  int maskFieldPerm = 0;

  FLTableMetaData::FLFieldMetaDataList *fL = tm->fieldList();
  if (!fL)
    return ;

  FLFieldMetaData *field;
  QDictIterator<FLFieldMetaData> it(*fL);

  while ((field = it.current()) != 0) {
    maskFieldPerm = maskPerm;
    ++it;

    if (hasAcos && (fieldPermPtr = (*acosPerms_)[ field->name()])) {
      fieldPerm = *fieldPermPtr;
      maskFieldPerm = 0;

      if (fieldPerm.left(1) == "r")
        maskFieldPerm |= 2;
      if (fieldPerm.right(1) == "w")
        maskFieldPerm |= 1;
    }

    switch (maskFieldPerm) {
      case 0:
        field->setVisible(false);
        field->setEditable(false);
        break;
      case 1:
        field->setVisible(false);
        field->setEditable(true);
        break;
      case 2:
        field->setVisible(true);
        field->setEditable(false);
        break;
      case 3:
        field->setVisible(true);
        field->setEditable(true);
        break;
    }
  }
}

void FLAccessControlTable::setFromObject(QObject *obj)
{
  FLTableMetaData *tm = ::qt_cast<FLTableMetaData *>(obj);
  if (!tm)
    return;

  if (acosPerms_) {
    acosPerms_->clear();
    delete acosPerms_;
  }

  acosPerms_ = new QDict < QString >(31);
  acosPerms_->setAutoDelete(true);

  FLTableMetaData::FLFieldMetaDataList *fL = tm->fieldList();
  if (!fL)
    return;

  FLFieldMetaData *field;
  QChar permR, permW;
  QDictIterator<FLFieldMetaData> it(*fL);

  while ((field = it.current()) != 0) {
    ++it;
    permR = '-';
    permW = '-';
    if (field->visible())
      permR = 'r';
    if (field->editable())
      permW = 'w';
    acosPerms_->replace(field->name(), new QString(QString(permR) + permW));
  }
}

FLAccessControl *FLAccessControlFactory::create(const QString &type)
{
  if (type.isEmpty())
    return 0;

  if (type == "mainwindow")
    return new FLAccessControlMainWindow();
  else if (type == "form")
    return new FLAccessControlForm();
  else if (type == "table")
    return new FLAccessControlTable();

  return 0;
}

QString FLAccessControlFactory::type(QObject *obj)
{
  if (!obj)
    qWarning("NO OBJ");

  if (::qt_cast<QMainWindow *>(obj))
    return "mainwindow";
  if (::qt_cast<FLTableMetaData *>(obj))
    return "table";
  if (::qt_cast<FLFormDB *>(obj))
    return "form";

  return QString::null;
}
