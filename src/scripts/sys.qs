/***************************************************************************
                                  sys.qs
                            -------------------
   begin                : lun abr 26 2004
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
   bajo  los  t�rminos  de  la  Licencia  P�blica General de GNU   en  su
   versi�n 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

function init() {
  var qry = new AQSqlQuery;
  qry.setSelect("valor");
  qry.setFrom("flsettings");
  qry.setWhere("flkey='sysmodver'");

  if (!qry.exec() || !qry.next())
    return;

  var modVer = qry.value(0).toString();
  if (modVer.charAt(0) == '@') {
    var txt = "";
    txt += sys.translate("La versi�n instalada de los m�dulos en esta  base\n");
    txt += sys.translate("de datos est� desactualizada, le recomendamos que\n");
    txt += sys.translate("la actualice lo antes posible.\n");
    txt += sys.translate("Mientras tanto, Eneboo necesita realizar algunos\n");
    txt += sys.translate("ajustes para  poder ser  ejecutado con las nuevas\n");
    txt += sys.translate("versiones.\n");
    txt += sys.translate("No  deber�a  continuar  si no tiene una  copia de\n");
    txt += sys.translate("seguridad que le  permita deshacer  los cambios y\n");
    txt += sys.translate("volver al estado anterior en cualquier momento.\n\n");
    txt += "\n\n";
    txt += sys.translate("�Desea continuar?");

    var res = MessageBox.warning(txt, MessageBox.No, MessageBox.Yes);
    if (res == MessageBox.No) {
      sys.AQTimer.singleShot(0, aqApp.quit);  
      return;
    }
    
    modVer = '#' + modVer.mid(1);
    AQSql.update("flsettings", ["valor"], [modVer], "flkey='sysmodver'");
    sys.AQTimer.singleShot(0, sys.reinit);
    return;
  }

  if (isLoadedModule("flfactppal")) {
    var util: FLUtil = new FLUtil();
    var codEjercicio: String = flfactppal.iface.pub_ejercicioActual();
    var nombreEjercicio: String = util.sqlSelect("ejercicios", "nombre", "codejercicio='" + codEjercicio + "'");
    setCaptionMainWidget(nombreEjercicio);
  }
  //return;
  if (sys.mainWidget() != undefined) {
    var util = new FLUtil();
    var curFiles = new FLSqlCursor("flfiles");
    curFiles.select();
    if (!curFiles.size()) {
      var continuar = MessageBox.warning(util.translate("scripts", "No hay m�dulos cargados en esta base de datos,\nAbanQ puede cargar autom�ticamente la base de m�dulos\nde Facturaci�n y Financiera incluidos en la instalaci�n.\n\n�Desea cargar ahora estos m�dulos base?\n"), MessageBox.Yes, MessageBox.No);
      if (continuar == MessageBox.Yes) {
        var dirModsFact = sys.installPrefix() + "/share/facturalux/modulos/facturacion/";
        var dirModsCont = sys.installPrefix() + "/share/facturalux/modulos/contabilidad/";
        formflreloadbatch.iface.pub_cargarModulo(Dir.cleanDirPath(dirModsFact + "principal/flfactppal.mod"));
        while (formRecordflmodules.child("log"))
        sys.processEvents();
        formflreloadbatch.iface.pub_cargarModulo(Dir.cleanDirPath(dirModsFact + "almacen/flfactalma.mod"));
        while (formRecordflmodules.child("log"))
        sys.processEvents();
        formflreloadbatch.iface.pub_cargarModulo(Dir.cleanDirPath(dirModsFact + "facturacion/flfacturac.mod"));
        while (formRecordflmodules.child("log"))
        sys.processEvents();
        formflreloadbatch.iface.pub_cargarModulo(Dir.cleanDirPath(dirModsFact + "tesoreria/flfactteso.mod"));
        while (formRecordflmodules.child("log"))
        sys.processEvents();
        formflreloadbatch.iface.pub_cargarModulo(Dir.cleanDirPath(dirModsFact + "informes/flfactinfo.mod"));
        while (formRecordflmodules.child("log"))
        sys.processEvents();
        formflreloadbatch.iface.pub_cargarModulo(Dir.cleanDirPath(dirModsCont + "principal/flcontppal.mod"));
        while (formRecordflmodules.child("log"))
        sys.processEvents();
        formflreloadbatch.iface.pub_cargarModulo(Dir.cleanDirPath(dirModsCont + "informes/flcontinfo.mod"));
        while (formRecordflmodules.child("log"))
        sys.processEvents();
        sys.reinit();
      }
    }
  }
}

function afterCommit_flfiles(curFiles) {
  if (curFiles.modeAccess() != curFiles.Browse) {
    var qry = new FLSqlQuery();
    qry.setTablesList("flserial");
    qry.setSelect("sha");
    qry.setFrom("flfiles");
    qry.setForwardOnly(true);
    if (qry.exec()) {
      if (qry.first()) {
        var util = new FLUtil();
        var v = util.sha1(qry.value(0));
        while (qry.next())
        v = util.sha1(v + qry.value(0));
        var curSerial = new FLSqlCursor("flserial");
        curSerial.select();
        if (!curSerial.first()) curSerial.setModeAccess(curSerial.Insert);
        else curSerial.setModeAccess(curSerial.Edit);
        curSerial.refreshBuffer();
        curSerial.setValueBuffer("sha", v);
        curSerial.commitBuffer();
      }
    } else {
      var curSerial = new FLSqlCursor("flserial");
      curSerial.select();
      if (!curSerial.first()) curSerial.setModeAccess(curSerial.Insert);
      else curSerial.setModeAccess(curSerial.Edit);
      curSerial.refreshBuffer();
      curSerial.setValueBuffer("sha", curFiles.valueBuffer("sha"));
      curSerial.commitBuffer();
    }
  }

  return true;
}

function statusDbLocksDialog( locks ) {
  var util = new FLUtil;
  var diag = new Dialog;
  var txtEdit = new TextEdit;

  diag.caption = util.translate( "scripts", "Bloqueos de la base de datos" );
  diag.width = 500;

  var html = "<html><table border=\"1\">";
      
  if ( locks != undefined && locks.length ) {
    var i = 0;
    var j = 0;
    var item = "";
    var fields = locks[0].split( "@" );
    var closeInfo = false;
    var closeRecord = false;
        
    var headInfo = "<table border=\"1\"><tr>";
    for ( i = 0; i < fields.length; ++i )
      headInfo += "<td><b>" + fields[i] + "</b></td>";
    headInfo += "</tr>";
    
    var headRecord = "<table border=\"1\"><tr><td><b>" + util.translate( "scripts", "Registro bloqueado" ) + "</b></td></tr>";
    
    for ( i = 1; i < locks.length; ++i ) {
        item = locks[i];
        
        if ( item.left( 2 ) == "##" ) {
            if ( closeInfo )
                html += "</table>";
            if ( !closeRecord )
                html += headRecord;
            
            html += "<tr><td>" + item.right( item.length - 2 ) + "</td></tr>";
            
            closeRecord = true;
            closeInfo = false
        } else {
            if ( closeRecord )
                html += "</table>";
            if ( !closeInfo )
                html += headInfo;
            
            html += "<tr>";
            fields = item.split( "@" );
            for ( j = 0; j < fields.length; ++j )
              html += "<td>" + fields[j] + "</td>";
            html += "</tr>";
            
            closeRecord = false;
            closeInfo = true
        }
    }
  }

  html += "</table></table></html>";
  
  txtEdit.text = html;
  diag.add( txtEdit );
  diag.exec();
}

function terminateChecksLocks( sqlCursor ) {
   if ( sqlCursor != undefined )
    sqlCursor.checkRisksLocks( true );
}

function execQSA(fileQSA, args) {
  var file = new File(fileQSA);
  try {
    file.open(File.ReadOnly);
  } catch(e) {
    debug(e);
    return;
  }
  var fn = new Function(file.read());
  fn(args);
}

class AQGlobalFunctions {
  static var functions_ = [];
  static var mappers_ = [];
  static var count_ = 0;
  
  static function set(functionName, globalFunction) {
    functions_[functionName] = globalFunction;
  }
  
  static function get(functionName) {
    return functions_[functionName];
  }

  static function exec(functionName) {
    var fn = functions_[functionName];
    if (fn != undefined)
      fn();
  }
  
  static function mapConnect(obj, signal, functionName) {
    const c = count_ % 100;
    var sigMap = mappers_[c] = new AQSignalMapper(obj);
    var killMapper = function() {
      mappers_[c] = undefined;
    }
    connect(sigMap, "mapped(QString)", sys.AQGlobalFunctions, "exec()");
    sigMap.setMapping(obj, functionName);
    connect(obj, signal, sigMap, "map()");
    ++count_;
  }
}

class AQTimer {
  static var timers_ = [];
  static var count_ = 0;
  
  static function singleShot(msec, timeoutFunction) {
    const c = count_ % 100;
    var callback = function() {
      killTimer(timers_[c]);
      timers_[c] = undefined;
      timeoutFunction();
    }
    timers_[c] = startTimer(msec, callback);
    ++count_;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

function loadModules(input, warnBackup)
{
  if (input == undefined) {
    var dir = new Dir(sys.installPrefix() + "/share/abanq/packages");
    dir.setCurrent();
    input = FileDialog.getOpenFileName(
              "AbanQ Packages (*.abanq)",
              AQUtil.translate("scripts", "Seleccionar Fichero")
            );
  }
  if (warnBackup == undefined)
    warnBackup = true;
  if (input)
    loadAbanQPackage(input, warnBackup);
}

function loadAbanQPackage(input, warnBackup)
{
  if (warnBackup) {
    var txt = "";
    txt += sys.translate("Aseg�rese de tener una copia de seguridad de todos los datos\n");
    txt += sys.translate("y de que  no hay ningun otro  usuario conectado a la base de\n");
    txt += sys.translate("datos mientras se realiza la carga.\n\n");
    txt += "\n\n";
    txt += sys.translate("�Desea continuar?");
    if (MessageBox.Yes != MessageBox.warning(txt, MessageBox.No, MessageBox.Yes))
      return;
  } 
  
  if (input) {
    var ok = true;
    var tmpVar = new FLVar;
    tmpVar.set("mrproper", "dirty");
    sys.Mr_Proper();
    if (tmpVar.get("mrproper") == "dirty")
      ok = false;

    if (ok) {
      var unpacker = new AQUnpacker(input);
      var errors = unpacker.errorMessages();
      if (errors.length != 0) {
        var msg = sys.translate(
                    "Hubo los siguientes errores al intentar cargar los m�dulos:"
                  );
        msg += "\n";
        for (var i = 0; i < errors.length; ++i)
          msg += errors[i] + "\n";
        errorMsgBox(msg);
        ok = false;
      }

      if (ok)
        ok = loadModulesDef(unpacker);

      if (ok)
        ok = loadFilesDef(unpacker);
    }

    if (!ok) {
      errorMsgBox(sys.translate(
                    "No se ha podido realizar la carga de los m�dulos."
                  ));
    } else {
      sys.Mr_Proper();
      MessageBox.information(sys.translate("La carga de m�dulos se ha realizado con �xito."),
                             MessageBox.Ok, MessageBox.NoButton,
                             MessageBox.NoButton, "Eneboo");
      sys.AQTimer.singleShot(0, sys.reinit);
    }
  }
}

function loadFilesDef(un)
{
  var filesDef = sys.toUnicode(un.getText(), "utf8");
  var doc = new QDomDocument;

  if (!doc.setContent(filesDef)) {
    errorMsgBox(sys.translate(
                  "Error XML al intentar cargar la definici�n de los ficheros."
                ));
    return false;
  }

  var ok = true;
  var root = doc.firstChild();
  var files = root.childNodes();

  AQUtil.createProgressDialog(sys.translate("Registrando ficheros"), files.length());

  for (var i = 0; i < files.length(); ++i) {
    var it = files.item(i);

    var fil = {
      id:         it.namedItem("name").toElement().text(),
      module:     it.namedItem("module").toElement().text(),
      text:       it.namedItem("text").toElement().text(),
      shatext:    it.namedItem("shatext").toElement().text(),
      binary:     it.namedItem("binary").toElement().text(),
      shabinary:  it.namedItem("shabinary").toElement().text()
    }

    AQUtil.setProgress(i);
    AQUtil.setLabelText(sys.translate("Registrando fichero") + " " + fil.id);

    if (fil.id.length == 0)
      continue;

    if (!registerFile(fil, un)) {
      errorMsgBox(sys.translate(
                    "Error registrando el fichero"
                  ) + " " + fil.id);
      ok = false;
      break;
    }
  }

  AQUtil.destroyProgressDialog();

  return ok;
}

function registerFile(fil, un)
{
  if (fil.id.endsWith(".xpm")) {
    var cur = new AQSqlCursor("flmodules");
    if (!cur.select("idmodulo='" + fil.module + "'"))
      return false;
    if (!cur.first())
      return false;

    cur.setModeAccess(AQSql.Edit);
    cur.refreshBuffer();
    cur.setValueBuffer("icono", un.getText());
    return cur.commitBuffer();
  }

  var cur = new AQSqlCursor("flfiles");
  if (!cur.select("nombre='" + fil.id + "'"))
    return false;

  cur.setModeAccess(cur.first() ? AQSql.Edit : AQSql.Insert);
  cur.refreshBuffer();
  cur.setValueBuffer("nombre", fil.id);
  cur.setValueBuffer("idmodulo", fil.module);
  cur.setValueBuffer("sha", fil.shatext);
  if (fil.text.length > 0)
    cur.setValueBuffer("contenido", un.getText());
  if (fil.binary.length > 0)
    un.getBinary() // drop the binary data.
    
  return cur.commitBuffer();
}

function loadModulesDef(un)
{
  var modulesDef = sys.toUnicode(un.getText(), "utf8");
  var doc = new QDomDocument;

  if (!doc.setContent(modulesDef)) {
    errorMsgBox(sys.translate(
                  "Error XML al intentar cargar la definici�n de los m�dulos."
                ));
    return false;
  }

  var ok = true;
  var root = doc.firstChild();
  var modules = root.childNodes();

  AQUtil.createProgressDialog(sys.translate("Registrando m�dulos"), modules.length());

  for (var i = 0; i < modules.length(); ++i) {
    var it = modules.item(i);
    var mod = {
      id:       it.namedItem("name").toElement().text(),
      alias:    trTagText(it.namedItem("alias").toElement().text()),
      area:     it.namedItem("area").toElement().text(),
      areaname: trTagText(it.namedItem("areaname").toElement().text()),
      version:  it.namedItem("version").toElement().text()
    }

    AQUtil.setProgress(i);
    AQUtil.setLabelText(sys.translate("Registrando m�dulo") + " " + mod.id);

    if (!registerArea(mod) || !registerModule(mod)) {
      errorMsgBox(sys.translate(
                    "Error registrando el m�dulo"
                  ) + " " + mod.id);
      ok = false;
      break;
    }
  }

  AQUtil.destroyProgressDialog();

  return ok;
}

function registerArea(mod)
{
  var cur = new AQSqlCursor("flareas");
  if (!cur.select("idarea='" + mod.area + "'"))
    return false;

  cur.setModeAccess(cur.first() ? AQSql.Edit : AQSql.Insert);
  cur.refreshBuffer();
  cur.setValueBuffer("idarea", mod.area);
  cur.setValueBuffer("descripcion", mod.areaname);
  return cur.commitBuffer();
}

function registerModule(mod)
{
  var cur = new AQSqlCursor("flmodules");
  if (!cur.select("idmodulo='" + mod.id + "'"))
    return false;

  cur.setModeAccess(cur.first() ? AQSql.Edit : AQSql.Insert);
  cur.refreshBuffer();
  cur.setValueBuffer("idmodulo", mod.id);
  cur.setValueBuffer("idarea", mod.area);
  cur.setValueBuffer("descripcion", mod.alias);
  cur.setValueBuffer("version", mod.version);
  return cur.commitBuffer();
}

function errorMsgBox(msg)
{
  msg += "\n";
  MessageBox.critical(msg, MessageBox.Ok, MessageBox.NoButton,
                      MessageBox.NoButton, "Eneboo");
}

function trTagText(tagText)
{
  if (!tagText.startsWith("QT_TRANSLATE_NOOP"))
    return tagText;
  var txt = tagText.mid(String("QT_TRANSLATE_NOOP").length + 1);
  txt = "[" + txt.mid(0, txt.length - 1) + "]";
  var arr = eval(txt);
  return sys.translate(arr[0], arr[1]);
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

class AbanQUpdater
{
  var w_;
  var lblSt_;
  var prBar_;
  var urlOp_;
  var state_;
  var data_;

  function AbanQUpdater()
  {
    this.w_ = new QDialog;
    this.w_.caption = "Eneboo";
    this.w_.name = "abanqUpdaterDialog";
    this.w_.modal = true;

    var lay = new QVBoxLayout(this.w_);

    this.lblSt_ = new QLabel(this.w_);
    this.lblSt_.alignment = AQS.AlignHCenter | AQS.AlignVCenter;
    lay.addWidget(this.lblSt_);

    this.prBar_ = new QProgressBar(this.w_);
    this.prBar_.setCenterIndicator(true);
    this.prBar_.setTotalSteps(100);
    lay.addWidget(this.prBar_);

    this.w_.resize(100, 50);

    this.data_ = "";
    this.urlOp_ = new QUrlOperator("http://updates.infosial.com");

    connect(this.urlOp_, "connectionStateChanged(int,const QString&)",
            this, "stateChanged()");
    connect(this.urlOp_, "finished(QNetworkOperation*)",
            this, "transferFinished()");
    connect(this.urlOp_, "dataTransferProgress(int,int,QNetworkOperation*)",
            this, "transferProgress()");
    connect(this.urlOp_, "data(const QByteArray&,QNetworkOperation*)",
            this, "transferData()");

    this.urlOp_.get("updater24.qs");
  }

  function stateChanged(state, data)
  {
    this.lblSt_.text = String("%1: %2").arg(state).arg(data);
  }

  function transferFinished(netOp)
  {
    this.state_ = netOp.state();
    this.w_.close();
    if (this.state_ == AQS.StFailed) {
      MessageBox.critical(netOp.protocolDetail(), MessageBox.Ok,
                          MessageBox.NoButton, MessageBox.NoButton, "Error");
    }
  }

  function transferProgress(bytesDone, bytesTotal, netOp)
  {
    if (bytesTotal > 0)
      this.prBar_.setTotalSteps(bytesTotal);
    this.prBar_.setProgress(bytesDone);
  }

  function transferData(data, netOp)
  {
    var dat = new QByteArray(data);
    this.data_ += dat.toVariant;
  }
}

function updateAbanQ()
{
  MessageBox.warning(sys.translate("Funcionalidad no soportada a�n en Eneboo."), MessageBox.Ok, MessageBox.NoButton,MessageBox.NoButton, "Eneboo");
  return;
  /*
  var msg = "";
  msg += sys.translate("Se va a proceder a conectar a trav�s de Internet\n");
  msg += sys.translate("con los sistemas de InfoSiAL S.L. para obtener la\n");
  msg += sys.translate("herramienta de actualizaci�n de AbanQ.\n\n");
  msg += sys.translate("� Desea continuar ?\n\n");
  if (MessageBox.Yes != MessageBox.warning(msg, MessageBox.No, MessageBox.Yes,
                        MessageBox.NoButton, "AbanQ"))
    return;
    
  var updater = new AbanQUpdater;
  updater.w_.exec();

  if (updater.state_ != AQS.StFailed) {
    var scriptInfos = [];
    var scrName = "abanqUpdaterScript";
    var scrCode = updater.data_;
    var scr = QSProject.script(scrName);
    
    if (!scr) {
      scriptInfos.push([scrName, scrCode, QSProject.New, ""]);
    } else if (scr.code() != scrCode) {
      scriptInfos.push([scrName, scrCode, QSProject.Changed, ""]);
    }

    if (scriptInfos.length > 0) {
      var scrInfo = scriptInfos[0];
      if (scrInfo[2] == QSProject.New) {
        var obj = new QObject(scrInfo[0]);
        QSProject.addObject(obj);
      }
      QSProject.evaluateScripts(scriptInfos, "updaterMain24");
    }
  }
  */
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

function exportModules()
{
  var dirBasePath = FileDialog.getExistingDirectory(Dir.home);
  if (!dirBasePath)
    return;
  dirBasePath = Dir.cleanDirPath(dirBasePath + "/modulos_exportados");

  var dir = new Dir;
  if (!dir.fileExists(dirBasePath)) {
    try {
      dir.mkdir(dirBasePath);
    } catch (e) {
      MessageBox.critical("" + e, MessageBox.Ok);
      return;
    }
  } else {
    MessageBox.warning(dirBasePath + 
                       sys.translate(" ya existe,\ndebe borrarlo antes de continuar"),
                       MessageBox.Ok, MessageBox.NoButton, MessageBox.NoButton, "Eneboo");
    return;
  }

  var qry = new AQSqlQuery;
  qry.setSelect("idmodulo");
  qry.setFrom("flmodules");
  if (!qry.exec() || qry.size() == 0)
    return;

  var p = 0;
  AQUtil.createProgressDialog(sys.translate("Exportando m�dulos"), qry.size() - 1);

  while (qry.next()) {
    var idMod = qry.value(0);
    if (idMod == "sys")
      continue;

    AQUtil.setLabelText(String("%1").arg(idMod));
    AQUtil.setProgress(++p);

    exportModule(idMod, dirBasePath);
  }

  AQUtil.destroyProgressDialog();
  MessageBox.information(sys.translate("M�dulos exportados en:\n") + dirBasePath,
                         MessageBox.Ok, MessageBox.NoButton,
                         MessageBox.NoButton, "Eneboo");
}
 
function xmlModule(idMod)
{
  var qry = new AQSqlQuery;
  qry.setSelect("descripcion,idarea,version");
  qry.setFrom("flmodules");
  qry.setWhere("idmodulo='" + idMod + "'");
  if (!qry.exec() || !qry.next())
    return;

  var doc = new QDomDocument("MODULE");
  var tagMod = doc.createElement("MODULE");
  doc.appendChild(tagMod);

  var tag = doc.createElement("name");
  tag.appendChild(doc.createTextNode(idMod));
  tagMod.appendChild(tag);

  var trNoop = "QT_TRANSLATE_NOOP(\"Eneboo\",\"%1\")";
  tag = doc.createElement("alias");
  tag.appendChild(doc.createTextNode(trNoop.argStr(qry.value(0))));
  tagMod.appendChild(tag);

  var idArea = qry.value(1);
  tag = doc.createElement("area");
  tag.appendChild(doc.createTextNode(idArea));
  tagMod.appendChild(tag);

  var areaName = AQUtil.sqlSelect("flareas", "descripcion", "idarea='" + idArea + "'");
  tag = doc.createElement("areaname");
  tag.appendChild(doc.createTextNode(trNoop.argStr(areaName)));
  tagMod.appendChild(tag);

  tag = doc.createElement("entryclass");
  tag.appendChild(doc.createTextNode(idMod));
  tagMod.appendChild(tag);

  tag = doc.createElement("version");
  tag.appendChild(doc.createTextNode(qry.value(2)));
  tagMod.appendChild(tag);

  tag = doc.createElement("icon");
  tag.appendChild(doc.createTextNode(idMod + ".xpm"));
  tagMod.appendChild(tag);

  return doc;
}

function exportModule(idMod, dirBasePath)
{
  var dir = new Dir;
  var dirPath = Dir.cleanDirPath(dirBasePath + "/" + idMod);

  if (!dir.fileExists(dirPath))
    dir.mkdir(dirPath);
  if (!dir.fileExists(dirPath + "/forms"))
    dir.mkdir(dirPath + "/forms");
  if (!dir.fileExists(dirPath + "/scripts"))
    dir.mkdir(dirPath + "/scripts");
  if (!dir.fileExists(dirPath + "/queries"))
    dir.mkdir(dirPath + "/queries");
  if (!dir.fileExists(dirPath + "/tables"))
    dir.mkdir(dirPath + "/tables");
  if (!dir.fileExists(dirPath + "/reports"))
    dir.mkdir(dirPath + "/reports");
  if (!dir.fileExists(dirPath + "/translations"))
    dir.mkdir(dirPath + "/translations");

  var xmlMod = xmlModule(idMod);
  File.write(dirPath + "/" + idMod + ".mod", xmlMod.toString(2));

  var xpmMod = AQUtil.sqlSelect("flmodules", "icono",
                                "idmodulo='" + idMod + "'");
  File.write(dirPath + "/" + idMod + ".xpm", xpmMod);

  var qry = new AQSqlQuery;
  qry.setSelect("nombre,contenido");
  qry.setFrom("flfiles");
  qry.setWhere("idmodulo='" + idMod + "'");
  if (!qry.exec() || qry.size() == 0)
    return;

  while (qry.next()) {
    var name = qry.value(0);
    var content = qry.value(1);
    var type = name.right(name.length - name.lastIndexOf("."));

    if (content.isEmpty())
      continue;

    switch (type) {
      case ".xml":
        File.write(dirPath + "/" + name, content);
        break;
      case ".ui":
        File.write(dirPath + "/forms/" + name, content);
        break;
      case ".qs":
        File.write(dirPath + "/scripts/" + name, content);
        break;
      case ".qry":
        File.write(dirPath + "/queries/" + name, content);
        break;
      case ".mtd":
        File.write(dirPath + "/tables/" + name, content);
        break;
      case ".kut":
      case ".ar":
      case ".jrxml":
      case ".svg":
        File.write(dirPath + "/reports/" + name, content);
        break;
      case ".ts":
        File.write(dirPath + "/translations/" + name, content);
        break;
    }
  }
}

function importModules(warnBackup)
{
  if (warnBackup == undefined)
    warnBackup = true;
  if (warnBackup) {
    var txt = "";
    txt += sys.translate("Aseg�rese de tener una copia de seguridad de todos los datos\n");
    txt += sys.translate("y de que  no hay ningun otro  usuario conectado a la base de\n");
    txt += sys.translate("datos mientras se realiza la importaci�n.\n\n");
    txt += "\n\n";
    txt += sys.translate("�Desea continuar?");
    if (MessageBox.Yes != MessageBox.warning(txt, MessageBox.No, MessageBox.Yes))
      return;
  }

  var key = "scripts/sys/modLastDirModules_" + sys.nameBD();

  var dirAnt = AQUtil.readSettingEntry(key);
  var dirMods = FileDialog.getExistingDirectory(dirAnt ? dirAnt : false,
                                                sys.translate("Directorio de M�dulos"));
  if (!dirMods)
    return;
  dirMods = Dir.cleanDirPath(dirMods);
  dirMods = Dir.convertSeparators(dirMods);
  Dir.current = dirMods;
  
  var listFilesMod = selectModsDialog(AQUtil.findFiles([dirMods], "*.mod", false));
  AQUtil.createProgressDialog(sys.translate("Importando"), listFilesMod.length);
  AQUtil.setProgress(1);
  
  for (var i = 0; i < listFilesMod.length; ++i) {
    AQUtil.setLabelText(listFilesMod[i]);
    AQUtil.setProgress(i);
    
    if (!importModule(listFilesMod[i])) {
      errorMsgBox(sys.translate("Error al cargar el m�dulo:\n") + listFilesMod[i]);
      break;
    }
  }

  AQUtil.destroyProgressDialog();
  AQUtil.writeSettingEntry(key, dirMods);
  MessageBox.information(sys.translate("Importaci�n de m�dulos finalizada."),
                         MessageBox.Ok, MessageBox.NoButton,
                         MessageBox.NoButton, "AbanQ");
  sys.AQTimer.singleShot(0, sys.reinit);
}

function selectModsDialog(listFilesMod)
{
  var dialog = new Dialog;
  dialog.okButtonText = sys.translate("Aceptar");
  dialog.cancelButtonText = sys.translate("Cancelar");

  var bgroup = new GroupBox;
  bgroup.title = sys.translate("Seleccione m�dulos a importar");
  dialog.add(bgroup);

  var res = [];
  var cB = [];
  for (var i = 0; i < listFilesMod.length; ++i) {
    cB[i] = new CheckBox;
    bgroup.add(cB[i]);
    cB[i].text = listFilesMod[i];
    cB[i].checked = true;
  }

  var idx = 0;
  if (dialog.exec()) {
    for (var i = 0; i < listFilesMod.length; ++i)
      if (cB[i].checked == true) res[idx++] = listFilesMod[i];
  }

  return res;
}

function importModule(modPath)
{
  var fileMod = new File(modPath);
  var contentMod = "";
  try {
    fileMod.open(File.ReadOnly);
    contentMod = fileMod.read();
  } catch (e) {
    errorMsgBox(sys.translate("Error leyendo fichero.") + "\n" + e);
    return false;
  }

  var mod;
  var xmlMod = new QDomDocument;
  if (xmlMod.setContent(contentMod)) {
    var nodeMod = xmlMod.namedItem("MODULE");
    if (!nodeMod) {
      errorMsgBox(sys.translate("Error en la carga del fichero xml .mod"));
      return false;
    }

    var mod = {
      id:       nodeMod.namedItem("name").toElement().text(),
      alias:    trTagText(nodeMod.namedItem("alias").toElement().text()),
      area:     nodeMod.namedItem("area").toElement().text(),
      areaname: trTagText(nodeMod.namedItem("areaname").toElement().text()),
      version:  nodeMod.namedItem("version").toElement().text()
    }

    if (!registerArea(mod) || !registerModule(mod)) {
      errorMsgBox(sys.translate("Error registrando el m�dulo") + " " + mod.id);
      return false;
    }

    if (!importFiles(fileMod.path, "*.xml", mod.id))
      return false;
    if (!importFiles(fileMod.path, "*.ui", mod.id))
      return false;
    if (!importFiles(fileMod.path, "*.qs", mod.id))
      return false;
    if (!importFiles(fileMod.path, "*.qry", mod.id))
      return false;
    if (!importFiles(fileMod.path, "*.mtd", mod.id))
      return false;
    if (!importFiles(fileMod.path, "*.kut", mod.id))
      return false;
    if (!importFiles(fileMod.path, "*.ar", mod.id))
      return false;
    if (!importFiles(fileMod.path, "*.jrxml", mod.id))
      return false;
    if (!importFiles(fileMod.path, "*.svg", mod.id))
      return false;
    if (!importFiles(fileMod.path, "*.ts", mod.id))
      return false;
  } else {
    errorMsgBox(sys.translate("Error en la carga del fichero xml .mod"));
    return false;
  }

  return true;
}

function importFiles(dirPath, ext, idMod)
{
  var ok = true;
  var listFiles = AQUtil.findFiles([dirPath], ext, false);

  AQUtil.createProgressDialog(sys.translate("Importando"), listFiles.length);
  AQUtil.setProgress(1);
  
  for (var i = 0; i < listFiles.length; ++i) {
    AQUtil.setLabelText(listFiles[i]);
    AQUtil.setProgress(i);

    if (!importFile(listFiles[i], idMod)) {
      errorMsgBox(sys.translate("Error al cargar :\n") + listFiles[i]);
      ok = false;
      break;
    }
  }

  AQUtil.destroyProgressDialog();
  return ok;
}

function importFile(filePath, idMod)
{
  var file = new File(filePath);
  var content = "";
  try {
    file.open(File.ReadOnly);
    content = file.read();
  } catch (e) {
    errorMsgBox(sys.translate("Error leyendo fichero.") + "\n" + e);
    return false;
  }

  var ok = true;
  var name = file.name;
  if ((!AQUtil.isFLDefFile(content) &&
       !name.endsWith(".qs") &&
       !name.endsWith(".ar") &&
       !name.endsWith(".svg")) ||
      name.endsWith("untranslated.ts"))
    return ok;

  var cur = new AQSqlCursor("flfiles");
  cur.select("nombre = '" + name + "'");
  if (!cur.first()) {
    if (name.endsWith(".ar")) {
      if (!importReportAr(filePath, idMod, content))
        return true;
    }
    cur.setModeAccess(AQSql.Insert);
    cur.refreshBuffer();
    cur.setValueBuffer("nombre", name);
    cur.setValueBuffer("idmodulo", idMod);
    cur.setValueBuffer("sha", AQUtil.sha1(content));
    cur.setValueBuffer("contenido", content);
    ok = cur.commitBuffer();
  } else {
    cur.setModeAccess(AQSql.Edit);
    cur.refreshBuffer();
    if (cur.valueBuffer("sha") != AQUtil.sha1(content)) {
      var contenidoCopia = cur.valueBuffer("contenido");
      cur.setModeAccess(AQSql.Insert);
      cur.refreshBuffer();
      var d = new Date;
      cur.setValueBuffer("nombre", name + d.toString());
      cur.setValueBuffer("idmodulo", idMod);
      cur.setValueBuffer("contenido", contenidoCopia);
      cur.commitBuffer();
      cur.select("nombre = '" + name + "'");
      cur.first();
      cur.setModeAccess(AQSql.Edit);
      cur.refreshBuffer();
      cur.setValueBuffer("idmodulo", idMod);
      cur.setValueBuffer("sha", AQUtil.sha1(content));
      cur.setValueBuffer("contenido", content);
      ok = cur.commitBuffer();
      if (name.endsWith(".ar")) {
        if (!importReportAr(filePath, idMod, content))
          return true;
      }
    }
  }

  return ok;
}

function importReportAr(filePath, idMod, content)
{
  if (!sys.isLoadedModule("flar2kut"))
    return false;

  if (AQUtil.readSettingEntry("scripts/sys/conversionAr") != "true")
    return false;

  content = sys.toUnicode(content, "UTF-8");
  content = flar2kut.iface.pub_ar2kut(content);

  filePath = filePath.left(filePath.length - 3) + ".kut";
  if (content) {
    var localEnc = util.readSettingEntry("scripts/sys/conversionArENC");
    if (!localEnc)
      var localEnc = "ISO-8859-15";

    content = sys.fromUnicode(content, localEnc);
    try {
      File.write(filePath, content);
    } catch (e) {
      errorMsgBox(sys.translate("Error escribiendo fichero.") + "\n" + e);
      return false;
    }
    return importFile(filePath, idMod);
  }

  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

function setObjText(container, component, value)
{
  var c = testObj(container, component)
  if (!c) {
    return false;
  }
  // Temporal hasta que los FL... dispongan de className()
  var clase = "editor" in c ?
              "FLFieldDB" :
              c.className();
  switch (clase) {
    case "QPushButton":
    case "QToolButton":
    case "QLabel": {
      runObjMethod(container, component, "text", "\"" + value + "\"");
      break;
    }
    case "FLFieldDB": {
      runObjMethod(container, component, "setValue", value);
      break;
    }
    default: {
      return false;
    }
  }
  return true;
}

function disableObj(container, component)
{
  var c = testObj(container, component);
  if (!c) {
    return false;
  }
  // Temporal hasta que ls FL... dispongan de className()
  var clase = "editor" in c ?
              "FLFieldDB" :
              ("tableName" in c ? "FLTableDB" : c.className());
  switch (clase) {
    case "QPushButton":
    case "QToolButton": {
      runObjMethod(container, component, "setEnabled", false);
      break;
    }
    case "FLFieldDB": {
      runObjMethod(container, component, "setDisabled", true);
      break;
    }
    default : {
      return false;
    }
  }
  return true;
}

function filterObj(container, component, filter)
{
  var c = testObj(container, component)
  if (!c) {
    return false;
  }
  // Temporal hasta que ls FL... dispongan de className()
  var clase = "editor" in c ?
              "FLFieldDB" :
              ("tableName" in c ? "FLTableDB" : c.className());
  switch (clase) {
    case "FLTableDB":
    case "FLFieldDB": {
      runObjMethod(container, component, "setFilter", filter);
      break;
    }
    default: {
      return false;
    }
  }
  return true;
}

function testObj(container, component)
{
  if (!container || container == undefined) {
    return false;
  }
  var c = container.child(component);
  if (!c || c == undefined) {
    debug(component + " no exitse");
    return false;
  }
  return c;
}

function testAndRun(container, component, method, param)
{
  var c = testObj(container, component)
  if (!c) {
    return false;
  }
  if (!runObjMethod(container, component, method, param)) {
    return false;
  }
  return true;
}

function runObjMethod(container, component, method, param)
{
  var c = container.child(component);
  if (method in c) {
    var s = container.name + ".child(\"" + component + "\")." + method;
    debug(s);
    var m = eval(s);
    if (typeof m == "function") { // M�todo
      m(param);
    } else { // Propiedad
      eval(s + " = " + param);
    }
  } else {
    debug(method  + " no existe");
  }
  return true;
}

