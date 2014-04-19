/***************************************************************************
                            FLTableDB.cpp
                         -------------------
begin                : Sun Jul 1 2001
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

#include <math.h>

#include "FLTableDB.h"
#include "FLDataTable.h"
#include "FLSqlCursor.h"
#include "FLTableMetaData.h"
#include "FLFieldMetaData.h"
#include "FLRelationMetaData.h"
#include "FLFormSearchDB.h"
#include "FLManager.h"
#include "FLManagerModules.h"
#include "FLAction.h"
#include "FLApplication.h"
#include "FLSqlDatabase.h"
#include "FLSqlConnections.h"
#include "FLFieldDB.h"
#include "FLUtil.h"
#include "FLSqlQuery.h"
#include "FLDiskCache.h"
#include "AQOds.h"
#include "FLSettings.h"

extern void qt_set_table_clipper_enabled(bool enabled);

FLTableDB::FLTableDB(QWidget *parent, const char *name) : FLWidgetTableDB(parent, name),
  tableRecords_(0), tableName_(QString::null), foreignField_(QString::null),
  fieldRelation_(QString::null), cursor_(0), cursorAux(0), topWidget(0),
  showed(false), filter_(QString::null), readonly_(false), reqReadOnly_(false),
  editonly_(false), reqEditOnly_(false), insertonly_(false), reqInsertOnly_(false),
  sortField_(0), initSearch_(QString::null), checkColumnEnabled_(false),
  aliasCheckColumn_(tr("Seleccionar")), fieldNameCheckColumn_(QString::null),
  checkColumnVisible_(false), sortColumn_(0), orderAsc_(true), sortColumn2_(1),
  orderAsc2_(true), sortColumn3_(2), orderAsc3_(true), tdbFilterLastWhere_(QString::null),
  findHidden_(false), filterHidden_(false), showAllPixmaps_(false), fakeEditor_(0)
{
#ifndef Q_OS_WIN32
  qt_set_table_clipper_enabled(false);
#endif
  topWidget = topLevelWidget();

  if (topWidget && !topWidget->inherits("FLFormDB")) {
    QWidget *topWid = parentWidget();
    while (topWid && !topWid->inherits("FLFormDB"))
      topWid = topWid->parentWidget();
    topWidget = topWid;
  }

  if (!topWidget) {
#ifdef FL_DEBUG
    qWarning(tr("FLTableDB : Uno de los padres o antecesores de FLTableDB deber ser de la clase FLFormDB o heredar de ella"));
#endif
    return ;
  } else {
    cursor_ = ::qt_cast<FLFormDB *>(topWidget) ->cursor();
    setFont(qApp->font());
  }

  if (!name)
    setName("FLTableDB");

  timer = new QTimer(this, QObject::name());
  connect(timer, SIGNAL(timeout()), SLOT(refreshDelayed()));
}

FLDataTable *FLTableDB::tableRecords() 
{
  if (!tableRecords_) {
    tableRecords_ = new FLDataTable(tabData, "tableRecords");
    tableRecords_->setFocusPolicy(QTable::StrongFocus);
    setFocusProxy(tableRecords_);
    tabDataLayout->addWidget(tableRecords_);
    setTabOrder(tableRecords_, lineEditSearch);
    setTabOrder(lineEditSearch, comboBoxFieldToSearch);
    setTabOrder(comboBoxFieldToSearch, comboBoxFieldToSearch2);
    lineEditSearch->installEventFilter(this);
    tableRecords_->installEventFilter(this);
    connect(tableRecords_->horizontalHeader(), SIGNAL(clicked(int)), this, SLOT(switchSortOrder(int)));
  }
  FLSqlCursor *tCursor = tableRecords_->cursor();
  if (cursor_ && cursor_ != tCursor && cursor_->metadata() &&
      (!tCursor || (tCursor && tCursor->metadata() && tCursor->metadata()->name() != cursor_->metadata()->name())))
    setTableRecordsCursor();
  return tableRecords_;
}

void FLTableDB::setTableRecordsCursor()
{
  if (!tableRecords_) {
    tableRecords_ = new FLDataTable(tabData, "tableRecords");
    tableRecords_->setFocusPolicy(QTable::StrongFocus);
    setFocusProxy(tableRecords_);
    tabDataLayout->addWidget(tableRecords_);
    setTabOrder(tableRecords_, lineEditSearch);
    setTabOrder(lineEditSearch, comboBoxFieldToSearch);
    setTabOrder(comboBoxFieldToSearch, comboBoxFieldToSearch2);
    lineEditSearch->installEventFilter(this);
    tableRecords_->installEventFilter(this);
  }
  FLSqlCursor *tCursor = tableRecords_->cursor();
  tableRecords_->setFLSqlCursor(cursor_);
  if (showed)
    disconnect(tableRecords_, SIGNAL(currentChanged(QSqlRecord *)), this, SIGNAL(currentChanged()));
  connect(tableRecords_, SIGNAL(currentChanged(QSqlRecord *)), this, SIGNAL(currentChanged()));
  if (tCursor)
    disconnect(tableRecords_, SIGNAL(recordChoosed()), tCursor, SLOT(chooseRecord()));
  connect(tableRecords_, SIGNAL(recordChoosed()), cursor_, SLOT(chooseRecord()));
}

bool FLTableDB::eventFilter(QObject *obj, QEvent *ev)
{
  if (!tableRecords_ || !lineEditSearch || !comboBoxFieldToSearch || !comboBoxFieldToSearch2 || !cursor_)
    return FLWidgetTableDB::eventFilter(obj, ev);
  if (ev->type() == QEvent::KeyPress && obj == tableRecords_) {
    QKeyEvent *k = static_cast<QKeyEvent *>(ev);
    if (k->key() == Key_F2) {
      comboBoxFieldToSearch->popup();
      return true;
    }
  }
  if (ev->type() == QEvent::KeyPress && obj == lineEditSearch) {
    QKeyEvent *k = static_cast<QKeyEvent *>(ev);
    if (k->key() == Key_Enter || k->key() == Key_Return) {
      tableRecords_->setFocus();
      return true;
    }
    if (k->key() == Key_Up) {
      comboBoxFieldToSearch->setFocus();
      return true;
    }
    if (k->key() == Key_Down) {
      tableRecords_->setFocus();
      return true;
    }
    if (k->key() == Key_F2) {
      comboBoxFieldToSearch->popup();
      return true;
    }
    if (k->text() == "'" || k->text() == "\\")
      return true;
  }

  if (obj == tableRecords_ || obj == lineEditSearch)
    return false;
  else
    return FLWidgetTableDB::eventFilter(obj, ev);
}

void FLTableDB::putFirstCol(const QString &c)
{
  if (c == "*") {
    QComboBox *cb = (QComboBox *) child("comboBoxFieldToSearch");
    cb->setCurrentItem(cb->count() - 1);
  } else {
    moveCol(c, QString::null);
  }
}

void FLTableDB::putFirstCol(int c)
{
  moveCol(c, 0);
}

// Silix - dpinelo
void FLTableDB::putSecondCol(const QString &c)
{
  // Aprovechamos que siempre, el combo box de filtro presenta sus items ordenados igual que el grid
  if (c != comboBoxFieldToSearch->text(comboBoxFieldToSearch->currentItem() + 1)) {
    moveCol(c, comboBoxFieldToSearch->text(comboBoxFieldToSearch->currentItem() + 1), false);
  }
}

// Silix - dpinelo
void FLTableDB::putSecondCol(int c)
{
  // Aprovechamos que siempre, el combo box de filtro presenta sus items ordenados igual que el grid
  if (c != (comboBoxFieldToSearch->currentItem() + 1)) {
    moveCol(c, (comboBoxFieldToSearch->currentItem() + 1), false);
  }
}

void FLTableDB::moveCol(const QString &from, const QString &to, bool firstSearch)
{
  if (!topWidget || !cursor_ || !showed)
    return ;

  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return ;

  QHeader *horizHeader = tableRecords()->horizontalHeader();
  int i = sortColumn_, hCount = horizHeader->count();
  int iFrom = (from.isEmpty() ? sortColumn_ : sortColumn_ - 1);
  int iTo = (to.isEmpty() ? sortColumn_ : sortColumn_ - 1);
  QString fromFieldName(tMD->fieldAliasToName(from));
  QString toFieldName(tMD->fieldAliasToName(to));

  while (i < hCount && (iFrom < sortColumn_ || iTo < sortColumn_)) {
    if (iFrom < sortColumn_ && horizHeader->label(i) == tMD->fieldNameToAlias(fromFieldName))
      iFrom = i;
    if (iTo < sortColumn_ && horizHeader->label(i) == tMD->fieldNameToAlias(toFieldName))
      iTo = i;
    ++i;
  }
  if (iFrom < sortColumn_) {
#ifdef FL_DEBUG
    qWarning(tr("FLTableDB : No se puede cambiar la posicion de la columna, campo ( %1 ) no encontrado").arg(from));
#endif
    return;
  }
  if (iTo < sortColumn_) {
#ifdef FL_DEBUG
    qWarning(tr("FLTableDB : No se puede cambiar la posicion de la columna, campo ( %1 ) no encontrado").arg(to));
#endif
    return;
  }
  moveCol(iFrom - sortColumn_, iTo - sortColumn_, firstSearch);
}

void FLTableDB::moveCol(int from, int to, bool firstSearch)
{
  if (from == to || !lineEditSearch || !comboBoxFieldToSearch || !comboBoxFieldToSearch2 || !cursor_)
    return ;

  if (comboBoxFieldToSearch->text(from) == "*" || comboBoxFieldToSearch->text(to) == "*")
    return;

  QHeader *horizHeader = tableRecords()->horizontalHeader();

  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return ;

  tableRecords_->hide();

  int fromS = from + sortColumn_, toS = to + sortColumn_;
  QString fieldName(tMD->fieldAliasToName(horizHeader->label(fromS)));
  QString fieldNameItem;
  QString textSearch(lineEditSearch->text());

  if (!textSearch.isEmpty())
    textSearch = cursor_->QSqlCursor::value(fieldName).toString();

  if (fromS > toS) {
    for (int i = fromS - 1; i >= toS; --i) {
      fieldNameItem = tMD->fieldAliasToName(horizHeader->label(i));
      tableRecords_->setColumn(i + 1, fieldNameItem, fieldNameItem);
    }
  } else {
    for (int i = fromS + 1; i <= toS; ++i) {
      fieldNameItem = tMD->fieldAliasToName(horizHeader->label(i));
      tableRecords_->setColumn(i - 1, fieldNameItem, fieldNameItem);
    }
  }
  tableRecords_->setColumn(toS, fieldName, fieldName);

  refresh(true);
  if (!textSearch.isEmpty()) {
    refresh(false, true);
    if (firstSearch) {
      disconnect(lineEditSearch, SIGNAL(textChanged(const QString &)), this, SLOT(filterRecords(const QString &)));
      lineEditSearch->setText(textSearch);
      connect(lineEditSearch, SIGNAL(textChanged(const QString &)), this, SLOT(filterRecords(const QString &)));
      lineEditSearch->selectAll();
    }
    seekCursor();
    QTimer::singleShot(0, tableRecords_, SLOT(ensureRowSelectedVisible()));
  } else
    refreshDelayed();
  if (!sender())
    lineEditSearch->setFocus();
}

void FLTableDB::setReadOnly(const bool mode)
{
  if (tableRecords_) {
    readonly_ = mode;
    tableRecords_->setFLReadOnly(mode);
    emit readOnlyChanged(mode);
  }
  reqReadOnly_ = mode;
}

void FLTableDB::setEditOnly(const bool mode)
{
  if (tableRecords_) {
    editonly_ = mode;
    tableRecords_->setEditOnly(mode);
    emit editOnlyChanged(mode);
  }
  reqEditOnly_ = mode;
}

void FLTableDB::setInsertOnly(const bool mode)
{
  if (tableRecords_) {
    insertonly_ = mode;
    tableRecords_->setInsertOnly(mode);
    emit insertOnlyChanged(mode);
  }
  reqInsertOnly_ = mode;
}

void FLTableDB::setFilter(const QString &f)
{
  filter_ = f;
}

QString FLTableDB::filter()
{
  return filter_;
}

QString FLTableDB::findFilter()
{
  return tdbFilterLastWhere_;
}


void FLTableDB::refreshDelayed(int msec, const bool refreshData)
{
  QString bfilter = filter_;
  if (!timer)
    return;

  timer->stop();

  const QObject *obj = sender();
  if (obj) {
    if (!obj->inherits("QTimer")) {
      timer->start(msec, true);
      return ;
    }
  } else {
    timer->start(msec, true);
    return ;
  }

  if (cursor_->modeAccess() != FLSqlCursor::BROWSE)
    return;
  if (refreshData) {
    refresh(false, true);
  }
  seekCursor();
}

void FLTableDB::refresh(const bool refreshHead, const bool refreshData)
{
  if (!lineEditSearch || !comboBoxFieldToSearch || !comboBoxFieldToSearch2 || !cursor_ || (topWidget && !topWidget->isShown()))
    return ;

  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return ;

  if (tableName_.isEmpty())
    tableName_ = tMD->name();

  if (checkColumnEnabled_) {
    if (!checkColumnVisible_) {
      FLFieldMetaData *fieldCheck = tMD->field(fieldNameCheckColumn_);
      if (!fieldCheck) {
        fieldNameCheckColumn_ = tMD->name() + "_check_column";
        fieldCheck = new FLFieldMetaData(fieldNameCheckColumn_, tr(aliasCheckColumn_), true, false, FLFieldMetaData::Check, 0, false, true,
                                         true, 0, 0, false, false, false, QVariant(), false, QString::null, true, false, false);
        tMD->addFieldMD(fieldCheck);
      }
      if (!cursor_->contains(fieldNameCheckColumn_)) {
        QSqlRecord recordCursor(*(cursor_->editBuffer()));
        QSqlFieldInfo fakeFieldInfo(fieldNameCheckColumn_, QVariant::Bool, -1, -1, -1, QVariant(), 0, false, false, true);
        cursor_->clear();
        cursor_->append(fakeFieldInfo);
        for (int i = 0; i < recordCursor.count(); ++i)
          cursor_->append(QSqlFieldInfo(*(recordCursor.field(i))));
        setTableRecordsCursor();
        tableRecords_->setColumn(0, fieldNameCheckColumn_, aliasCheckColumn_);
        sortColumn_ = 1;
        sortColumn2_ = 2;
        sortColumn3_ = 3;
        QSqlRecord *buffer_ = cursor_->editBuffer(true);
        for (uint i = 0; i < buffer_->count(); ++i)
          buffer_->setGenerated(i, true);
      }
      checkColumnVisible_ = true;
    }
  } else {
    if (checkColumnVisible_) {
      tMD->removeFieldMD(fieldNameCheckColumn_);
      if (cursor_->contains(fieldNameCheckColumn_)) {
        QSqlRecord recordCursor(*(cursor_->editBuffer()));
        cursor_->clear();
        for (int i = 1; i < recordCursor.count(); ++i)
          cursor_->append(QSqlFieldInfo(*(recordCursor.field(i))));
        setTableRecordsCursor();
        sortColumn_ = 0;
        sortColumn2_ = 1;
        sortColumn3_ = 2;
      }
      checkColumnVisible_ = false;
    }
  }

  if (refreshHead) {
    QHeader *horizHeader = tableRecords()->horizontalHeader();
    horizHeader->hide();
    tableRecords_->QDataTable::refresh(QDataTable::RefreshColumns);

    FLFieldMetaData *field;
    for (int i = sortColumn_; i < tableRecords_->numCols(); ++i) {
      field = tMD->field(tMD->fieldAliasToName(horizHeader->label(i)));
      if (!field)
        continue;
      if (!field->visibleGrid()) {
        horizHeader->removeLabel(i);
        tableRecords_->removeColumn(i);
        if (i < tableRecords_->numCols())
          i--;
      }
    }

    QStringList s = QStringList() << tMD->fieldAliasToName(horizHeader->label(sortColumn_)) + (orderAsc_ ? " ASC" : " DESC");
    s << tMD->fieldAliasToName(horizHeader->label(sortColumn2_)) + (orderAsc2_ ? " ASC" : " DESC");
    s << tMD->fieldAliasToName(horizHeader->label(sortColumn3_)) + (orderAsc3_ ? " ASC" : " DESC");

    QString functionQSA = "";

    if (functionQSA.isEmpty()) {
      QString idMod(cursor_->db()->managerModules()->idModuleOfFile(cursor_->metadata()->name() +
                                                             QString::fromLatin1(".mtd")));
      functionQSA = idMod + QString::fromLatin1(".tableDB_setSort_") + cursor_->metadata()->name();
    }
                                                     
    if (!functionQSA.isEmpty()) {
      QValueList<QVariant> vargs = QValueList<QVariant>();
      vargs.append(s);
      vargs.append(tMD->fieldAliasToName(horizHeader->label(sortColumn_)));
      vargs.append(orderAsc_);
      vargs.append(tMD->fieldAliasToName(horizHeader->label(sortColumn2_)));
      vargs.append(orderAsc2_);
      vargs.append(tMD->fieldAliasToName(horizHeader->label(sortColumn3_)));
      vargs.append(orderAsc3_);
      QSArgumentList args = QSArgumentList(vargs);
      QVariant v = aqApp->call(functionQSA,args, 0).variant();
      QStringList ret = v.asStringList();
      if (!v.isNull()) {
        s = ret;   
        qDebug("functionQSA:" + functionQSA + " : " + ret.join(", "));
      } else {
        qDebug("functionQSA:" + functionQSA + " -> NULL");
      }
    } else {
      qDebug("functionQSA: (empty)");
    }      
    
    tableRecords_->setSort(s);
    tableRecords_->QDataTable::refresh(QDataTable::RefreshColumns);
    comboBoxFieldToSearch->clear();
    for (int i = sortColumn_; i < tableRecords_->numCols(); ++i) {
      field = tMD->field(tMD->fieldAliasToName(horizHeader->label(i)));
      if (!field)
        continue;
      if (i == sortColumn_)
        sortField_ = field;
      if (comboBoxFieldToSearch->count() == (i - sortColumn_))
        comboBoxFieldToSearch->insertItem(field->alias());
      horizHeader->setLabel(i, field->alias());
      tableRecords_->setColumn(i, field->name(), field->alias());
    }
    comboBoxFieldToSearch2->clear();
    for (int i = sortColumn2_; i < tableRecords_->numCols(); ++i) {
      field = tMD->field(tMD->fieldAliasToName(horizHeader->label(i)));
      if (!field)
        continue;
      if (i == sortColumn2_)
        sortField2_ = field;
      if (comboBoxFieldToSearch2->count() == (i - sortColumn2_))
        comboBoxFieldToSearch2->insertItem(field->alias());
      horizHeader->setLabel(i, field->alias());
      tableRecords_->setColumn(i, field->name(), field->alias());
    }
    comboBoxFieldToSearch->insertItem("*");
    comboBoxFieldToSearch2->setCurrentText(comboBoxFieldToSearch->text(comboBoxFieldToSearch->currentItem() + 1));
    horizHeader->setClickEnabled(false);
    horizHeader->setClickEnabled(true, sortColumn_);
    horizHeader->setClickEnabled(true, sortColumn2_);
    horizHeader->setSortIndicator(-1, Qt::Ascending);
    horizHeader->setSortIndicator(sortColumn_, (orderAsc_ ? Qt::Ascending : Qt::Descending));
    horizHeader->show();
  }

  if (refreshData || sender()) {
    QString finalFilter = filter_;
    if (!tdbFilterLastWhere_.isEmpty()) {
      if (finalFilter.isEmpty())
        finalFilter = tdbFilterLastWhere_;
      else
        finalFilter += " and " + tdbFilterLastWhere_;
    }
    tableRecords()->setPersistentFilter(finalFilter);
    tableRecords_->refresh();
  }

  if (!initSearch_.isEmpty()) {
    disconnect(lineEditSearch, SIGNAL(textChanged(const QString &)), this, SLOT(filterRecords(const QString &)));
    lineEditSearch->setText(initSearch_);
    connect(lineEditSearch, SIGNAL(textChanged(const QString &)), this, SLOT(filterRecords(const QString &)));
    lineEditSearch->selectAll();
    initSearch_ = QString::null;
    seekCursor();
  }

  if (readonly_ != reqReadOnly_)
    setReadOnly(reqReadOnly_);
  if (editonly_ != reqEditOnly_)
    setEditOnly(reqEditOnly_);
  if (insertonly_ != reqInsertOnly_)
    setInsertOnly(reqInsertOnly_);

  if (showed && tableRecords_ && tableRecords_->isHidden())
    tableRecords_->show();
}

void FLTableDB::filterRecords(const QString &p)
{
  if (!topWidget || !cursor_ || (p.isEmpty() && filter_.isEmpty()))
    return ;

  if (!sortField_) {
    FLTableMetaData *tMD = cursor_->metadata();
    if (!tMD)
      return;
    sortField_ = tMD->field(tMD->primaryKey());
  }

  if (sortField_->type() == FLFieldMetaData::Unlock)
    return ;

  bool refreshData = filter_.contains("%");
  QString fieldSearch = comboBoxFieldToSearch->text(comboBoxFieldToSearch->currentItem());
  bool allFields = fieldSearch == "*";
  int msec_refresh = 400;
  QString bfilter = "";
  if (!p.isEmpty() && allFields) {
    QString FTSFunction = cursor_->metadata()->FTSFunction();
    if (FTSFunction.isEmpty()) {
      FLTableMetaData::FLFieldMetaDataList *fieldList = cursor_->metadata() ->fieldList();
      if (fieldList) {
	FLFieldMetaData *field;
	QDictIterator<FLFieldMetaData> it(*fieldList);
	bfilter = "( false";
	while ((field = it.current()) != 0) {
	  ++it;
	  bool searchField = true;
	  if (!field->visibleGrid()) searchField = false;
	  if (field->type() != QVariant::String) searchField = false;
	  QStringList sOptions = field->searchOptions();
	  QString allFieldSearchInclude = "allfieldsearch:include";
	  QString allFieldSearchExclude = "allfieldsearch:exclude";

	  if (sOptions.contains(allFieldSearchExclude) > 0) {
	    if (searchField) {
	      //qDebug("Excluding field in allfield search: " + field->name());
	      searchField = false;
	    }
	  }
	  if (sOptions.contains(allFieldSearchInclude) > 0) {
	    if (!searchField) {
	      //qDebug("Including field in allfield search: " + field->name());
	      searchField = true;
	    }
	  }
        
	  if (searchField) {
	    bfilter += " OR ";
	    bfilter += cursor_->db()->manager()->formatAssignValueLike(field, p, true);
	  }
	}
        bfilter += ")";
        msec_refresh = 800;
      }
    } else {
      QString ftsfilter = p;
      QString tablename = cursor_->metadata()->name();
      ftsfilter = ftsfilter.replace(",", " ");
      ftsfilter = ftsfilter.replace("&", " ");
      ftsfilter = ftsfilter.replace("|", " ");
      ftsfilter = ftsfilter.replace("'", " ");
      ftsfilter = ftsfilter.replace("\"", " ");
      ftsfilter = ftsfilter.replace(":", " ");
      ftsfilter = ftsfilter.replace("%", " ");
      ftsfilter = ftsfilter.replace(QRegExp("\\s+"), " ");
      ftsfilter = ftsfilter.replace(QRegExp("(^\\s+|\\s+$)"), "");
      ftsfilter = ftsfilter.replace(" ", " & ");
      bfilter += FTSFunction + "(" + tablename + ") @@ to_tsquery('" + ftsfilter + ":*')";
      qDebug("Using Full Text Search: " + bfilter);
    }
  }

  if ((!p.isEmpty() || !refreshData) && !allFields && !p.contains("'") && !p.contains("\\")) {
    if (cursor_->metadata()->isQuery()) {
      FLSqlQuery *qry = cursor_->db()->manager()->query(cursor_->metadata()->query(), cursor_);

      if (qry) {
        QStringList list(qry->fieldList());
        QString qField;

        for (QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
          qField = *it;
          if (qField.endsWith("." + sortField_->name()))
            break;
        }
        bfilter = cursor_->db()->manager()->formatAssignValueLike(qField, sortField_, p, true);
        qry->deleteLater();
      }
    } else
      bfilter = cursor_->db()->manager()->formatAssignValueLike(sortField_, p, true);
    fieldSearch = sortField_->name();
  }

  QString functionQSA = tableDB_filterRecords_functionName_;

  if (functionQSA.isEmpty()) {
    QString idMod(cursor_->db()->managerModules()->idModuleOfFile(cursor_->metadata()->name() +
                                                           QString::fromLatin1(".mtd")));
    functionQSA = idMod + QString::fromLatin1(".tableDB_filterRecords_") + cursor_->metadata()->name();
  }
                                                   
  if (!functionQSA.isEmpty()) {
    QValueList<QVariant> vargs = QValueList<QVariant>();
    vargs.append(cursor_->metadata()->name());
    vargs.append(p);
    vargs.append(fieldSearch);
    vargs.append(bfilter);
    QSArgumentList args = QSArgumentList(vargs);
    QVariant v = aqApp->call(functionQSA,args, 0).variant();
    QString ret = v.toString();
    if (!ret.isNull()) {
      bfilter = ret;   
      qDebug("functionQSA:" + functionQSA + " : " + ret.replace("%","%%"));
    } else {
      qDebug("functionQSA:" + functionQSA + " -> NULL");
    }
  } else {
    qDebug("functionQSA: (empty)");
  }

  refreshDelayed(msec_refresh, !bfilter.isEmpty() || refreshData);
  filter_ = bfilter;
}

QString FLTableDB::tableName() const
{
  return tableName_;
}

QString FLTableDB::foreignField() const
{
  return foreignField_;
}

QString FLTableDB::fieldRelation() const
{
  return fieldRelation_;
}

void FLTableDB::setTableName(const QString &fT)
{
  tableName_ = fT;
  if (showed) {
    if (topWidget)
      initCursor();
    else
      initFakeEditor();
  }
}

void FLTableDB::setForeignField(const QString &fN)
{
  foreignField_ = fN;
  if (showed) {
    if (topWidget)
      initCursor();
    else
      initFakeEditor();
  }
}

void FLTableDB::setFieldRelation(const QString &fN)
{
  fieldRelation_ = fN;
  if (showed) {
    if (topWidget)
      initCursor();
    else
      initFakeEditor();
  }
}

void FLTableDB::deleteRecord()
{
  QWidget *w = ::qt_cast<QWidget *>(sender());
  if (w && (!cursor_ || readonly_ || insertonly_ || editonly_
            || (cursor_->cursorRelation() && cursor_->cursorRelation()->isLocked()))) {
    w->setDisabled(true);
    return ;
  }
  cursor_->deleteRecord();
}

void FLTableDB::browseRecord()
{
  if (!cursor_)
    return ;
  cursor_->browseRecord();
}

void FLTableDB::editRecord()
{
  QWidget *w = ::qt_cast<QWidget *>(sender());
  if (w && (!cursor_ || readonly_ || insertonly_
            || (cursor_->cursorRelation() && cursor_->cursorRelation()->isLocked()))) {
    w->setDisabled(true);
    return ;
  }
  cursor_->editRecord();
}

void FLTableDB::insertRecord()
{
  QWidget *w = ::qt_cast<QWidget *>(sender());
  if (w && (!cursor_ || readonly_ || editonly_
            || (cursor_->cursorRelation() && cursor_->cursorRelation()->isLocked()))) {
    w->setDisabled(true);
    return ;
  }
  cursor_->insertRecord();
}

void FLTableDB::copyRecord()
{
  QWidget *w = ::qt_cast<QWidget *>(sender());
  if (w && (!cursor_ || readonly_ || editonly_
            || (cursor_->cursorRelation() && cursor_->cursorRelation()->isLocked()))) {
    w->setDisabled(true);
    return ;
  }
  cursor_->copyRecord();
}

void FLTableDB::initCursor()
{
  if (!topWidget || !cursor_)
    return;

  if (!cursor_->metadata())
    return;

  FLTableMetaData *tMD = 0;

  if (!sortField_) {
    tMD = cursor_->metadata();
    if (tMD)
      sortField_ = tMD->field(tMD->primaryKey());
  }

  if (!tableName_.isEmpty()) {
    if (!cursor_->db()->manager()->existsTable(tableName_))
      tMD = cursor_->db()->manager()->createTable(tableName_);
    else
      tMD = cursor_->db()->manager()->metadata(tableName_);

    if (!tMD)
      return ;

    if (foreignField_.isEmpty() || fieldRelation_.isEmpty()) {
      if (!cursor_->metadata()) {
        return ;
      }
      if (cursor_->metadata() ->name() != tableName_) {
        QObject *ctxt = cursor_->context();
        cursor_ = new FLSqlCursor(tableName_, true, cursor_->db()->connectionName(), 0, 0, this);
        if (cursor_) {
          cursor_->setContext(ctxt);
          cursorAux = 0;
        }
        return ;
      }
    } else {
      FLSqlCursor *cursorTopWidget = ::qt_cast<FLFormDB *>(topWidget) ->cursor();
      if (cursorTopWidget && cursorTopWidget->metadata() ->name() != tableName_)
        cursor_ = cursorTopWidget;
    }
  }

  if (tableName_.isEmpty() || foreignField_.isEmpty()
      || fieldRelation_.isEmpty() || cursorAux)
    return ;

  cursorAux = cursor_;
  QString curName(cursor_->metadata()->name());
  FLRelationMetaData *rMD = cursor_->metadata()->relation(foreignField_, fieldRelation_, tableName_);
  FLRelationMetaData *testM1 = tMD->relation(fieldRelation_, foreignField_, curName);
  bool checkIntegrity = false;

  if (!rMD) {
    if (testM1)
      checkIntegrity = (testM1->cardinality() == FLRelationMetaData::RELATION_M1);
    FLFieldMetaData *fMD = cursor_->metadata()->field(foreignField_);
    if (fMD) {
      rMD = new FLRelationMetaData(tableName_, fieldRelation_, FLRelationMetaData::RELATION_1M, false, false, checkIntegrity);
      fMD->addRelationMD(rMD);
#ifdef FL_DEBUG
      qWarning(tr("FLTableDB : La relación entre la tabla del formulario %1 y esta tabla %2 de este campo no existe, pero sin embargo se han indicado los campos de relación( %3, %4 )")
               .arg(curName).arg(tableName_).arg(fieldRelation_).arg(foreignField_));
      qWarning(tr("FLTableDB : Creando automáticamente %1.%2 --1M--> %3.%4")
               .arg(curName).arg(foreignField_).arg(tableName_).arg(fieldRelation_));
#endif
    }
#ifdef FL_DEBUG
    else {
      qWarning(tr("FLTableDB : El campo ( %1 ) indicado en la propiedad foreignField no se encuentra en la tabla ( %2 )")
               .arg(foreignField_).arg(curName));
    }
#endif
  }

  rMD = testM1;
  if (!rMD) {
    FLFieldMetaData *fMD = tMD->field(fieldRelation_);
    if (fMD) {
      rMD = new FLRelationMetaData(curName, foreignField_, FLRelationMetaData::RELATION_1M, false, false, false);
      fMD->addRelationMD(rMD);
#ifdef FL_DEBUG
      qWarning(tr("FLTableDB : Creando automáticamente %1.%2 --1M--> %3.%4")
               .arg(tableName_).arg(fieldRelation_).arg(curName).arg(foreignField_));
#endif
    }
#ifdef FL_DEBUG
    else {
      qWarning(tr("FLTableDB : El campo ( %1 ) indicado en la propiedad fieldRelation no se encuentra en la tabla ( %2 )")
               .arg(fieldRelation_).arg(tableName_));
    }
#endif
  }

  cursor_ = new FLSqlCursor(tableName_, true, cursor_->db()->connectionName(), cursorAux, rMD, this);

  if (!cursor_) {
    cursor_ = cursorAux;
    cursorAux = 0;
  } else {
    cursor_->setContext(cursorAux->context());
    if (showed)
      disconnect(cursorAux, SIGNAL(newBuffer()), this, SLOT(refresh()));
    connect(cursorAux, SIGNAL(newBuffer()), this, SLOT(refresh()));
  }

  if (cursorAux && topWidget->isA("FLFormSearchDB")) {
    topWidget->setCaption(cursor_->metadata() ->alias());
    ::qt_cast<FLFormSearchDB *>(topWidget) ->setCursor(cursor_);
  }
}

void FLTableDB::showEvent(QShowEvent *e)
{
  QWidget::showEvent(e);
  showWidget();
}

void FLTableDB::showWidget()
{
  if (showed)
    return;

  if (!topWidget) {
    initFakeEditor();
    showed = true;
    return;
  }

  if (!cursor_)
    return;

  showed = true;

  FLTableMetaData *tMD;

  if (!tableName_.isEmpty()) {
    if (!cursor_->db()->manager()->existsTable(tableName_))
      tMD = cursor_->db()->manager()->createTable(tableName_);
    else
      tMD = cursor_->db()->manager()->metadata(tableName_);
    if (!tMD)
      return ;
  }

  tableRecords();

  if (!cursorAux) {
    if (!initSearch_.isEmpty()) {
      refresh(true, true);
      QTimer::singleShot(0, tableRecords_, SLOT(ensureRowSelectedVisible()));
    } else {
      refresh(true);
      if (tableRecords_->numRows() <= 0)
        refresh(false, true);
      else
        refreshDelayed();
    }
    if (!topWidget->isA("FLFormRecordDB"))
      lineEditSearch->setFocus();
  }

  if (cursorAux) {
    if (topWidget->isA("FLFormRecordDB")
        && cursorAux->modeAccess() == FLSqlCursor::BROWSE) {
      cursor_->setEdition(false);
      setReadOnly(true);
    }
    if (!initSearch_.isEmpty()) {
      refresh(true, true);
      QTimer::singleShot(0, tableRecords_, SLOT(ensureRowSelectedVisible()));
    } else {
      refresh(true);
      if (tableRecords_->numRows() <= 0)
        refresh(false, true);
      else
        refreshDelayed();
    }
  } else if (topWidget->isA("FLFormRecordDB")
             && cursor_->modeAccess() == FLSqlCursor::BROWSE && !tMD->isQuery()) {
    cursor_->setEdition(false);
    setReadOnly(true);
  }
}

FLSqlCursor *FLTableDB::cursor()
{
  return cursor_;
}

void FLTableDB::setOrderCols(QStringList &fields)
{
  if (!cursor_)
    return;
  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return;

  if (!showed)
    showWidget();

  for (QStringList::Iterator it = fields.begin(); it != fields.end();) {
    FLFieldMetaData *fmd = tMD->field(*it);
    if (!fmd || !fmd->visibleGrid())
      it = fields.remove(it);
    else
      ++it;
  }

  QHeader *horizHeader = tableRecords()->horizontalHeader();
  int i = fields.count(), hCount = horizHeader->count();

  if (i > (hCount - sortColumn_))
    return;

  int c = 0;
  QString fieldName;
  QString fieldNameAlias;
  QString fieldAlias;
  QString itStr;
  QStringList::const_iterator it(fields.end());

  tableRecords_->hide();

  do {
    --it;
    --i;
    itStr = *it;
    fieldAlias = tMD->fieldNameToAlias(itStr);

    for (c = sortColumn_; horizHeader->label(c) != fieldAlias && c < hCount; ++c) {}

    if (c < hCount && c != i + sortColumn_) {
      fieldNameAlias = horizHeader->label(i + sortColumn_);
      fieldName = tMD->fieldAliasToName(fieldNameAlias);

      if (fieldName != itStr) {
        tableRecords_->setColumn(i + sortColumn_, itStr, fieldAlias);
        horizHeader->setLabel(i + sortColumn_, fieldAlias);

        tableRecords_->setColumn(c, fieldName, fieldNameAlias);
        horizHeader->setLabel(c, fieldNameAlias);

        tableRecords_->QDataTable::refresh(QDataTable::RefreshColumns);
      }
    }
  } while (it != fields.begin());

  QString textSearch(lineEditSearch->text());
  if (!textSearch.isEmpty())
    textSearch = cursor_->QSqlCursor::value(fieldName).toString();

  refresh(true);
  if (!textSearch.isEmpty()) {
    refresh(false, true);
    disconnect(lineEditSearch, SIGNAL(textChanged(const QString &)), this, SLOT(filterRecords(const QString &)));
    lineEditSearch->setText(textSearch);
    connect(lineEditSearch, SIGNAL(textChanged(const QString &)), this, SLOT(filterRecords(const QString &)));
    lineEditSearch->selectAll();
    seekCursor();
    QTimer::singleShot(0, tableRecords_, SLOT(ensureRowSelectedVisible()));
  } else
    refreshDelayed();
}

QStringList FLTableDB::orderCols()
{
  QStringList list;

  if (!cursor_)
    return list;
  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return list;

  if (!showed)
    showWidget();

  QHeader *horizHeader = tableRecords()->horizontalHeader();
  int hCount = horizHeader->count();

  for (int i = sortColumn_; i < hCount; ++i)
    list.append(tMD->fieldAliasToName(horizHeader->label(i)));

  return list;
}

void FLTableDB::seekCursor()
{
  if (!cursor_)
    return ;

  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return ;

  QString textSearch(lineEditSearch->text());
  if (textSearch.isEmpty())
    return;

  QString fN(sortField_->name());
  textSearch.replace("%", "");

  if (!textSearch.contains("'") && !textSearch.contains("\\")) {
    QString sql(cursor_->executedQuery() + " LIMIT 1");
    QSqlQuery qry(sql, cursor_->db()->db());

    if (qry.first())
      cursor_->seek(cursor_->atFromBinarySearch(fN, qry.value(0).toString(), orderAsc_), false, true);
  }
}

bool FLTableDB::checkColumnEnabled() const
{
  return checkColumnEnabled_;
}

void FLTableDB::setCheckColumnEnabled(bool b)
{
  checkColumnEnabled_ = b;
}

QString FLTableDB::aliasCheckColumn() const
{
  return aliasCheckColumn_;
}

void FLTableDB::setAliasCheckColumn(const QString &t)
{
  aliasCheckColumn_ = t;
}

void FLTableDB::switchSortOrder(int col)
{
  if (checkColumnVisible_)
    --col;
  if (col == 0) {
    orderAsc_ = !orderAsc_;
  } else if (col == 1) {
    orderAsc2_ = !orderAsc2_;
  }
  tableRecords()->hide();
  refresh( true, true );
}

void FLTableDB::setSortOrder(int ascending)
{
  if (orderAsc_ == ascending) return;
  
  orderAsc_ = ascending;
  tableRecords()->hide();
  refresh(true, true);
}

bool FLTableDB::isSortOrderAscending()
{
    return orderAsc_;
}


void FLTableDB::activeTabData(bool on)
{
  if (topWidget && tabTable->visibleWidget() != tabData) {
    refreshTabData();
    tabTable->raiseWidget(tabData);
  }
}

void FLTableDB::activeTabFilter(bool on)
{
  if (topWidget && tabTable->visibleWidget() != tabFilter) {
    refreshTabFilter();
    tabTable->raiseWidget(tabFilter);
  }
}

void FLTableDB::refreshTabData()
{
  QString tdbWhere(tdbFilterBuildWhere());
  if (tdbWhere != tdbFilterLastWhere_) {
    tdbFilterLastWhere_ = tdbWhere;
    refresh(false, true);
  }
}

void FLTableDB::refreshTabFilter()
{
  QHeader *horizHeader = tableRecords()->horizontalHeader();
  if (!horizHeader)
    return;

  int hCount = horizHeader->count() - sortColumn_;

  if (tdbFilter->numRows() < hCount && cursor_) {
    FLTableMetaData *tMD = cursor_->metadata();
    if (!tMD)
      return ;

    FLFieldMetaData *field = 0;
    QWidget *editor_ = 0;
    int type, len, partInteger, partDecimal;
    QString rX;
    bool ol;

    tdbFilter->setSelectionMode(QTable::NoSelection);
    tdbFilter->setNumCols(5);
    tdbFilter->setNumRows(hCount);
    tdbFilter->setColumnReadOnly(0, true);
    tdbFilter->setColumnLabels(QStringList::split(',', tr("Campo,Condición,Valor,Desde,Hasta")));

    mapCondType.insert(tr("Todos"),              FLTableDB::All);
    mapCondType.insert(tr("Contiene Valor"),     FLTableDB::Contains);
    mapCondType.insert(tr("Empieza por Valor"),  FLTableDB::Starts);
    mapCondType.insert(tr("Acaba por Valor"),    FLTableDB::End);
    mapCondType.insert(tr("Igual a Valor"),      FLTableDB::Equal);
    mapCondType.insert(tr("Distinto de Valor"),  FLTableDB::Dist);
    mapCondType.insert(tr("Mayor que Valor"),    FLTableDB::Greater);
    mapCondType.insert(tr("Menor que Valor"),    FLTableDB::Less);
    mapCondType.insert(tr("Desde - Hasta"),      FLTableDB::FromTo);
    mapCondType.insert(tr("Vacío"),              FLTableDB::Null);
    mapCondType.insert(tr("No Vacío"),           FLTableDB::NotNull);

    for (int i = 0; i < hCount; ++i) {
      tdbFilter->setText(i, 0, horizHeader->label(i + sortColumn_)) ;

      field = tMD->field(tMD->fieldAliasToName(horizHeader->label(i + sortColumn_)));
      if (!field)
        continue;

      type = field->type();
      len = field->length();
      partInteger = field->partInteger();
      partDecimal = field->partDecimal();
      rX = field->regExpValidator();
      ol = field->hasOptionsList();

      QComboBox *cond;

      if (type != QVariant::Pixmap) {
        QStringList condList;
        condList << tr("Todos") <<
                 tr("Igual a Valor") <<
                 tr("Distinto de Valor") <<
                 tr("Vacío") <<
                 tr("No Vacío");
        if (type != QVariant::Bool) {
          condList << tr("Contiene Valor") <<
                   tr("Empieza por Valor") <<
                   tr("Acaba por Valor") <<
                   tr("Mayor que Valor") <<
                   tr("Menor que Valor") <<
                   tr("Desde - Hasta");
        }
        cond = new QComboBox;
        cond->insertStringList(condList);
        tdbFilter->setCellWidget(i, 1, cond);
      }

      for (int j = 2; j < 5; ++j) {
        editor_ = 0;
        switch (type) {
          case QVariant::UInt:
          case QVariant::Int:
          case QVariant::Double:
          case QVariant::String:
          case QVariant::StringList:
            if (ol) {
              editor_ = new QComboBox;
              QStringList olTranslated;
              QStringList olNoTranslated(field->optionsList());
              int countOl = olNoTranslated.count();
              for (int i = 0; i < countOl; ++i)
                olTranslated << FLUtil::translate("MetaData", olNoTranslated[ i ]);
              ::qt_cast<QComboBox *>(editor_)->insertStringList(olTranslated);
            } else {
              editor_ = new FLLineEdit(0);

              if (type == QVariant::Double) {
                ::qt_cast<FLLineEdit *>(editor_)->setValidator(new FLDoubleValidator(0, pow(10, partInteger) - 1, partDecimal, editor_));
                ::qt_cast<FLLineEdit *>(editor_)->setAlignment(Qt::AlignRight);
              } else {
                if (type == QVariant::UInt || type == QVariant::Int) {
                  if (type == QVariant::UInt)
                    ::qt_cast<FLLineEdit *>(editor_)->setValidator(new FLUIntValidator(0, ((int) pow(10, partInteger) - 1), editor_));
                  else
                    ::qt_cast<FLLineEdit *>(editor_)->setValidator(new FLIntValidator(((int)(pow(10, partInteger) - 1) * (-1)),
                                                                                      ((int) pow(10, partInteger) - 1), editor_));
                  ::qt_cast<FLLineEdit *>(editor_)->setAlignment(Qt::AlignRight);
                } else {
                  if (len > 0)
                    ::qt_cast<FLLineEdit *>(editor_)->setMaxLength(len);
                  if (!rX.isEmpty()) {
                    QRegExp r(rX);
                    ::qt_cast<FLLineEdit *>(editor_)->setValidator(new QRegExpValidator(r, editor_));
                  }
                  ::qt_cast<FLLineEdit *>(editor_)->setAlignment(Qt::AlignLeft);
                }
              }
            }
            break;

          case FLFieldMetaData::Serial:
            editor_ = new FLSpinBox;
            ::qt_cast<FLSpinBox *>(editor_)->setMaxValue(((int) pow(10, partInteger) - 1));
            break;

          case QVariant::Pixmap:
            tdbFilter->setRowReadOnly(i, true);
            break;

          case QVariant::Date: {
            editor_ = new QDateEdit;
            ::qt_cast<QDateEdit *>(editor_)->setOrder(QDateEdit::DMY);
            ::qt_cast<QDateEdit *>(editor_)->setAutoAdvance(true);
            ::qt_cast<QDateEdit *>(editor_)->setSeparator("-");
            QDate da;
            ::qt_cast<QDateEdit *>(editor_)->setDate(da.currentDate());
          }
          break;

          case QVariant::Time: {
            editor_ = new QTimeEdit;
            QTime timeNow = QTime::currentTime();
            ::qt_cast<QTimeEdit *>(editor_) ->setTime(timeNow);
          }
          break;

          case FLFieldMetaData::Unlock:
          case QVariant::Bool:
            editor_ = new FLCheckBox(0);
            break;
        }

        if (editor_)
          tdbFilter->setCellWidget(i, j, editor_);
      }
    }

    for (int k = 0; k < 5; ++k)
      tdbFilter->adjustColumn(k);
  }
}

FLTableDB::CondType FLTableDB::decodeCondType(const QString &strCondType) const
{
  if (mapCondType.contains(strCondType))
    return mapCondType[ strCondType ];
  return FLTableDB::All;
}

QString FLTableDB::tdbFilterBuildWhere()
{
  if (!topWidget)
    return QString::null;
  int rCount = tdbFilter->numRows();
  if (!rCount || !cursor_)
    return QString::null;

  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return QString::null;

  FLFieldMetaData *field = 0;
  QComboBox *cond = 0;
  int type;
  CondType condType;
  QString fieldName, condValue, where, fieldArg, arg2, arg4;
  bool ol;

  for (int i = 0; i < rCount; ++i) {
    fieldName = tMD->fieldAliasToName(tdbFilter->text(i, 0));
    field = tMD->field(fieldName);
    if (!field)
      continue;

    cond = ::qt_cast<QComboBox *>(tdbFilter->cellWidget(i, 1));

    if (!cond)
      continue;

    condType = decodeCondType(cond->currentText());

    if (condType == FLTableDB::All)
      continue;

    if (tMD->isQuery()) {
      FLSqlQuery *qry = cursor_->db()->manager()->query(cursor_->metadata()->query(), cursor_);

      if (qry) {
        QStringList list(qry->fieldList());
        QString qField;

        for (QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
          qField = *it;
          if (qField.endsWith("." + fieldName))
            break;
        }

        fieldName = qField;
      }
    } else
      fieldName = tMD->name() + '.' + fieldName;

    fieldArg = fieldName;
    arg2 = arg4 = QString::null;
    type = field->type();
    ol = field->hasOptionsList();

    switch (type) {
      case QVariant::String:
      case QVariant::StringList: {
        fieldArg = "upper(" + fieldName + ")";
        case QVariant::UInt:
        case QVariant::Int:
        case QVariant::Double:
          if (ol) {
            if (condType == FLTableDB::FromTo) {
              QComboBox *editorOp1 = ::qt_cast<QComboBox *>(tdbFilter->cellWidget(i, 3));
              QComboBox *editorOp2 = ::qt_cast<QComboBox *>(tdbFilter->cellWidget(i, 4));
              arg2 = cursor_->db()->manager()->formatValue(type, editorOp1->currentText(), true);
              arg4 = cursor_->db()->manager()->formatValue(type, editorOp2->currentText(), true);
            } else {
              QComboBox *editorOp1 = ::qt_cast<QComboBox *>(tdbFilter->cellWidget(i, 2));
              arg2 = cursor_->db()->manager()->formatValue(type, editorOp1->currentText(), true);
            }
          } else {
            if (condType == FLTableDB::FromTo) {
              FLLineEdit *editorOp1 = ::qt_cast<FLLineEdit *>(tdbFilter->cellWidget(i, 3));
              FLLineEdit *editorOp2 = ::qt_cast<FLLineEdit *>(tdbFilter->cellWidget(i, 4));
              arg2 = cursor_->db()->manager()->formatValue(type, editorOp1->text(), true);
              arg4 = cursor_->db()->manager()->formatValue(type, editorOp2->text(), true);
            } else {
              FLLineEdit *editorOp1 = ::qt_cast<FLLineEdit *>(tdbFilter->cellWidget(i, 2));
              arg2 = cursor_->db()->manager()->formatValue(type, editorOp1->text(), true);
            }
          }
        }
        break;

      case FLFieldMetaData::Serial:
        if (condType == FLTableDB::FromTo) {
          FLSpinBox *editorOp1 = ::qt_cast<FLSpinBox *>(tdbFilter->cellWidget(i, 3));
          FLSpinBox *editorOp2 = ::qt_cast<FLSpinBox *>(tdbFilter->cellWidget(i, 4));
          arg2 = QString::number(editorOp1->value());
          arg4 = QString::number(editorOp2->value());
        } else {
          FLSpinBox *editorOp1 = ::qt_cast<FLSpinBox *>(tdbFilter->cellWidget(i, 2));
          arg2 = QString::number(editorOp1->value());
        }
        break;

      case QVariant::Pixmap:
        break;

      case QVariant::Date: {
        if (condType == FLTableDB::FromTo) {
          QDateEdit *editorOp1 = ::qt_cast<QDateEdit *>(tdbFilter->cellWidget(i, 3));
          QDateEdit *editorOp2 = ::qt_cast<QDateEdit *>(tdbFilter->cellWidget(i, 4));
          arg2 = cursor_->db()->manager()->formatValue(type, QVariant(editorOp1->date().toString("dd-MM-yyyy")));
          arg4 = cursor_->db()->manager()->formatValue(type, QVariant(editorOp2->date().toString("dd-MM-yyyy")));
        } else {
          QDateEdit *editorOp1 = ::qt_cast<QDateEdit *>(tdbFilter->cellWidget(i, 2));
          arg2 = cursor_->db()->manager()->formatValue(type, QVariant(editorOp1->date().toString("dd-MM-yyyy")));
        }
      }
      break;
      case QVariant::Time: {
        if (condType == FLTableDB::FromTo) {
          QTimeEdit *editorOp1 = ::qt_cast<QTimeEdit *>(tdbFilter->cellWidget(i, 3));
          QTimeEdit *editorOp2 = ::qt_cast<QTimeEdit *>(tdbFilter->cellWidget(i, 4));
          arg2 = cursor_->db()->manager()->formatValue(type, QVariant(editorOp1->time().toString(Qt::ISODate)));
          arg4 = cursor_->db()->manager()->formatValue(type, QVariant(editorOp2->time().toString(Qt::ISODate)));
        } else {
          QTimeEdit *editorOp1 = ::qt_cast<QTimeEdit *>(tdbFilter->cellWidget(i, 2));
          arg2 = cursor_->db()->manager()->formatValue(type, QVariant(editorOp1->time().toString(Qt::ISODate)));
        }
      }
      break;

      case FLFieldMetaData::Unlock:
      case QVariant::Bool: {
        FLCheckBox *editorOp1 = ::qt_cast<FLCheckBox *>(tdbFilter->cellWidget(i, 2));
        arg2 = cursor_->db()->manager()->formatValue(type, QString(editorOp1->isChecked() ? tr("Sí") : tr("No")));
      }
      break;
    }

    if (!where.isEmpty())
      where += " and";

    condValue = " " + fieldArg;
    if (condType == FLTableDB::Contains)
      condValue += " like '%%" + arg2.replace("'", "") + "%%'";
    else if (condType == FLTableDB::Starts)
      condValue += " like '" + arg2.replace("'", "") + "%%'";
    else if (condType == FLTableDB::End)
      condValue += " like '%%" + arg2.replace("'", "") + "'";
    else if (condType == FLTableDB::Equal)
      condValue += " = " + arg2;
    else if (condType == FLTableDB::Dist)
      condValue += " <> " + arg2;
    else if (condType == FLTableDB::Greater)
      condValue += " > " + arg2;
    else if (condType == FLTableDB::Less)
      condValue += " < " + arg2;
    else if (condType == FLTableDB::FromTo)
      condValue += " >= " + arg2 + " and " + fieldArg + " <= " + arg4;
    else if (condType == FLTableDB::Null)
      condValue += " is null ";
    else if (condType == FLTableDB::NotNull)
      condValue += " is not null ";

    where += condValue;
  }

  return where;
}

void FLTableDB::tdbFilterClear()
{
  if (!topWidget)
    return;
  int rCount = tdbFilter->numRows();
  QComboBox *cond = 0;
  for (int i = 0; i < rCount; ++i) {
    cond = ::qt_cast<QComboBox *>(tdbFilter->cellWidget(i, 1));
    if (cond)
      cond->setCurrentItem(0);
  }
}

bool FLTableDB::findHidden() const
{
  return findHidden_;
}

void FLTableDB::setFindHidden(bool h)
{
  if (findHidden_ != h) {
    findHidden_ = h;
    if (h)
      FrameFind->hide();
    else
      FrameFind->show();
  }
}

bool FLTableDB::filterHidden() const
{
  return filterHidden_;
}

void FLTableDB::setFilterHidden(bool h)
{
  if (filterHidden_ != h) {
    filterHidden_ = h;
    if (h)
      bgrDataFilter->hide();
    else
      bgrDataFilter->show();
  }
}

void FLTableDB::initFakeEditor()
{
  if (!fakeEditor_) {
    fakeEditor_ = new QTextEdit(tabData);
    fakeEditor_->setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 7, QSizePolicy::Expanding,
                                           fakeEditor_->sizePolicy().hasHeightForWidth()));
    ::qt_cast<QTextEdit *>(fakeEditor_)->setTabChangesFocus(true);
    fakeEditor_->setFocusPolicy(QTable::StrongFocus);
    setFocusProxy(fakeEditor_);
    tabDataLayout->addWidget(fakeEditor_);
    setTabOrder(fakeEditor_, lineEditSearch);
    setTabOrder(fakeEditor_, comboBoxFieldToSearch);
    fakeEditor_->show();
  }

  QString prty;
  if (!tableName_.isEmpty())
    prty += "tableName: " + tableName_ + "\n";
  if (!foreignField_.isEmpty())
    prty += "foreignField: " + foreignField_ + "\n";
  if (!fieldRelation_.isEmpty())
    prty += "fieldRelation: " + fieldRelation_ + "\n";
  ::qt_cast<QTextEdit *>(fakeEditor_)->setText(prty);
}

void FLTableDB::setEnabled(bool mode)
{
  setReadOnly(!mode);
}

void FLTableDB::setColumnWidth(const QString &field, int w)
{
  tableRecords()->setColumnWidth(field, w);
}

int FLTableDB::columnWidth(int c)
{
  return tableRecords()->columnWidth(c);
}

void FLTableDB::setRowHeight(int row, int h)
{
  tableRecords()->setRowHeight(row, h);
}

int FLTableDB::rowHeight(int row)
{
  return tableRecords()->rowHeight(row);
}

bool FLTableDB::showAllPixmaps() const
{
  return showAllPixmaps_;
}

void FLTableDB::setShowAllPixmaps(bool s)
{
  showAllPixmaps_ = s;
  if (topWidget)
    tableRecords()->setShowAllPixmaps(s);
}

QString FLTableDB::functionGetColor() const
{
  return functionGetColor_;
}

void FLTableDB::setFunctionGetColor(const QString &f)
{
  functionGetColor_ = f;
  if (topWidget) {
    if (f.contains('.'))
      tableRecords()->setFunctionGetColor(f);
    else
      tableRecords()->setFunctionGetColor(QString(topWidget->QObject::name()) + '.' + f);
  }
}

void FLTableDB::exportToOds()
{
  if (!cursor_)
    return ;
//-->Aulla : Desactiva exportar a ODS
if (FLSettings::readBoolEntry("ebcomportamiento/FLTableExport2Calc",false))
	{
	QMessageBox::information(this, tr("Opción deshabilitada"),
                                                 tr("Esta opción ha sido deshabilitada por el administrador"),
                                                  QMessageBox::Yes);
	return;
	}
//>--Aulla : Desactiva exportar a ODS
  FLTableMetaData *mtd = cursor_->metadata();
  if (!mtd)
    return;

  const AQOdsStyle titleStyle(Style::ALIGN_CENTER | Style::TEXT_BOLD);
  const AQOdsStyle borderBot(Style::BORDER_BOTTOM);
  const AQOdsStyle borderRight(Style::BORDER_RIGHT);
  const AQOdsStyle borderLeft(Style::BORDER_LEFT);
  const AQOdsStyle italic(Style::TEXT_ITALIC);

  FLDataTable *tdb = tableRecords();
  QHeader *horHeader = tdb->horizontalHeader();

  AQOdsGenerator odsGen;
  AQOdsSpreadSheet spreadsheet(odsGen);
  AQOdsSheet sheet(spreadsheet, mtd->alias());

  QProgressDialog progress(tr("Procesando..."),
                           tr("Cancelar"), tdb->numRows(),
                           this, tr("odsprogress"), true);
  progress.setProgress(0);

  {
    AQOdsRow row(sheet);
    row.addBgColor(AQOdsColor(0xe7e7e7));
    for (int i = 0; i < horHeader->count(); ++i) {
      row.opIn(titleStyle)
      .opIn(borderBot)
      .opIn(borderLeft)
      .opIn(borderRight)
      .opIn(horHeader->label(i));
    }
    row.close();
  }

  QSqlCursor *cur = tdb->sqlCursor();
  int curRow = tdb->currentRow();

  cur->QSqlCursor::first();

  for (int r = 0; r < tdb->numRows(); ++r) {
    if (progress.wasCanceled())
      break;
    AQOdsRow row(sheet);
    for (int c = 0; c < tdb->numCols(); ++c) {
      QVariant val(cur->QSqlCursor::value(tdb->indexOf(c)));
      switch (val.type()) {
        case QVariant::Double: {
          row.opIn(val.toDouble());
        }
        break;

        case QVariant::Date: {
          QString str(val.toDate().toString("dd/MM/yyyy"));
          if (str.isEmpty())
            row.coveredCell();
          else
            row.opIn(str);
        }
        break;

        case QVariant::Bool: {
          QString str(val.toBool() ? tr("Sí") : tr("No"));
          row.opIn(italic);
          row.opIn(str);
        }
        break;

        default: {
          QString str(val.toString());
	  if (str.contains('RK@')) 
				{
 				QSqlQuery q(QString::null, cursor_->db()->db());
  				q.exec("SELECT contenido FROM fllarge WHERE refkey = '" + str + "'");
  				while (q.next()) {
						str = q.value(0).toString();
						}
		   QString fileNameXpm(AQ_DISKCACHE_DIRPATH + '/' + mtd->name() +
                   QDateTime::currentDateTime().toString("ddMMyyyyhhmmsszzz") +
                   QString::fromLatin1(".xpm"));
                   QString fileNamePng(AQ_DISKCACHE_DIRPATH + '/' + mtd->name() +
                   QDateTime::currentDateTime().toString("ddMMyyyyhhmmsszzz") +
                   QString::fromLatin1(".png"));
						 QFile f( fileNameXpm );
						 f.open( IO_WriteOnly );
						QTextStream out(&f);
						 out << str;
						 f.close();
						QImage imagen;
						QPixmap xpm(fileNameXpm);
						if(!xpm.isNull())
								{
						imagen = xpm.convertToImage();
						imagen.save(fileNamePng,"PNG");
						uint alto = ((imagen.width() * 2.54)/98) * 1000;
						uint ancho = ((imagen.height() * 2.54)/98) * 1000;
						QString nombreImagen = "Imagen"+ QString::number(r);						
						AQOdsImage img(nombreImagen,alto,ancho,0,0,fileNamePng);
						row.opIn(img);
								} else 
								{
								qWarning("La imagen está vacia");
								row.opIn(str);
								}      

						
				}
				else
				{
          			if (str.isEmpty())
           			 row.coveredCell();
          			else
            			row.opIn(str);
				}
        }
      }
    
    }
    row.close();
    progress.setProgress(r);
    qApp->processEvents();
    cur->QSqlCursor::next();
  }

  cur->QSqlCursor::seek(curRow);

  sheet.close();
  spreadsheet.close();

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QString fileName(AQ_DISKCACHE_DIRPATH + '/' + mtd->name() +
                   QDateTime::currentDateTime().toString("ddMMyyyyhhmmsszzz") +
                   QString::fromLatin1(".ods"));
  odsGen.generateOds(fileName);
  aqApp->call("sys.openUrl", QSArgumentList(fileName), 0);

  QApplication::restoreOverrideCursor();
}
