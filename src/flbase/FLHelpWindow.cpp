/***************************************************************************
      FLHelpWindow.cpp  -  Modificaciones para adaptar a AbanQ
                           Iconos incluido en código
                           Mensajes originales en español
                           Modifiaciones en barra de menus
                           Historial y favoritos se almacenan en el directorio
                           HOME del usuario
                         -------------------
begin                : Mon May 13 2002
copyright            : (C) 2002-2005 by InfoSiAL S.L.
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
/****************************************************************************
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qstatusbar.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qiconset.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstylesheet.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qapplication.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qobjectlist.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qprinter.h>
#include <qsimplerichtext.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>

#include <ctype.h>

#include "FLDiskCache.h"
#include "FLHelpWindow.h"

FLHelpWindow::FLHelpWindow(const QString &home_,
                           const QString &_path,
                           QWidget *parent,
                           const char *name) :
  QMainWindow(parent, name, WDestructiveClose),
  pathCombo(0), selectedURL()
{
  readHistory();
  readBookmarks();

  browser = new QTextBrowser(this);

  browser->mimeSourceFactory() ->setFilePath(_path);
  browser->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  connect(browser, SIGNAL(textChanged()), this, SLOT(textChanged()));

  setCentralWidget(browser);

  if (!home_.isEmpty())
    browser->setSource(home_);

  connect(browser, SIGNAL(highlighted(const QString &)), statusBar(),
          SLOT(message(const QString &)));

  resize(640, 700);

  QPopupMenu *file = new QPopupMenu(this);

  file->insertItem(tr("&Imprimir"), this, SLOT(print()), CTRL + Key_I);
  file->insertSeparator();
  file->insertItem(tr("&Cerrar"), this, SLOT(close()), Key_Escape);

  // The same three icons are used twice each.
  QIconSet icon_back(QPixmap::fromMimeSource("previous.png"));
  QIconSet icon_forward(QPixmap::fromMimeSource("next.png"));
  QIconSet icon_home(QPixmap::fromMimeSource("home.png"));

  QPopupMenu *go = new QPopupMenu(this);

  backwardId =
    go->insertItem(icon_back, tr("&Atrás"), browser, SLOT(backward()),
                   CTRL + Key_Left);
  forwardId =
    go->insertItem(icon_forward, tr("&Adelante"), browser,
                   SLOT(forward()), CTRL + Key_Right);
  go->insertItem(icon_home, tr("&Inicio"), browser, SLOT(home()));

  QPopupMenu *help = new QPopupMenu(this);

  help->insertItem(tr("&Acerca ..."), this, SLOT(about()));
  help->insertItem(tr("Acerca de &Qt ..."), this, SLOT(aboutQt()));

  hist = new QPopupMenu(this);
  QStringList::Iterator it = history.begin();
  for (; it != history.end(); ++it)
    mHistory[ hist->insertItem(*it)] = *it;
  connect(hist, SIGNAL(activated(int)), this, SLOT(histChosen(int)));

  bookm = new QPopupMenu(this);
  bookm->insertItem(tr("Agregar Favorito"), this, SLOT(addBookmark()));
  bookm->insertSeparator();

  QStringList::Iterator it2 = bookmarks.begin();
  for (; it2 != bookmarks.end(); ++it2)
    mBookmarks[ bookm->insertItem(*it2)] = *it2;
  connect(bookm, SIGNAL(activated(int)), this, SLOT(bookmChosen(int)));

  menuBar() ->insertItem(tr("&Archivo"), file);
  menuBar() ->insertItem(tr("&Ir"), go);
  menuBar() ->insertItem(tr("Historial"), hist);
  menuBar() ->insertItem(tr("Favoritos"), bookm);
  menuBar() ->insertSeparator();
  menuBar() ->insertItem(tr("&Ayuda"), help);

  menuBar() ->setItemEnabled(forwardId, FALSE);
  menuBar() ->setItemEnabled(backwardId, FALSE);
  connect(browser, SIGNAL(backwardAvailable(bool)), this,
          SLOT(setBackwardAvailable(bool)));
  connect(browser, SIGNAL(forwardAvailable(bool)), this,
          SLOT(setForwardAvailable(bool)));


  QToolBar *toolbar = new QToolBar(this);

  addToolBar(toolbar, "Toolbar");
  QToolButton *button;

  button =
    new QToolButton(icon_back, tr("Atrás"), "", browser, SLOT(backward()),
                    toolbar);
  connect(browser, SIGNAL(backwardAvailable(bool)), button,
          SLOT(setEnabled(bool)));
  button->setEnabled(FALSE);
  button =
    new QToolButton(icon_forward, tr("Adelante"), "", browser,
                    SLOT(forward()), toolbar);
  connect(browser, SIGNAL(forwardAvailable(bool)), button,
          SLOT(setEnabled(bool)));
  button->setEnabled(FALSE);
  button =
    new QToolButton(icon_home, tr("Inicio"), "", browser, SLOT(home()),
                    toolbar);

  toolbar->addSeparator();

  pathCombo = new QComboBox(TRUE, toolbar);
  connect(pathCombo, SIGNAL(activated(const QString &)), this,
          SLOT(pathSelected(const QString &)));
  toolbar->setStretchableWidget(pathCombo);
  setRightJustification(TRUE);
  setDockEnabled(DockLeft, FALSE);
  setDockEnabled(DockRight, FALSE);

  pathCombo->insertItem(home_);
  browser->setFocus();
}


void FLHelpWindow::setBackwardAvailable(bool b)
{
  menuBar() ->setItemEnabled(backwardId, b);
}

void FLHelpWindow::setForwardAvailable(bool b)
{
  menuBar() ->setItemEnabled(forwardId, b);
}


void FLHelpWindow::textChanged()
{
  if (browser->documentTitle().isNull())
    setCaption(tr("Ayuda - ") + browser->context());
  else
    setCaption(tr("Ayuda - ") + browser->documentTitle());

  selectedURL = browser->context();

  if (!selectedURL.isEmpty() && pathCombo) {
    bool exists = FALSE;
    int i;

    for (i = 0; i < pathCombo->count(); ++i) {
      if (pathCombo->text(i) == selectedURL) {
        exists = TRUE;
        break;
      }
    }
    if (!exists) {
      pathCombo->insertItem(selectedURL, 0);
      pathCombo->setCurrentItem(0);
      mHistory[ hist->insertItem(selectedURL)] = selectedURL;
    } else
      pathCombo->setCurrentItem(i);
    selectedURL = QString::null;
  }
}

FLHelpWindow::~FLHelpWindow()
{
  history.clear();
  QMap < int, QString >::Iterator it = mHistory.begin();

  for (; it != mHistory.end(); ++it)
    history.append(*it);

  QString historial(AQ_DISKCACHE_DIRPATH + "/.FLhistory");
  QFile f(historial);

  f.open(IO_WriteOnly);
  QDataStream s(&f);

  s << history;
  f.close();

  bookmarks.clear();
  QMap < int, QString >::Iterator it2 = mBookmarks.begin();

  for (; it2 != mBookmarks.end(); ++it2)
    bookmarks.append(*it2);

  QString bookmarks(AQ_DISKCACHE_DIRPATH + "/.FLbookmarks");
                    QFile f2(bookmarks);

                    f2.open(IO_WriteOnly);
                    QDataStream s2(&f2);

                    s2 << bookmarks;
                    f2.close();
                  }

                  void FLHelpWindow::about()
{
  QMessageBox::about(this, "HelpViewer Example",
                     "<p>This example implements a simple HTML help viewer "
                     "using Qt's rich text capabilities</p>"
                     "<p>It's just about 100 lines of C++ code, so don't expect too much :-)</p>");
}


void FLHelpWindow::aboutQt()
{
  QMessageBox::aboutQt(this, "QBrowser");
}

void FLHelpWindow::openFile()
{
#ifndef QT_NO_FILEDIALOG
  QString fn =
    QFileDialog::getOpenFileName(QString::null, QString::null, this);
  if (!fn.isEmpty())
    browser->setSource(fn);
#endif
}

void FLHelpWindow::newWindow()
{
  (new FLHelpWindow(browser->source(), "qbrowser")) ->show();
}

void FLHelpWindow::print()
{
#ifndef QT_NO_PRINTER
  QPrinter printer;

  printer.setFullPage(TRUE);
  if (printer.setup(this)) {
    QPainter p(&printer);
    QPaintDeviceMetrics metrics(p.device());
    int dpix = metrics.logicalDpiX();
    int dpiy = metrics.logicalDpiY();
    const int margin = 72; // pt
    QRect body(margin * dpix / 72, margin * dpiy / 72,
               metrics.width() - margin * dpix / 72 * 2,
               metrics.height() - margin * dpiy / 72 * 2);
    QSimpleRichText richText(browser->text(), QFont(), browser->context(),
                             browser->styleSheet(),
                             browser->mimeSourceFactory(), body.height());
    richText.setWidth(&p, body.width());
    QRect view(body);
    int page = 1;

    do {
      richText.draw(&p, body.left(), body.top(), view, colorGroup());
      view.moveBy(0, body.height());
      p.translate(0, -body.height());
      p.drawText(view.right() -
                 p.fontMetrics().width(QString::number(page)),
                 view.bottom() + p.fontMetrics().ascent() + 5,
                 QString::number(page));
      if (view.top() >= richText.height())
        break;
      printer.newPage();
      page++;
    } while (TRUE);
  }
#endif
}

void FLHelpWindow::pathSelected(const QString &_path)
{
  browser->setSource(_path);
  QMap < int, QString >::Iterator it = mHistory.begin();
  bool exists = FALSE;

  for (; it != mHistory.end(); ++it) {
    if (*it == _path) {
      exists = TRUE;
      break;
    }
  }
  if (!exists)
    mHistory[ hist->insertItem(_path)] = _path;
}

void FLHelpWindow::readHistory()
{
  QString historial(AQ_DISKCACHE_DIRPATH + "/.FLhistory");

  if (QFile::exists(historial)) {
  QFile f(historial);

    f.open(IO_ReadOnly);
    QDataStream s(&f);

    s >> history;
    f.close();
    while (history.count() > 20)
      history.remove(history.begin());
  }
}

void FLHelpWindow::readBookmarks()
{
  QString bookmarks(AQ_DISKCACHE_DIRPATH + "/.FLbookmarks");

  if (QFile::exists(bookmarks)) {
  QFile f(bookmarks);

    f.open(IO_ReadOnly);
    QDataStream s(&f);

    s >> bookmarks;
    f.close();
  }
}

void FLHelpWindow::histChosen(int i)
{
  if (mHistory.contains(i))
    browser->setSource(mHistory[ i ]);
}

void FLHelpWindow::bookmChosen(int i)
{
  if (mBookmarks.contains(i))
    browser->setSource(mBookmarks[ i ]);
}

void FLHelpWindow::addBookmark()
{
  mBookmarks[ bookm->insertItem(caption())] = browser->context();
}
