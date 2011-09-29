/****************************************************************************
** $Id: smtp.cpp,v 1.1.1.9 2006/05/07 17:31:13 chehrlic Exp $
**
** Copyright (C) 1992-2005 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "smtp.h"

#include <qtextstream.h>
#include <qsocket.h>
#include <qdns.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qregexp.h>


Smtp::Smtp( const QString &from, const QString &to, const QString &m, const QString & mS ) : mailServer_( mS )
{
    socket = new QSocket( this );
    connect ( socket, SIGNAL( readyRead() ),
              this, SLOT( readyRead() ) );
    connect ( socket, SIGNAL( connected() ),
              this, SLOT( connected() ) );

    message = m;

    this->from = from;
    rcpt = to;

    if ( mailServer_.isEmpty() ) {
        mxLookup = new QDns( to.mid( to.find( '@' ) + 1 ), QDns::Mx );
        connect( mxLookup, SIGNAL(resultsReady()),
                 this, SLOT(dnsLookupHelper()) );
    } else
        connectToMailServer( mailServer_ );

    state = Init;
}


Smtp::~Smtp()
{
    delete t;
    delete socket;
}


void Smtp::connectToMailServer( const QString & mS )
{
    emit status( tr( "Conectando a %1" ).arg( mS ) );

    socket->connectToHost( mS, 25 );
    t = new QTextStream( socket );
}

void Smtp::dnsLookupHelper()
{
    QValueList<QDns::MailServer> s = mxLookup->mailServers();
    if ( s.isEmpty() ) {
        if ( !mxLookup->isWorking() )
            emit status( tr( "Error en el registro MX del DNS" ) );
        return;
    }

    connectToMailServer( s.first().name );
}


void Smtp::connected()
{
    emit status( tr( "Conectado a %1" ).arg( socket->peerName() ) );
}

void Smtp::readyRead()
{
    // SMTP is line-oriented
    if ( !socket->canReadLine() )
        return;

    QString responseLine;
    do {
        responseLine = socket->readLine();
        response += responseLine;
    } while ( socket->canReadLine() && responseLine[3] != ' ' );
    responseLine.truncate( 3 );

    if ( state == Init && responseLine[0] == '2' ) {
        // banner was okay, let's go on
        // *t << "HELO there\r\n";
        // helo should be a full qualified hostname, we can use doaminpart of from
        QString helo = from.mid( from.find( '@' ) + 1 );
        *t << "HELO " << helo << "\r\n";
        state = Mail;
    } else if ( state == Mail && responseLine[0] == '2' ) {
        // HELO response was okay (well, it has to be)
        *t << "MAIL FROM: <" << from << ">\r\n";
        state = Rcpt;
    } else if ( state == Rcpt && responseLine[0] == '2' ) {
        *t << "RCPT TO: <" << rcpt << ">\r\n";
        state = Data;
    } else if ( state == Data && responseLine[0] == '2' ) {
        *t << "DATA\r\n";
        state = Body;
    } else if ( state == Body && responseLine[0] == '3' ) {
        *t << message << ".\r\n";
        state = Quit;
    } else if ( state == Quit && responseLine[0] == '2' ) {
        *t << "QUIT\r\n";
        // here, we just close.
        state = Close;
        emit status( tr( "Envío completado" ) );
    } else if ( state == Close ) {
        deleteLater();
        return;
    } else {
        // something broke.
        QMessageBox::warning( qApp->activeWindow(),
                              tr( "Error SMTP" ),
                              tr( "Respuesta inesperada desde el servidor SMTP:\n\n" ) +
                              response );
        state = Close;
    }

    response = "";
}
