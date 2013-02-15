/***************************************************************************
                                flfiles.qs
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
  this.child("contenido").text = this.cursor().valueBuffer("contenido");

  var botonEditar = this.child("botonEditar");
  var pbXMLEditor = this.child("pbXMLEditor");

  var cursor = this.cursor();
  if (cursor.modeAccess() == cursor.Browse) {
    botonEditar.setEnabled(false);
    pbXMLEditor.setEnabled(false);
  } else {
    connect(botonEditar, "clicked()", this, "editarFichero");

    var nombre = cursor.valueBuffer("nombre");
    var tipo = tipoDeFichero(nombre);
    if (tipo == ".ui" || tipo == ".ts" || tipo == ".qs") pbXMLEditor.setEnabled(false);
    else connect(pbXMLEditor, "clicked()", this, "editarFicheroXML");
  }
}

function acceptedForm() {
  this.cursor().setValueBuffer("contenido", this.child("contenido").text);
}

function tipoDeFichero(nombre) {
  var posPunto = nombre.lastIndexOf(".");
  return nombre.right(nombre.length - posPunto);
}

function editarFichero() {
  var cursor = this.cursor();
  var util = new FLUtil();

  if (cursor.checkIntegrity()) {
    this.child("nombre").setDisabled(true);
    var nombre = cursor.valueBuffer("nombre");
    var tipo = tipoDeFichero(nombre);
    var temporal = System.getenv("TMP");
    if (temporal.isEmpty()) temporal = System.getenv("TMPDIR");
    if (temporal.isEmpty()) temporal = System.getenv("HOME");
    if (temporal.isEmpty()) temporal = sys.installPrefix() + "/share/facturalux/tmp";
    temporal = temporal + "/" + cursor.valueBuffer("nombre");

    var contenido = this.child("contenido").text;
    var comando:String;
    switch (tipo) {
    case ".ui":
	if (util.getOS() == "MACX")
			{ 
			File.write( temporal, contenido + "\n\n\n\n\n\n\n\n\n\n\n\n\n\n" );
			comando = sys.installPrefix() + "/bin/designer.app/Contents/MacOS/designer";
			}
	else
			{
      			File.write(temporal, contenido);
      			comando = sys.installPrefix() + "/bin/designer";
			}      
	this.setDisabled(true);
      Process.execute([comando, temporal]);
      this.child("contenido").text = File.read(temporal);
      this.setDisabled(false);
      break;
    case ".ts":
	if (util.getOS() == "MACX")
			{
		        File.write( temporal, contenido + "\n\n\n\n\n\n\n\n\n\n\n\n\n\n" );
      			comando = sys.installPrefix() + "/bin/linguist.app/Contents/MacOS/linguist";
			}
	else
			{
     			File.write(temporal, contenido);
      			comando = sys.installPrefix() + "/bin/linguist";
      			}
      this.setDisabled(true);
      Process.execute([comando, temporal]);
      this.child("contenido").text = File.read(temporal);
      this.setDisabled(false);
      break;
    case ".kut":
	if (util.getOS() == "MACX")
			{
		        File.write( temporal, contenido + "\n\n\n\n\n\n\n\n\n\n\n\n\n\n" );
      			comando = sys.installPrefix() + "/bin/kudesigner.app/Contents/MacOS/kudesigner";
			}
	else
			{
      			File.write(temporal, contenido);
      			comando = sys.installPrefix() + "/bin/kudesigner";
			}
      this.setDisabled(true);
      Process.execute([comando, temporal]);
      this.child("contenido").text = File.read(temporal);
      this.setDisabled(false);
      break;
    case ".qs":
      this.setDisabled(true);
      var editor = new FLScriptEditor(nombre);
      editor.exec();
      this.child("contenido").text = editor.code();
      this.setDisabled(false);
      break;
    default:
      this.setDisabled(true);
      var dialog = new Dialog();
      dialog.width = 600;
      dialog.cancelButtonText = "";
      var editor = new TextEdit();
      editor.textFormat = editor.PlainText;
      editor.text = contenido;
      dialog.add(editor);
      dialog.exec();
      this.child("contenido").text = editor.text;
      this.setDisabled(false);
    }
  }
}

function editarFicheroXML() {
  var cursor = this.cursor();
  var util = new FLUtil();
  if (cursor.checkIntegrity()) {
    var temporal = System.getenv("TMP");
    if (temporal.isEmpty()) temporal = System.getenv("TMPDIR");
    if (temporal.isEmpty()) temporal = System.getenv("HOME");
    if (temporal.isEmpty()) temporal = sys.installPrefix() + "/share/facturalux/tmp";
    temporal = temporal + "/" + cursor.valueBuffer("nombre");
    var comando:String;
    var contenido = this.child("contenido").text;
    if (util.getOS() == "MACX")
			{
		        File.write( temporal, contenido + "\n\n\n\n\n\n\n\n\n\n\n\n\n\n" );
      			comando = sys.installPrefix() + "/bin/teddy.app/Contents/MacOS/teddy";
			}
	else
			{
    			File.write(temporal, contenido);
   			comando = sys.installPrefix() + "/bin/teddy";
			}
    this.setDisabled(true);
    Process.execute([comando, temporal]);
    this.child("contenido").text = File.read(temporal);
    this.setDisabled(false);
  }
}

function calculateField(fN) {
  if (fN == "sha") {
    var util = new FLUtil();
    return util.sha1(this.cursor().valueBuffer("contenido"));
  }
}
