/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "proparser.h"
#include "profileevaluator.h"
#include <stdio.h>
#include <proreader.h>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QStringList>

// assumes that the list is sorted (or more correctly, that all equal elements are grouped together).
void removeDuplicates(QStringList *strings, bool alreadySorted /*= true*/)
{
    if (!alreadySorted) {
        strings->sort();
    }
    QString prev;
    QStringList::iterator it = strings->begin();
    while (it != strings->end()) {
        if (*it == prev) {
            it = strings->erase(it);
        }else{
            prev = *it;
            ++it;
        }
    }
}

bool evaluateProFile(const QString &fileName, bool verbose,QMap<QByteArray, QStringList> *varMap)
{
    bool ok = true;

    QStringList sourceFiles;
    QString codecForTr;
    QString codecForSource;
    QStringList tsFileNames;

    ProFileTranslationsScanner *visitor = new ProFileTranslationsScanner(verbose);
    QFileInfo fi(fileName);
    QDir rootPath;
    ok = fi.exists();
    if (ok) {
        rootPath.setPath(fi.absolutePath());
        ProFile *pro = visitor->queryProFile(fi.absoluteFilePath());
        if (!pro)
            ok = false;
        else
            ok = pro->Accept(visitor);
    }
    if (ok) {
        // app/lib template
        sourceFiles += visitor->absFileNames(QLatin1String("SOURCES"));
        sourceFiles += visitor->absFileNames(QLatin1String("HEADERS"));
        QStringList tsFiles = visitor->values(QLatin1String("TRANSLATIONS"));
        for (int i = 0; i < tsFiles.count(); ++i) {
            tsFileNames << rootPath.absoluteFilePath(tsFiles.at(i));
        }

        QStringList trcodec = visitor->values(QLatin1String("CODEC"))
            + visitor->values(QLatin1String("DEFAULTCODEC"))
            + visitor->values(QLatin1String("CODECFORTR"));
        if (!trcodec.isEmpty())
            codecForTr = trcodec.last();

        QStringList srccodec = visitor->values(QLatin1String("CODECFORSRC"));
        if (!srccodec.isEmpty()) 
            codecForSource = srccodec.last();
        
        QStringList forms = visitor->absFileNames(QLatin1String("INTERFACES"))
            + visitor->absFileNames(QLatin1String("FORMS"))
            + visitor->absFileNames(QLatin1String("FORMS3"));
        sourceFiles << forms;

    }
    if (ok) {
        removeDuplicates(&sourceFiles, false);
        removeDuplicates(&tsFileNames, false);

        varMap->insert("SOURCES", sourceFiles);
        varMap->insert("CODECFORTR", QStringList() << codecForTr);
        varMap->insert("CODECFORSRC", QStringList() << codecForSource);
        varMap->insert("TRANSLATIONS", tsFileNames);
    }
    return ok;

}

QStringList getListOfProfiles(const QStringList &proFiles, bool verbose)
{
    QStringList profileList = proFiles;
    bool ok = true;

    int i = 0;
    while( i < profileList.count() ) {
        QFileInfo fi(profileList.at(i));
        ProFileTranslationsScanner *visitor = new ProFileTranslationsScanner(verbose);
        ok = fi.exists();
        if (ok) {
            ProFile *pro = visitor->queryProFile(fi.absoluteFilePath());
            if (!pro)
                ok = false;
            else
                ok = pro->Accept(visitor);

            if (ok && visitor->templateType() == ProFileEvaluator::TT_Subdirs) {
                QStringList subdirs = visitor->values(QLatin1String("SUBDIRS"));
                for (int is = 0; is < subdirs.count(); ++is) {
                    QString subdir = subdirs[is];
                    QDir dir( subdir );
                    QStringList profiles = dir.entryList(QStringList() << QLatin1String("*.pro"));
                    if (profiles.count())
                        profileList << subdir + QLatin1Char('/') + profiles[0];
                }
                profileList.removeAt(i);
            } else {
                ++i;
            }
        }
        delete visitor;
    }
    if (!ok)
        profileList.clear();
    return profileList;
}

