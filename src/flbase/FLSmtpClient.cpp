/***************************************************************************
                           FLSmtpClient.cpp
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

#include "FLSmtpClient.h"
#include "smtp.h"
#include "localmailfolder.h"
#include "headerclass.h"
#include "messageclass.h"
#include "indexclass.h"
#include "messagedevice.h"
#include "mimepart.h"
#include "charsets.h"
#include "FLUtil.h"

#include "AQConfig.h"
#include "FLDiskCache.h"

QMimeSourceFactory *FLSmtpClient::mimeSourceFactory_ = 0;

FLSmtpClient::FLSmtpClient(QObject *parent) :
  QObject(parent), from_(QString::null), to_(QString::null),
  subject_(QString::null), body_(QString::null), mailServer_(QString::null) {}

FLSmtpClient::~FLSmtpClient() {}

void FLSmtpClient::setFrom(const QString &from)
{
  from_ = from;
}

void FLSmtpClient::setTo(const QString &to)
{
  to_ = to;
}

void FLSmtpClient::setSubject(const QString &subject)
{
  subject_ = subject;
}

void FLSmtpClient::setBody(const QString &body)
{
  body_ = body;
}

void FLSmtpClient::addAttachment(const QString &attach, const QString &cid)
{
  if (QFile::exists(attach) && QFileInfo(attach).isReadable()) {
    if (!attachments_.contains(attach) && !attach.isEmpty()) {
      attachments_ << attach;
      if (!cid.isEmpty())
        mapAttachCid_.insert(attach, cid);
    }
  } else
    QMessageBox::warning(qApp->activeWindow(), tr("Error"),
                         tr("El fichero %1 no existe o no se puede leer\n\n").arg(attach));
}

void FLSmtpClient::addTextPart(const QString &text, const QString &mimeType)
{
  if (!text.isEmpty()) {
    textParts_ << text;
    textParts_ << mimeType;
  }
}

void FLSmtpClient::setMailServer(const QString &mS)
{
  mailServer_ = mS;
}

void FLSmtpClient::startSend()
{
  emit sendStarted();

  emit sendTotalSteps(attachments_.count() + 3);

  int step = 0;

  emit sendStepNumber(++step);

  emit status(tr("Componiendo mensaje"));

  // only send logo when configured
  if (FLUtil::readSettingEntry("email/sendMailLogo", "true").toBool()) {
    QString logo(FLUtil::readSettingEntry("email/mailLogo",
                                          AQ_DATA + "/logo_mail.png").toString());
    if (!QFile::exists(logo)) {
      logo = AQ_DISKCACHE_DIRPATH + "/logo.png";
      QPixmap::fromMimeSource("logo_abanq.png").save(logo, "PNG");
    }

    attachments_ << logo;
    mapAttachCid_.insert(logo, QFileInfo(logo).fileName() + "@3d8b627b6292");
  }

  QString headerStr = QString::fromLatin1("from: ") + from_ +
                      QString::fromLatin1("\nto: ") + to_ +
                      QString::fromLatin1("\nsubject: ") + subject_;

  CharSets chS;
  HeaderClass hdr(headerStr.local8Bit());
  MessageClass msg(hdr);
  LocalMailFolder folder(AQ_DISKCACHE_DIRPATH + '/');
  QString idMessage = folder.createMessage(body_.local8Bit(), QCString(), QDateTime::currentDateTime(), "", &msg);
  IndexClass *idx = folder.indexIDict()[ idMessage ];
  MessageDevice msgDev(idx);

  MimePart *part0 = new MimePart();
  part0->charset = chS.getDefaultCharset();

  part0->mimetype = (mimeType_.isEmpty() ? "text/plain" : mimeType_);
  part0->encoding = "quoted-printable";
  part0->name = "body";
  part0->type = "text";
  QByteArray partData0 = body_.local8Bit();
  part0->length = partData0.count();
  int npart0 = idx->addPart(part0, false);
  msgDev.setPartData(npart0, partData0);

  emit sendStepNumber(++step);

  for (uint i = 0; i < textParts_.count(); i += 2) {
    part0 = new MimePart();
    part0->charset = chS.getDefaultCharset();
    part0->mimetype = textParts_[i+1];
    part0->encoding = "quoted-printable";
    part0->type = "text";
    partData0 = textParts_[i].local8Bit();
    part0->length = partData0.count();
    npart0 = idx->addPart(part0, false);
    msgDev.setPartData(npart0, partData0);

    emit sendStepNumber(++step);
  }

  QStringList::Iterator itAttach;
  QString fileName;
  for (uint i = 0; i < attachments_.count(); i++) {
    itAttach = attachments_.at(i);
    fileName = *itAttach;

    emit status(tr("Adjuntando %1").arg(QFileInfo(fileName).fileName()));

    MimePart *part1 = new MimePart();
    part1->charset = chS.getDefaultCharset();
    QFile f(fileName);
    f.open(IO_Raw | IO_ReadOnly);
    QByteArray partData1 = f.readAll();
    f.close();
    part1->length = partData1.count();
    part1->name = QFileInfo(fileName).fileName();
    if (mapAttachCid_.contains(fileName))
      part1->cid = mapAttachCid_[fileName];
    const QMimeSource *mime = 0;
    QMimeSourceFactory *mimeFactory = mimeSourceFactory();
    if (mimeFactory) {
      mime = mimeFactory->data(fileName);
    } else {
      mime = QMimeSourceFactory::defaultFactory()->data(fileName);
    }
    if (mime && mime->format()) {
      part1->mimetype = mime->format();
      part1->encoding =  "base64";
    }
    int npart1 = idx->addPart(part1, false);
    msgDev.setPartData(npart1, partData1);

    emit sendStepNumber(++step);
  }

  emit status(tr("Buscando servidores de correo"));

  Smtp *smtp = new Smtp(from_, to_, msgDev.rfc822Header(), mailServer_);

  emit sendStepNumber(++step);

  connect(smtp, SIGNAL(destroyed()), this, SIGNAL(sendEnded()));
  connect(smtp, SIGNAL(status(const QString &)), this, SIGNAL(status(const QString &)));
}

void FLSmtpClient::setMimeType(const QString &mT)
{
  mimeType_ = mT;
}

QMimeSourceFactory *FLSmtpClient::mimeSourceFactory()
{
  if (!mimeSourceFactory_) {
    mimeSourceFactory_ = new QMimeSourceFactory();

    if (mimeSourceFactory_) {
      //application
      mimeSourceFactory_->setExtensionType("odt", "application/vnd.oasis.opendocument.text");
      mimeSourceFactory_->setExtensionType("ods", "application/vnd.oasis.opendocument.spreadsheet");
      mimeSourceFactory_->setExtensionType("mdb", "application/msaccess") ;
      mimeSourceFactory_->setExtensionType("doc", "application/msword") ;
      mimeSourceFactory_->setExtensionType("dot", "application/msword") ;
      mimeSourceFactory_->setExtensionType("xls", "application/msexcel") ;
      mimeSourceFactory_->setExtensionType("sdc", "application/vnd.stardivision.calc") ;
      mimeSourceFactory_->setExtensionType("sdw", "application/x-staroffice-words") ;
      mimeSourceFactory_->setExtensionType("sxw", "application/vnd.sun.xml.writer") ;
      mimeSourceFactory_->setExtensionType("sxc", "application/vnd.sun.xml.calc") ;
      mimeSourceFactory_->setExtensionType("bin", "application/octet-stream") ;
      mimeSourceFactory_->setExtensionType("pdf", "application/pdf") ;
      mimeSourceFactory_->setExtensionType("ps", "application/postscript") ;
      mimeSourceFactory_->setExtensionType("rar", "application/rar") ;
      mimeSourceFactory_->setExtensionType("rtf", "application/rtf") ;
      mimeSourceFactory_->setExtensionType("zip", "application/zip") ;
      mimeSourceFactory_->setExtensionType("tgz", "application/x-tgz") ;
      mimeSourceFactory_->setExtensionType("tar.gz", "application/x-tgz") ;
      mimeSourceFactory_->setExtensionType("pps", "application/vnd.ms-powerpoint") ;
      mimeSourceFactory_->setExtensionType("tar", "application/x-tar") ;
      mimeSourceFactory_->setExtensionType("php3", "application/x-httpd-php3") ;
      mimeSourceFactory_->setExtensionType("php4", "application/x-httpd-php4") ;
      mimeSourceFactory_->setExtensionType("latex", "x-latex") ;
      mimeSourceFactory_->setExtensionType("com", "application/x-msdos-program") ;
      mimeSourceFactory_->setExtensionType("exe", "application/x-msdos-program") ;
      mimeSourceFactory_->setExtensionType("bat", "application/x-msdos-program") ;
      mimeSourceFactory_->setExtensionType("dll", "application/x-msdos-program") ;
      mimeSourceFactory_->setExtensionType("o", "application/x-object") ;
      mimeSourceFactory_->setExtensionType("au", "audio/basic") ;
      mimeSourceFactory_->setExtensionType("snd", "audio/basic") ;
      mimeSourceFactory_->setExtensionType("mid", "audio/midi") ;
      mimeSourceFactory_->setExtensionType("midi", "audio/midi") ;
      mimeSourceFactory_->setExtensionType("mpga", "audio/mpeg") ;
      mimeSourceFactory_->setExtensionType("mpega", "audio/mpeg") ;
      mimeSourceFactory_->setExtensionType("mp2", "audio/mpeg") ;
      mimeSourceFactory_->setExtensionType("mp3", "audio/mpeg") ;
      mimeSourceFactory_->setExtensionType("wma", "audio/x-ms-wma") ;
      mimeSourceFactory_->setExtensionType("ra", "audio/x-pn-realaudio-plugin") ;
      mimeSourceFactory_->setExtensionType("rm", "audio/x-pn-realaudio-plugin") ;
      mimeSourceFactory_->setExtensionType("ram", "audio/x-pn-realaudio-plugin") ;
      mimeSourceFactory_->setExtensionType("ra", "audio/x-pn-realaudio-plugin") ;
      //image
      mimeSourceFactory_->setExtensionType("gif", "image/gif") ;
      mimeSourceFactory_->setExtensionType("jpeg", "image/jpeg") ;
      mimeSourceFactory_->setExtensionType("jpg", "image/jpg") ;
      mimeSourceFactory_->setExtensionType("jpe", "image/jpe") ;
      mimeSourceFactory_->setExtensionType("pcx", "image/pcx") ;
      mimeSourceFactory_->setExtensionType("png", "image/png") ;
      mimeSourceFactory_->setExtensionType("tiff", "image/tiff") ;
      mimeSourceFactory_->setExtensionType("tif", "image/tiff") ;
      mimeSourceFactory_->setExtensionType("ico", "image/x-icon") ;
      mimeSourceFactory_->setExtensionType("bmp", "image/x-ms-bmp") ;
      mimeSourceFactory_->setExtensionType("pnm", "image/x-portable-anymap") ;
      mimeSourceFactory_->setExtensionType("pbm", "image/x-portable-bitmap") ;
      mimeSourceFactory_->setExtensionType("pgm", "image/x-portable-graymap") ;
      mimeSourceFactory_->setExtensionType("ppm", "image/x-portable-pixmap") ;
      //text
      mimeSourceFactory_->setExtensionType("csv", "text/comma-separated-values") ;
      mimeSourceFactory_->setExtensionType("htm", "text/html") ;
      mimeSourceFactory_->setExtensionType("html", "text/html") ;
      mimeSourceFactory_->setExtensionType("shtml", "text/html") ;
      mimeSourceFactory_->setExtensionType("asc", "text/plain") ;
      mimeSourceFactory_->setExtensionType("txt", "text/plain") ;
      mimeSourceFactory_->setExtensionType("text", "text/plain") ;
      mimeSourceFactory_->setExtensionType("diff", "text/plain") ;
      mimeSourceFactory_->setExtensionType("pot", "text/plain") ;
      mimeSourceFactory_->setExtensionType("rtf", "text/richtext") ;
      mimeSourceFactory_->setExtensionType("h++", "text/x-c++hdr") ;
      mimeSourceFactory_->setExtensionType("hpp", "text/x-c++hdr") ;
      mimeSourceFactory_->setExtensionType("hxx", "text/x-c++hdr") ;
      mimeSourceFactory_->setExtensionType("hh", "text/x-c++hdr") ;
      mimeSourceFactory_->setExtensionType("c++", "text/x-c++src") ;
      mimeSourceFactory_->setExtensionType("cpp", "text/x-c++src") ;
      mimeSourceFactory_->setExtensionType("cxx", "text/x-c++src") ;
      mimeSourceFactory_->setExtensionType("cc", "text/x-c++src") ;
      mimeSourceFactory_->setExtensionType("h", "text/x-chdr") ;
      mimeSourceFactory_->setExtensionType("csh", "text/x-csh") ;
      mimeSourceFactory_->setExtensionType("c", "text/x-csrc") ;
      mimeSourceFactory_->setExtensionType("java", "text/x-java") ;
      mimeSourceFactory_->setExtensionType("moc", "text/x-moc") ;
      mimeSourceFactory_->setExtensionType("p", "text/x-pascal") ;
      mimeSourceFactory_->setExtensionType("pas", "text/x-pascal") ;
      mimeSourceFactory_->setExtensionType("vcs", "text/x-vcalendar") ;
      mimeSourceFactory_->setExtensionType("vcf", "text/x-vcard") ;
      //video
      mimeSourceFactory_->setExtensionType("mpeg", "video/mpeg") ;
      mimeSourceFactory_->setExtensionType("mpg", "video/mpeg") ;
      mimeSourceFactory_->setExtensionType("mpe", "video/mpeg") ;
      mimeSourceFactory_->setExtensionType("mp4", "video/mp4") ;
      mimeSourceFactory_->setExtensionType("qt", "video/quicktime") ;
      mimeSourceFactory_->setExtensionType("mov", "video/quicktime") ;
      mimeSourceFactory_->setExtensionType("wmv", "video/x-ms-wmv") ;
      mimeSourceFactory_->setExtensionType("avi", "video/x-msvideo") ;
      mimeSourceFactory_->setExtensionType("movie", "video/x-sgi-movie") ;
      mimeSourceFactory_->setExtensionType("ice", "x-conference/x-cooltalk") ;
    }
  }

  return mimeSourceFactory_;
}
