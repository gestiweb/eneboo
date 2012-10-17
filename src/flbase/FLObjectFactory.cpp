/***************************************************************************
 FLObjectFactory.cpp
 -------------------
 begin                : jue abr 23 2003
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

#include "FLFieldMetaData.h"
#include "FLObjectFactory.h"
#include "FLJasperViewer.h"
#include "FLJasperEngine.h"

// Ver FLUtilInterface::fastRand()
uint FLUtilInterface::aq_seed_rand = RAND_MAX;
uint FLUtilInterface::aq_seed_rand_x = 123456789;
uint FLUtilInterface::aq_seed_rand_y = 362436069;
uint FLUtilInterface::aq_seed_rand_z = 521288629;

class FLSqlCursorsCache
{
private:

  FLSqlCursorsCache() :
    ptrDictCursors(QPtrDict<FLSqlCursorInterface> (41)) {
  }

  FLSqlCursorInterface *sqlCursor(void *o) {
    FLSqlCursorInterface *c = ptrDictCursors.find(o);
    if (!c) {
      c = new FLSqlCursorInterface(static_cast<QObject *>(o));
      ptrDictCursors.insert(o, c);
    }
    return c;
  }

  void removeSqlCursor(void *o) {
    ptrDictCursors.remove(o);
  }

  void clear() {
    ptrDictCursors.clear();
  }

  QPtrDict<FLSqlCursorInterface> ptrDictCursors;

  friend class FLObjectFactory;
  friend class FLSqlCursorInterface;
};

FLSqlCursorsCache *FLSqlCursorInterface::cursorsCache = 0;

class FLDomNodesCache
{
private:

  FLDomNodesCache() :
    ptrDictNodes(QPtrDict<FLDomNodeInterface> (991)),
    dictNodes(QDict<FLDomNodeInterface> (991, false)) {
  }

  void clear() {
    ptrDictNodes.clear();
    dictNodes.clear();
  }

  FLDomNodeInterface *node(const QDomNode &n) {
    void *impl = n.internalImpl();
    if (!impl)
      return 0;
    FLDomNodeInterface *nodeIface = ptrDictNodes.find(impl);
    if (!nodeIface) {
      FLDomNodeInterface *parentNodeIface = node(n.parentNode());
      if (parentNodeIface && parentNodeIface->type() != QDomNode::DocumentNode)
        nodeIface = new FLDomNodeInterface(n, parentNodeIface);
      else
        nodeIface = new FLDomNodeInterface(n);
      ptrDictNodes.insert(n.internalImpl(), nodeIface);
    } else if (nodeIface->isNull())
      nodeIface->updateNode(n);
    return nodeIface;
  }

  FLDomNodeInterface *node(const QDomElement &n) {
    void *impl = n.internalImpl();
    if (!impl)
      return 0;
    FLDomNodeInterface *nodeIface = ptrDictNodes.find(impl);
    if (!nodeIface) {
      nodeIface = new FLDomNodeInterface(n);
      ptrDictNodes.insert(n.internalImpl(), nodeIface);
    } else if (nodeIface->isNull() || nodeIface->type() != QDomNode::ElementNode)
      nodeIface->updateNode(n);
    return nodeIface;
  }

  FLDomNodeInterface *node(const QDomText &n) {
    void *impl = n.internalImpl();
    if (!impl)
      return 0;
    FLDomNodeInterface *nodeIface = ptrDictNodes.find(impl);
    if (!nodeIface) {
      nodeIface = new FLDomNodeInterface(n);
      ptrDictNodes.insert(n.internalImpl(), nodeIface);
    } else if (nodeIface->isNull() || nodeIface->type() != QDomNode::TextNode)
      nodeIface->updateNode(n);
    return nodeIface;
  }

  FLDomNodeInterface *node(const QDomDocument &n) {
    void *impl = n.internalImpl();
    if (!impl)
      return 0;
    FLDomNodeInterface *nodeIface = ptrDictNodes.find(impl);
    if (!nodeIface) {
      nodeIface = new FLDomNodeInterface(n);
      ptrDictNodes.insert(n.internalImpl(), nodeIface);
    } else if (nodeIface->isNull() || nodeIface->type() != QDomNode::DocumentNode)
      nodeIface->updateNode(n);
    return nodeIface;
  }

  FLDomNodeInterface *node(const QString &id) {
    return dictNodes.find(id);
  }

  void insertNode(const QString &id, FLDomNodeInterface *nIface) {
    if (nIface)
      dictNodes.insert(id, nIface);
  }

  void updateNode(FLDomNodeInterface *nIface) {
    if (nIface) {
      QDomNode *n = nIface->ptroObj();
      if (n) {
        void *nImpl = n->internalImpl();
        if (nImpl && ptrDictNodes.find(nImpl) != nIface)
          ptrDictNodes.insert(nImpl, nIface);
      }
    }
  }

  bool removeNode(const QString &id) {
    FLDomNodeInterface *nIface = dictNodes.find(id);
    if (nIface) {
      QDomNode *n = nIface->ptroObj();
      void *impl;
      if (n && (impl = n->internalImpl())) {
        ptrDictNodes.remove(impl);
        QDomNodeList nl(n->childNodes());
        FLDomNodeInterface *nodeIface;
        for (uint i = 0; i < nl.count(); ++i) {
          nodeIface = ptrDictNodes.find(nl.item(i).internalImpl());
          if (nodeIface)
            nodeIface->deleteLater();
        }
      }
      //qWarning( "REMOVE count nods %d ptrnos %d remove " + id, dictNodes.count() - 1, ptrDictNodes.count() );
      return dictNodes.remove(id);
    }
    return false;
  }

private:

  QPtrDict<FLDomNodeInterface> ptrDictNodes;
  QDict<FLDomNodeInterface> dictNodes;

  friend class FLObjectFactory;
  friend class FLDomNodeInterface;
};

FLDomNodesCache *FLDomNodeInterface::nodesCache = 0;
uint FLDomNodeInterface::idNodeGlob = 0;

FLDomNodeInterface::~FLDomNodeInterface()
{
  //qWarning( "FLDomNodeInterface::~FLDomNodeInterface() %d", idNode_ );
  nodesCache->removeNode(QString::number(idNode_));
  clear();
}

FLDomNodeInterface *FLDomNodeInterface::nodeFromIdNode(const QString &id) const
{
  return nodesCache->node(id);
}

FLDomNodeInterface *FLDomNodeInterface::nodeInterface(const QDomNode &n)
{
  return nodesCache->node(n);
}

FLDomNodeInterface *FLDomNodeInterface::nodeInterface(const QDomElement &n)
{
  return nodesCache->node(n);
}

FLDomNodeInterface *FLDomNodeInterface::nodeInterface(const QDomText &n)
{
  return nodesCache->node(n);
}

FLDomNodeInterface *FLDomNodeInterface::nodeInterface(const QDomDocument &n)
{
  return nodesCache->node(n);
}

void FLDomNodeInterface::setIdNode()
{
  idNode_ = idNodeGlob++;
  nodesCache->insertNode(QString::number(idNode_), this);
}

bool FLDomNodeInterface::setContent(const QString &buffer)
{
  if (type_ != QDomNode::DocumentNode)
    return false;
  bool ret = FLUtil::domDocumentSetContent(*(static_cast<QDomDocument *>(DN_)), buffer);
  nodesCache->updateNode(this);
  return ret;
}

void FLDomNodeInterface::cleanupDomNodesCache()
{
  nodesCache->clear();
  delete nodesCache;
  nodesCache = new FLDomNodesCache();
}

void FLDomNodeInterface::cleanup()
{
  cleanupDomNodesCache();
}

void FLListViewInterface::connects() const
{
  disconnect(obj_, 0, this, 0);
  connect(obj_, SIGNAL(doubleClicked(QListViewItem *)),
          this, SLOT(emitDoubleClicked(QListViewItem *)));
  connect(obj_, SIGNAL(expanded(QListViewItem *)),
          this, SLOT(emitExpanded(QListViewItem *)));
  connect(obj_, SIGNAL(selectionChanged(QListViewItem *)),
          this, SLOT(emitSelectionChanged(QListViewItem *)));
}

void FLListViewInterface::emitDoubleClicked(QListViewItem *lvI)
{
  FLListViewItemInterface *iface = new FLListViewItemInterface(static_cast<FLListViewItem *>(lvI));
  emit doubleClicked(iface);
}

void FLListViewInterface::emitExpanded(QListViewItem *lvI)
{
  FLListViewItemInterface *iface = new FLListViewItemInterface(static_cast<FLListViewItem *>(lvI));
  emit expanded(iface);
}

void FLListViewInterface::emitSelectionChanged(QListViewItem *lvI)
{
  FLListViewItemInterface *iface = new FLListViewItemInterface(static_cast<FLListViewItem *>(lvI));
  emit selectionChanged(iface);
}

FLListViewItemInterface *FLListViewInterface::firstChild() const
{
  FLListViewItemInterface *lvI = new FLListViewItemInterface(static_cast<FLListViewItem *>(obj_->firstChild()));
  if (lvI->obj())
    return lvI;
  else
    return 0;
}

FLListViewItemInterface *FLListViewItemInterface::firstChild() const
{
  FLListViewItemInterface *lvI = new FLListViewItemInterface(static_cast<FLListViewItem *>(obj_->firstChild()));
  if (lvI->obj())
    return lvI;
  else
    return 0;
}

FLListViewItemInterface *FLListViewItemInterface::nextSibling() const
{
  FLListViewItemInterface *lvI = new FLListViewItemInterface(static_cast<FLListViewItem *>(obj_->nextSibling()));
  if (lvI->obj())
    return lvI;
  else
    return 0;
}

FLListViewItemInterface *FLListViewItemInterface::parent() const
{
  FLListViewItemInterface *lvI = new FLListViewItemInterface(static_cast<FLListViewItem *>(obj_->parent()));
  if (lvI->obj())
    return lvI;
  else
    return 0;
}

void FLListViewItemInterface::setPixmap(int column, const QVariant &pm)
{
  QCString cs;
  if (pm.isValid() && !pm.isNull())
    cs = pm.toCString();
  if (cs.isEmpty())
    return;
  QPixmap pix;
  if (!QPixmapCache::find(cs.left(100), pix)) {
    pix.loadFromData(cs);
    QPixmapCache::insert(cs.left(100), pix);
  }
  if (!pix.isNull())
    obj_->setPixmap(column, pix);
}

QString FLApplicationInterface::dialogGetFileImage()
{
  QFileDialog *fd = new QFileDialog(qApp->focusWidget(), 0, true);
  FLPixmapView *p = new FLPixmapView(fd);

  p->setAutoScaled(true);
  fd->setContentsPreviewEnabled(TRUE);
  fd->setContentsPreview(p, p);
  fd->setPreviewMode(QFileDialog::Contents);
  fd->setCaption(tr("Elegir archivo"));
  fd->setFilter("*");

  QString filename;
  if (fd->exec() == QDialog::Accepted)
    filename = fd->selectedFile();
  fd->deleteLater();
  return filename;
}

QPixmap FLApplicationInterface::toPixmap(const QVariant &v)
{
  QPixmap pix;
  QCString cs = v.toCString();
  if (!cs.isEmpty()) {
    if (!QPixmapCache::find(cs.left(100), pix)) {
      pix.loadFromData(cs);
      QPixmapCache::insert(cs.left(100), pix);
    }
  }
  return pix;
}

QVariant FLApplicationInterface::fromPixmap(const QPixmap &p)
{
  if (p.isNull())
    return QVariant();

  QApplication::setOverrideCursor(waitCursor);

  QCString s;
  QBuffer buffer(s);

  buffer.open(IO_WriteOnly);
  p.save(&buffer, "XPM");
  if (!s.isEmpty()) {
    if (!QPixmapCache::find(s.left(100)))
      QPixmapCache::insert(s.left(100), p);
  }

  QApplication::restoreOverrideCursor();

  return QVariant(s);
}

QPixmap FLApplicationInterface::scalePixmap(const QPixmap &p, int w, int h, FLScaleMode mode)
{
  QImage img(p.convertToImage());
  return img.smoothScale(w, h, (QImage::ScaleMode) mode);
}

FLDomNodeInterface *FLApplicationInterface::toXmlReportData(FLSqlQueryInterface *q)
{
  FLReportEngine *rpt = new FLReportEngine;
  rpt->setReportData(q);
  FLDomNodeInterface *ret = rpt->reportData();
  delete rpt;
  return ret;
}

void FLApplicationInterface::popupWarn(const QString &msgWarn,
                                       const QMap<QString, QVariant> &scriptCalls)
{
  QMap<QString, QSArgumentList> calls;
  for (QMap<QString, QVariant>::const_iterator it = scriptCalls.begin();
       it != scriptCalls.end(); ++it) {
    calls.insert(it.key(), QSArgumentList(*it));
  }
  obj_->popupWarn(msgWarn, calls);
}

void FLSqlCursorInterface::setObj(FLSqlCursor *obj)
{
  obj_ = obj;
  if (obj_) {
    if (!obj_->metadata()) {
      qWarning("FLObjectFactory : Imposible inicializar interface de FLSqlCursor, no existen metadatos.");
      return;
    }
    if (obj_->cursorRelation())
      cursorRelation_ = sqlCursorInterface(obj_->cursorRelation());
    connects();
  }
}

FLSqlCursorInterface *FLSqlCursorInterface::sqlCursorInterface(FLSqlCursor *c)
{
  if (c) {
    FLSqlCursorInterface *ci = cursorsCache->sqlCursor(c);
    ci->cached_ = true;
    return ci;
  }
  return new FLSqlCursorInterface;
}

void FLSqlCursorInterface::removeCache()
{
  cursorsCache->removeSqlCursor(obj_);
}

void FLSqlCursorInterface::connects() const
{
  disconnect(obj_, 0, this, 0);
  connect(obj_, SIGNAL(newBuffer()),
          this, SIGNAL(newBuffer()));
  connect(obj_, SIGNAL(bufferChanged(const QString &)),
          this, SIGNAL(bufferChanged(const QString &)));
  connect(obj_, SIGNAL(cursorUpdated()),
          this, SIGNAL(cursorUpdated()));
  connect(obj_, SIGNAL(recordChoosed()),
          this, SIGNAL(recordChoosed()));
  connect(obj_, SIGNAL(currentChanged(int)),
          this, SIGNAL(currentChanged(int)));
  connect(obj_, SIGNAL(autoCommit()),
          this, SIGNAL(autoCommit()));
  connect(obj_, SIGNAL(bufferCommited()),
          this, SIGNAL(bufferCommited()));
}

void FLFormDBInterface::connects() const
{
  disconnect(obj_, 0, this, 0);
  connect(obj_, SIGNAL(closed()),
          this, SIGNAL(closed()));
  connect(obj_, SIGNAL(formReady()),
          this, SIGNAL(formReady()));
}

void FLFormRecordDBInterface::connects() const
{
  disconnect(obj_, 0, this, 0);
  connect(obj_, SIGNAL(closed()),
          this, SIGNAL(closed()));
  connect(obj_, SIGNAL(formReady()),
          this, SIGNAL(formReady()));
}

void FLFormSearchDBInterface::connects() const
{
  disconnect(obj_, 0, this, 0);
  connect(obj_, SIGNAL(closed()),
          this, SIGNAL(closed()));
  connect(obj_, SIGNAL(formReady()),
          this, SIGNAL(formReady()));
}

void FLFieldDBInterface::connects() const
{
  disconnect(obj_, 0, this, 0);
  connect(obj_, SIGNAL(activatedAccel(int)),
          this, SIGNAL(activatedAccel(int)));
  connect(obj_, SIGNAL(keyF2Pressed()),
          this, SIGNAL(keyF2Pressed()));
  connect(obj_, SIGNAL(keyF4Pressed()),
          this, SIGNAL(keyF4Pressed()));
  connect(obj_, SIGNAL(keyReturnPressed()),
          this, SIGNAL(keyReturnPressed()));
  connect(obj_, SIGNAL(lostFocus()),
          this, SIGNAL(lostFocus()));
}

QStringList FLTableDBInterface::primarysKeysChecked()
{
  FLDataTable *dt = obj_->tableRecords();
  QStringList retList;
  if (dt) {
    QValueList<QVariant> valueList(dt->primarysKeysChecked());
    for (QValueList<QVariant>::const_iterator it = valueList.begin(); it != valueList.end(); ++it)
      retList.append((*it).toString());
  }
  return retList;
}

void FLTableDBInterface::connects() const
{
  disconnect(obj_, 0, this, 0);
  connect(obj_, SIGNAL(readOnlyChanged(bool)),
          this, SIGNAL(readOnlyChanged(bool)));
  connect(obj_, SIGNAL(editOnlyChanged(bool)),
          this, SIGNAL(editOnlyChanged(bool)));
  connect(obj_, SIGNAL(insertOnlyChanged(bool)),
          this, SIGNAL(insertOnlyChanged(bool)));
  connect(obj_, SIGNAL(currentChanged()),
          this, SIGNAL(currentChanged()));
  FLDataTable *dt = obj_->tableRecords();
  if (dt) {
    disconnect(dt, SIGNAL(primaryKeyToggled(const QVariant &, bool)),
               this, SIGNAL(primaryKeyToggled(const QVariant &, bool)));
    connect(dt, SIGNAL(primaryKeyToggled(const QVariant &, bool)),
            this, SIGNAL(primaryKeyToggled(const QVariant &, bool)));
  }
}

QStringList FLTableInterface::selectedRows()
{
  NEW_TABLE
  QStringList listRows;
  QTableSelection ts;
  int row;

  for (int numSel = 0; numSel < obj_->numSelections(); ++numSel) {
    ts = obj_->selection(numSel);
    for (row = ts.topRow(); row <= ts.bottomRow(); ++row) {
      if (listRows.isEmpty())
        listRows = QString::number(row);
      else
        listRows.append(QString::number(row));
    }
  }

  listRows.sort();
  return listRows;
}

QStringList FLTableInterface::selectedCols()
{
  NEW_TABLE
  QStringList listCols;
  QTableSelection ts;
  int col;

  for (int numSel = 0; numSel < obj_->numSelections(); ++numSel) {
    ts = obj_->selection(numSel);
    for (col = ts.leftCol(); col <= ts.rightCol(); ++col) {
      if (listCols.isEmpty())
        listCols = QString::number(col);
      else
        listCols.append(QString::number(col));
    }
  }

  listCols.sort();
  return listCols;
}

QStringList FLTableInterface::selectedCells()
{
  NEW_TABLE
  QStringList listCells;
  QTableSelection ts;
  int row, col;

  for (int numSel = 0; numSel < obj_->numSelections(); ++numSel) {
    ts = obj_->selection(numSel);
    for (row = ts.topRow(); row <= ts.bottomRow(); ++row) {
      for (col = ts.leftCol(); col <= ts.rightCol(); ++col) {
        if (listCells.isEmpty())
          listCells = QString::number(row) + QString::fromLatin1(".") +
                      QString::number(col);
        else
          listCells.append(QString::number(row) + QString::fromLatin1(".") +
                           QString::number(col));
      }
    }
  }

  listCells.sort();
  return listCells;
}

void FLTableInterface::clear()
{
  NEW_TABLE
  int nRows = obj_->numRows();
  QMemArray<int> filas(nRows);
  for (int fila = 0; fila < nRows; ++fila)
    filas[fila] = fila;
  obj_->removeRows(filas);
}

void FLTableInterface::connects() const
{
  disconnect(obj_, 0, this, 0);
  connect(obj_, SIGNAL(currentChanged(int, int)),
          this, SIGNAL(currentChanged(int, int)));
  connect(obj_, SIGNAL(clicked(int, int, int, const QPoint &)),
          this, SIGNAL(clicked(int, int)));
  connect(obj_, SIGNAL(doubleClicked(int, int, int, const QPoint &)),
          this, SIGNAL(doubleClicked(int, int)));
  connect(obj_, SIGNAL(valueChanged(int, int)),
          this, SIGNAL(valueChanged(int, int)));
}

void FLTabWidgetInterface::setTabEnabled(const QString &name, bool enable)
{
  QWidget *w = 0;
  for (int i = 0; i < obj_->count(); i++) {
    w = obj_->page(i);
    if (w->name() == name) {
      obj_->setTabEnabled(w, enable);
      break;
    }
  }
}

void FLTabWidgetInterface::removePage(const QString &name)
{
  QWidget *w = 0;
  for (int i = 0; i < obj_->count(); i++) {
    w = obj_->page(i);
    if (w->name() == name) {
      obj_->removePage(w);
      break;
    }
  }
}

void FLTabWidgetInterface::showPage(const QString &name)
{
  QWidget *w = 0;
  for (int i = 0; i < obj_->count(); i++) {
    w = obj_->page(i);
    if (w->name() == name) {
      obj_->showPage(w);
      break;
    }
  }
}

void FLTabWidgetInterface::connects() const
{
  disconnect(obj_, 0, this, 0);
  connect(obj_, SIGNAL(currentChanged(QWidget *)),
          this, SLOT(emitCurrentChanged(QWidget *)));
}

QString FLUtilInterface::fieldNameToAlias(const QString &fN, const QString &tN, const QString &connName) const
{
  if (tN.isEmpty())
    return fN;
  FLTableMetaData *mtd = FLSqlConnections::database(connName)->manager()->metadata(tN);
  if (!mtd)
    return fN;
  return mtd->fieldNameToAlias(fN);
}

QString FLUtilInterface::tableNameToAlias(const QString &tN, const QString &connName) const
{
  if (tN.isEmpty())
    return QString::null;
  FLTableMetaData *mtd = FLSqlConnections::database(connName)->manager()->metadata(tN);
  if (!mtd)
    return QString::null;
  return mtd->alias();
}

QString FLUtilInterface::fieldAliasToName(const QString &aN, const QString &tN, const QString &connName) const
{
  if (tN.isEmpty())
    return aN;
  FLTableMetaData *mtd = FLSqlConnections::database(connName)->manager()->metadata(tN);
  if (!mtd)
    return aN;
  return mtd->fieldAliasToName(aN);
}

int FLUtilInterface::fieldType(const QString &fN, const QString &tN, const QString &connName) const
{
  if (tN.isEmpty())
    return QVariant::Invalid;
  FLTableMetaData *mtd = FLSqlConnections::database(connName)->manager()->metadata(tN);
  if (!mtd)
    return QVariant::Invalid;
  return mtd->fieldType(fN);
}

int FLUtilInterface::fieldLength(const QString &fN, const QString &tN, const QString &connName) const
{
  if (tN.isEmpty())
    return 0;
  FLTableMetaData *mtd = FLSqlConnections::database(connName)->manager()->metadata(tN);
  if (!mtd)
    return 0;
  return mtd->fieldLength(fN);
}

bool FLUtilInterface::fieldAllowNull(const QString &fN, const QString &tN, const QString &connName) const
{
  if (tN.isEmpty())
    return false;
  FLTableMetaData *mtd = FLSqlConnections::database(connName)->manager()->metadata(tN);
  if (!mtd)
    return false;
  return mtd->fieldAllowNull(fN);
}

bool FLUtilInterface::fieldIsPrimaryKey(const QString &fN, const QString &tN, const QString &connName) const
{
  if (tN.isEmpty())
    return false;
  FLTableMetaData *mtd = FLSqlConnections::database(connName)->manager()->metadata(tN);
  if (!mtd)
    return false;
  return mtd->fieldIsPrimaryKey(fN);
}

bool FLUtilInterface::fieldIsCompoundKey(const QString &fN, const QString &tN, const QString &connName) const
{
  if (tN.isEmpty())
    return false;
  FLTableMetaData *mtd = FLSqlConnections::database(connName)->manager()->metadata(tN);
  if (!mtd)
    return false;
  return mtd->field(fN)->isCompoundKey();
}

QVariant FLUtilInterface::fieldDefaultValue(const QString &fN, const QString &tN, const QString &connName) const
{
  if (tN.isEmpty())
    return QVariant();
  FLTableMetaData *mtd = FLSqlConnections::database(connName)->manager()->metadata(tN);
  if (!mtd)
    return QVariant();
  return mtd->field(fN)->defaultValue();
}

QString FLUtilInterface::formatValue(int t, const QVariant &v, const bool upper, const QString &connName) const
{
  return FLSqlConnections::database(connName)->manager()->formatValue(t, v, upper);
}

void FLSmtpClientInterface::connects() const
{
  disconnect(obj_, 0, this, 0);
  connect(obj_, SIGNAL(status(const QString &)),
          this, SIGNAL(status(const QString &)));
  connect(obj_, SIGNAL(sendStarted()),
          this, SIGNAL(sendStarted()));
  connect(obj_, SIGNAL(sendEnded()),
          this, SIGNAL(sendEnded()));
  connect(obj_, SIGNAL(sendTotalSteps(int)),
          this, SIGNAL(sendTotalSteps(int)));
  connect(obj_, SIGNAL(sendStepNumber(int)),
          this, SIGNAL(sendStepNumber(int)));
}

FLObjectFactory::FLObjectFactory() :
  QSObjectFactory()
{
  voidObject_ = new QObject(0, "voidObject");
  util_ = new FLUtilInterface(voidObject_);
  FLSqlCursorInterface::cursorsCache = new FLSqlCursorsCache;
  FLDomNodeInterface::nodesCache = new FLDomNodesCache;

  registerClass("FLSqlCursor", "FLSqlCursorInterface");
  registerClass("FLSqlQuery", "FLSqlQueryInterface");
  registerClass("FLFormDB", "FLFormDBInterface");
  registerClass("FLFormRecordDB", "FLFormRecordDBInterface");
  registerClass("FLFormSearchDB", "FLFormSearchDBInterface");
  registerClass("FLFieldDB", "FLFieldDBInterface");
  registerClass("FLTableDB", "FLTableDBInterface");
  registerClass("FLUtil", "FLUtilInterface");
  registerClass("FLReportViewer", "FLReportViewerInterface");
  registerClass("FLCodBar", "FLCodBarInterface");
  registerClass("FLApplication", "FLAplicationInterface");
  //###
  //registerClass("QTable", "FLTableInterface");
  registerClass("FLTable", "FLTablePlusInterface");
  //###
  //registerClass("QListView", "FLListViewInterface");
  registerClass("FLListViewItem", "FLListViewInterfaceItem");
  registerClass("FLScriptEditor", "FLScriptEditorInterface");
  //###
  registerClass("FLTabWidget", "FLTabWidgetInterface");
  registerClass("FLVar", "FLVarInterface");
  //###
  //registerClass("QDomNode", "FLDomNodeInterface");
  registerClass("FLDomNode", "FLDomNodeInterface");
  registerClass("FLDomElement", "FLDomNodeInterface");
  registerClass("FLDomDocument", "FLDomNodeInterface");
  registerClass("FLDomText", "FLDomNodeInterface");
  registerClass("FLDomNodeList", "FLDomNodeListInterface");
  registerClass("FLProcess", "FLProcessInterface");
  registerClass("FLPosPrinter", "FLPosPrinterInterface");
  registerClass("FLSmtpClient", "FLSmtpClientInterface");
  registerClass("FLImageViewer", "FLImageViewerInterface");
  registerClass("FLSerialPort", "FLSerialPortInterface");
  registerClass("Picture", "FLPicture");
  registerClass("FLNetwork", "FLNetwork");
  registerClass("FLDigiDoc", "FLDigiDocInterface");
  registerClass("FLJasperViewer", "FLJasperViewer");
  registerClass("FLJasperEngine", "FLJasperEngine");
  registerClass("FLReportEngine", "FLReportEngine");

  registerClass("AQUtil", "FLUtilInterface", util_);
  registerClass("AQSignalMapper", "AQSignalMapper");
  registerClass("AQObjectQueryList", "AQObjectQueryList");
}

FLObjectFactory::~FLObjectFactory()
{
  registerClass("AQUtil", QString::null, new QObject);

  FLSqlCursorInterface::cursorsCache->clear();
  delete FLSqlCursorInterface::cursorsCache;
  FLSqlCursorInterface::cursorsCache = 0;

  FLDomNodeInterface::nodesCache->clear();
  delete FLDomNodeInterface::nodesCache;
  FLDomNodeInterface::nodesCache = 0;

  voidObject_->deleteLater();
}

QObject *FLObjectFactory::create(const QString &className, const QSArgumentList &arguments, QObject *context)
{
  if (className == "FLSqlCursor") {
    if (arguments.count() == 1)
      return new FLSqlCursorInterface(arguments[0].variant().toString());
    if (arguments.count() == 2)
      return new FLSqlCursorInterface(arguments[0].variant().toString(), arguments[1].variant().toString());
  }

  if (className == "FLSqlQuery") {
    if (arguments.count() == 1)
      return new FLSqlQueryInterface(arguments[0].variant().toString());
    if (arguments.count() == 2)
      return new FLSqlQueryInterface(arguments[0].variant().toString(), arguments[1].variant().toString());
    return new FLSqlQueryInterface();
  }

  if (className == "FLFormDB") {
    if (arguments.count() == 3)
      return new FLFormDBInterface(arguments[0].variant().toString(), ::qt_cast<QWidget *>(arguments[1].qobject()),
                                   arguments[2].variant().toInt());
    if (arguments.count() == 4)
      return new FLFormDBInterface(::qt_cast<FLSqlCursorInterface *>(arguments[0].qobject()),
                                   arguments[1].variant().toString(), ::qt_cast<QWidget *>(arguments[2].qobject()),
                                   arguments[3].variant().toInt());
  }

  if (className == "FLFormRecordDB") {
    if (arguments.count() == 4)
      return new FLFormRecordDBInterface(::qt_cast<FLSqlCursorInterface *>(arguments[0].qobject()),
                                         arguments[1].variant().toString(),
                                         ::qt_cast<QWidget *>(arguments[2].qobject()), arguments[3].variant().toBool());
  }

  if (className == "FLFormSearchDB") {
    if (arguments.count() == 1)
      return new FLFormSearchDBInterface(arguments[0].variant().toString());
    if (arguments.count() == 2)
      return new FLFormSearchDBInterface(::qt_cast<FLSqlCursorInterface *>(arguments[0].qobject()),
                                         arguments[1].variant().toString());
  }

  if (className == "FLFieldDB") {
    if (arguments.count() == 2)
      return new FLFieldDBInterface(::qt_cast<QWidget *>(arguments[0].qobject()), arguments[1].variant().toCString());
  }

  if (className == "FLTableDB") {
    if (arguments.count() == 2)
      return new FLTableDBInterface(::qt_cast<QWidget *>(arguments[0].qobject()), arguments[1].variant().toCString());
  }

  if (className == "QTable") {
    if (arguments.count() == 2)
      return new FLTableInterface(::qt_cast<QWidget *>(arguments[0].qobject()), arguments[1].variant().toCString());
  }

  if (className == "FLTable") {
    if (arguments.count() == 2)
      return new FLTablePlusInterface(::qt_cast<QWidget *>(arguments[0].qobject()), arguments[1].variant().toCString());
  }

  if (className == "QListView") {
    if (arguments.count() == 2)
      return new FLListViewInterface(::qt_cast<QWidget *>(arguments[0].qobject()), arguments[1].variant().toCString());
  }

  if (className == "FLListViewItem") {
    if (arguments.count() == 1) {
      QObject *o = arguments[0].qobject();
      if (o->isA("FLListViewItemInterface"))
        return new FLListViewItemInterface(::qt_cast<FLListViewItemInterface *>(arguments[0].qobject()));
      else
        return new FLListViewItemInterface(::qt_cast<FLListViewInterface *>(arguments[0].qobject()));
    }
  }

  if (className == "FLTabWidget") {
    return new FLTabWidgetInterface();
  }

  if (className == "FLUtil") {
    return util_;
  }

  if (className == "FLReportViewer") {
    if (arguments.count() == 1) {
      QWidget *w = ::qt_cast<QWidget *>(arguments[0].qobject());
      if (w) {
        return new FLReportViewerInterface(w, true);
      } else {
        FLReportEngine *r = ::qt_cast<FLReportEngine *>(arguments[0].qobject());
        if (r)
          return new FLReportViewerInterface(r);
      }
    }
    return new FLReportViewerInterface();
  }

  if (className == "FLScriptEditor") {
    return new FLScriptEditorInterface(arguments[0].variant().toString());
  }

  if (className == "FLCodBar") {
    if (arguments.count() == 1) {
      return new FLCodBarInterface(static_cast<FLCodBar *>(arguments[0].ptr()));
    } else {
      return new FLCodBarInterface(arguments[0].variant().toString(), arguments[1].variant().toInt(),
                                   arguments[2].variant().toInt(), arguments[3].variant().toDouble(),
                                   arguments[4].variant().toDouble(), arguments[5].variant().toInt(),
                                   arguments[6].variant().toBool());
    }
  }

  if (className == "FLApplication") {
    return new FLApplicationInterface(::qt_cast<FLApplication *>(arguments[0].qobject()));
  }

  if (className == "FLVar") {
    return new FLVarInterface();
  }

  if (className == "QDomNode") {
    return new FLDomNodeInterface();
  }

  if (className == "FLDomNode") {
    return new FLDomNodeInterface();
  }

  if (className == "FLDomDocument") {
    return new FLDomNodeInterface(QDomNode::DocumentNode);
  }

  if (className == "FLDomElement") {
    return new FLDomNodeInterface(QDomNode::ElementNode);
  }

  if (className == "FLDomText") {
    return new FLDomNodeInterface(QDomNode::TextNode);
  }

  if (className == "FLDomNodeList") {
    return new FLDomNodeListInterface();
  }

  if (className == "FLProcess") {
    return new FLProcessInterface(arguments[0].variant().toString());
  }

  if (className == "FLPosPrinter") {
    return new FLPosPrinterInterface();
  }

  if (className == "FLSmtpClient") {
    return new FLSmtpClientInterface();
  }

  if (className == "FLImageViewer") {
    return new FLImageViewerInterface();
  }

  if (className == "FLSerialPort") {
    return new FLSerialPortInterface(arguments[0].variant().toString());
  }

  if (className == "Picture") {
    if (arguments.count() == 1) {
      FLPicture *pic = ::qt_cast<FLPicture *>(arguments[0].qobject());
      if (pic)
        return new FLPicture(pic);
    }
    return new FLPicture();
  }

  if (className == "FLNetwork")
    return new FLNetwork(arguments[0].variant().toString());

  if (className == "FLDigiDoc") {
    if (arguments.count() == 1)
      return new FLDigiDocInterface(arguments[ 0 ].variant().toString());
    else
      return new FLDigiDocInterface();
  }

  if (className == "FLJasperViewer")
    return new FLJasperViewer();

  if (className == "FLJasperEngine")
    return new FLJasperEngine();

  if (className == "FLReportEngine")
    return new FLReportEngine();

  if (className == "AQSignalMapper") {
    if (arguments.count() == 1)
      return new AQSignalMapper(arguments[0].qobject());
    return new AQSignalMapper(0);
  }
  if (className == "AQObjectQueryList") {
    if (arguments.count() == 1)
      return new AQObjectQueryList(arguments[0].qobject());
    else if (arguments.count() == 3)
      return new AQObjectQueryList(arguments[0].qobject(),
                                   arguments[1].variant().toString(),
                                   arguments[2].variant().toString(),
                                   true, true);
    else if (arguments.count() == 5)
      return new AQObjectQueryList(arguments[0].qobject(),
                                   arguments[1].variant().toString(),
                                   arguments[2].variant().toString(),
                                   arguments[3].variant().toBool(),
                                   arguments[4].variant().toBool());
  }

  return util_;
}
