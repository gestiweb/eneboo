/***************************************************************************
                         FLScriptEditor.cpp
                         -------------------
begin                : mie mar 3 2004
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

#include "FLScriptEditor.h"
#include "FLApplication.h"
#include "FLSqlDatabase.h"
#include "FLManagerModules.h"

#ifdef Q_OS_WIN32
#undef QSDEBUGGER
#endif

#ifdef QSDEBUGGER
#include "../qsa/src/ide/replacetext.h"
#include "../qsa/src/ide/preferencescontainer.h"
#include "../qsa/tools/designer/editor/preferences.h"
#endif

FLScriptEditor::FLScriptEditor(const QString &nFS, QWidget *parent, const char *name)
  : FLWidgetScriptEditor(parent, name, true, Qt::WType_Dialog | Qt::WShowModal |
                         Qt::WStyle_Maximize | Qt::WStyle_SysMenu)
{
  ed = new QSEditor(frMain);
  ed->setText(aqApp->db()->managerModules()->contentCode(nFS));
  ed->setInterpreter(aqApp->project()->interpreter());
  frMainLayout->addWidget(ed);

  ed->setFocus();

  connect(pbOk, SIGNAL(clicked()), SLOT(accept()));

#ifdef QSDEBUGGER
  replaceText = new QSReplaceText(this, 0, FALSE);
  connect(pbReplace, SIGNAL(clicked()), this, SLOT(showReplace()));
  connect(replaceText->pushReplace, SIGNAL(clicked()), this, SLOT(editReplace()));
  connect(replaceText->pushReplaceAll, SIGNAL(clicked()), this, SLOT(editReplaceAll()));

  preferencesContainer = new QSPreferencesContainer(this, 0);
  QBoxLayout *preferencesLayout = new QBoxLayout(preferencesContainer->frame, QBoxLayout::Down);
  qsaEditorSyntax = new PreferencesBase(preferencesContainer->frame, "qsaeditor_syntax");
  preferencesLayout->addWidget(qsaEditorSyntax);
  qsaEditorSyntax->setPath(QString::fromLatin1("/Trolltech/QSAScriptEditor/"));
  connect(preferencesContainer->pushOk, SIGNAL(clicked()), this, SLOT(savePreferences()));
  connect(pbPref, SIGNAL(clicked()), this, SLOT(editPreferences()));
#endif
}

FLScriptEditor::~FLScriptEditor() {}

QString FLScriptEditor::code() const
{
  if (ed)
    return ed->text().remove("var form = this;\n");
  else
    return QString::null;
}

void FLScriptEditor::setCode(const QString &c)
{
  if (ed)
    ed->setText(c);
}

bool FLScriptEditor::find(const QString &expr)
{
  if (expr.isEmpty())
    return true;
  lastSearch = expr;
  if (ed->find(expr, true, false, true, true))
    return true;
  if (ed->find(expr, true, false, false, true))
    return true;
  return ed->find(expr, true, false, true, false);
}

bool FLScriptEditor::reFind()
{
  return find(lastSearch);
}

void FLScriptEditor::gotoLine(const QString &line)
{
  ed->gotoLine(line.toInt());
  QTextEdit *textEdit = ed->textEdit();
  textEdit->ensureCursorVisible();
  int para, index;
  textEdit->getCursorPosition(&para, &index);
  textEdit->setSelection(para, index, para, index + 1);
}

void FLScriptEditor::showReplace()
{
#ifdef QSDEBUGGER
  replaceText->show();
#endif
}

void FLScriptEditor::editReplace()
{
  editReplace(false);
}

void FLScriptEditor::editReplaceAll()
{
  editReplace(true);
}

void FLScriptEditor::editReplace(bool all)
{
#ifdef QSDEBUGGER
  QString findString = replaceText->comboFind->currentText();
  replaceText->comboFind->insertItem(findString);
  QString replaceString = replaceText->comboReplace->currentText();
  replaceText->comboFind->insertItem(replaceString);
  bool caseSensitive = replaceText->checkCase->isChecked();
  bool wholeWordsOnly = replaceText->checkWhole->isChecked();
  bool startAtCursor = !replaceText->checkStart->isChecked();
  bool forward = replaceText->radioForward->isChecked();
  ed->replace(findString, replaceString, caseSensitive, wholeWordsOnly, forward, startAtCursor, all);
#endif
}

void FLScriptEditor::editPreferences()
{
#ifdef QSDEBUGGER
  qsaEditorSyntax->reInit();
  preferencesContainer->show();
#endif
}

void FLScriptEditor::savePreferences()
{
#ifdef QSDEBUGGER
  qsaEditorSyntax->save();
  ed->readSettings();
#endif
}

void FLScriptEditor::keyPressEvent(QKeyEvent *e)
{
  if (e->key() == Key_Escape) {
    e->ignore();
    return ;
  }
  QDialog::keyPressEvent(e);
}
