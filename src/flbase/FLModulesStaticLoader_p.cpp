/***************************************************************************
 FLModulesStaticLoader_p.cpp
 -------------------
 begin                : 08/01/2011
 copyright            : (C) 2003-2011 by InfoSiAL S.L.
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

#include <qtable.h>
#include <qfiledialog.h>
#include <qlineedit.h>

#include "FLStaticLoaderUI.h"
#include "FLApplication.h"
#include "FLManagerModules.h"
#include "FLDataTable.h"
#include "FLModulesStaticLoader_p.h"
#include "FLSettings.h"

#define AQ_SETTINGS_KEY_BD(B) \
  QString key("StaticLoader/" + B + "/")

inline AQStaticDirInfo::AQStaticDirInfo(const QString &entry)
{
  active_ = entry.left(1).toUInt();
  path_ = entry.mid(1);
}

inline AQStaticDirInfo::AQStaticDirInfo(uint active,
                                        const QString &path)
{
  active_ = active;
  path_ = path;
}

AQStaticDirInfo *AQStaticBdInfo::findPath(const QString &path)
{
  for (AQStaticDirInfo *info = dirs_.first(); info; info = dirs_.next()) {
    if (info->path_ == path)
      return info;
  }
  return 0;
}

void AQStaticBdInfo::readSettings()
{
  AQ_SETTINGS_KEY_BD(bd_);

  enabled_ = FLSettings::readBoolEntry(key + "enabled", false);

  dirs_.clear();
  QStringList dirs(FLSettings::readListEntry(key + "dirs"));
  for (QStringList::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
    dirs_.append(new AQStaticDirInfo(*it));
}

void AQStaticBdInfo::writeSettings()
{
  AQ_SETTINGS_KEY_BD(bd_);

  FLSettings::writeEntry(key + "enabled", enabled_);

  QStringList dirs;
  QStringList activeDirs;
  for (AQStaticDirInfo *info = dirs_.first(); info; info = dirs_.next()) {
    dirs << QString::number(info->active_).append(info->path_);
    if (info->active_ == 1)
      activeDirs << info->path_;
  }
  FLSettings::writeEntry(key + "dirs", dirs);
  FLSettings::writeEntry(key + "activeDirs", activeDirs);
}

class FLStaticLoaderSetup : public FLStaticLoaderUI
{
  Q_OBJECT

public:

  FLStaticLoaderSetup(AQStaticBdInfo *b) :
    FLStaticLoaderUI(0, 0, true), b_(b) {
    pixOn->setHidden(true);
    tblDirs->verticalHeader()->setHidden(true);
    tblDirs->setLeftMargin(0);
    tblDirs->horizontalHeader()->setResizeEnabled(false);
    tblDirs->horizontalHeader()->setClickEnabled(false);
    tblDirs->setColumnStretchable(0, true);
    tblDirs->adjustColumn(1);

    load();

    connect(pbAddDir, SIGNAL(clicked()), this, SLOT(addDir()));
    connect(pbModDir, SIGNAL(clicked()), this, SLOT(modDir()));
    connect(pbDelDir, SIGNAL(clicked()), this, SLOT(delDir()));
    connect(chkEnabled, SIGNAL(toggled(bool)), this, SLOT(setEnabled(bool)));
  }

private slots:

  void load();
  void addDir();
  void modDir();
  void delDir();
  void setEnabled(bool on);
  void setChecked(bool on);

private:

  AQStaticBdInfo *b_;
};

void FLStaticLoaderSetup::load()
{
  b_->readSettings();

  lblBdTop->setText(b_->bd_);
  chkEnabled->setChecked(b_->enabled_);

  if (!b_->dirs_.isEmpty()) {
    int nRows = tblDirs->numRows();
    if (nRows > 0) {
      QMemArray<int> rows(nRows);
      for (int row = 0; row < nRows; ++row)
        rows[row] = row;
      tblDirs->removeRows(rows);
    }

    nRows = 0;
    tblDirs->setNumRows(b_->dirs_.count());

    for (AQStaticDirInfo *info = b_->dirs_.first(); info; info = b_->dirs_.next()) {
      tblDirs->setText(nRows, 0, info->path_);

      FLCheckBox *chk = new FLCheckBox(tblDirs, nRows);
      chk->setChecked(info->active_ == 1);
      connect(chk, SIGNAL(toggled(bool)), this, SLOT(setChecked(bool)));

      tblDirs->setCellWidget(nRows, 1, chk);
      ++nRows;
    }

    tblDirs->setCurrentCell(0, 0);
  }
}

void FLStaticLoaderSetup::addDir()
{
  int curRow = tblDirs->currentRow();
  QString dirInit(
    curRow != -1 ? QFileInfo(tblDirs->text(curRow, 0)).dirPath(true) : QString::null
  );
  QString dir(
    QFileDialog::getExistingDirectory(dirInit, this, "aq_static_loader_adddir",
                                      tr("Seleccione el directorio a insertar"))
  );
  if (!dir.isEmpty()) {
    int nRows = tblDirs->numRows();
    tblDirs->setNumRows(nRows + 1);
    tblDirs->setText(nRows, 0, dir);

    FLCheckBox *chk = new FLCheckBox(tblDirs, nRows);
    chk->setChecked(true);
    connect(chk, SIGNAL(toggled(bool)), this, SLOT(setChecked(bool)));

    tblDirs->setCellWidget(nRows, 1, chk);
    tblDirs->setCurrentCell(nRows, 0);

    b_->dirs_.append(new AQStaticDirInfo(1, dir));
  }
}

void FLStaticLoaderSetup::modDir()
{
  int curRow = tblDirs->currentRow();
  if (curRow == -1)
    return;
  QString dirInit(QFileInfo(tblDirs->text(curRow, 0)).dirPath(true));
  QString dir(
    QFileDialog::getExistingDirectory(dirInit, this, "aq_static_loader_moddir",
                                      tr("Seleccione el directorio a modificar"))
  );
  if (!dir.isEmpty()) {
    AQStaticDirInfo *info = b_->findPath(tblDirs->text(curRow, 0));
    if (info)
      info->path_ = dir;
    tblDirs->setText(curRow, 0, dir);
  }
}

void FLStaticLoaderSetup::delDir()
{
  int curRow = tblDirs->currentRow();
  if (curRow == -1)
    return;
  if (QMessageBox::No == QMessageBox::information(this, tr("Borrar registro"),
                                                  tr("El registro activo será borrado. ¿ Está seguro ?"),
                                                  QMessageBox::Yes,
                                                  QMessageBox::No |
                                                  QMessageBox::Default |
                                                  QMessageBox::Escape))
    return;
  AQStaticDirInfo *info = b_->findPath(tblDirs->text(curRow, 0));
  if (info)
    b_->dirs_.removeRef(info);
  tblDirs->removeRow(curRow);
}

void FLStaticLoaderSetup::setEnabled(bool on)
{
  b_->enabled_ = on;
}

void FLStaticLoaderSetup::setChecked(bool on)
{
  FLCheckBox *chk = ::qt_cast<FLCheckBox *>(sender());
  if (!chk)
    return;
  AQStaticDirInfo *info = b_->findPath(tblDirs->text(chk->row(), 0));
  if (info)
    info->active_ = (on ? 1 : 0);
}

void FLModulesStaticLoader::setup(AQStaticBdInfo *b)
{
  FLStaticLoaderSetup *diagSetup = new FLStaticLoaderSetup(b);
  if (QDialog::Accepted == diagSetup->exec())
    b->writeSettings();
  delete diagSetup;
}

class FLStaticLoaderWarning : public QObject
{
  Q_OBJECT

public:

  FLStaticLoaderWarning() : QObject(aqApp) {}

public slots:

  void popupWarnings();
  void updateScripts();

public:

  QStringList warns_;
  QStringList paths_;
};

void FLStaticLoaderWarning::popupWarnings()
{
  if (warns_.isEmpty() || aqApp->project()->interpreter()->isRunning())
    return;

  QString msg(
    QString::fromLatin1(
      "<p><img source=\"about.png\" align=\"right\">"
      "<b><u>CARGA ESTATICA ACTIVADA</u></b><br><br>"
      "<font face=\"Monospace\">"
    )
  );
  for (QStringList::const_iterator it = warns_.begin(); it != warns_.end(); ++it)
    msg += (*it) + "<br>";
  msg += "</font><br></p>";
  warns_.clear();
  aqApp->popupWarn(msg);
}

static inline QSScript *scriptBaseFileName(const QString &name)
{
  QPtrList<QSScript> scripts(aqApp->project()->scripts());
  QPtrListIterator<QSScript> it(scripts);
  QSScript *scr;
  while ((scr = it())) {
    if (scr->baseFileName() == name)
      return scr;
  }
  return 0;
}

void FLStaticLoaderWarning::updateScripts()
{
  if (paths_.isEmpty())
    return;

  QString n;
  QString *scrName = 0;
  QSScript *scr = 0;
  for (QStringList::const_iterator it = paths_.begin(); it != paths_.end(); ++it) {
    n = (*it).section(':', 0, 0);
    if (!n.endsWith(".qs"))
      continue;
    scr = scriptBaseFileName(n);
    if (!scr)
      continue;
    scr->setFileName((*it).section(':', 1, 1) + n);
  }
  paths_.clear();
}

//#define AQ_STATIC_LOADER_POPUP_WARN

FLStaticLoaderWarning *FLModulesStaticLoader::warn_ = 0;

QString FLModulesStaticLoader::content(const QString &n, AQStaticBdInfo *b)
{
  for (AQStaticDirInfo *info = b->dirs_.first(); info; info = b->dirs_.next()) {
    if (info->active_ && QFile::exists(info->path_ + n)) {
      if (!warn_)
        warn_ = new FLStaticLoaderWarning;
#ifdef AQ_STATIC_LOADER_POPUP_WARN
      if (warn_->warns_.isEmpty())
        QTimer::singleShot(500, warn_, SLOT(popupWarnings()));
#endif
      if (warn_->paths_.isEmpty())
        QTimer::singleShot(1500, warn_, SLOT(updateScripts()));
      QString msg(n.leftJustify(20, '_', true) +
                  " -> ..." +
                  info->path_.right(40));
      if (!warn_->warns_.contains(msg)) {
        warn_->warns_ << msg;
        warn_->paths_ << QString(n + ':' + info->path_);
        qWarning("CARGA ESTATICA ACTIVADA:" + n + " -> " + info->path_);
      }
      return FLManagerModules::contentFS(info->path_ + n);
    }
  }
  return QString::null;
}

#include "FLModulesStaticLoader_p.moc"
