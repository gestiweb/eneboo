/***************************************************************************
                               flupdates.qs
                            -------------------
   begin                : lun oct 13 2008
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
}
//// INTERNA /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
/** @class_declaration oficial */
//////////////////////////////////////////////////////////////////
//// OFICIAL /////////////////////////////////////////////////////
class oficial extends interna {
  var netOp;
  var codeBuffer;
  var util;

  function oficial(context) {
    interna(context);
  }
  function recepcionDatos(datos) {
    this.ctx.oficial_recepcionDatos(datos);
  }
  function finDatos() {
    this.ctx.oficial_finDatos();
  }
  function progresoDatos(bytesDone, bytesTotal) {
    this.ctx.oficial_progresoDatos(bytesDone, bytesTotal);
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
/** @class_declaration ifaceCtx */
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
function interna_main() {
  this.iface.util = new FLUtil;

  if (MessageBox.Yes == MessageBox.warning(this.iface.util.translate("scripts", "Se va a proceder a conectar a través de Internet\n" + "con los sistemas de InfoSiAL S.L. para obtener la\n" + "herramienta de actualización de AbanQ.\n\n" + "¿ Desea continuar ?\n\n"), MessageBox.No, MessageBox.Yes)) {
    this.iface.netOp = new FLNetwork("http://updates.infosial.com");

    connect(this.iface.netOp, "data(QString)", this, "iface.recepcionDatos");
    connect(this.iface.netOp, "finished()", this, "iface.finDatos");
    connect(this.iface.netOp, "dataTransferProgress(int,int)", this, "iface.progresoDatos");

    this.iface.util.createProgressDialog(this.iface.util.translate("scripts", "Conectando"), 100);
    this.iface.codeBuffer = "";
    this.iface.netOp.get("updater.qs");
  } else {
    MessageBox.information(this.iface.util.translate("scripts", "Actualización cancelada."), MessageBox.Ok);
  }
}

//// INTERNA /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/** @class_definition oficial */
//////////////////////////////////////////////////////////////////
//// OFICIAL /////////////////////////////////////////////////////
function oficial_recepcionDatos(datos) {
  if (datos != undefined) this.iface.codeBuffer += datos;
}

function oficial_finDatos() {
  this.iface.codeBuffer = sys.toUnicode(this.iface.codeBuffer, "Latin1");
  this.iface.util.destroyProgressDialog();

  if (sys.addSysCode(this.iface.codeBuffer)) {
    sys.setScriptEntryFunction("sys.updaterMain");
    sys.processEvents();
    sys.reinit();
  } else {
    MessageBox.critical(
    this.iface.util.translate("scripts", "Actualización cancelada.\n" + "Se ha producido un error al intentar obtener la herramienta de actualización.\n" + "Pruebe a repetir la actualización más tarde y si el problema persiste\n" + "puede obtener ayuda y soporte en http://www.infosial.com InfoSiAL S.L."), MessageBox.Ok);
  }
}

function oficial_progresoDatos(bytesDone, bytesTotal) {
  this.iface.util.setTotalSteps(bytesTotal);
  this.iface.util.setProgress(bytesDone);
  this.iface.util.setLabelText(this.iface.util.translate("scripts", "Descargando %1 de %2").arg(bytesDone).arg(bytesTotal));
}

//// OFICIAL /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/** @class_definition head */
/////////////////////////////////////////////////////////////////
//// DESARROLLO /////////////////////////////////////////////////
//// DESARROLLO /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

