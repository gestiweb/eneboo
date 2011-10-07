/***************************************************************************
                                flloadmod.qs
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

var util = new FLUtil();

function main() {
  var continuar = MessageBox.warning(util.translate("scripts", "Antes de cargar un módulo asegúrese de tener una copia de seguridad de todos los datos,\ny de que no hay ningun otro usuario conectado a la base de datos mientras se realiza la carga.\n\n¿Desea continuar?"), MessageBox.Yes, MessageBox.No);
  if (continuar == MessageBox.No) return;

  var nombreFichero = FileDialog.getOpenFileName("modfiles(*.mod)", util.translate("scripts", "Elegir Fichero"));
  if (nombreFichero) {
    var fichero = new File(nombreFichero);
    if (!formRecordflmodules.aceptarLicenciaDelModulo(fichero.path + "/")) {
      MessageBox.critical(util.translate("scripts", "Imposible cargar el módulo.\nLicencia del módulo no aceptada."), MessageBox.Ok);
      return;
    }
    var modulo;
    var descripcion;
    var area;
    var desArea;
    var version;
    var nombreIcono;
    var versionMinimaFL;
    var dependencias = [];
    fichero.open(File.ReadOnly);
    var f = fichero.read();
    var xmlModule = new FLDomDocument();
    if (xmlModule.setContent(f)) {

      var nodeModule = xmlModule.namedItem("MODULE");
      if (!nodeModule) {
        MessageBox.critical(util.translate("scripts", "Error en la carga del fichero xml .mod"), MessageBox.Ok, MessageBox.NoButton);
      }
      modulo = nodeModule.namedItem("name").toElement().text();
      descripcion = nodeModule.namedItem("alias").toElement().text();
      area = nodeModule.namedItem("area").toElement().text();
      desArea = nodeModule.namedItem("areaname").toElement().text();
      version = nodeModule.namedItem("version").toElement().text();
      nombreIcono = nodeModule.namedItem("icon").toElement().text();

      if (nodeModule.namedItem("flversion")) versionMinimaFL = nodeModule.namedItem("flversion").toElement().text();
      if (nodeModule.namedItem("dependencies")) {
        var nodeDepend = xmlModule.elementsByTagName("dependency");
        for (var i = 0; i < nodeDepend.length(); i++) {
          dependencias[i] = nodeDepend.item(i).toElement().text();
        }
      }

    } else {

      var aF = f.split("\n");
      modulo = dameValor(aF[0]);
      descripcion = dameValor(aF[1]);
      area = dameValor(aF[2]);
      desArea = dameValor(aF[3]);
      version = dameValor(aF[4]);
      nombreIcono = dameValor(aF[5]);
      if (aF.length > 6) versionMinimaFL = dameValor(aF[6]);
      if (aF.length > 7) dependencias = dameValor(aF[7]).split(/ [ , ; ] /);
    }

    descripcion = traducirCadena(descripcion, fichero.path, modulo);
    desArea = traducirCadena(desArea, fichero.path, modulo);

    var fichIcono = new File(fichero.path + "/" + nombreIcono);
    fichIcono.open(File.ReadOnly);
    var icono = fichIcono.read();

    var versionSys = sys.version().match(/ [ 0 - 9 ] + .[ 0 - 9 ] + /);
    if (compararVersiones(versionSys, versionMinimaFL) == 2) {
      var contVersion = MessageBox.warning(
      util.translate("scripts", "Este módulo necesita la versión ") + versionMinimaFL + util.translate("scripts", " o superior de la aplicación base,\nactualmente la versión instalada es la ") + sys.version() + util.translate("scripts", ".\nFacturaLUX puede fallar por esta causa.\n¿Desea continuar la carga?"), MessageBox.Yes, MessageBox.No);
      if (contVersion == MessageBox.No) return;
    }

    if (evaluarDependencias(dependencias) == false) return;

    if (!valorPorClave("flareas", "idarea", "idarea = '" + area + "'")) {
      var crearArea = MessageBox.warning(util.translate("scripts", "El área con el identificador ") + area + util.translate("scripts", " no existe. ¿Desea crearla?"), MessageBox.Yes, MessageBox.No);
      if (crearArea == MessageBox.No) return;
      var dialogo = new Dialog;
      dialogo.width = 400;
      dialogo.caption = util.translate("scripts", "Crear área ") + area + ":";
      dialogo.okButtonText = util.translate("scripts", "Aceptar");
      dialogo.cancelButtonText = util.translate("scripts", "Cancelar");
      var leDesArea = new LineEdit;
      leDesArea.text = desArea;
      leDesArea.label = util.translate("scripts", "Descripción: ");
      dialogo.add(leDesArea);
      if (dialogo.exec()) {
        var curArea = new FLSqlCursor("flareas");
        curArea.setModeAccess(curArea.Insert);
        curArea.refreshBuffer();
        curArea.setValueBuffer("idarea", area);
        curArea.setValueBuffer("descripcion", leDesArea.text);
        curArea.commitBuffer();
      } else return;
    }

    var recargar;
    if (valorPorClave("flmodules", "idmodulo", "idmodulo = '" + modulo + "'")) {
      recargar = MessageBox.warning(util.translate("scripts", "El módulo ") + modulo + util.translate("scripts", " ya existe. ¿Desea recargarlo?"), MessageBox.Yes, MessageBox.No);
      if (recargar == MessageBox.No) return;
    }

    var curModulo = new FLSqlCursor("flmodules");
    if (recargar == MessageBox.Yes) {
      with(curModulo) {
        select("idmodulo = '" + modulo + "'");
        first();
        setModeAccess(curModulo.Edit);
      }
    } else {
      curModulo.setModeAccess(curModulo.Insert);
    }

    with(curModulo) {
      refreshBuffer();
      setValueBuffer("idmodulo", modulo);
      setValueBuffer("descripcion", descripcion);
      setValueBuffer("idarea", area);
      setValueBuffer("version", version);
      setValueBuffer("icono", icono);
      commitBuffer();
    }

    var curSeleccion = new FLSqlCursor("flmodules");
    curModulo.setMainFilter("idmodulo = '" + modulo + "'");
    curModulo.editRecord();
    formRecordflmodules.cargarDeDisco(fichero.path + "/", false);
    formRecordflmodules.accept();
    //sys.cleanupMetaData();
    var setting: String = "scripts/sys/modLastModule_" + sys.nameBD();
    util.writeSettingEntry(setting, nombreFichero);
  }
}

function dameValor(linea) {
  return linea;
}

function valorPorClave(tabla, campo, where) {
  var valor;
  var query = new FLSqlQuery();
  query.setTablesList(tabla);
  query.setSelect(campo);
  query.setFrom(tabla);
  query.setWhere(where + ";");
  query.exec();
  if (query.next()) valor = query.value(0);
  return valor;
}

function compararVersiones(v1, v2) {
  var a1, a2;
  if (v1 && v2) {
    a1 = v1.split(".");
    a2 = v2.split(".");
    for (var i = 0; i < a1.length; i++) {
      if (parseInt(a1[i]) > parseInt(a2[i])) return 1;
      if (parseInt(a1[i]) < parseInt(a2[i])) return 2;
    }
  }
  return 0;
}

function evaluarDependencias(dependencias) {
  var res;
  if (!dependencias) return true;
  //var a = dependencias.split(/[,; ]/);
  for (var i = 0; i < dependencias.length; i++) {
    if (dependencias[i].isEmpty()) continue;

    if (sys.isLoadedModule(dependencias[i]) == false) {
      res = MessageBox.warning(util.translate("scripts", "Este módulo depende del módulo ") + dependencias[i] + util.translate("scripts", ", que no está instalado.\nFacturaLUX puede fallar por esta causa.\n¿Desea continuar la carga?"), MessageBox.Yes, MessageBox.No);
      if (res == MessageBox.No) return false;
    }
  }
  return true;
}

function traducirCadena(cadena, path, modulo) {
  if (cadena.find("QT_TRANSLATE_NOOP") == -1) return cadena;
  cadena = cadena.mid(41, cadena.length - 43);

  var nombreFichero;
  try {
    nombreFichero = path + "/translations/" + modulo + "." + util.getIdioma() + ".ts";
  } catch(e) {
    return cadena;
  }

  if (!File.exists(nombreFichero)) return cadena;

  var fichero = new File(nombreFichero);
  fichero.open(File.ReadOnly);
  var f = fichero.read();
  var xmlTranslations = new FLDomDocument();

  if (xmlTranslations.setContent(f)) {

    var nodeMess = xmlTranslations.elementsByTagName("message");
    for (var i = 0; i < nodeMess.length(); i++) {
      if (nodeMess.item(i).namedItem("source").toElement().text() == cadena) {
        traduccion = nodeMess.item(i).namedItem("translation").toElement().text();
        if (traduccion) cadena = traduccion;
      }
    }
  }
  return cadena;
}
