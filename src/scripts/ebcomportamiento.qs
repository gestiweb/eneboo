/***************************************************************************
                                  ebcomportamiento.qs
                            -------------------
   begin                : 01 mar 2013
   copyright            : (C) 2013 by Aulla Sistemas
   email                : aullasistemas@gmail.com
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

 	var mng = aqApp.db().managerModules();
	this.w_ = mng.createUI("ebcomportamiento.ui");
	var w = this.w_;
	var botonAceptar = w.child("pbnAceptar"); 
	var botonCancelar = w.child("pbnCancelar");
	connect(botonAceptar , "clicked()", this, "guardar_clicked");
	connect(botonCancelar , "clicked()", this, "cancelar_clicked");
 	cargarConfiguracion();
 	w_.show();
}

function cargarConfiguracion() {
	var w = this.w_;
	w.child("leNombreVertical").text = leerValorGlobal("verticalName");
	w.child("leFormRecordSize").text = leerValorLocal("FLFormRecordDBButton");
	w.child("leFormDBSize").text = leerValorLocal("FLFormDBButton");
	w.child("leFormSearchDBSize").text = leerValorLocal("FLFormSearchDBButton");
	//w.child("leFormRecordSize").text = leerValorLocal("FLFormRecordDBButton");
	if ( w.child("leFormRecordSize").text == "false" ) w.child("leFormRecordSize").text = "22";
	if ( w.child("leFormDBSize").text == "false" ) w.child("leFormDBSize").text = "22";
	if ( w.child("leFormSearchDBSize").text == "false" ) w.child("leFormSearchDBSize").text = "22";
	if ( w.child("leNombreVertical").text == "false" ) w.child("leNombreVertical").text = "";
	w.child("cbFLTableDC").checked = leerValorLocal("FLTableDoubleClick");
	w.child("cbFLTableSC").checked = leerValorLocal("FLTableShortCut");
	w.child("cbFLTableCalc").checked = leerValorLocal("FLTableExport2Calc");
	w.child("cbDebuggerMode").checked = leerValorLocal("isDebuggerMode");
	w.child("cbSLConsola").checked = leerValorLocal("SLConsola");
	w.child("cbSLInterface").checked = leerValorLocal("SLInterface");

}



function leerValorGlobal(valorName):String {
	var util : FLUtil = new FLUtil();
	var valor : String =  util.sqlSelect("flsettings", "valor", "flkey='" + valorName + "'");
	if (!valor) //Entendemos que el valor existe
	  
		{
		debug("No existe la entrada " + valorName + " (" + valor + ")");
		if(!util.sqlInsert("flsettings", "flkey,valor" , valorName + "," + valor))
			{
			debug("Error al crear la entrada");
			MessageBox.warning("Se ha producido un problema al crear el valor " + valorName + "." , MessageBox.Yes);
			valor = "Error";
			}
		}
	return valor;
}

function grabarValorGlobal(valorName,value) {
	var util : FLUtil = new FLUtil();
	util.sqlUpdate("flsettings", "valor", value, "flkey = '" + valorName + "'");
}

function leerValorLocal(valorName):String {
	var util : FLUtil = new FLUtil();
	var valor : String;
	
	if (valorName == "isDebuggerMode")
		valor =  util.readSettingEntry("application/"+ valorName );
	else
		valor =  util.readSettingEntry("ebcomportamiento/"+ valorName );
	if (!valor) //Entendemos que el valor existe
		debug("No existe la entrada " + valorName + " (" + valor + ") local.");
	return valor;
}

function grabarValorLocal(valorName,value) {
	var util : FLUtil = new FLUtil();
	if (valorName == "isDebuggerMode")
		util.writeSettingEntry("application/"+ valorName, value);
	else
	util.writeSettingEntry("ebcomportamiento/"+ valorName, value);
	
}

















function guardar_clicked()
{
var w = this.w_;
grabarValorGlobal("verticalName",w.child("leNombreVertical").text);
grabarValorLocal("FLFormRecordDBButton",w.child("leFormRecordSize").text); //Hay que comprobar que sea numero mayor de 0!!
grabarValorLocal("FLFormDBButton",w.child("leFormDBSize").text); //Hay que comprobar que sea numero mayor de 0!!
grabarValorLocal("FLFormSearchDBButton",w.child("leFormSearchDBSize").text); //Hay que comprobar que sea numero mayor de 0!!
grabarValorLocal("FLTableDoubleClick",w.child("cbFLTableDC").checked);
grabarValorLocal("FLTableShortCut",w.child("cbFLTableSC").checked);
grabarValorLocal("FLTableExport2Calc",w.child("cbFLTableCalc").checked);
grabarValorLocal("isDebuggerMode",w.child("cbDebuggerMode").checked);
grabarValorLocal("SLConsola",w.child("cbSLConsola").checked);
grabarValorLocal("SLInterface",w.child("cbSLInterface").checked);

w_.close();

}

function cancelar_clicked()
{
w_.close();
}
