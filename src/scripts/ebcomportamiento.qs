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
	w.child("leFormRecordSize").text = leerValorGlobal("FLFormRecordDBButton");
	w.child("leFormDBSize").text = leerValorGlobal("FLFormDBButton");
	w.child("leFormSearchDBSize").text = leerValorGlobal("FLFormSearchDBButton");
	//w.child("leFormRecordSize").text = leerValorGlobal("FLFormRecordDBButton");
	if ( w.child("leFormRecordSize").text == "false" ) w.child("leFormRecordSize").text = "22";
	if ( w.child("leFormDBSize").text == "false" ) w.child("leFormDBSize").text = "22";
	if ( w.child("leFormSearchDBSize").text == "false" ) w.child("leFormSearchDBSize").text = "22";
	if ( w.child("leNombreVertical").text == "false" ) w.child("leNombreVertical").text = "";
	w.child("cbFLTableDC").checked = leerValorGlobal("FLTableDoubleClick");
	w.child("cbFLTableSC").checked = leerValorGlobal("FLTableShortCut");
	w.child("cbFLTableCalc").checked = leerValorGlobal("FLTableExport2Calc");
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

function guardar_clicked()
{
var w = this.w_;
grabarValorGlobal("verticalName",w.child("leNombreVertical").text);
grabarValorGlobal("FLFormRecordDBButton",w.child("leFormRecordSize").text); //Hay que comprobar que sea numero mayor de 0!!
grabarValorGlobal("FLFormDBButton",w.child("leFormDBSize").text); //Hay que comprobar que sea numero mayor de 0!!
grabarValorGlobal("FLFormSearchDBButton",w.child("leFormSearchDBSize").text); //Hay que comprobar que sea numero mayor de 0!!
//grabarValorGlobal("FLFormRecordDBButton",w.child("leFormRecordSize").text); //Hay que comprobar que sea numero mayor de 0!!
grabarValorGlobal("FLTableDoubleClick",w.child("cbFLTableDC").checked);
grabarValorGlobal("FLTableShortCut",w.child("cbFLTableSC").checked);
grabarValorGlobal("FLTableExport2Calc",w.child("cbFLTableCalc").checked);

w_.close();

}

function cancelar_clicked()
{
w_.close();
}
