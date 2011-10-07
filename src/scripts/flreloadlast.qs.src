/***************************************************************************
                 flreloadlast.qs  -  description
                             -------------------
    begin                : jue abr 9 2005
    copyright            : (C) 2005 by InfoSiAL S.L.
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

/** @file */

/** @class_declaration interna */
////////////////////////////////////////////////////////////////////////////
//// DECLARACION ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//// INTERNA /////////////////////////////////////////////////////
class interna {
  var ctx: Object;
  function interna(context) {
    this.ctx = context;
  }
  function main() {
    this.ctx.interna_main();
  }
  function init() {
    this.ctx.interna_init();
  }
}
//// INTERNA /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
/** @class_declaration oficial */
//////////////////////////////////////////////////////////////////
//// OFICIAL /////////////////////////////////////////////////////
class oficial extends interna {
  var pathLocal: String;
  var idFuncional: String;
  var bloqueo: Boolean;

  function oficial(context) {
    interna(context);
  }
  function cargarModulo(nombreFichero: String) : Boolean {
    return this.ctx.oficial_cargarModulo(nombreFichero);
  }
  function compararVersiones(v1: String, v2: String) : Number {
    return this.ctx.oficial_compararVersiones(v1, v2);
  }
  function traducirCadena(cadena: String, path: String, modulo: String) : String {
    return this.ctx.oficial_traducirCadena(cadena, path, modulo);
  }
}
//// OFICIAL /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
/** @class_declaration head */
/////////////////////////////////////////////////////////////////
//// DESARROLLO /////////////////////////////////////////////////
class head extends oficial {
  function head(context) {
    oficial(context);
  }
}
//// DESARROLLO /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/** @class_declaration ifaceCtx*/
/////////////////////////////////////////////////////////////////
//// INTERFACE  /////////////////////////////////////////////////
class ifaceCtx extends head {
  function ifaceCtx(context) {
    head(context);
  }
}

const iface = new ifaceCtx(this);
//// INTERFACE  /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/** @class_definition interna */
////////////////////////////////////////////////////////////////////////////
//// DEFINICION ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//// INTERNA /////////////////////////////////////////////////////
function interna_init() {}

/** \D Carga los módulos de la funcionalidad o proyecto que el usuario seleccione, creando las correspondientes áreas si es necesario.
\end */
function interna_main() {
  var util: FLUtil = new FLUtil();
  //if (!sys.isDebugEnabled()) {
  //	MessageBox.information( util.translate( "scripts", "Esta opción actualmente no está disponible.\nUtilice la opción Cargar Módulo del menú Principal" ), MessageBox.Ok );
  //	return;
  //}
  var setting: String = "scripts/sys/modLastModule_" + sys.nameBD();

  var fichMod: String = util.readSettingEntry(setting);
  if (!fichMod) {
    fichMod = FileDialog.getOpenFileName(util.translate("scripts", "Módulo a cargar (*.mod)"), util.translate("scripts", "Módulo a cargar"));

    if (!fichMod) return;

    util.writeSettingEntry(setting, fichMod);
  }

  sys.processEvents();
  this.iface.cargarModulo(fichMod);
  sys.reinit();
}

//// INTERNA /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/** @class_definition oficial */
//////////////////////////////////////////////////////////////////
//// OFICIAL /////////////////////////////////////////////////////
/** \D Carga o recarga un módulo en la base de datos

@param	dirModulo: Ruta al fichero .mod del módulo a cargar
@return	verdadero si la carga es correcta, falso en caso contrario
\end */
function oficial_cargarModulo(nombreFichero: String) : Boolean {
  var util: FLUtil = new FLUtil;
  var fichero = new File(nombreFichero);

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

  descripcion = this.iface.traducirCadena(descripcion, fichero.path, modulo);
  desArea = this.iface.traducirCadena(desArea, fichero.path, modulo);

  var fichIcono = new File(fichero.path + "/" + nombreIcono);
  fichIcono.open(File.ReadOnly);
  var icono = fichIcono.read();

  var versionSys = sys.version().match(/ [ 0 - 9 ] + .[ 0 - 9 ] + /);
  if (this.iface.compararVersiones(versionSys, versionMinimaFL) == 2) {
    var contVersion = MessageBox.warning(util.translate("scripts", "Este módulo necesita la versión ") + versionMinimaFL + util.translate("scripts", " o superior de la aplicación base,\nactualmente la versión instalada es la ") + sys.version() + util.translate("scripts", ".\nFacturaLUX puede fallar por esta causa.\n¿Desea continuar la carga?"), MessageBox.Yes, MessageBox.No);
    if (contVersion == MessageBox.No) return;
  }

  if (!util.sqlSelect("flareas", "idarea", "idarea = '" + area + "'")) {
    if (!util.sqlInsert("flareas", "idarea,descripcion", area + "," + desArea)) {
      MessageBox.warning(util.translate("scripts", "Error al crear el área:\n") + area, MessageBox.Ok, MessageBox.NoButton);
      return false;
    }
  }

  var recargar: String = util.sqlSelect("flmodules", "idmodulo", "idmodulo = '" + modulo + "'");
  var curModulo = new FLSqlCursor("flmodules");
  if (recargar) {
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
  return true;
}

/** \D Compara dos cadenas que representan la versión de los módulos. Su formato es XX.YY

@param	v1: Primera versión a comparar
@param	v2: Segunda versión a comparar
@return	Los posibles valores de retorno son:
	0: Las versiones son iguales
	1: La primera versión es mayor
	2: La segunda versión es mayor
\end */
function oficial_compararVersiones(v1: String, v2: String) : Number {
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

/** \D Traduce la cadena de descripción de un área o módulo
@param	cadena: Cadena a traducir
@param	path: Ruta al fichero mod
@param	modulo: Nombre del módulo
@return	Cadena traducida
*/
function oficial_traducirCadena(cadena: String, path: String, modulo: String) : String {
  var util: FLUtil = new FLUtil();
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

//// OFICIAL /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/** @class_definition head */
/////////////////////////////////////////////////////////////////
//// DESARROLLO /////////////////////////////////////////////////
//// DESARROLLO /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

