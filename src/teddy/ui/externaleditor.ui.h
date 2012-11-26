/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

#include <qfiledialog.h>
#include <qcombobox.h>
#include <qtextcodec.h>



void ExternalEditorPref::init() {
	setDefaultEditor();
	
	QTextCodec *codec;
	for (int i = 0; (codec = QTextCodec::codecForIndex(i)); i++) {
		QString nm(codec->mimeName());
		nm = nm.stripWhiteSpace();
		if (nm == "UTF-8"
		|| nm == "UTF-16") continue;
		comboBox_outputEncoding->insertItem( nm );
	}
}

	
void ExternalEditorPref::selectApp()
{
	QFileDialog* fd = new QFileDialog(
		this, "Select External Editor", TRUE
	);
	fd->raise();
	QString fileName;
	if ( fd->exec() == QDialog::Accepted ) {
        fileName = fd->selectedFile();
		lineEdit_extEditor->setText(fileName);
	}
}



void ExternalEditorPref::setEditor( const QString &val )
{
	lineEdit_extEditor->setText(val);
}


QString ExternalEditorPref::editor()
{
	return lineEdit_extEditor->text();
}



void ExternalEditorPref::setDefaultEditor()
{
#if defined(Q_WS_WIN)
	QString defEditor("C:\\Program Files\\Windows NT\\Accessories\\wordpad.exe");
#endif
#if defined(Q_WS_X11)
	QString defEditor("/opt/kde3/bin/kate");
#endif
#if defined(Q_WS_MAC)
	QString defEditor("/Applications/TextEdit.app/Contents/MacOS/TextEdit");
#endif
#if defined(Q_WS_QWS)
	QString defEditor("");
#endif
	setEditor(defEditor);
	setEditorActive(TRUE);
	setOutputEncoding("UTF-8");
}


bool ExternalEditorPref::editorActive()
{
	return checkBox_allowExtEditor->isChecked();
}


void ExternalEditorPref::setEditorActive( bool active )
{
	checkBox_allowExtEditor->setChecked(active);
}


QString ExternalEditorPref::outputEncoding()
{
	return comboBox_outputEncoding->currentText();
}


void ExternalEditorPref::setOutputEncoding( const QString &val )
{
	comboBox_outputEncoding->setCurrentText(val);
}
