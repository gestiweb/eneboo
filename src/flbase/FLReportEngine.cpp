/***************************************************************************
                      FLReportEngine.cpp  -  description
                         -------------------
begin                : jue jun 27 2002
copyright            : (C) 2002-2004 by InfoSiAL S.L.
email                : mail@infosial.com
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "FLSqlCursor.h"
#include "FLSqlQuery.h"
#include "FLGroupByQuery.h"
#include "FLManagerModules.h"
#include "FLTableMetaData.h"
#include "FLFieldMetaData.h"
#include "FLManager.h"
#include "FLSqlDatabase.h"
#include "FLSqlConnections.h"
#include "FLReportPages.h"
#include "FLObjectFactory.h"
#include "FLReportEngine.h"

class FLReportEnginePrivate
{
public:

  FLReportEnginePrivate(FLReportEngine *q) :
    qry_(0), qFieldMtdList_(0), qGroupDict_(0), q_(q)
  {}

  /**
  Añade los datos del registro activo de la consulta dada al origen de datos del informe.

  Todos los campos del registro son agregados a la estructura XML con los datos de origen
  del informe, en un elemento tipo "Row", indicando a que nivel pertenece el registro según
  el parámetro l. La consulta debe pasarse en un estado válido, es decir, con un registro activo.

  @param l Nivel al  que pertenece el registro
  */
  inline void addRowToReportData(int l);

  /**
  Agrupa los registros de la consulta recursivamente según el nivel de agrupamiento, y los
  va sacando al objeo raiz XML, en el orden adecuado.
  */
  inline void groupBy(int levelMax, QStringList &vA);

  inline void setQuery(FLSqlQuery *qry);

  /**
  Registros de la consulta en formato de nodos XML para Kugar
  */
  QDomDocumentFragment rows_;

  /**
  Nombre de la plantilla del informe (fichero .kut)
  */
  QString template_;

  /**
  Guarda la consulta origen
  */
  FLSqlQuery *qry_;
  QStringList qFieldList_;
  QStringList qDoubleFieldList_;
  QValueStack<int> qImgFields_;
  FLTableMetaData::FLFieldMetaDataList *qFieldMtdList_;
  FLSqlQuery::FLGroupByQueryDict *qGroupDict_;

  FLReportEngine *q_;
};

inline void FLReportEnginePrivate::addRowToReportData(int l)
{
  if (!qry_->isValid())
    return;

  QDomElement row(q_->rptXmlData()->createElement("Row"));
  row.setAttribute("level", l);

  QValueStack<int> imgFieldsBack;
  int i = 0;

  for (QStringList::const_iterator it = qFieldList_.begin();
       it != qFieldList_.end(); ++it, ++i) {
    QVariant rawVal(qry_->value(i, true));
    if (!qImgFields_.isEmpty() && qImgFields_.top() == i) {
      QString strVal(rawVal.toString());
      imgFieldsBack.push_front(qImgFields_.pop());
      if (strVal.isEmpty()) {
        row.setAttribute(*it, strVal);
        continue;
      }
      QString imgFile(AQ_DISKCACHE_DIRPATH + '/' + strVal + ".png");
      if (!QFile::exists(imgFile)) {
        QPixmap pix;
        pix.loadFromData(qry_->value(i).toCString());
        pix.save(imgFile, "PNG");
      }
      row.setAttribute(*it, imgFile);
    } else {
      // ###
      //if (!rawVal.isValid() || rawVal.isNull())
      //  continue;
      row.setAttribute(*it, rawVal.toString());
    }
  }
  rows_.appendChild(row);
  qImgFields_ = imgFieldsBack;
}

inline void FLReportEnginePrivate::groupBy(int levelMax, QStringList &vA)
{
  if (!qry_->isValid())
    return;

  int l, i;
  FLSqlQuery::FLGroupByQueryDict *g = qGroupDict_;

  for (l = 0;
       l < levelMax &&
       (*vA.at(l)) == qry_->value((*g)[QString::number(l)]->field()).toString();
       ++l);

  for (i = l; i < levelMax; ++i) {
    addRowToReportData(i);
    (*vA.at(i)) = qry_->value((*g)[QString::number(i)]->field()).toString();
  }
  addRowToReportData(levelMax);
}

inline void FLReportEnginePrivate::setQuery(FLSqlQuery *qry)
{
  qry_ = qry;
  if (qry_) {
    qFieldList_ = qry_->fieldList();
    qFieldMtdList_ = qry_->fieldMetaDataList();
    qGroupDict_ = qry_->groupDict();
    qDoubleFieldList_.clear();
    qImgFields_.clear();

    if (!qFieldMtdList_)
      return;

    int i = qFieldList_.size() - 1;
    QStringList::const_iterator it(qFieldList_.end());
    while (i >= 0) {
      --it;
      FLFieldMetaData *fmtd = qFieldMtdList_->find((*it).section('.', 1, 1).lower());
      if (fmtd) {
        switch (fmtd->type()) {
          case QVariant::Pixmap:
            qImgFields_.push(i);
            break;
          case QVariant::Double:
            qDoubleFieldList_ << *it;
            break;
        }
      }
      --i;
    }
  } else {
    qFieldList_.clear();
    qDoubleFieldList_.clear();
    qImgFields_.clear();
    qFieldMtdList_ = 0;
    qGroupDict_ = 0;
  }
}

FLReportEngine::FLReportEngine(QObject *parent) :
  MReportEngine(parent)
{
  d = new FLReportEnginePrivate(this);
}

FLReportEngine::~FLReportEngine()
{
  delete d;
}

bool FLReportEngine::setReportData(FLSqlQuery *q)
{
  if (!q)
    return false;
  if (!rd)
    rd = new QDomDocument("KUGAR_DATA");

  QDomDocument tmpDoc("KUGAR_DATA");

  d->rows_ = tmpDoc.createDocumentFragment();
  d->setQuery(q);
  q->setForwardOnly(true);

  if (!q->exec()) {
#ifdef FL_DEBUG
    qWarning("FLReportEngine : " +  tr(" No se ha podido ejecutar la consulta del informe"));
#endif
    return false;
  }
  if (!q->next()) {
#ifdef FL_DEBUG
    qWarning("FLReportEngine : " +  tr(" Consulta del informe vacía"));
#endif
    return false;
  }

  FLSqlQuery::FLGroupByQueryDict *g = d->qGroupDict_;
  if (!g) {
    do {
      d->addRowToReportData(0);
    } while (q->next());
  } else {
    QStringList vA;
    for (int i = 0; i < 10; ++i)
      vA.append(QString::null);
    do {
      d->groupBy(g->count(), vA);
    } while (q->next());
  }

  QDomElement data(tmpDoc.createElement("KugarData"));
  data.appendChild(d->rows_);
  tmpDoc.appendChild(data);
  *rd = tmpDoc;
  d->rows_.clear();

  MReportEngine::initData();
  return true;
}

bool FLReportEngine::setReportData(FLSqlCursor *t)
{
  t = 0;
#ifdef FL_DEBUG
  qWarning("No implementadio todavía");
#endif
  return false;
}

bool FLReportEngine::setFLReportData(QDomNode n)
{
  d->setQuery(0);
  return MReportEngine::setReportData(n);
}

bool FLReportEngine::setFLReportTemplate(QDomNode n)
{
  d->template_ = QString::null;
  return MReportEngine::setReportTemplate(n);
}

bool FLReportEngine::setFLReportTemplate(const QString &t)
{
  d->template_ = t;
  if (!d->qry_) {
    return MReportEngine::setReportTemplate(
             FLSqlConnections::database()->managerModules()->contentCached(t + ".kut")
           );
  } else {
    return MReportEngine::setReportTemplate(
             d->qry_->db()->managerModules()->contentCached(t + ".kut")
           );
  }
}

FLSqlQuery *FLReportEngine::rptQueryData() const
{
  return d->qry_;
}

QString FLReportEngine::rptNameTemplate() const
{
  return d->template_;
}


// Intefaz scripts
bool FLReportEngine::setReportData(FLSqlQueryInterface *q)
{
  return setReportData(q->obj());
}

bool FLReportEngine::setReportData(FLDomNodeInterface *n)
{
  return setFLReportData(n->obj());
}

bool FLReportEngine::setReportTemplate(const QString &t)
{
  return setFLReportTemplate(t);
}

bool FLReportEngine::setReportTemplate(FLDomNodeInterface *n)
{
  return setFLReportTemplate(n->obj());
}

FLDomNodeInterface *FLReportEngine::reportData() const
{
  return FLDomNodeInterface::nodeInterface(rd ? *rd : QDomDocument());
}

FLDomNodeInterface *FLReportEngine::reportTemplate() const
{
  return FLDomNodeInterface::nodeInterface(rt ? *rt : QDomDocument());
}

void FLReportEngine::exportToOds(FLReportPages *pages)
{
  if (!pages || !pages->pageCollection())
    return;
  MReportEngine::exportToOds(pages->pageCollection());
}

FLReportPages *FLReportEngine::renderReport(int initRow, int initCol,
                                            bool fillRecords, FLReportPages *pages)
{
  MPageCollection *pgc = MReportEngine::renderReport(
                           initRow, initCol,
                           (pages ? pages->pageCollection() : 0),
                           (fillRecords ? MReportEngine::FillRecords : 0)
                         );

  FLReportPages *pgs = new FLReportPages();
  pgs->setPageCollection(pgc);
  if (!fillRecords || !d->qry_ || !d->qFieldMtdList_ || d->qDoubleFieldList_.isEmpty())
    return pgs;

  QDomNodeList nl(rd->elementsByTagName("Row"));
  QDomNamedNodeMap nm;
  QDomNode itm, ita;
  QString sVal;
  double dVal;
  bool ok;

  for (uint i = 0; i < nl.count(); ++i) {
    itm = nl.item(i);
    if (itm.isNull())
      continue;
    nm = itm.attributes();

    for (QStringList::const_iterator it = d->qDoubleFieldList_.begin();
         it != d->qDoubleFieldList_.end(); ++it) {
      ita = nm.namedItem(*it);
      if (ita.isNull())
        continue;
      sVal = ita.nodeValue();
      if (sVal.isEmpty() || sVal.upper() == "NAN")
        continue;
      dVal = sVal.toDouble(&ok);
      if (!ok)
        dVal = 0;
      ita.setNodeValue(
        FLUtil::formatoMiles(
          QString::number(
            dVal, 'f',
            d->qFieldMtdList_->find((*it).section('.', 1, 1).lower())->partDecimal()
          )
        )
      );
    }
  }

  return pgs;
}


// Intefaz scripts
