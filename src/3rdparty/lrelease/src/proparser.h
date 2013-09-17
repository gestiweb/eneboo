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

#ifndef PROPARSER_H
#define PROPARSER_H

#include "profileevaluator.h"

// Subclass it to intercept the logMessage method
class ProFileTranslationsScanner : public ProFileEvaluator {
public:
    ProFileTranslationsScanner(bool verbose) : ProFileEvaluator() 
    {
        m_verbose = verbose;
    }

    ProFile *queryProFile(const QString &filename) {
        return ProFileEvaluator::queryProFile(filename);
    }

private:
    /* reimp */
    void logMessage(const LogMessage &msg) {
        if (m_verbose && (msg.m_type == ProFileEvaluator::MT_DebugLevel1
            || msg.m_type == ProFileEvaluator::MT_Error)) {
            ProFileEvaluator::logMessage(msg);
        }
    }

private:
    bool m_verbose;
};

#endif

void removeDuplicates(QStringList *strings, bool alreadySorted = true);

bool evaluateProFile(const QString &fileName, bool verbose, QMap<QByteArray, QStringList> *varMap);

QStringList getListOfProfiles(const QStringList &proFiles, bool verbose);
