/***************************************************************************
                           FLConnectDBDialog.cpp
                         -------------------
begin                : Sun Jul 1 2001
copyright            : (C) 2001-2005 by InfoSiAL, S.L.
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
#include <stdio.h>

#include <qtoolbutton.h>

#include "FLConnectDBDialog.h"
#include "FLSqlDatabase.h"
#include "FLApplication.h"
#include "FLSqlConnections.h"
#include "FLSettings.h"

#include "AQConfig.h"

static inline void comboBoxInsert(QComboBox *c, const QStringList &l)
{
  QStringList inserts;
  for (QStringList::const_iterator it = l.begin(); it != l.end(); ++it) {
    if (inserts.contains(*it))
      continue;
    c->insertItem(*it);
    inserts.append(*it);
  }
}

FLConnectDBDialog::FLConnectDBDialog(bool disabled, QWidget *parent, const char *name,
                                     WFlags fl, const QString &strConn)
  : FLWidgetConnectDB(parent, name, false, fl | AQ_CONN_DIAG_FLAGS), error_(true)
{
  setDisabled(disabled);
  setBackgroundMode(Qt::NoBackground);

  logo = QImage::fromMimeSource("logo_abanq.png");

  QString verMsg("<p align=\"center\"><b>Eneboo %1</b><br>"
                 "</p>");
  lblCredito->setText(verMsg.arg(AQ_VERSION));

  rememberPasswd_ = FLSettings::readBoolEntry("DBA/rememberPasswd");
  bool ok = false;
  oldApi_ = FLSettings::readBoolEntry("application/oldApi", ok, &ok);
  if (!ok)
    setOldApi(oldApi_);

  comboBoxInsert(comboBoxMarks, FLSettings::readListEntry("DBA/marks"));
  lineEditUser->setText(FLSettings::readEntry("DBA/username"));
  if (rememberPasswd_) {
    chkRemember->setChecked(true);
    lineEditPassword->setText(FLSettings::readEntry("DBA/password"));
  }
  if (oldApi_)
    chkOldApi->setChecked(true);
  lineEditHost->setText(FLSettings::readEntry("DBA/hostname", "localhost"));
  comboBoxNameDB->clear();
  comboBoxInsert(comboBoxNameDB, FLSettings::readListEntry("DBA/namesDB"));
  comboBoxNameDB->setCurrentText(FLSettings::readEntry("DBA/lastDB", "abanq"));
  lineEditUser->setFocus();

  connect(comboBoxNameDB, SIGNAL(activated(const QString &)), this, SLOT(changeDB(const QString &)));

  comboBoxInsert(comboBoxDB, FLSqlDatabase::driverAliases());
  QString alias = FLSettings::readEntry("DBA/db", FLSqlDatabase::defaultAlias());
  comboBoxDB->setCurrentText(alias);
  driverChanged(comboBoxDB->currentText());
  lineEditPort->setText(FLSettings::readEntry("DBA/port", FLSqlDatabase::defaultPort(alias)));

  if (!strConn.isEmpty())
    selectMark(strConn);
}

FLConnectDBDialog::~FLConnectDBDialog() {}

void FLConnectDBDialog::tryConnect()
{
  FLSqlDatabase *db = new FLSqlDatabase();

  if (!db->loadDriver(FLSqlDatabase::driverAliasToDriverName(comboBoxDB->currentText()))) {
    QMessageBox::critical(0, tr("Driver no cargado"),
                          tr("Error al cargar el driver %1.").arg(comboBoxDB->currentText()),
                          QMessageBox::Ok, 0, 0);
    error_ = true;
    delete db;
    this->accept();
    return ;
  }

  if (!db->connectDB(comboBoxNameDB->currentText(), lineEditUser->text(),
                     lineEditPassword->text(), lineEditHost->text(), lineEditPort->text().toInt())) {
    error_ = true;
    delete db;
    this->accept();
    return ;
  }

  FLSettings::writeEntry("DBA/rememberPasswd", rememberPasswd_);
  FLSettings::writeEntry("DBA/username", lineEditUser->text());
  if (rememberPasswd_)
    FLSettings::writeEntry("DBA/password", lineEditPassword->text());
  else
    FLSettings::writeEntry("DBA/password", QString::null);
  FLSettings::writeEntry("DBA/port", lineEditPort->text());
  FLSettings::writeEntry("DBA/hostname", lineEditHost->text());
  FLSettings::writeEntry("DBA/db", comboBoxDB->currentText());
  FLSettings::writeEntry("DBA/lastDB", comboBoxNameDB->currentText());
  QStringList names;
  for (int i = 0; i < comboBoxNameDB->count(); i++)
    names << comboBoxNameDB->text(i);
  if (names.findIndex(comboBoxNameDB->currentText()) == -1)
    names << comboBoxNameDB->currentText();
  FLSettings::writeEntry("DBA/namesDB", names);

  error_ = false;
  FLSqlConnections::addDatabase(db);
  this->accept();
}

void FLConnectDBDialog::cancel()
{
  error_ = false;
  this->reject();
}

void FLConnectDBDialog::driverChanged(const QString &driverAlias)
{
  lineEditUser->setEnabled(FLSqlDatabase::needConnOption(driverAlias, FLSqlDatabase::USER));
  lineEditPassword->setEnabled(FLSqlDatabase::needConnOption(driverAlias, FLSqlDatabase::PASSWORD));
  lineEditPort->setEnabled(FLSqlDatabase::needConnOption(driverAlias, FLSqlDatabase::PORT));
  lineEditHost->setEnabled(FLSqlDatabase::needConnOption(driverAlias, FLSqlDatabase::HOST));
  lineEditPort->setText(FLSqlDatabase::defaultPort(driverAlias));
}

void FLConnectDBDialog::changeDB(const QString &DB)
{
  int i = 0;
  bool found = FALSE;
  for (i = 0; i < comboBoxNameDB->count(); ++i) {
    if (comboBoxNameDB->text(i) == DB) {
      found = TRUE;
      break;
    }
  }

  if (found)
    comboBoxNameDB->setCurrentItem(i);
  else {
    comboBoxNameDB->insertItem(DB);
    comboBoxNameDB->setCurrentItem(comboBoxNameDB->count() - 1);
  }
}

void FLConnectDBDialog::closeEvent(QCloseEvent *e)
{
  error_ = false;
  QWidget::closeEvent(e);
}

void FLConnectDBDialog::addMark()
{
  QString datConn;
  QString driverAlias = comboBoxDB->currentText();

  if (comboBoxNameDB->currentText().isEmpty()) {
    QMessageBox::warning(qApp->focusWidget(), tr("Agregar marcador"),
                         tr("Establezca el nombre de la base de datos"), QMessageBox::Ok, 0, 0);
    comboBoxNameDB->setFocus();
    return;
  }
  datConn = comboBoxNameDB->currentText();

  datConn += ":" + lineEditUser->text();
  datConn += ":" + driverAlias;

  if (FLSqlDatabase::needConnOption(driverAlias, FLSqlDatabase::HOST) &&
      lineEditHost->text().isEmpty()) {
    QMessageBox::warning(qApp->focusWidget(), tr("Agregar marcador"),
                         tr("Establezca el nombre del servidor"), QMessageBox::Ok, 0, 0);
    lineEditHost->setFocus();
    return;
  }
  datConn += ":" + lineEditHost->text();

  if (FLSqlDatabase::needConnOption(driverAlias, FLSqlDatabase::PORT) &&
      lineEditPort->text().isEmpty()) {
    QMessageBox::warning(qApp->focusWidget(), tr("Agregar marcador"),
                         tr("Establezca el número de puerto"), QMessageBox::Ok, 0, 0);
    lineEditPort->setFocus();
    return;
  }
  datConn += ":" + lineEditPort->text();

  comboBoxMarks->insertItem(datConn);
  comboBoxMarks->setCurrentText(datConn);

  QStringList names;
  for (int i = 0; i < comboBoxMarks->count(); i++)
    names << comboBoxMarks->text(i);

  FLSettings::writeEntry("DBA/marks", names);
}

void FLConnectDBDialog::removeMark()
{
  comboBoxMarks->removeItem(comboBoxMarks->currentItem());

  QStringList names;
  for (int i = 0; i < comboBoxMarks->count(); i++)
    names << comboBoxMarks->text(i);

  FLSettings::writeEntry("DBA/marks", names);
}

void FLConnectDBDialog::selectMark(const QString &mark)
{
  if (mark.isEmpty())
    return;

  QStringList dat = QStringList::split(':', mark, true);
  int i = 0;
  for (QStringList::Iterator it = dat.begin(); it != dat.end(); ++it, ++i) {
    switch (i) {
      case 0:
        comboBoxNameDB->insertItem(*it);
        comboBoxNameDB->setCurrentText(*it);
        break;
      case 1:
        lineEditUser->setText(*it);
        break;
      case 2:
        comboBoxDB->setCurrentText(*it);
        break;
      case 3:
        lineEditHost->setText(*it);
        break;
      case 4:
        lineEditPort->setText(*it);
        break;
    }
  }
}

void FLConnectDBDialog::popupMarks()
{
  QPopupMenu *marks = new QPopupMenu(this, "marks");
  QActionGroup *ag = new QActionGroup(this, 0);
  QSignalMapper *marksMapper = new QSignalMapper(this);

  connect(marksMapper, SIGNAL(mapped(const QString &)), this, SLOT(tryConnectMark(const QString &)));

  QStringList list;
  for (int i = 0; i < comboBoxMarks->count(); i++)
    list << comboBoxMarks->text(i);
  list.sort();
  QDict < int >marksDict(17, false);

  for (QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
    QString marksStr = *it;
    QString marksAccel = marksStr;

    if (marksDict[ marksAccel.left(1)]) {
      for (uint i = 0; i < marksAccel.length(); i++) {
        if (!marksDict[ marksAccel.mid(i, 1)]) {
          marksDict.insert(marksAccel.mid(i, 1), (const int *) 1);
          marksAccel = marksAccel.insert(i, '&');
          break;
        }
      }
    } else {
      marksDict.insert(marksAccel.left(1), (const int *) 1);
      marksAccel = "&" + marksAccel;
    }
    QAction *a = new QAction(marksStr, QIconSet(), marksAccel, 0, ag, 0, ag->isExclusive());
    connect(a, SIGNAL(activated()), marksMapper, SLOT(map()));
    marksMapper->setMapping(a, a->text());
  }
  ag->addTo(marks);

  marks->setPalette(palette());
  marks->popup(QCursor::pos());
}

void FLConnectDBDialog::tryConnectMark(const QString &mark)
{
  selectMark(mark);
  tryConnect();
}

void FLConnectDBDialog::paintEvent(QPaintEvent *pe)
{
  QPixmap pix(width(), height());
  pix.fill(QColor(logo.pixel(1, 1)));

  QPainter p(&pix);

  int dx = width() - logo.width();
  int dy = height() - logo.height();
  p.drawImage(QPoint(dx, dy), logo);

  if (pbnMore->isOn()) {
#if defined(Q_OS_MACX)
    QFont font7(qApp->font().family(), 9);
    QFont font8(qApp->font().family(), 10);
#else
    QFont font7(qApp->font().family(), 7);
    QFont font8(qApp->font().family(), 8);
#endif
    font8.setBold(true);
    QString copyright("");
    int lineWidth = QFontMetrics(font8).width(copyright);
    int lineHeight = QFontMetrics(font8).height();
    p.setFont(font8);
    p.drawText(QRect(6, height() - lineHeight * 4 - 2, lineWidth + 10, lineHeight),
               Qt::AlignVCenter | Qt::AlignJustify | Qt::SingleLine, copyright);
    p.drawImage(lineWidth + 20 , height() - lineHeight * 4, QImage::fromMimeSource("defFactory"));
    lineWidth = QFontMetrics(font7).width("This software is provided under the terms");
    lineHeight = QFontMetrics(font7).height();
    p.setFont(font7);
    p.drawText(QRect(6, height() - lineHeight * 3 - 2, lineWidth + 10, lineHeight),
               Qt::AlignVCenter | Qt::AlignJustify | Qt::SingleLine, "This software is provided under the terms");
    p.drawText(QRect(6, height() - lineHeight * 2 - 2, lineWidth + 10, lineHeight),
               Qt::AlignVCenter | Qt::AlignJustify | Qt::SingleLine, "of GNU General Public License ( GPL ) and");
    p.drawText(QRect(6, height() - lineHeight - 2, lineWidth + 10, lineHeight),
               Qt::AlignVCenter | Qt::AlignJustify | Qt::SingleLine, "without warranty of any kind.");
  }

  p.drawImage(QRect(6, 5, 45, 15), QImage::fromMimeSource("defFactory"));
  p.end();

  bitBlt(this, 0, 0, &pix);
  QWidget::paintEvent(pe);
}

void FLConnectDBDialog::setRememberPasswd(bool on)
{
  rememberPasswd_ = on;
}

void FLConnectDBDialog::setOldApi(bool on)
{
  oldApi_ = on;
  printf("--> Setting old api: %s\n", on ? "True" : "False");
  FLSettings::writeEntry("application/oldApi", oldApi_);
}
