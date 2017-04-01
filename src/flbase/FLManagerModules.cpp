/***************************************************************************
                 FLManagerModules.cpp  -  description
                         -------------------
begin                : mie dic 24 2003
copyright            : (C) 2003-2004 by InfoSiAL, S.L.
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

#include <qdom.h>
#include <qdict.h>
#include <qpixmapcache.h>
#include <qbuffer.h>
#include <qwidgetfactory.h>

#include "FLManagerModules.h"
#include "FLMemCache.h"
#include "FLDiskCache.h"
#include "FLAction.h"
#include "FLSqlCursor.h"
#include "FLManager.h"
#include "FLApplication.h"
#include "FLUtil.h"
#include "FLSqlDatabase.h"
#include "FLModulesStaticLoader_p.h"
#include "FLSettings.h"

#include "AQConfig.h"


#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <openssl/sha.h>

using namespace std;

void test_sha256(const string name, const string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    if (ss.str() == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")
    	qWarning("FLManagerModules : Fichero " + QString(name) + " vacío.");
    else
    	{
    ss << "  " << name;
    const std::string tmp = ss.str();
    const char* cstr = tmp.c_str();
    qDebug(cstr);
    	}
    // return ss.str();
}


class FLInfoMod
{
public:
  FLInfoMod() {}
  QString idModulo;
  QString idArea;
  QString descripcion;
  QString version;
  QCString icono;
  QString areaDescripcion;
  QDomDocument signatures;
  QDomDocument certificates;
};

FLManagerModules::FLManagerModules(FLSqlDatabase *db) : db_(db)
{
  dictKeyFiles = 0;
  listAllIdModules_ = 0;
  listIdAreas_ = 0;
  dictModFiles = 0;
  dictInfoMods = 0;
  staticBdInfo_ = 0;
}

FLManagerModules::~ FLManagerModules()
{
  finish();
}

void FLManagerModules::loadAllIdModules()
{
  if (!db_->dbAux())
    return;

  if (!listAllIdModules_)
    listAllIdModules_ = new QStringList();
  else
    listAllIdModules_->clear();
  listAllIdModules_->append("sys");

  if (!dictInfoMods) {
    dictInfoMods = new QDict<FLInfoMod>(37);
    dictInfoMods->setAutoDelete(true);
  } else
    dictInfoMods->clear();

  QSqlQuery q(QString::null, db_->dbAux());
  q.setForwardOnly(true);
  q.exec("SELECT idmodulo,flmodules.idarea,flmodules.descripcion,version,icono,flareas.descripcion "
         "FROM flmodules left join flareas on flmodules.idarea = flareas.idarea");
  bool sysModuleFound = false;
  while (q.next()) {
    FLInfoMod *infoMod = new FLInfoMod();
    infoMod->idModulo = q.value(0).toString();
    infoMod->idArea = q.value(1).toString();
    infoMod->descripcion = q.value(2).toString();
    infoMod->version = q.value(3).toString();
    infoMod->icono = q.value(4).toCString();
    infoMod->areaDescripcion = q.value(5).toString();
    dictInfoMods->replace(infoMod->idModulo.upper(), infoMod);

    QString signatures = content(infoMod->idModulo + ".signatures");
    QString certificates = content(infoMod->idModulo + ".certificates");

    QDomDocument xmlDocSignatures;
    xmlDocSignatures.setContent(signatures);

    QDomDocument xmlDocCertificates;
    xmlDocCertificates.setContent(certificates);
    
    infoMod->signatures = xmlDocSignatures;
    infoMod->certificates = xmlDocCertificates;
    
    if (infoMod->idModulo != "sys")
      listAllIdModules_->append(infoMod->idModulo);
    else
      sysModuleFound = true;
    
    checkSignatures(infoMod);
        
    
  }
  if (!sysModuleFound) {
    FLInfoMod *infoMod = new FLInfoMod();
    infoMod->idModulo = QString("sys");
    infoMod->idArea = QString("sys");
    infoMod->descripcion = QString("Administración");
    infoMod->version = QString("0.0");
    infoMod->icono = contentFS(AQ_DATA + "/sys.xpm");
    infoMod->areaDescripcion = QString("Sistema");
    dictInfoMods->replace(infoMod->idModulo.upper(), infoMod);
  }
}


void FLManagerModules::checkSignatures(FLInfoMod *mod)
{
    QDomDocument sig = mod->signatures;
    QDomDocument cert = mod->certificates;
    
    QDomElement sigRoot = sig.firstChild().toElement();
    if (sigRoot.isNull()) {
        return;
    }
    if (sigRoot.tagName() != "eneboo-signatures") {
        qWarning("Signatures: XML Root Tag for signatures unknown: " + sigRoot.tagName());
        return;
    }
    QDomNode n = sigRoot.firstChild();
    while ( !n.isNull() ) {
        if ( n.isElement() ) {
            QDomElement e = n.toElement();
            if (e.tagName() != "signed-document") {
                qWarning("Signatures: tag unknown: " + e.tagName());
            } else {
                bool checkSignature = true;
                qDebug("Signature found.");
                QString checkval = e.attribute("check","true").lower();
                if (
                    checkval == "false" ||
                    checkval == "f" ||
                    checkval == "no" ||
                    checkval == "n" ||
                    checkval == "0"
                    ) {
                    checkSignature = false;
                    qDebug("Signature will be ignored.");
                } else if (
                    checkval == "true" ||
                    checkval == "t" ||
                    checkval == "yes" ||
                    checkval == "y" ||
                    checkval == "1"
                    ) {
                    checkSignature = true;
                    qDebug("Signature will be fully checked.");
                } else {
                    qWarning("Signatures: signed-document: Attribute check has unrecognized value: " + checkval);
                }
                QDomElement certificate, document, signature;
                
                QDomNode n1 = e.firstChild();
                while ( !n1.isNull() ) {
                    if ( n1.isElement() ) {
                        QDomElement e = n.toElement();
                        if (e.tagName() == "signer-certificate") {
                            certificate = e;
                        } else if (e.tagName() == "document") {
                            document = e;
                        } else if (e.tagName() == "signature") {
                            signature = e;
                        } else {
                            qWarning("Signatures: tag unknown: " + e.tagName());
                        }
                    }
                    n1 = n1.nextSibling();
                }
                
                if (certificate.isNull()) {
                    qWarning("Signatures: Certificate tag not found");
                    continue;
                }
                
                if (document.isNull()) {
                    qWarning("Signatures: Document tag not found");
                    continue;
                }
                
                if (signature.isNull()) {
                    qWarning("Signatures: Signature tag not found");
                    continue;
                }
                

                
                
                
            }
        }
        n = n.nextSibling();
    }
    
    

}

void FLManagerModules::loadIdAreas()
{
  if (!db_->dbAux())
    return;

  if (!listIdAreas_)
    listIdAreas_ = new QStringList();
  else
    listIdAreas_->clear();

  QSqlQuery q(QString::null, db_->dbAux());
  q.setForwardOnly(true);
  q.exec("SELECT idarea FROM flareas WHERE idarea <> 'sys'");
  while (q.next())
    listIdAreas_->append(q.value(0).toString());
  listIdAreas_->append("sys");
}

void FLManagerModules::loadKeyFiles()
{
  if (!dictKeyFiles) {
    dictKeyFiles = new QDict < QString >(3989);
    dictKeyFiles->setAutoDelete(true);
  } else
    dictKeyFiles->clear();

  if (!dictModFiles) {
    dictModFiles = new QDict < QString >(3989);
    dictModFiles->setAutoDelete(true);
  } else
    dictModFiles->clear();

  QSqlQuery q(QString::null, db_->dbAux());
  q.setForwardOnly(true);
  q.exec("SELECT nombre,sha,idmodulo FROM flfiles");
  QString name;
  while (q.next()) {
    name = q.value(0).toString();
    dictKeyFiles->replace(name, new QString(q.value(1).toString()));
    dictModFiles->replace(name.upper(), new QString(q.value(2).toString()));
  }
}

void FLManagerModules::init()
{
  rootDir_    = AQ_DATA + QString::fromLatin1("/");
  scriptsDir_ = AQ_DATA + QString::fromLatin1("/scripts/");
  tablesDir_  = AQ_DATA + QString::fromLatin1("/tables/");
  formsDir_   = AQ_DATA + QString::fromLatin1("/forms/");
  reportsDir_ = AQ_DATA + QString::fromLatin1("/reports/");
  queriesDir_ = AQ_DATA + QString::fromLatin1("/queries/");
  transDir_   = AQ_DATA + QString::fromLatin1("/translations/");

#ifndef FL_QUICK_CLIENT
  if (!db_->dbAux())
    return;

  if (db_->connectionName() != "default")
    return;

  FLTableMetaData *tmpTMD;

  tmpTMD = db_->manager()->createSystemTable("flfiles");
  tmpTMD = db_->manager()->createSystemTable("flsettings");
  tmpTMD = db_->manager()->createSystemTable("flserial");
  tmpTMD = db_->manager()->createSystemTable("flvar");
//-->FLLarge único   
  if (aqApp->singleFLLarge())
    tmpTMD = db_->manager()->createSystemTable("fllarge");
//<--FLarge único

  tmpTMD = db_->manager()->createSystemTable("flupdates");

  FLSqlCursor curSet("flsettings", true, db_->dbAux());
  QString modVer;
  bool checkVer = curSet.select("flkey = 'sysmodver'") &&
                  curSet.first();

  if (checkVer) {
    QString driverName(db_->driverName());
    modVer = curSet.valueBuffer("valor").toString();
    if (!modVer.isEmpty() && modVer[0] != '#') {
      if (driverName == "FLsqlite") {
        if (!db_->dbAux()->recordInfo("flfiles").contains("binario"))
          modVer = QString::null;
      } else {
        QSqlQuery qryFil("select * from flfiles limit 1", db_->dbAux());
        if (!db_->dbAux()->recordInfo(qryFil).contains("binario"))
          modVer = QString::null;
      }
    }
    if (modVer.isEmpty()) {
      modVer = '@';
    } else if (modVer[0] == '#') {
      tmpTMD = db_->manager()->metadata("flfiles");
      if (db_->regenTable("flfiles", tmpTMD)) {
        modVer = QString::null;
      } else if (driverName != "FLQPSQL7") {
        QString xmlNew(contentCached("flfiles.mtd"));
        QString xmlOld(xmlNew);
        db_->manager()->alterTable(xmlOld.replace("255", "300"), xmlNew);
        modVer = QString::null;
      }
    } else if (modVer != AQ_VERSION && modVer[0] != '@') {
      int numVer = modVer[0].digitValue() * 10 + modVer[2].digitValue();
      modVer = (numVer < 24) ? modVer.prepend('@') : QString::null;
    }
  }

  if (modVer.isEmpty()) {
    AQ_DISKCACHE_CLR();
    QSqlQuery qry(QString::null, db_->dbAux());
    qry.exec("DROP TABLE flserial CASCADE");
    // ###
    //qry.exec("DROP TABLE flvar CASCADE");
    modVer = AQ_VERSION;
  }
  if (curSet.isValid())
    curSet.setModeAccess(FLSqlCursor::EDIT);
  else
    curSet.setModeAccess(FLSqlCursor::INSERT);
  curSet.refreshBuffer();
  curSet.setValueBuffer("flkey", "sysmodver");
  curSet.setValueBuffer("valor", modVer);
  curSet.commitBuffer();

  tmpTMD = db_->manager()->createSystemTable("flfiles");
  tmpTMD = db_->manager()->createSystemTable("flserial");
  tmpTMD = db_->manager()->createSystemTable("flvar");

  tmpTMD = db_->manager()->createSystemTable("flareas");
  if (tmpTMD) {
    FLSqlCursor cursor("flareas", true, db_->dbAux());
    cursor.setModeAccess(FLSqlCursor::INSERT);
    cursor.refreshBuffer();
    cursor.setValueBuffer("idarea", "sys");
    cursor.setValueBuffer("descripcion", QApplication::tr("Sistema"));
    cursor.setValueBuffer("bloqueo", QVariant(false, 0));
    cursor.commitBuffer();
  }

  tmpTMD = db_->manager()->createSystemTable("flmodules");
  if (tmpTMD) {
    FLSqlCursor cursor("flmodules", true, db_->dbAux());
    cursor.setModeAccess(FLSqlCursor::INSERT);
    cursor.refreshBuffer();
    cursor.setValueBuffer("idmodulo", "sys");
    cursor.setValueBuffer("idarea", "sys");
    cursor.setValueBuffer("descripcion", QApplication::tr("Administración"));
    cursor.setValueBuffer("icono", contentFS(AQ_DATA + "/sys.xpm"));
    cursor.setValueBuffer("bloqueo", QVariant(false, 0));
    cursor.commitBuffer();
  }

  staticBdInfo_ = new AQStaticBdInfo(db_->database());
#endif

  readState();
}

void FLManagerModules::finish()
{
  if (listAllIdModules_) {
    delete listAllIdModules_;
    listAllIdModules_ = 0;
  }

  if (listIdAreas_) {
    delete listIdAreas_;
    listIdAreas_ = 0;
  }

  if (dictModFiles) {
    delete dictModFiles;
    dictModFiles = 0;
  }

  if (dictInfoMods) {
    delete dictInfoMods;
    dictInfoMods = 0;
  }

  if (staticBdInfo_) {
    delete staticBdInfo_;
    staticBdInfo_ = 0;
  }

  if (dictKeyFiles) {
    writeState();
    delete dictKeyFiles;
    dictKeyFiles = 0;
    FLMemCache::clear();
  }
}

void FLManagerModules::staticLoaderSetup()
{
  FLModulesStaticLoader::setup(staticBdInfo_);
}

QString FLManagerModules::contentStatic(const QString &n)
{
  QString str_ret(FLModulesStaticLoader::content(n, staticBdInfo_));

  if (!str_ret.isEmpty()) {
    QString sha(FLUtil::sha1(str_ret));
    QString *s = 0;
    if (dictKeyFiles && (s = dictKeyFiles->find(n)) && *s == sha) {
      return QString::null;
    } else if (dictKeyFiles && n.endsWith(".qs")) {
      dictKeyFiles->replace(n, new QString(sha));
    }

    if (n.endsWith(".mtd")) {
      FLTableMetaData *mtd;
      QDomDocument doc(n);
      QDomElement docElem;

      if (FLUtil::domDocumentSetContent(doc, str_ret)) {
        FLManager *mgr = db_->manager();
        docElem = doc.documentElement();
        mtd = mgr->metadata(&docElem, true);

        if (!mtd || mtd->isQuery())
          return str_ret;

        if (!mgr->existsTable(mtd->name()))
          mgr->createTable(mtd);
        else if (db_->canRegenTables())
          db_->regenTable(mtd->name(), mtd);
      }
    }
  }

  return str_ret;
}

QString FLManagerModules::content(const QString &n)
{
  if (n.isEmpty() || n.length() <= 3)
    return QString::null;

  QString retFS;
  bool notSysTable = db_->dbAux() && n.left(3) != "sys" && !db_->manager()->isSystemTable(n);

  if (notSysTable && staticBdInfo_ && staticBdInfo_->enabled_) {
    retFS = contentStatic(n);
    if (!retFS.isEmpty()) {
      return retFS;
    }
  }

  if (n.endsWith(".xml"))
    retFS = contentFS(rootDir_ + n);
  else if (n.endsWith(".qs"))
    retFS = contentFS(scriptsDir_ + n);
  else if (n.endsWith(".mtd"))
    retFS = contentFS(tablesDir_ + n);
  else if (n.endsWith(".ui"))
    retFS = contentFS(formsDir_ + n);
  else if (n.endsWith(".kut"))
    retFS = contentFS(reportsDir_ + n);
  else if (n.endsWith(".qry"))
    retFS = contentFS(queriesDir_ + n);
  else if (n.endsWith(".ts"))
    retFS = contentFS(transDir_ + n);

  if (!retFS.isEmpty()) {
    return retFS;
  }

  if (notSysTable) {
    QString formatVal(db_->manager()->formatAssignValue("nombre", QVariant::String, n, true));
    QSqlQuery q(QString::null, db_->dbAux());
    q.setForwardOnly(true);
    q.exec(QString::fromLatin1("SELECT contenido,sha FROM flfiles WHERE ") + formatVal);
    if (q.next()) {
      QString ret = q.value(0).toString();

      /* **** PRUEBAS COMPROBACION FICHEROS EN SHA-256 ****** */
      QTextCodec *codec = QTextCodec::codecForName("ISO8859-15"); 
      QCString ret_latin = codec->fromUnicode( ret );
      
      test_sha256(n.latin1(), (const char *)ret_latin);
      QString path = QString(".cache/") + n;
      ofstream myfile;
      myfile.open(path.latin1());
      myfile << ret_latin;
      myfile.close();  
      /* **** PRUEBAS COMPROBACION FICHEROS EN SHA-256 ****** */
      
      
      if (q.value(1).toString().isEmpty()) {
        FLSqlCursor cursor("flfiles", true, db_->dbAux());
        cursor.select(formatVal);

        if (cursor.lastError().type() != QSqlError::None) {
          //          QString msg("<p><img source=\"remove.png\" align=\"right\"><b><u>SQL ERROR</u></b><br><br>" +
          //                      QString(cursor.lastError().driverText()) + "<br>" +
          //                      QString(cursor.lastError().databaseText()) + "</p>");
          //          msg.replace("\n", "<br>");
          //          aqApp->popupWarn(msg);
          return QString::null;
        }

        cursor.setModeAccess(FLSqlCursor::EDIT);
        cursor.first();
        cursor.setValueBuffer("sha", FLUtil::sha1(ret));
        cursor.commitBuffer();
      }
      return ret;
    }
  }

  return QString::null;
}

static QTextCodec *codecByte = 0;

QString FLManagerModules::byteCodeToStr(const QByteArray &byteCode) const
{
  QDataStream in(byteCode, IO_ReadOnly);
  uint size = byteCode.size();
  QString strOut;
  QTextStream out(&strOut, IO_WriteOnly);
  Q_UINT8 c1;
  Q_UINT8 c2;
  while (size > 0) {
    in >> c1;
    AQ_CIN(c1);
    --size;
    if (size <= 0) {
      if (c1)
        out << QChar((Q_UINT16)c1);
      break;
    }
    in >> c2;
    AQ_CIN(c2);
    --size;
    if (c2 & 0x80) {
      out << QChar(c2, c1);
    } else {
      if (c1)
        out << QChar((Q_UINT16)c1);
      if (c2)
        out << QChar((Q_UINT16)c2);
    }
  }
  if (codecByte == 0)
    codecByte = QTextCodec::codecForName("ISO8859-15");
  return codecByte->toUnicode(strOut);
}

#if 0
QByteArray FLManagerModules::contentCodeFromBd(const QString &n, bool *isNull)
{
  if (isNull)
    *isNull = false;
  QString formatVal(db_->manager()->formatAssignValue("nombre", QVariant::String, n, true));
  QSqlQuery q(QString::null, db_->dbAux());
  q.setForwardOnly(true);
  if (!q.exec(QString::fromLatin1("SELECT binario FROM flfiles WHERE ") + formatVal))
    return QByteArray();
  if (!q.next())
    return QByteArray();
  if (q.isNull(0)) {
    if (isNull)
      *isNull = true;
    return QByteArray();
  }
  return q.value(0).toByteArray();
}

QString FLManagerModules::contentCode(const QString &n)
{
  QString sr;
  bool notSysTable = db_->dbAux() && n.left(3) != "sys" && !db_->manager()->isSystemTable(n);
  if (notSysTable && staticBdInfo_ && staticBdInfo_->enabled_) {
    sr = contentStatic(n);
    if (!sr.isEmpty())
      return sr;
  }

  QByteArray byteCode;
  if (n == "sys.qs" ||
      n == "aqapplication.qs" ||
      n == "plus_sys.qs") {
    QFile f(scriptsDir_ + n);
    if (!f.open(IO_ReadOnly))
      return QString::null;
    QDataStream dt(&f);
    dt >> byteCode;
  } else {
    bool isNull = false;
#ifndef AQ_NEBULA_BUILD
    QString *s = dictKeyFiles ? dictKeyFiles->find(n) : 0;
    QString key(s ? *s : shaOfFile(n));

    if (!key.isEmpty()) {
      if (!AQ_DISKCACHE_FIND(key, byteCode)) {
        byteCode = contentCodeFromBd(n, &isNull);
        if (!isNull && !byteCode.isEmpty())
          AQ_DISKCACHE_INS(key, byteCode);
      }
    } else
#endif
      byteCode = contentCodeFromBd(n, &isNull);
    if (isNull)
      return contentCode("sys.qs");
  }

  sr = byteCodeToStr(byteCode);
  return sr;
}
#else
QString FLManagerModules::contentCode(const QString &n)
{
  if (n == "sys.qs" || n == "plus_sys.qs")
    return content(n);
  QString s(contentCached(n));
  if (!s.left(45).lower().contains("var form"))
    s.prepend("var form = this;\n");

#if 0
  QRegExp rx;
  rx.setPattern("ctx\\s*:\\s*Object\\s*");
  s.replace(rx, "ctx");
  rx.setPattern("\\)\\s*:\\s*(FL\\w+|Object\\w*|String\\w*|Date\\w*|Number\\w*|Boolean\\w*|Array\\w*)");
  s.replace(rx, ")");
  rx.setPattern(":\\s*(FL\\w+|Object\\w*|String\\w*|Date\\w*|Number\\w*|Boolean\\w*|Array\\w*)\\s*\\=");
  s.replace(rx, "=");
  rx.setPattern("[\n\r]{3,}");
  s.replace(rx, "\n");
  //rx.setMinimal(true);
  //rx.setPattern("class\\s+(\\w+)\\s+extends\\s+\\1([\\s\n]*\\{.*\\}[\\s\n]*\\})");
  //scode.replace(rx, "/* ¡¡ ERROR !! : LA CLASE HEREDA DE ELLA MISMA."
  //               "\nCODIGO INHABILITADO AUTOMÁTICAMENTE POR AbanQ :\n\n"
  //               "class \\1 extends \\1 \\2\n\n ¡¡ FIN ERROR !! */");
#endif

  return s;
}
#endif

QString FLManagerModules::contentFS(const QString &pN)
{
  QFile fi(pN);
  if (!fi.open(IO_ReadOnly))
    return QString::null;
  QTextStream t(&fi);
  t.setEncoding(QTextStream::Latin1);
  QString str_ret(t.read());
  fi.close();
  return str_ret;
}

QString FLManagerModules::contentCached(const QString &n, QString *shaKey)
{
  if (n.isEmpty() || n.length() <= 3)
    return QString::null;

  QString str_ret;
#ifndef AQ_NEBULA_BUILD
  bool notSysTable = (db_->dbAux() && n.left(3) != "sys" &&
                      !db_->manager()->isSystemTable(n));

  if (notSysTable && staticBdInfo_ && staticBdInfo_->enabled_) {
    str_ret = contentStatic(n);
    if (!str_ret.isEmpty())
      return str_ret;
  }

  QString key;
  QString *s = 0;

  if (notSysTable) {
    if (dictKeyFiles)
      s = dictKeyFiles->find(n);
    if (s) {
      key = *s;
      if (shaKey)
        *shaKey = key;
    } else {
      s = FLMemCache::find(n);
      if (s)
        return *s;
      key = shaOfFile(n);
      if (shaKey)
        *shaKey = key;
    }
  } else
    return content(n);

  if (key.isEmpty()) {
    str_ret = content(n);
    FLMemCache::insert(n, str_ret);
    return str_ret;
  }

  if (!AQ_DISKCACHE_FIND(key, str_ret)) {
    str_ret = content(n);
    if (!str_ret.isEmpty())
      AQ_DISKCACHE_INS(key, str_ret);
  }
#else
  str_ret = content(n);
#endif

  return str_ret;
}

void FLManagerModules::setContent(const QString &n, const QString &idM, const QString &content)
{
  if (!db_->dbAux())
    return;

  QString formatVal(db_->manager()->formatAssignValue("nombre", QVariant::String, n, true));
  QString formatVal2(db_->manager()->formatAssignValue("idmodulo", QVariant::String, idM, true));

  FLSqlCursor cursor("flfiles", true, db_->dbAux());
  cursor.select(formatVal + QString::fromLatin1(" AND ") + formatVal2);

  if (cursor.first()) {
    cursor.setModeAccess(FLSqlCursor::EDIT);
    cursor.refreshBuffer();
  } else {
    cursor.setModeAccess(FLSqlCursor::INSERT);
    cursor.refreshBuffer();
    cursor.setValueBuffer("nombre", n);
    cursor.setValueBuffer("idmodulo", idM);
  }

  cursor.setValueBuffer("contenido", content);
  cursor.setValueBuffer("sha", FLUtil::sha1(content));
  cursor.commitBuffer();
}

QString FLManagerModules::shaOfFile(const QString &n)
{
  if (db_->dbAux() && n.left(3) != "sys" && !db_->manager()->isSystemTable(n)) {
    QString formatVal(db_->manager()->formatAssignValue("nombre", QVariant::String, n, true));
    QSqlQuery q(QString::null, db_->dbAux());
    q.setForwardOnly(true);
    q.exec(QString::fromLatin1("SELECT sha FROM flfiles WHERE ") + formatVal);
    if (q.next())
      return q.value(0).toString();
    return QString::null;
  } else
    return QString::null;
}

QWidget *FLManagerModules::createUI(const QString &n, QObject *connector,
                                    QWidget *parent, const char *name)
{
  QBuffer b;
  b.open(IO_ReadWrite);
  QTextStream t(&b);
  t.setEncoding(QTextStream::Latin1);
  t << contentCached(n);
  b.reset();
  QWidget *w = QWidgetFactory::create(&b, connector, parent, name);
  return w;
}

QWidget *FLManagerModules::createForm(const FLAction *a, QObject *connector,
                                      QWidget *parent, const char *name)
{
  return createUI(a->form(), connector, parent, name);
}

QWidget *FLManagerModules::createFormRecord(const FLAction *a, QObject *connector,
                                            QWidget *parent, const char *name)
{
  return createUI(a->formRecord(), connector, parent, name);
}

void FLManagerModules::setActiveIdModule(const QString &id)
{
#if defined (FL_QUICK_CLIENT)
  if (id == "sys") {
    activeIdModule_ = QString::null;
    activeIdArea_ = QString::null;
    return;
  }
#endif

  if (id.isEmpty() || !dictInfoMods) {
    activeIdArea_ = QString::null;
    activeIdModule_ = QString::null;
    return;
  }

  FLInfoMod *iM = (*dictInfoMods)[ id.upper()];
  if (iM) {
    activeIdArea_ = iM->idArea;
    activeIdModule_ = id;
  } else {
#ifdef FL_DEBUG
    qWarning(QApplication::tr("FLManagerModules : Se ha intentando activar un módulo inexistente"));
#endif
    activeIdArea_ = QString::null;
    activeIdModule_ = QString::null;
  }
}

void FLManagerModules::readState()
{
  QString idDB = "noDB";
  if (db_->dbAux())
    idDB = db_->dbAux()->databaseName() + db_->dbAux()->hostName() + db_->dbAux()->userName() +
           db_->dbAux()->driverName() + QString::number(db_->dbAux()->port());

  activeIdModule_ = FLSettings::readEntry("Modules/activeIdModule/" + idDB, QString::null);
  activeIdArea_ = FLSettings::readEntry("Modules/activeIdArea/" + idDB, QString::null);
  shaLocal_ = FLSettings::readEntry("Modules/shaLocal/" + idDB, QString::null);

#if defined (FL_QUICK_CLIENT)
  if (activeIdModule_ == "sys") {
    activeIdModule_ = QString::null;
    activeIdArea_ = QString::null;
  }
#endif

  if (activeIdModule_.isEmpty() || !listAllIdModules().contains(activeIdModule_))
    setActiveIdModule(QString::null);
}

void FLManagerModules::writeState()
{
  QString idDB = "noDB";
  if (db_->dbAux())
    idDB = db_->database() + db_->host() + db_->user() + db_->driverName() + QString::number(db_->port());

  FLSettings::writeEntry("Modules/activeIdModule/" + idDB, activeIdModule_);
  FLSettings::writeEntry("Modules/activeIdArea/" + idDB, activeIdArea_);
  FLSettings::writeEntry("Modules/shaLocal/" + idDB, shaLocal_);
}

QStringList FLManagerModules::listIdAreas()
{
  if (listIdAreas_)
    return * listIdAreas_;

  QStringList ret;
  if (!db_->dbAux())
    return ret;

  QSqlQuery q(QString::null, db_->dbAux());
  q.setForwardOnly(true);
  q.exec("SELECT idarea FROM flareas WHERE idarea <> 'sys'");
  while (q.next())
    ret << q.value(0).toString();
  ret << "sys";

  return ret;
}

QStringList FLManagerModules::listIdModules(const QString &idA)
{
  QStringList ret;
  if (dictInfoMods) {
    QDictIterator < FLInfoMod > it(*dictInfoMods);
    while (it.current()) {
      if (it.current()->idArea.upper() == idA.upper())
        ret << it.current()->idModulo;
      ++it;
    }
  }
  return ret;
}

QStringList FLManagerModules::listAllIdModules()
{
  if (listAllIdModules_)
    return *listAllIdModules_;

  QStringList ret;
  if (!db_->dbAux())
    return ret;

  ret << "sys";
  QSqlQuery q(QString::null, db_->dbAux());
  q.setForwardOnly(true);
  q.exec("SELECT idmodulo FROM flmodules WHERE idmodulo <> 'sys'");
  while (q.next())
    ret << q.value(0).toString();

  return ret;
}

QString FLManagerModules::idAreaToDescription(const QString &idA)
{
  if (dictInfoMods) {
    QDictIterator < FLInfoMod > it(*dictInfoMods);
    while (it.current()) {
      if (it.current()->idArea.upper() == idA.upper())
        return it.current()->areaDescripcion;
      ++it;
    }
  }
  return idA;
}

QString FLManagerModules::idModuleToDescription(const QString &idM)
{
  if (!dictInfoMods)
    return idM;
  FLInfoMod *iM = (*dictInfoMods)[ idM.upper()];
  if (iM)
    return iM->descripcion;
  else
    return idM;
}

QPixmap FLManagerModules::iconModule(const QString &idM)
{
  QPixmap pix;
  if (dictInfoMods) {
    FLInfoMod *iM = (*dictInfoMods)[ idM.upper()];
    if (iM) {
      QCString cs(iM->icono);
      if (!cs.isEmpty()) {
        if (!QPixmapCache::find(cs.left(100), pix)) {
          pix.loadFromData(cs);
          QPixmapCache::insert(cs.left(100), pix);
        }
      }
    }
  }
  return pix;
}

QString FLManagerModules::versionModule(const QString &idM)
{
  if (!dictInfoMods)
    return idM;
  FLInfoMod *iM = (*dictInfoMods)[ idM.upper()];
  if (iM)
    return iM->version;
  else
    return idM;
}

QString FLManagerModules::shaGlobal()
{
#ifdef FL_QUICK_CLIENT
  return "quick_client";
#else
  if (!db_->dbAux())
    return QString::null;

  QSqlQuery q(QString::null, db_->dbAux());
  q.setForwardOnly(true);
  q.exec("SELECT sha FROM flserial");
  if (q.lastError().type() != QSqlError::None) {
    //    QString msg("<p><img source=\"remove.png\" align=\"right\"><b><u>SQL ERROR</u></b><br><br>" +
    //                QString(q.lastError().driverText()) + "<br>" +
    //                QString(q.lastError().databaseText()) + "</p>");
    //    msg.replace("\n", "<br>");
    //    aqApp->popupWarn(msg);
    return "error";
  }
  if (q.next())
    return q.value(0).toString();
  else
    return QString::null;
#endif
}

QString FLManagerModules::shaLocal()
{
#ifdef FL_QUICK_CLIENT
  return "quick_client";
#else
  return FLManagerModules::shaLocal_;
#endif
}

void FLManagerModules::setShaLocalFromGlobal()
{
  shaLocal_ = shaGlobal();
}

QString FLManagerModules::idModuleOfFile(const QString &n)
{
  if (!dictModFiles)
    return QString::null;
  QString *ret = (*dictModFiles)[ n.upper()];
  if (!ret)
    return QString::null;
  return *ret;
}
