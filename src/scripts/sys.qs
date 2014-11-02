var form = this;
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
   bajo  los  términos  de  la  Licencia  Pública General de GNU   en  su
   versión 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

function init() {
  if (isLoadedModule("flfactppal")) {
    var util: FLUtil = new FLUtil();
    var codEjercicio: String = flfactppal.iface.pub_ejercicioActual();
    var nombreEjercicio: String = util.sqlSelect("ejercicios", "nombre", "codejercicio='" + codEjercicio + "'");
    setCaptionMainWidget(nombreEjercicio);
    var settings = new AQSettings;
    var oldApi:Boolean = settings.readBoolEntry("application/oldApi");
 if (!oldApi)
 	{
    	var valor:String = util.readSettingEntry("ebcomportamiento/ebCallFunction");
	if (valor)
     		{
        	var funcion = new Function( valor );
    		try {
    			funcion(); //Ejecuta la función
  		    } catch(e) {
    				debug(e);
  			       }
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
        if (!curSerial.first())
          curSerial.setModeAccess(curSerial.Insert);
        else
          curSerial.setModeAccess(curSerial.Edit);
        curSerial.refreshBuffer();
        curSerial.setValueBuffer("sha", v);
        curSerial.commitBuffer();
      }
    } else {
      var curSerial = new FLSqlCursor("flserial");
      curSerial.select();
      if (!curSerial.first())
        curSerial.setModeAccess(curSerial.Insert);
      else
        curSerial.setModeAccess(curSerial.Edit);
      curSerial.refreshBuffer();
      curSerial.setValueBuffer("sha", curFiles.valueBuffer("sha"));
      curSerial.commitBuffer();
    }
  }

  return true;
}

function statusDbLocksDialog(locks)
{
  var util = new FLUtil;
  var diag = new Dialog;
  var txtEdit = new TextEdit;

  diag.caption = util.translate("scripts", "Bloqueos de la base de datos");
  diag.width = 500;

  var html = "<html><table border=\"1\">";

  if (locks != undefined && locks.length) {
    var i = 0;
    var j = 0;
    var item = "";
    var fields = locks[0].split("@");
    var closeInfo = false;
    var closeRecord = false;

    var headInfo = "<table border=\"1\"><tr>";
    for (i = 0; i < fields.length; ++i)
      headInfo += "<td><b>" + fields[i] + "</b></td>";
    headInfo += "</tr>";

    var headRecord = "<table border=\"1\"><tr><td><b>" + util.translate("scripts", "Registro bloqueado") + "</b></td></tr>";

    for (i = 1; i < locks.length; ++i) {
      item = locks[i];

      if (item.left(2) == "##") {
        if (closeInfo)
          html += "</table>";
        if (!closeRecord)
          html += headRecord;

        html += "<tr><td>" + item.right(item.length - 2) + "</td></tr>";

        closeRecord = true;
        closeInfo = false
      } else {
        if (closeRecord)
          html += "</table>";
        if (!closeInfo)
          html += headInfo;

        html += "<tr>";
        fields = item.split("@");
        for (j = 0; j < fields.length; ++j)
          html += "<td>" + fields[j] + "</td>";
        html += "</tr>";

        closeRecord = false;
        closeInfo = true
      }
    }
  }

              html += "</table></table></html>";

  txtEdit.text = html;
  diag.add(txtEdit);
  diag.exec();
}

function terminateChecksLocks(sqlCursor)
{
  if (sqlCursor != undefined)
    sqlCursor.checkRisksLocks(true);
}

function execQSA(fileQSA, args)
{
  var file = new File(fileQSA);
  try {
    file.open(File.ReadOnly);
  } catch (e) {
    debug(e);
    return;
  }
  var fn = new Function(file.read());
  fn(args);
}

class AQGlobalFunctions
{
  static var functions_ = [];
  static var mappers_ = [];
  static var count_ = 0;

  static function set(functionName, globalFunction)
  {
    functions_[functionName] = globalFunction;
  }

  static function get(functionName)
  {
    return functions_[functionName];
  }

  static function exec(functionName)
  {
    var fn = functions_[functionName];
    if (fn != undefined)
      fn();
  }

  static function mapConnect(obj, signal, functionName)
  {
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

class AQTimer
{
  static var timers_ = [];
  static var count_ = 0;

  static function singleShot(msec, timeoutFunction)
  {
    const c = count_ % 100;
    var callback = function() {
      killTimer(timers_[c]);
      timers_[c] = undefined;
      timeoutFunction();
      aqApp.startTimerIdle();
    }
    aqApp.stopTimerIdle();
    timers_[c] = startTimer(msec, callback);
    ++count_;
  }
}

function mvProjectXml()
{
  var docRet = new QDomDocument;

  var strXml = AQUtil.sqlSelect("flupdates", "modulesdef", "actual='true'");
  if (!strXml)
    return docRet;

  var doc = new QDomDocument;
  if (!doc.setContent(strXml))
    return docRet;

  strXml = "";
  var nodes = doc.childNodes();

  for (var i = 0; i < nodes.length(); ++i) {
    var it = nodes.item(i);
    if (it.isComment()) {
      var data = it.toComment().data();
      if (!data.isEmpty() && data.startsWith("<mvproject ")) {
        strXml = data;
        break;
      }
    }
  }

  if (strXml.isEmpty())
    return docRet;

  docRet.setContent(strXml);
  return docRet;
}

function mvProjectModules()
{
  var ret = [];
  var doc = mvProjectXml();

  var mods = doc.elementsByTagName("module");
  for (var i = 0; i < mods.length(); ++i) {
    var it = mods.item(i).toElement();

    var mod = {
      name:    it.attribute("name"),
      version: it.attribute("version")
    }

    if (mod.name.length == 0)
      continue;

    ret[mod.name] = mod;
  }

  return ret;
}

function mvProjectExtensions()
{
  var ret = [];
  var doc = mvProjectXml();

  var exts = doc.elementsByTagName("extension");
  for (var i = 0; i < exts.length(); ++i) {
    var it = exts.item(i).toElement();

    var ext = {
      name:    it.attribute("name"),
      version: it.attribute("version")
    }

    if (ext.name.length == 0)
      continue;

    ret[ext.name] = ext;
  }

  return ret;
}

function calculateShaGlobal()
{
  var v = "";
  var qry = new AQSqlQuery;
  qry.setSelect("sha");
  qry.setFrom("flfiles");
  if (qry.exec() && qry.first()) {
    v = AQUtil.sha1(qry.value(0).toString());
    while (qry.next())
      v = AQUtil.sha1(v + qry.value(0).toString());
  }
  return v;
}

function registerUpdate(input)
{
  if (!input)
    return;

  var unpacker = new AQUnpacker(input);
  var errors = unpacker.errorMessages();
  if (errors.length != 0) {
    var msg = sys.translate(
                "Hubo los siguientes errores al intentar cargar los módulos:"
              );
    msg += "\n";
    for (var i = 0; i < errors.length; ++i)
      msg += errors[i] + "\n";
    errorMsgBox(msg);
    return;
  }
  unpacker.jump(); //Espacio1
  unpacker.jump(); //Espacio2
  unpacker.jump(); //Espacio3

  var now = new Date;
  var file = new File(input);
  var fileName = file.name;
  var modulesDef = sys.toUnicode(unpacker.getText(), "utf8");
  var filesDef = sys.toUnicode(unpacker.getText(), "utf8");
  var shaGlobal = calculateShaGlobal();

  AQSql.update("flupdates", ["actual"], [false]);
  AQSql.insert("flupdates",
               ["fecha", "hora", "nombre", "modulesdef", "filesdef", "shaglobal"],
               [now, now.toString().right(8), fileName, modulesDef, filesDef, shaGlobal]);
}

function warnLocalChanges(changes)
{
  if (changes == undefined)
    changes = localChanges();

  if (changes.size == 0)
    return true;

  var diag = new QDialog;
  diag.caption = sys.translate("Detectados cambios locales");
  diag.modal = true;

  var txt = "";
  txt += sys.translate("¡¡ CUIDADO !! DETECTADOS CAMBIOS LOCALES\n\n");
  txt += sys.translate("Se han detectado cambios locales en los módulos desde\n");
  txt += sys.translate("la última actualización/instalación de un paquete de módulos.\n");
  txt += sys.translate("Si continua es posible que estos cambios sean sobreescritos por\n");
  txt += sys.translate("los cambios que incluye el paquete que quiere cargar.\n\n");
  txt += "\n\n";
  txt += sys.translate("Registro de cambios");

  var lay = new QVBoxLayout(diag);
  lay.margin = 6;
  lay.spacing = 6;

  var lbl = new QLabel(diag);
  lbl.text = txt;
  lbl.alignment = AQS.AlignTop | AQS.WordBreak;
  lay.addWidget(lbl);

  var ted = new QTextEdit(diag);
  ted.textFormat = TextEdit.LogText;
  ted.alignment = AQS.AlignHCenter | AQS.AlignVCenter;
  ted.append(reportChanges(changes));
  lay.addWidget(ted);

  var lbl2 = new QLabel(diag);
  lbl2.text = sys.translate("¿Que desea hacer?");
  lbl2.alignment = AQS.AlignTop | AQS.WordBreak;
  lay.addWidget(lbl2);

  var lay2 = new QHBoxLayout(lay);
  lay2.margin = 6;
  lay2.spacing = 6;

  var pbCancel = new QPushButton(diag);
  pbCancel.text = sys.translate("Cancelar");
  var pbAccept = new QPushButton(diag);
  pbAccept.text = sys.translate("continue");
  lay2.addWidget(pbCancel);
  lay2.addWidget(pbAccept);

  connect(pbAccept, "clicked()", diag, "accept()");
  connect(pbCancel, "clicked()", diag, "reject()");

  return diag.exec() == 0 ? false : true;
}

function reportChanges(changes)
{
  var ret = "";

  for (var key in changes) {
    if (key == "size")
      continue;
    var chg = changes[key].split('@');
    ret += "Nombre: " + chg[0] + "\n";
    ret += "Estado: " + chg[1] + "\n";
    ret += "ShaOldTxt: " + chg[2] + "\n";
    //ret += "ShaOldBin: " + chg[3] + "\n";
    ret += "ShaNewTxt: " + chg[4] + "\n";
    //ret += "ShaNewBin: " + chg[5] + "\n";
    ret += "###########################################\n";
  }

  return ret;
}

function localChanges()
{
  var ret = [];
  ret["size"] = 0;

  var strXmlUpt = AQUtil.sqlSelect("flupdates", "filesdef", "actual='true'");
  if (!strXmlUpt)
    return ret;

  var docUpt = new QDomDocument;
  if (!docUpt.setContent(strXmlUpt)) {
    errorMsgBox(sys.translate(
                  "Error XML al intentar cargar la definición de los ficheros."
                ));
    return ret;
  }

  var docBd = xmlFilesDefBd();

  ret = diffXmlFilesDef(docBd, docUpt);
  return ret;
}

function diffXmlFilesDef(xmlOld, xmlNew)
{
  var arrOld = filesDefToArray(xmlOld);
  var arrNew = filesDefToArray(xmlNew);
  var ret = [];
  var size = 0;

  for (var key in arrOld) {
    if (!(key in arrNew)) {
      var info = [
                   key,
                   "del",
                   arrOld[key].shatext,
                   arrOld[key].shabinary,
                   "",
                   ""
                 ]
                 ret[key] = info.join('@');
      ++size;
    }
  }

  for (var key in arrNew) {
    if (!(key in arrOld)) {
      var info = [
                   key,
                   "new",
                   "",
                   "",
                   arrNew[key].shatext,
                   arrNew[key].shabinary
                 ]
                 ret[key] = info.join('@');
      ++size;
    } else if (arrNew[key].shatext != arrOld[key].shatext || 
               arrNew[key].shabinary != arrOld[key].shabinary) {
      var info = [
                   key,
                   "mod",
                   arrOld[key].shatext,
                   arrOld[key].shabinary,
                   arrNew[key].shatext,
                   arrNew[key].shabinary
                 ]
                 ret[key] = info.join('@');
      ++size;
    }
  }

  ret["size"] = size;
  return ret;
}

function filesDefToArray(xml)
{
  var root = xml.firstChild();
  var files = root.childNodes();
  var ret = [];

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

    if (fil.id.length == 0)
      continue;

    ret[fil.id] = fil;
  }

  return ret;
}

function xmlFilesDefBd()
{
  var doc = new QDomDocument("files_def");
  var root = doc.createElement("files");

  doc.appendChild(root);

  var qry = new AQSqlQuery;
  qry.setSelect("idmodulo,nombre,contenido");
  qry.setFrom("flfiles");

  if (!qry.exec())
    return doc;

  var shaSum = "";
  var shaSumTxt = "";
  var shaSumBin = "";

  while (qry.next()) {
    var idMod = qry.value(0).toString();
    if (idMod == "sys")
      continue;

    var fName = qry.value(1).toString();
    var ba = new QByteArray;
    ba.string = sys.fromUnicode(qry.value(2).toString(), "iso-8859-15");
    var sha = ba.sha1();

    var nf = doc.createElement("file");
    root.appendChild(nf);

    var ne = doc.createElement("module");
    nf.appendChild(ne);
    var nt = doc.createTextNode(idMod);
    ne.appendChild(nt);

    ne = doc.createElement("name");
    nf.appendChild(ne);
    nt = doc.createTextNode(fName);
    ne.appendChild(nt);

    if (textPacking(fName)) {
      ne = doc.createElement("text");
      nf.appendChild(ne);
      nt = doc.createTextNode(fName);
      ne.appendChild(nt);

      ne = doc.createElement("shatext");
      nf.appendChild(ne);
      nt = doc.createTextNode(sha);
      ne.appendChild(nt);

      var ba = new QByteArray;
      ba.string = shaSum + sha;
      shaSum = ba.sha1();
      var ba = new QByteArray;
      ba.string = shaSumTxt + sha;
      shaSumTxt = ba.sha1();
    }
 try {
    if (binaryPacking(fName)) {
      ne = doc.createElement("binary");
      nf.appendChild(ne);
      nt = doc.createTextNode(fName + ".qso");
      ne.appendChild(nt);

      sha = AQS.sha1(qry.value(3));
      ne = doc.createElement("shabinary");
      nf.appendChild(ne);
      nt = doc.createTextNode(sha);
      ne.appendChild(nt);

      var ba = new QByteArray;
      ba.string = shaSum + sha;
      shaSum = ba.sha1();
      var ba = new QByteArray;
      ba.string = shaSumBin + sha;
      shaSumBin = ba.sha1();
    }
    } catch (e) {
    }
  }

  qry = new AQSqlQuery;
  qry.setSelect("idmodulo,icono");
  qry.setFrom("flmodules");

  if (qry.exec()) {
    while (qry.next()) {
      var idMod = qry.value(0).toString();
      if (idMod == "sys")
        continue;
      var fName = idMod + ".xpm";
      var ba = new QByteArray;
      ba.string = qry.value(1).toString();
      var sha = ba.sha1();

      var nf = doc.createElement("file");
      root.appendChild(nf);

      var ne = doc.createElement("module");
      nf.appendChild(ne);
      var nt = doc.createTextNode(idMod);
      ne.appendChild(nt);

      ne = doc.createElement("name");
      nf.appendChild(ne);
      nt = doc.createTextNode(fName);
      ne.appendChild(nt);

      if (textPacking(fName)) {
        ne = doc.createElement("text");
        nf.appendChild(ne);
        nt = doc.createTextNode(fName);
        ne.appendChild(nt);

        ne = doc.createElement("shatext");
        nf.appendChild(ne);
        nt = doc.createTextNode(sha);
        ne.appendChild(nt);

        var ba = new QByteArray;
        ba.string = shaSum + sha;
        shaSum = ba.sha1();
        var ba = new QByteArray;
        ba.string = shaSumTxt + sha;
        shaSumTxt = ba.sha1();
      }
    }
  }

  var ns = doc.createElement("shasum");
  ns.appendChild(doc.createTextNode(shaSum));
  root.appendChild(ns);

  ns = doc.createElement("shasumtxt");
  ns.appendChild(doc.createTextNode(shaSumTxt));
  root.appendChild(ns);

  ns = doc.createElement("shasumbin");
  ns.appendChild(doc.createTextNode(shaSumBin));
  root.appendChild(ns);

  return doc;
}

function textPacking(ext)
{
  return ext.endsWith(".ui") || 
         ext.endsWith(".qry") || 
         ext.endsWith(".kut") || 
         ext.endsWith(".jrxml") || 
         ext.endsWith(".ar") || 
         ext.endsWith(".mtd") || 
         ext.endsWith(".ts") || 
         ext.endsWith(".qs") || 
         ext.endsWith(".xml") || 
         ext.endsWith(".xpm") || 
         ext.endsWith(".svg");
}

function binaryPacking(ext)
{
  return ext.endsWith(".qs");
}

function loadModules(input, warnBackup)
{
  if (input == undefined) {
    var dir = new Dir(sys.installPrefix() + "/share/eneboo/packages");
    dir.setCurrent();
    input = FileDialog.getOpenFileName(
              "Eneboo Packages (*.eneboopkg)\nAbanQ Packages (*.abanq)",
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
  if (warnBackup && interactiveGUI()) {
    var txt = "";
    txt += sys.translate("Asegúrese de tener una copia de seguridad de todos los datos\n");
    txt += sys.translate("y de que  no hay ningun otro  usuario conectado a la base de\n");
    txt += sys.translate("datos mientras se realiza la carga.\n\n");
    txt += "\n\n";
    txt += sys.translate("¿Desea continuar?");
    if (MessageBox.Yes != MessageBox.warning(txt, MessageBox.No, MessageBox.Yes))
      return;
  }

  if (input) {
    var ok = true;

    var changes = localChanges();
    if (changes.size != 0) {
      if (!warnLocalChanges(changes))
        return;
    }

    if (ok) {
      var unpacker = new AQUnpacker(input);
      var errors = unpacker.errorMessages();
      if (errors.length != 0) {
        var msg = sys.translate(
                    "Hubo los siguientes errores al intentar cargar los módulos:"
                  );
        msg += "\n";
        for (var i = 0; i < errors.length; ++i)
          msg += errors[i] + "\n";
        errorMsgBox(msg);
        ok = false;
      }
      //debug("Versión " + unpacker.getVersion()); //devuelve el fabricante del paquete
          //Por ahora los paquetes a cargar usan la misma estructura
                   unpacker.jump(); //Espacio1
                   unpacker.jump(); //Espacio2
                   unpacker.jump(); //Espacio3

 
      if (ok)
        ok = loadModulesDef(unpacker);

      if (ok)
        ok = loadFilesDef(unpacker);
    }

    if (!ok) {
      errorMsgBox(sys.translate(
                    "No se ha podido realizar la carga de los módulos."
                  ));
    } else {
      registerUpdate(input);
      infoMsgBox(sys.translate("La carga de módulos se ha realizado con éxito."));
      sys.AQTimer.singleShot(0, sys.reinit);
      var tmpVar = new FLVar;
      tmpVar.set("mrproper", "dirty");
    }
  }
}

function loadFilesDef(un)
{
  var filesDef = sys.toUnicode(un.getText(), "utf8");
  var doc = new QDomDocument;

  if (!doc.setContent(filesDef)) {
    errorMsgBox(sys.translate(
                  "Error XML al intentar cargar la definición de los ficheros."
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
      skip:       it.namedItem("skip").toElement().text(),
      module:     it.namedItem("module").toElement().text(),
      text:       it.namedItem("text").toElement().text(),
      shatext:    it.namedItem("shatext").toElement().text(),
      binary:     it.namedItem("binary").toElement().text(),
      shabinary:  it.namedItem("shabinary").toElement().text()
    }

    AQUtil.setProgress(i);
    AQUtil.setLabelText(sys.translate("Registrando fichero") + " " + fil.id);

    if (fil.id.length == 0 || fil.skip == "true")
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
var Dump;
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
  if (fil.text.length > 0) {
    if (fil.id.endsWith(".qs"))
      cur.setValueBuffer("contenido", sys.toUnicode(un.getText(), "ISO8859-15"));
    else
      cur.setValueBuffer("contenido", un.getText());
  }
  if (fil.binary.length > 0) {
    un.getBinary(); // Hay que solicitarlo para que cuente el espacio.
    //if (fil.id.endsWith(".qs"))
    //  AQUtil.writeDBSettingEntry(fil.id.left(30), fil.shatext);
  }
  return cur.commitBuffer();
}

function checkProjectName(proName)
{
  if (!proName || proName == undefined)
    proName = "";

  var dbProName = AQUtil.readDBSettingEntry("projectname");
  if (!dbProName)
    dbProName = "";

  if (proName == dbProName)
    return true;

  if (!proName.isEmpty() && dbProName.isEmpty())
    return AQUtil.writeDBSettingEntry("projectname", proName);

  var txt = "";
  txt += sys.translate("¡¡ CUIDADO !! POSIBLE INCOHERENCIA EN LOS MÓDULOS\n\n");
  txt += sys.translate("Está intentando cargar un proyecto o rama de módulos cuyo\n");
  txt += sys.translate("nombre difiere del instalado actualmente en la base de datos.\n");
  txt += sys.translate("Es posible que la estructura de los módulos que quiere cargar\n");
  txt += sys.translate("sea completamente distinta a la instalada actualmente, y si continua\n");
  txt += sys.translate("podría dañar el código, datos y la estructura de tablas de Eneboo.\n\n");
  txt += sys.translate("- Nombre del proyecto instalado: %1\n").arg(dbProName);
  txt += sys.translate("- Nombre del proyecto a cargar: %1\n\n").arg(proName);
  txt += "\n\n";

  if (!interactiveGUI()) {
    debug(txt);
    return false;
  }

  txt += sys.translate("¿Desea continuar?");
  return (MessageBox.Yes == MessageBox.warning(txt, MessageBox.No, MessageBox.Yes,
                                               MessageBox.NoButton, "AbanQ"));
}

function loadModulesDef(un)
{
  var modulesDef = sys.toUnicode(un.getText(), "utf8");
  var doc = new QDomDocument;

  if (!doc.setContent(modulesDef)) {
    errorMsgBox(sys.translate(
                  "Error XML al intentar cargar la definición de los módulos."
                ));
    return false;
  }

  var root = doc.firstChild();

  if (!checkProjectName(root.toElement().attribute("projectname", "")))
    return false;

  var ok = true;
  var modules = root.childNodes();

  AQUtil.createProgressDialog(sys.translate("Registrando módulos"), modules.length());

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
    AQUtil.setLabelText(sys.translate("Registrando módulo") + " " + mod.id);

    if (!registerArea(mod) || !registerModule(mod)) {
      errorMsgBox(sys.translate(
                    "Error registrando el módulo"
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

function infoMsgBox(msg)
{
  if ((typeof msg) != "string")
    return;
  msg += "\n";
  if (interactiveGUI()) {
    MessageBox.information(msg, MessageBox.Ok, MessageBox.NoButton,
                           MessageBox.NoButton, "AbanQ");
  } else {
    debug("INFO: " + msg);
  }
}

function warnMsgBox(msg)
{
  if ((typeof msg) != "string")
    return;
  msg += "\n";
  if (interactiveGUI()) {
    MessageBox.warning(msg, MessageBox.Ok, MessageBox.NoButton,
                       MessageBox.NoButton, "AbanQ");
  } else {
    debug("WARN: " + msg);
  }
}

function errorMsgBox(msg)
{
  if ((typeof msg) != "string")
    return;
  msg += "\n";
  if (interactiveGUI()) {
    MessageBox.critical(msg, MessageBox.Ok, MessageBox.NoButton,
                      MessageBox.NoButton, "Eneboo");
  } else {
    debug("ERROR: " + msg);
  }
}

function infoPopup(msg)
{
  if ((typeof msg) != "string")
    return;
  var caption = sys.translate("AbanQ Información");
  var regExp = new RegExp("\n");
  regExp.global = true;
  var msgHtml = "<img source=\"about.png\" align=\"right\">" +
                "<b><u>" + caption + "</u></b><br><br>" +
                msg.replace(regExp, "<br>") + "<br>";
  sys.popupWarn(msgHtml, []);
}

function warnPopup(msg)
{
  if ((typeof msg) != "string")
    return;
  var caption = sys.translate("AbanQ Aviso");
  var regExp = new RegExp("\n");
  regExp.global = true;
  var msgHtml = "<img source=\"bug.png\" align=\"right\">" +
                "<b><u>" + caption + "</u></b><br><br>" +
                msg.replace(regExp, "<br>") + "<br>";
  sys.popupWarn(msgHtml, []);
}

function errorPopup(msg)
{
  if ((typeof msg) != "string")
    return;
  var caption = sys.translate("AbanQ Error");
  var regExp = new RegExp("\n");
  regExp.global = true;
  var msgHtml = "<img source=\"remove.png\" align=\"right\">" +
                "<b><u>" + caption + "</u></b><br><br>" +
                msg.replace(regExp, "<br>") + "<br>";
  sys.popupWarn(msgHtml, []);
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

function questionMsgBox(msg, keyRemember, txtRemember, forceShow,
                        txtCaption, txtYes, txtNo)
{
  var settings = new AQSettings;
  var key = "QuestionMsgBox/";
  var valRemember = false;

  if (keyRemember) {
    valRemember = settings.readBoolEntry(key + keyRemember);
    if (valRemember && !forceShow)
      return MessageBox.Yes;
  }

  if (!interactiveGUI())
    return true;

  var diag = new QDialog;
  diag.caption = txtCaption ? txtCaption : "Eneboo";
  diag.modal = true;

  var lay = new QVBoxLayout(diag);
  lay.margin = 6;
  lay.spacing = 6;

  var lay2 = new QHBoxLayout(lay);
  lay2.margin = 6;
  lay2.spacing = 6;

  var lblPix = new QLabel(diag);
  lblPix.pixmap = AQS.Pixmap_fromMimeSource("help_index.png");
  lblPix.alignment = AQS.AlignTop;
  lay2.addWidget(lblPix);

  var lbl = new QLabel(diag);
  lbl.text = msg;
  lbl.alignment = AQS.AlignTop | AQS.WordBreak;
  lay2.addWidget(lbl);

  var lay3 = new QHBoxLayout(lay);
  lay3.margin = 6;
  lay3.spacing = 6;

  var pbYes = new QPushButton(diag);
  pbYes.text = txtYes ? txtYes : sys.translate("Sí");
  var pbNo = new QPushButton(diag);
  pbNo.text = txtNo ? txtNo : sys.translate("No");
  lay3.addWidget(pbYes);
  lay3.addWidget(pbNo);

  connect(pbYes, "clicked()", diag, "accept()");
  connect(pbNo, "clicked()", diag, "reject()");

  var chkRemember;
  if (keyRemember && txtRemember) {
    chkRemember = new QCheckBox(txtRemember, diag);
    chkRemember.checked = valRemember;
    lay.addWidget(chkRemember);
  }

  var ret = (diag.exec() == 0) ? MessageBox.No : MessageBox.Yes;

  if (chkRemember != undefined)
    settings.writeEntry(key + keyRemember, chkRemember.checked);

  return ret;
}

function decryptFromBase64(str)
{
  var ba = new QByteArray;
  ba.string = str;
  return AQS.decryptInternal(AQS.fromBase64(ba)).toString();
}

class AbanQUpdater
{
  var w_;
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
    lay.margin = 0;
    lay.spacing = 0;

    this.prBar_ = new QProgressBar(this.w_);
    this.prBar_.setCenterIndicator(true);
    this.prBar_.setTotalSteps(100);
    lay.addWidget(this.prBar_);

    this.data_ = "";
    this.urlOp_ = new QUrlOperator(
      sys.decryptFromBase64("lKvF+hkDxk2dS6hrf0jVURL4EceyJIFPeigGw6lZAU/3ovk/v0iZfhklru4Q6t6M")
    );

    connect(this.urlOp_, "finished(QNetworkOperation*)",
            this, "transferFinished()");
    connect(this.urlOp_, "dataTransferProgress(int,int,QNetworkOperation*)",
            this, "transferProgress()");
    connect(this.urlOp_, "data(const QByteArray&,QNetworkOperation*)",
            this, "transferData()");

    this.urlOp_.get(
      sys.decryptFromBase64("wYZ6GifNhk4W+qnjzToiKooKL24mrW5bt0+RS6hQzW0=")
    );
  }

  function transferFinished(netOp)
  {
    this.state_ = netOp.state();
    this.w_.close();
    if (this.state_ == AQS.StFailed) {
      errorMsgBox(netOp.protocolDetail());
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
  MessageBox.warning(sys.translate("Funcionalidad no soportada aún en Eneboo."), MessageBox.Ok, MessageBox.NoButton,MessageBox.NoButton, "Eneboo");
  return;
  /*
  var msg = "";
  msg += sys.translate("Se va a proceder a conectar a través de Internet\n");
  msg += sys.translate("con los sistemas de InfoSiAL S.L. para obtener la\n");
  msg += sys.translate("herramienta de actualización de AbanQ.\n\n");
  msg += sys.translate("Esta es una nueva herramienta que le permitirá mantener\n");
  msg += sys.translate("actualizados, de forma cómoda y totalmente automática,\n");
  msg += sys.translate("los módulos y extensiones que tenga instalados. Además le\n");
  msg += sys.translate("mantendrá informado de las últimas mejoras que están o\n");
  msg += sys.translate("estarán disponibles en próximas versiones.\n\n");
  msg += sys.translate("¿ Desea continuar ?\n");

  var txtRem = "";
  txtRem += sys.translate("No volver a mostrar este mensaje, permitir siempre\n");
  txtRem += sys.translate("conectar automáticamente con InfoSiAL");

  if (questionMsgBox(msg, "autoConnectInfoSiAL", txtRem) != MessageBox.Yes)
    return;

  var updater = new AbanQUpdater;
  updater.w_.exec();

  if (updater.state_ != AQS.StFailed) {
    var scriptInfos = [];
    var scrName = "abanqUpdaterScript";
    var baCode = new QByteArray;
    baCode.string = updater.data_;
    var baCode = baCode.fromBase64();
    var mng = aqApp.db().managerModules();
    var scrCode = mng.byteCodeToStr(baCode);
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
      QSProject.evaluateScripts(scriptInfos, "aqUpdaterMain24");
    } else
      aqUpdaterMain24();
  }
  */
}

function exportModules()
{
  var dirBasePath = FileDialog.getExistingDirectory(Dir.home);
  if (!dirBasePath)
    return;
  var dataBaseName = aqApp.db().database();
  dirBasePath = Dir.cleanDirPath(dirBasePath + "/modulos_exportados_" +
                                 dataBaseName.mid(dataBaseName.lastIndexOf("/") + 1));

  var dir = new Dir;
  if (!dir.fileExists(dirBasePath)) {
    try {
      dir.mkdir(dirBasePath);
    } catch (e) {
      errorMsgBox("" + e);
      return;
    }
  } else {
    warnMsgBox(dirBasePath +
               sys.translate(" ya existe,\ndebe borrarlo antes de continuar"));
    return;
  }

  var qry = new AQSqlQuery;
  qry.setSelect("idmodulo");
  qry.setFrom("flmodules");
  if (!qry.exec() || qry.size() == 0)
    return;

  var p = 0;
  AQUtil.createProgressDialog(sys.translate("Exportando módulos"), qry.size() - 1);

  while (qry.next()) {
    var idMod = qry.value(0);
    if (idMod == "sys")
      continue;

    AQUtil.setLabelText(String("%1").arg(idMod));
    AQUtil.setProgress(++p);

    try {
      exportModule(idMod, dirBasePath);
    } catch (e) {
      AQUtil.destroyProgressDialog();
      errorMsgBox("" + e);
      return;
    }
  }

  var dbProName = AQUtil.readDBSettingEntry("projectname");
  if (!dbProName)
    dbProName = "";
  if (!dbProName.isEmpty()) {
    var doc = new QDomDocument;
    var tag = doc.createElement("mvproject");
    tag.toElement().setAttribute("name", dbProName);
    doc.appendChild(tag);
    try {
      File.write(dirBasePath + "/mvproject.xml", doc.toString(2));
    } catch (e) {
      AQUtil.destroyProgressDialog();
      errorMsgBoxl("" + e);
      return;
    }
  }

  AQUtil.destroyProgressDialog();
  infoMsgBox(sys.translate("Módulos exportados en:\n") + dirBasePath);
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

function fileWriteIso(fileName, content)
{
  var fileISO = new QFile(fileName);
  if (!fileISO.open(File.WriteOnly)) {
    debug("Error abriendo fichero " + fileName + " para escritura");
    return false;
  }
  var tsISO = new QTextStream(fileISO.ioDevice());
  tsISO.setCodec(AQS.TextCodec_codecForName("ISO8859-15"));
  tsISO.opIn(content);
  fileISO.close();
}

function fileWriteUtf8(fileName, content)
{
  var fileUTF = new QFile(fileName);
  if (!fileUTF.open(File.WriteOnly)) {
    debug("Error abriendo fichero " + fileName + " para escritura");
    return false;
  }
  var tsUTF = new QTextStream(fileUTF.ioDevice());
  tsUTF.setCodec(AQS.TextCodec_codecForName("utf8"));
  tsUTF.opIn(content);
  fileUTF.close();
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
  sys.fileWriteIso(dirPath + "/" + idMod + ".mod", xmlMod.toString(2));

  var xpmMod = AQUtil.sqlSelect("flmodules", "icono",
                                "idmodulo='" + idMod + "'");
  sys.fileWriteIso(dirPath + "/" + idMod + ".xpm", xpmMod);

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
        sys.fileWriteIso(dirPath + "/" + name, content);
        break;
      case ".ui":
        sys.fileWriteIso(dirPath + "/forms/" + name, content);
        break;
      case ".qs":
        sys.fileWriteIso(dirPath + "/scripts/" + name, content);
        break;
      case ".qry":
        sys.fileWriteIso(dirPath + "/queries/" + name, content);
        break;
      case ".mtd":
        sys.fileWriteIso(dirPath + "/tables/" + name, content);
        break;
      case ".kut":
      case ".ar":
      case ".jrxml":
      case ".svg":
        sys.fileWriteIso(dirPath + "/reports/" + name, content);
        break;
      case ".ts":
        sys.fileWriteIso(dirPath + "/translations/" + name, content);
        break;
    }
  }
}

function importModules(warnBackup)
{
  if (warnBackup == undefined)
    warnBackup = true;
  if (warnBackup && interactiveGUI()) {
    var txt = "";
    txt += sys.translate("Asegúrese de tener una copia de seguridad de todos los datos\n");
    txt += sys.translate("y de que  no hay ningun otro  usuario conectado a la base de\n");
    txt += sys.translate("datos mientras se realiza la importación.\n\n");
    txt += sys.translate("Obtenga soporte en");
    txt += " http://www.infosial.com\n(c) InfoSiAL S.L.";
    txt += "\n\n";
    txt += sys.translate("¿Desea continuar?");
    if (MessageBox.Yes != MessageBox.warning(txt, MessageBox.No, MessageBox.Yes))
      return;
  }

  var key = "scripts/sys/modLastDirModules_" + sys.nameBD();

  var dirAnt = AQUtil.readSettingEntry(key);
  var dirMods = FileDialog.getExistingDirectory(dirAnt ? dirAnt : false,
                                                sys.translate("Directorio de Módulos"));
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
      errorMsgBox(sys.translate("Error al cargar el módulo:\n") + listFilesMod[i]);
      break;
    }
  }

  AQUtil.destroyProgressDialog();
  AQUtil.writeSettingEntry(key, dirMods);
  infoMsgBox(sys.translate("Importación de módulos finalizada."));
  sys.AQTimer.singleShot(0, sys.reinit);
}

function selectModsDialog(listFilesMod)
{
  var dialog = new Dialog;
  dialog.okButtonText = sys.translate("Aceptar");
  dialog.cancelButtonText = sys.translate("Cancelar");

  var bgroup = new GroupBox;
  bgroup.title = sys.translate("Seleccione módulos a importar");
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
      errorMsgBox(sys.translate("Error registrando el módulo") + " " + mod.id);
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
    var ba = new QByteArray;
    ba.string = content;
    cur.setValueBuffer("sha", ba.sha1());
    cur.setValueBuffer("contenido", content);
    ok = cur.commitBuffer();
  } else {
    cur.setModeAccess(AQSql.Edit);
    cur.refreshBuffer();
    var ba = new QByteArray;
    ba.string = content;
    var shaCnt = ba.sha1();
    if (cur.valueBuffer("sha") != shaCnt) {
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
      cur.setValueBuffer("sha", shaCnt);
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

class AbanQDbDumper
{
  const SEP_CSV = '\u00b6';

  var db_;
  var showGui_;
  var dirBase_;
  var fileName_;
  var w_;
  var lblDirBase_;
  var pbChangeDir_;
  var tedLog_;
  var pbInitDump_;
  var state_;
  var funLog_;
  var proc_;

  function AbanQDbDumper(db, dirBase, showGui, funLog)
  {
    this.db_ = (db == undefined ? aqApp.db() : db);
    this.showGui_ = (showGui == undefined) ? true : showGui;
    this.dirBase_ = (dirBase == undefined) ? Dir.home : dirBase;
    this.funLog_ = (funLog == undefined) ? this.addLog : funLog;
    this.fileName_ = this.genFileName();
  }

  function init()
  {
    if (this.showGui_) {
      this.buildGui();
      this.w_.exec();
    }
  }

  function buildGui()
  {
    this.w_ = new QDialog;
    this.w_.caption = sys.translate("Copias de seguridad");
    this.w_.modal = true;
    this.w_.resize(800, 600);

    var lay = new QVBoxLayout(this.w_, 6, 6);

    var frm = new QFrame(this.w_);
    frm.frameShape = AQS.Box;
    frm.lineWidth = 1;
    frm.frameShadow = AQS.Plain;
    var layFrm =  new QVBoxLayout(frm, 6, 6);

    var lbl = new QLabel(frm);
    lbl.text = sys.translate("Driver: %1")
               .arg(this.db_.driverNameToDriverAlias(this.db_.driverName()));
    lbl.alignment = AQS.AlignTop;
    layFrm.addWidget(lbl);

    lbl = new QLabel(frm);
    lbl.text = sys.translate("Base de datos: %1")
               .arg(this.db_.database());
    lbl.alignment = AQS.AlignTop;
    layFrm.addWidget(lbl);

    lbl = new QLabel(frm);
    lbl.text = sys.translate("Host: %1")
               .arg(this.db_.host());
    lbl.alignment = AQS.AlignTop;
    layFrm.addWidget(lbl);

    lbl = new QLabel(frm);
    lbl.text = sys.translate("Puerto: %1")
               .arg(this.db_.port());
    lbl.alignment = AQS.AlignTop;
    layFrm.addWidget(lbl);

    lbl = new QLabel(frm);
    lbl.text = sys.translate("Usuario: %1")
               .arg(this.db_.user());
    lbl.alignment = AQS.AlignTop;
    layFrm.addWidget(lbl);

    var layAux = new QHBoxLayout(layFrm);
    this.lblDirBase_ = new QLabel(frm);
    this.lblDirBase_.text = sys.translate("Directorio Destino: %1")
                            .arg(this.dirBase_);
    this.lblDirBase_.alignment = AQS.AlignVCenter;
    layAux.addWidget(this.lblDirBase_);
    this.pbChangeDir_ = new QPushButton(sys.translate("Cambiar"), frm);
    this.pbChangeDir_.setSizePolicy(AQS.Maximum, AQS.Preferred);
    connect(this.pbChangeDir_, "clicked()", this, "changeDirBase()");
    layAux.addWidget(this.pbChangeDir_);

    lay.addWidget(frm);

    this.pbInitDump_ = new QPushButton(sys.translate("INICIAR COPIA"), this.w_);
    connect(this.pbInitDump_, "clicked()", this, "initDump()");
    lay.addWidget(this.pbInitDump_);

    lbl = new QLabel(this.w_);
    lbl.text = "Log:";
    lay.addWidget(lbl);

    this.tedLog_ = new QTextEdit(this.w_);
    this.tedLog_.textFormat = TextEdit.LogText;
    this.tedLog_.alignment = AQS.AlignHCenter | AQS.AlignVCenter;
    lay.addWidget(this.tedLog_);
  }

  function initDump()
  {
    var gui = this.showGui_ && this.w_ != undefined;
    if (gui)
      this.w_.enabled = false;
    this.dumpDatabase();
    if (gui)
      this.w_.enabled = true;
    if (this.state_.ok) {
      if (gui) {
        infoMsgBox(this.state_.msg);
      }
      this.w_.close();
    } else if (gui)
      sys.errorMsgBox(this.state_.msg);
  }

  function genFileName()
  {
    var now = new Date;
    var timeStamp = now.toString();
    var regExp = new RegExp("[-|:]");
    regExp.global = true;
    timeStamp = timeStamp.replace(regExp, "");
    var fileName = this.dirBase_ + "/dump_" +
                   this.db_.database() + "_" +
                   timeStamp;
    fileName = Dir.cleanDirPath(fileName);
    fileName = Dir.convertSeparators(fileName);
    return fileName;
  }

  function changeDirBase(dir)
  {
    var dirBasePath = dir;
    if (!dirBasePath) {
      dirBasePath = FileDialog.getExistingDirectory(this.dirBase_);
      if (!dirBasePath)
        return;
    }
    this.dirBase_ = dirBasePath;
    if (this.showGui_ && this.lblDirBase_ != undefined)
      this.lblDirBase_.text = sys.translate("Directorio Destino: %1")
                              .arg(this.dirBase_);
    this.fileName_ = this.genFileName();
  }

  function addLog(msg)
  {
    if (this.showGui_ && this.tedLog_ != undefined)
      this.tedLog_.append(msg);
    else
      debug(msg);
  }

  function setState(ok, msg)
  {
    this.state_ = {
      ok: ok,
      msg: msg
    };
  }

  function state()
  {
    return this.state_;
  }

  function launchProc(command)
  {
    this.proc_ = new QProcess;
    this.proc_.setArguments(command);

    connect(this.proc_, "readyReadStdout()", this, "readFromStdout()");
    connect(this.proc_, "readyReadStderr()", this, "readFromStderr()");

    var ok = this.proc_.start();
    while (ok && this.proc_.isRunning())
      sys.processEvents();
    return ok;
  }

  function readFromStdout()
  {
    this.funLog_(this.proc_.readStdout().toString());
  }

  function readFromStderr()
  {
    this.funLog_(this.proc_.readStderr().toString());
  }

  function dumpDatabase()
  {
    var driver = this.db_.driverName();
    var typeBd = 0;

    if (driver.startsWith("FLQPSQL"))
      typeBd = 1;
    else if (driver.startsWith("FLQMYSQL"))
      typeBd = 2;

    if (typeBd == 0) {
      this.setState(
        false,
        sys.translate("Este tipo de base de datos no soporta el volcado a disco.")
      );
      this.funLog_(this.state_.msg);
      this.dumpAllTablesToCsv();
      return false;
    }

    var file = new File(this.fileName_);
    try {
      file.open(File.WriteOnly);
      file.close();
      file.remove();
      var dir = new Dir(this.fileName_);
      dir.mkdir();
      dir.rmdir();
    } catch (e) {
      this.setState(false, "" + e);
      this.funLog_(this.state_.msg);
      return false;
    }

    var ok = true;
    switch (typeBd) {
      case 1:
        ok = this.dumpPostgreSQL();
        break;
      case 2:
        ok = this.dumpMySQL();
        break;
    }

    if (!ok)
      this.dumpAllTablesToCsv();

    if (!ok) {
      this.setState(
        false,
        sys.translate("No se ha podido realizar la copia de seguridad.")
      );
      this.funLog_(this.state_.msg);
    } else {
      this.setState(
        true,
        sys.translate("Copia de seguridad realizada con éxito en:\n%1").arg(this.fileName_)
      );
      this.funLog_(this.state_.msg);
    }

    return ok;
  }

  function dumpPostgreSQL()
  {
    var pgDump = "pg_dump";
    var command;
    var fileName = this.fileName_ + ".sql";
    var db = this.db_;

    if (sys.osName() == "WIN32") {
      pgDump += ".exe";
      System.setenv("PGPASSWORD", db.password());
      command = [pgDump,
                 "-f", fileName,
                 "-h", db.host(),
                 "-p", db.port(),
                 "-U", db.user(),
                 db.database()];
    } else {
      System.setenv("PGPASSWORD", db.password());
      command = [pgDump, "-v",
                 "-f", fileName,
                 "-h", db.host(),
                 "-p", db.port(),
                 "-U", db.user(),
                 db.database()];
    }

    if (!this.launchProc(command)) {
      this.setState(
        false,
        sys.translate("No se ha podido volcar la base de datos a disco.\n") +
        sys.translate("Es posible que no tenga instalada la herramienta ") + pgDump
      );
      this.funLog_(this.state_.msg);
      return false;
    }
    this.setState(true, "");
    return true;
  }

  function dumpMySQL()
  {
    var myDump = "mysqldump";
    var command;
    var fileName = this.fileName_ + ".sql";
    var db = this.db_;

    if (sys.osName() == "WIN32") {
      myDump += ".exe";
      command = [myDump, "-v",
                 "--result-file=" + fileName,
                 "--host=" + db.host(),
                 "--port=" + db.port(),
                 "--password=" + db.password(),
                 "--user=" + db.user(),
                 db.database()];
    } else {
      command = [myDump, "-v",
                 "--result-file=" + fileName,
                 "--host=" + db.host(),
                 "--port=" + db.port(),
                 "--password=" + db.password(),
                 "--user=" + db.user(),
                 db.database()];
    }

    if (!this.launchProc(command)) {
      this.setState(
        false,
        sys.translate("No se ha podido volcar la base de datos a disco.\n") +
        sys.translate("Es posible que no tenga instalada la herramienta ") + myDump
      );
      this.funLog_(this.state_.msg);
      return false;
    }
    this.setState(true, "");
    return true;
  }

  function dumpTableToCsv(table, dirBase)
  {
    var fileName = dirBase + table + ".csv";
    var file = new QFile(fileName);
    if (!file.open(File.WriteOnly))
      return false;

    var ts = new QTextStream(file.ioDevice());
    ts.setCodec(AQS.TextCodec_codecForName("utf8"));

    var qry = new AQSqlQuery;
    qry.setSelect(table + ".*");
    qry.setFrom(table);
    if (!qry.exec())
      return false;

    var rec = "";
    var fieldNames = qry.fieldList();

    for (var i = 0; i < fieldNames.length; ++i) {
      if (i > 0)
        rec += this.SEP_CSV;
      rec += fieldNames[i];
    }
    ts.opIn(rec + "\n");

    AQUtil.createProgressDialog(sys.translate("Haciendo copia en CSV de ") + table, qry.size());
    var p = 0;

    while (qry.next()) {
      rec = "";
      for (var i = 0; i < fieldNames.length; ++i) {
        if (i > 0)
          rec += this.SEP_CSV;
        rec += qry.value(i).toString();
      }
      ts.opIn(rec + "\n");
      AQUtil.setProgress(++p);
    }

    file.close();
    AQUtil.destroyProgressDialog();
    return true;
  }

  function dumpAllTablesToCsv()
  {
    var fileName = this.fileName_;
    var db = this.db_;
    var tables = db.tables(AQSql.Tables);

    var dir = new Dir(fileName);
    dir.mkdir();
    var dirBase = Dir.convertSeparators(fileName + "/");

    for (var i = 0; i < tables.length; ++i)
      this.dumpTableToCsv(tables[i], dirBase);
    return true;
  }
}

function dumpDatabase()
{
  var aqDumper = new AbanQDbDumper;
  aqDumper.init();
}

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

function enableObj(container, component)
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
      runObjMethod(container, component, "setEnabled", true);
      break;
    }
    case "FLFieldDB": {
      runObjMethod(container, component, "setDisabled", false);
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
    debug(component + " no existe");
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
    var m = eval(s);
    if (typeof m == "function") { // Método
      m(param);
    } else { // Propiedad
      eval(s + " = " + param);
    }
  } else {
    debug(method  + " no existe");
  }
  return true;
}

/// Realiza una conexión comprobando antes si el emisor existe
function connectSS(ssSender, ssSignal, ssReceiver, ssSlot)
{
  if (!ssSender) {
    return false;
  }
  connect(ssSender, ssSignal, ssReceiver, ssSlot);
  return true;
}
/** Encapsula una función en una transacción.
Ejemplo de uso:
    var oParam = new Object;
    oParam.curImport = cursor;
    oParam.errorMsg = util.translate("scripts", "Error al crear el asiento");
    var f = new Function("oParam", "return formRecorda3_importaciones.iface.crearAsiento(oParam)");
    if (!sys.runTransaction(f, oParam)) {
      return false;
    }
*/
function runTransaction(f, oParam)
{
  var curT = new FLSqlCursor("flfiles");
  curT.transaction(false);
  var valor, errorMsg;
  var gui = interactiveGUI();
  if (gui) {
    try {
      AQS.Application_setOverrideCursor(AQS.WaitCursor);
    } catch (e) {}
  }
  try {
    valor = f(oParam);
    errorMsg = "errorMsg" in oParam ? oParam.errorMsg : false;
    if (valor) {
      curT.commit();
    } else {
      curT.rollback();
      if (gui) {
        try {
          AQS.Application_restoreOverrideCursor();
        } catch (e) {}
      }
      if (errorMsg) {
        warnMsgBox(errorMsg);
      } else {
        warnMsgBox(translate("Error al ejecutar la función"));
      }
      return false;
    }
  } catch (e) {
    curT.rollback();
    if (gui) {
      try {
        AQS.Application_restoreOverrideCursor();
      } catch (e) {}
    }
    if (errorMsg) {
      warnMsgBox(errorMsg  + ": " + e.toString());
    } else {
      warnMsgBox(translate("Error ejecutando la función") + ":\n" + e);
    }
    return false;
  }
  if (gui) {
    try {
      AQS.Application_restoreOverrideCursor();
    } catch (e) {}
  }
  return valor;
}

function openUrl(url)
{
  if (!url || (typeof url) != "string" || url.isEmpty())
    return false;

  switch (sys.osName()) {
    case "LINUX": {
      if (launchCommand(["xdg-open", url]))
        return true;
      if (launchCommand(["gnome-open", url]))
        return true;
      if (launchCommand(["kfmclient openURL", url]))
        return true;
      if (launchCommand(["kfmclient exec", url]))
        return true;
      if (launchCommand(["firefox", url]))
        return true;
      if (launchCommand(["mozilla", url]))
        return true;
      if (launchCommand(["opera", url]))
        return true;
      if (launchCommand(["google-chrome", url]))
        return true;
    }
    break;

    case "WIN32": {
      if (url.startsWith("mailto")) {
        var rxp = new RegExp("&");
        rxp.global = true;
        url = url.replace(rxp, "^&");
      }
      return launchCommand(["cmd.exe", "/C", "start", "", url]);
    }
    break;

    case "MACX": {
      return launchCommand(["open", url]);
    }
    break;
  }

  return false;
}

function launchCommand(command)
{
  if (!command || (typeof command) != "object" || command.length == 0)
    return false;

  try {
    Process.execute(command);
    return true;
  } catch (e) {
    return false;
  }
}

function isUserBuild()
{
  return sys.version().upper().indexOf("USER") != -1;
}

function isDeveloperBuild()
{
  return sys.version().upper().indexOf("DEVELOPER") != -1;
}

function interactiveGUI()
{
  return aqApp.db().interactiveGUI();
}

function qsaExceptions()
{
  return aqApp.db().qsaExceptions();
}

function serverTime()
{
  var db = aqApp.db().db();
  // o así
  //var db = AQSql.database().db();
  var sql = "select current_time";
  var ahora;
  var q = new QSqlSelectCursor(sql, db);
  if (q.isActive() && q.next()) {
    ahora = q.value(0);
  }
  return ahora;
}
