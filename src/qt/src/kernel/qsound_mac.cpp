/****************************************************************************
** $Id: qt/qsound_mac.cpp   3.3.8   edited Jan 11 14:38 $
**
** Implementation of QSound class and QAuServer internal class
**
** Created : 001019
**
** Copyright (C) 1992-2007 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Macintosh may use this file in accordance with the Qt Commercial
** License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#define QT_CLEAN_NAMESPACE

#include "qsound.h"

#ifndef QT_NO_SOUND

#include <qdir.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qapplication.h>
#include "qt_mac.h"
#undef check

/*****************************************************************************
  External functions
 *****************************************************************************/
OSErr qt_mac_create_fsspec(const QString &file, FSSpec *spec); //qglobal.cpp


/*****************************************************************************
  Internal variables and functions
 *****************************************************************************/
class QAuServerMac : public QAuServer {
    Q_OBJECT

public:
    QAuServerMac(QObject* parent);
    ~QAuServerMac();

    void play(const QString& filename);
    void play(QSound *s);
    void stop(QSound*);
    bool okay() { return TRUE; }

private:
    QPixmap *offscreen;
    friend class QAuServerMacCleanupHandler;
};
class QAuServerMacCleanupHandler : public QObject {
private:
    int loops;
    QAuServerMac *qsound_server;
    QSound *qsound;
    Movie movie;
    QTCallBack callback; //QT as in Quicktime :)
    static QTCallBackUPP movieCallbackProc;
    static QPtrList<QAuServerMacCleanupHandler> cleanups;
    static void movieEnd(QTCallBack, long data) {
	QAuServerMacCleanupHandler *iteration = (QAuServerMacCleanupHandler*)data;
	if((--iteration->loops) <= 0) {
            iteration->deleteLater();
	    return;
	}
	if(iteration->qsound)
	    iteration->qsound_server->decLoop(iteration->qsound);
	GoToBeginningOfMovie(iteration->movie);
	CallMeWhen(iteration->callback, movieCallbackProc, (long)iteration, triggerAtStop, 0, 0);
	StartMovie(iteration->movie); //play it again Sam..
    }
    void init(int l, Movie m) {
	movie = m;
	loops = l;
	callback = NewCallBack(GetMovieTimeBase(movie), callBackAtExtremes|callBackAtInterrupt);
	if(!movieCallbackProc)
	    movieCallbackProc = NewQTCallBackUPP(QTCallBackProcPtr(movieEnd));
	CallMeWhen(callback, movieCallbackProc, (long)this, triggerAtStop, 0, 0);
	cleanups.append(this);
    }

public:
    QAuServerMacCleanupHandler(QAuServerMac *ss, QSound *s, Movie m) :
	qsound_server(ss), qsound(s) { init(s->loopsRemaining(), m); }
    QAuServerMacCleanupHandler(int l, Movie m) : qsound_server(0), qsound(0) { init(l, m); }
    ~QAuServerMacCleanupHandler() {
	cleanups.remove(this);
	if(callback) {
	    CancelCallBack(callback);
	    DisposeCallBack(callback);
	}
	if(movie) {
            StopMovie(movie);
	    DisposeMovie(movie);
	}
    }
    static void cleanup() {
	while(QAuServerMacCleanupHandler *cu = cleanups.first())
	    delete cu;
	if(movieCallbackProc) {
	    DisposeQTCallBackUPP(movieCallbackProc);
	    movieCallbackProc = 0;
	}
    }
    static void stop(QSound *s) {
	if(!s)
	    return;
	for(QPtrList<QAuServerMacCleanupHandler>::Iterator it = cleanups.begin(); it != cleanups.end(); ++it) {
	    if((*it)->qsound == s) {
		delete (*it); //the destructor removes it..
		break;
	    }
	}
    }

};
QTCallBackUPP QAuServerMacCleanupHandler::movieCallbackProc = 0;
QPtrList<QAuServerMacCleanupHandler> QAuServerMacCleanupHandler::cleanups;

static int servers = 0;
QAuServerMac::QAuServerMac(QObject* parent) : QAuServer(parent,"Mac Audio Server")
{
    if(!servers++)
	EnterMovies();
    offscreen = new QPixmap(1, 1); //what should the size be? FIXME
}

QAuServerMac::~QAuServerMac()
{
    if(!(--servers)) {
	QAuServerMacCleanupHandler::cleanup();
	ExitMovies();
    }
}

static Movie get_movie(const QString &filename, QPixmap *offscreen)
{
    FSSpec fileSpec;
    if(qt_mac_create_fsspec(filename, &fileSpec) != noErr)
	return NULL;
    Movie aMovie = nil;
    short movieResFile;
    if(OpenMovieFile(&fileSpec, &movieResFile, fsCurPerm) != noErr)
	return NULL;
    if(NewMovieFromFile(&aMovie, movieResFile, 0, 0, newMovieActive, 0) != noErr)
	return NULL;
    SetMovieGWorld(aMovie, (GWorldPtr)offscreen->handle(), 0); //just a temporary offscreen
    CloseMovieFile(movieResFile);
    return aMovie;
}

void QAuServerMac::play(const QString& filename)
{
    if(Movie movie = get_movie(filename, offscreen)) {
        GoToBeginningOfMovie(movie);
        SetMovieVolume(movie, kFullVolume);
        (void)new QAuServerMacCleanupHandler(1, movie); //this will handle the cleanup
        StartMovie(movie);
    }
}

void QAuServerMac::play(QSound *s)
{
    if(Movie movie = get_movie(s->fileName(), offscreen)) {
        GoToBeginningOfMovie(movie);
        SetMovieVolume(movie, kFullVolume);
        (void)new QAuServerMacCleanupHandler(this, s, movie); //this will handle the cleanup
        StartMovie(movie);
    }
}

void QAuServerMac::stop(QSound *s)
{
    QAuServerMacCleanupHandler::stop(s);
}

QAuServer* qt_new_audio_server()
{
    return new QAuServerMac(qApp);
}

#include "qsound_mac.moc"

#endif // QT_NO_SOUND
