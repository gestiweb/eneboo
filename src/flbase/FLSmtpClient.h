/***************************************************************************
                            FLSmtpClient.h
                          -------------------
 begin                : vie dic 1 2006
 copyright            : (C) 2006 by InfoSiAL S.L.
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

#ifndef FLSMTPCLIENT_H
#define FLSMTPCLIENT_H

#include <qobject.h>
#include <qstringlist.h>
#include <qmime.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qpixmap.h>

#include "AQGlobal.h"

/**
Cliente SMTP para correo electrónico

Esta clase es utilizada para componer un mensaje de correo electrónico y
enviarlo mediante el protocolo SMTP.

@author InfoSiAL, S.L.
*/
class AQ_EXPORT FLSmtpClient : public QObject
{

  Q_OBJECT

public:

  /**
  constructor
  */
  FLSmtpClient(QObject *parent = 0);

  /**
  destructor
  */
  ~FLSmtpClient();

  /**
  Establece el remitente del mensaje

  @param from Dirección de correo del remitente
  */
  void setFrom(const QString &from);
  QString from() const {
    return from_;
  }

  /**
  Establece el contestar-a del mensaje

  @param replyTo Dirección de correo del a quien contestar
  */
  void setReplyTo(const QString &replyTo);
  QString replyTo() const {
    return replyTo_;
  }

  /**
  Establece el destinatario del mensaje

  @param to Dirección de correo del destinatario
  */
  void setTo(const QString &to);
  QString to() const {
    return to_;
  }

  /**
  Establede el destinatario copia

  @param cc Dirección de correo del destinatario copia.
            Pueden ser direcciones de correo separadas por ','.
  */
  void setCC(const QString &cc);
  QString cc() const {
    return cc_;
  }

  /**
  Establede el destinatario copia oculta

  @param bcc Dirección de correo del destinatario copia oculta.
             Pueden ser direcciones de correo separadas por ','.
  */
  void setBCC(const QString &cc);
  QString bcc() const {
    return bcc_;
  }

  /**
  Establece la organización

  @param org Nombre de la organizción que envia el mensaje.
  */
  void setOrganization(const QString &org);
  QString organization() const {
    return organization_;
  }

  /**
  Establece la prioridad del mensaje

  @param prio Prioridad del mensaje. Valor 1,2,3,4 o 5. 1: Mayor Prioridad.
  */
  void setPriority(int prio);
  int priority() const {
    return priority_;
  }

  /**
  Establece el asunto del mensaje

  @param subject Texto del asunto
  */
  void setSubject(const QString &subject);
  QString subject() const {
    return subject_;
  }

  /**
  Establece el cuerpo del mensaje

  @param body Texto del cuerpo
  */
  void setBody(const QString &body);
  QString body() const {
    return body_;
  }

  /**
  Adjunta un fichero al mensaje

  @param attach Ruta y nombre del fichero a adjuntar
  @param cid    Id para incrustar fichero en HTML
  */
  void addAttachment(const QString &attach, const QString &cid = QString::null);

  /**
  Añade una parte de texto al mensaje

  @param text     Texto a añadir
  @param mimeType Formato MIME del texto. Por defecto text/plain
  */
  void addTextPart(const QString &text, const QString &mimeType = "text/plain");

  /**
  Servidor de correo saliente.

  Si no se indica un servidor de correo saliente intentará averiguarlo
  mediante los registros MX de los servidores de DNS.

  @param mS Nombre o direccion IP del servidor de correo saliente
  */
  void setMailServer(const QString &mS);
  QString mailServer() const {
    return mailServer_;
  }

  /**
  Formato MIME del cuerpo del mensaje

  @param mT Nombre del formato
  */
  void setMimeType(const QString &mT);
  QString mimeType() const {
    return mimeType_;
  }

  /**
  Establecer el puerto de conexión. Por defecto el 25
  */
  void setPort(int p);
  int port() const {
    return port_;
  }

  /**
  Devuelve el último mensaje de estado
  */
  QString lastStatusMsg() const {
    return statusMsg_;
  }

  /**
  Devuelve el último código de estado
  */
  int lastStateCode() const {
    return stateCode_;
  }

  /**
  Login
  */
  void setUser(const QString &user);
  QString user() const;
  void setPassword(const QString &password);
  QString password() const;
  void setConnectionType(int ct);
  int connectionType() const;
  void setAuthMethod(int method);
  int authMethod() const;

public slots:

  /**
  Inicia el envío del mensaje
  */
  void startSend();

private slots:

  /**
  Cambia del estado del envío
  */
  void changeStatus(const QString &statusMsg, int stateCode);

signals:

  /**
  Informa del estado del envío
  */
  void status(const QString &);

  /**
  Indica que se ha iniciado el envío del mensaje
  */
  void sendStarted();

  /**
  Indica que se ha finalizado el envío del mensaje
  */
  void sendEnded();

  /**
  Indica los pasos totales que se ejecutarán en el proceso de envío. Usado para diálogos de progreso.
  */
  void sendTotalSteps(int);

  /**
  Indica el número de paso que se van a ejecutar. Usado para diálogos de progreso.
  */
  void sendStepNumber(int);

  /**
  Informa del estado del envío, incluyendo código de estado
  */
  void statusChanged(const QString &, int);

protected:

  /**
  Mantiene la fábrica de fuentes MIME para ficheros
  */
  static QMimeSourceFactory *mimeSourceFactory_;

  /**
  Crea la fábrica de fuentes MIME para ficheros
  */
  static QMimeSourceFactory *mimeSourceFactory();

private:

  enum State {
    Init,
    Mail,
    Rcpt,
    Data,
    Body,
    Quit,
    Close,
    SmtpError,
    Connecting,
    Connected,
    MxDnsError,
    SendOk,
    SocketError,
    Composing,
    Attach,
    AttachError,
    ServerError,    // 4xx smtp error
    ClientError,    // 5xx smtp error
    StartTTLS,
    WaitingForSTARTTLS,
    SendAuthPlain,
    SendAuthLogin,
    WaitingForAuthPlain,
    WaitingForAuthLogin,
    WaitingForUser,
    WaitingForPass
  };

  enum AuthMethod {
    NoAuth,
    AuthPlain,
    AuthLogin
  };

  enum ConnectionType {
    TcpConnection,
    SslConnection,
    TlsConnection       // STARTTLS
  };

  /**
  Remitente del mensaje
  */
  QString from_;

  /**
  Destinatario del mensaje
  */
  QString to_;

  /**
  Asunto del mensaje
  */
  QString subject_;

  /**
  Cuerpo del mensaje
  */
  QString body_;

  /**
  Lista de rutas y nombres de ficheros adjuntos
  */
  QStringList attachments_;

  /**
  Mapa de adjuntos con sus identificadores
  */
  QMap<QString, QString> mapAttachCid_;

  /**
  Servidor de correo saliente
  */
  QString mailServer_;

  /**
  Formato MIME
  */
  QString mimeType_;

  /**
  Lista de partes de texto
  */
  QStringList textParts_;

  /**
  Estados del envío
  */
  QString statusMsg_;
  int stateCode_;

  /**
  El contestar-a
  */
  QString replyTo_;

  /**
  El remite copia. Pueden ser direcciones de correo separadas por ','
  */
  QString cc_;

  /**
  El remite copia oculta. Pueden ser direcciones de correo separadas por ','
  */
  QString bcc_;

  /**
  La organización
  */
  QString organization_;

  /**
  La prioridad del mensaje. Valor 1,2,3,4 o 5. 1: Mayor Prioridad.
  */
  int priority_;

  /**
  Puerto de conexión
  */
  int port_;

  /**
  Login
  */
  QString user_;
  QString password_;
  FLSmtpClient::ConnectionType connectionType_;
  FLSmtpClient::AuthMethod authMethod_;
};

#endif
