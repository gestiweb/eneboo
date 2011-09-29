/***************************************************************************
 AQSObjectFactory.cpp
 -------------------
 begin                : 29/01/2011
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

#include "AQSObjectFactory.h"
#include "AQS_g.h"
#include "AQSObjectFactory_p.h"
#include "FLApplication.h"

static AQS globalAQS_;
AQS *globalAQS = &globalAQS_;
QSInterpreter *globalAQSInterpreter = 0;
AQSEvent *AQSObject::filterEvent_ = 0;
QSArgumentList *AQSObject::filterArgs_ = 0;

AQSObjectFactory::AQSObjectFactory()
{
  d = new AQSObjectFactoryPrivate;

  globalAQSInterpreter = aqApp->project()->interpreter();

  AQ_GEN_REG_CLASS

  registerClass("AQS", "AQS", globalAQS);
  registerClass("aqApp", "AQSApplication", new AQSApplication(aqApp));
  registerClass("QSProject", "AQSSProject", new AQSSProject(aqApp->project()));
  registerClass("AQSql", QString::null, new AQSql);
}

AQSObjectFactory::~AQSObjectFactory()
{
  registerClass("AQS", QString::null, new QObject);
  delete d;
}

QObject *AQSObjectFactory::create(const QString &className,
                                  const QSArgumentList &arguments,
                                  QObject *context)
{
  d->candidateCtors_.clear();
  QObject *ret = staticCreate(className, arguments, context, &d->candidateCtors_);

  if (!ret || !d->candidateCtors_.isEmpty()) {
    QString errMsg(
      QString::fromLatin1("Failed to construct %1(%2)")
      .arg(className)
      .arg(argsSignature(arguments))
    );

    if (!d->candidateCtors_.isEmpty()) {
      errMsg.append(QString::fromLatin1("\n"));
      errMsg.append(QString::fromLatin1("Candidates:\n"));

      QString sgt("  %1(%2)\n");
      for (QMap<int, QStringList>::const_iterator it = d->candidateCtors_.begin();
           it != d->candidateCtors_.end(); ++it) {
        QStringList cdts(*it);
        for (QStringList::const_iterator it2 = cdts.begin(); it2 != cdts.end(); ++it2) {
          errMsg.append(sgt.arg(className).arg(*it2));
        }
      }
    }

    throwError(errMsg);
    qWarning(errMsg);
  }

  return ret;
}

QObject *AQSObjectFactory::staticCreate(const QString &className,
                                        const QSArgumentList &arguments,
                                        QObject *context,
                                        QMap<int, QStringList> *candidateCtors)
{
  Q_UNUSED(context);

  AQ_GEN_CRE_CLASS

  return 0;
}

bool AQS::ColorDrag_decode(QMimeSource *e, AQSColor *col)
{
  return QColorDrag::decode(e, *col);
}

bool AQS::ImageDrag_decode(QMimeSource *e, AQSImage *img)
{
  return QImageDrag::decode(e, *img);
}

bool AQS::ImageDrag_decode(QMimeSource *e, AQSPixmap *pm)
{
  return QImageDrag::decode(e, *pm);
}

