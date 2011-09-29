/****************************************************************************
** $Id: smtp.h,v 1.1.1.8 2006/05/07 17:31:13 chehrlic Exp $
**
** Copyright (C) 1992-2005 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef SMTP_H
#define SMTP_H

#include <qobject.h>
#include <qstring.h>

class QSocket;
class QTextStream;
class QDns;

class Smtp : public QObject
{
    Q_OBJECT

public:
    Smtp( const QString &from, const QString &to, const QString &m, const QString & mS = QString::null );
    ~Smtp();

    QString mailServer() {
        return mailServer_;
    }

signals:
    void status( const QString & );

private slots:
    void connectToMailServer( const QString & mS );
    void dnsLookupHelper();
    void readyRead();
    void connected();

private:
    enum State {
        Init,
        Mail,
        Rcpt,
        Data,
        Body,
        Quit,
        Close
    };

    QString message;
    QString from;
    QString rcpt;
    QSocket *socket;
    QTextStream * t;
    int state;
    QString response;
    QDns * mxLookup;
    QString mailServer_;
};

#endif
