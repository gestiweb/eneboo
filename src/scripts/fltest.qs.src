/***************************************************************************
                                  fltest.qs
                            -------------------
   begin                : mie dic 28 2005
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

function main() {
  var util: FLUtil = new FLUtil;
  if (!sys.isTestEnabled()) {
    MessageBox.warning(util.translate("scripts", "La aplicación no se compiló en modo test, por lo que esta opoción no está disponible"), MessageBox.Ok, MessageBox.NoButton);
    return;
  }

  util.sqlSelect("tt_test", "idtest", "1 = 1", "tt_test");
  util.sqlSelect("tt_step", "idstep", "1 = 1", "tt_step");
  util.sqlSelect("tt_session", "idsession", "1 = 1", "tt_session");

  var dialog = new Dialog;
  dialog.caption = util.translate("scripts", "Lanzar sesión de pruebas");
  dialog.okButtonText = util.translate("scripts", "Lanzar");
  dialog.cancelButtonText = util.translate("scripts", "Cancelar");

  var bd = new LineEdit;
  bd.label = util.translate("scripts", "Base de datos");
  dialog.add(bd);

  /*
	var funcional = new LineEdit;
	funcional.label = util.translate("scripts", "Funcional");
	dialog.add( funcional );
	
	var sesion = new LineEdit;
	sesion.label = util.translate("scripts", "Id. Sesión");
	dialog.add( sesion );
	*/

  if (!dialog.exec()) return;

  var listaSesiones: String = sys.testSessionsList(bd.text);
  if (!listaSesiones || listaSesiones == "") {
    MessageBox.critical(util.translate("scripts", "Error al obtener la lista de sesiones para la base de datos ") + bd.text, MessageBox.Ok, MessageBox.NoButton);
    return;
  }
  var sesiones: Array = listaSesiones.split("**");
  var sesion: String = Input.getItem(util.translate("scripts", "Seleccione sesión"), sesiones);
  if (sesion) {
    var datosSesion: Array = sesion.split("//");
    sys.startTest(datosSesion[1], datosSesion[0], bd.text);
  }
}
