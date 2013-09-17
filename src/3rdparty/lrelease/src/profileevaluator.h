/****************************************************************************
**
** Copyright (C) 2006-2007 Trolltech ASA. All rights reserved.
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

#ifndef PROFILEEVALUATOR_H
#define PROFILEEVALUATOR_H
#include "proitems.h"
#include "abstractproitemvisitor.h"
#include <QtCore/QIODevice>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QStack>

class ProFile;

class ProFileEvaluator : public AbstractProItemVisitor {
public:
    typedef enum {
        TT_Unknown = 0,
        TT_Application,
        TT_Library,
        TT_Subdirs
    } TemplateType;

    typedef enum {
        MT_Error            = 1,    // Catastrophic error, processing stops
        MT_DebugLevel1,             // serious errors
        MT_DebugLevel2,             // default
        MT_DebugLevel3,             // less serious
        MT_ProMessage       = 100,  // output of the profiles' message(string).
        MT_ProError,                // output of error(string). In this implementation, processing will not stop.
    } MessageType;

    struct LogMessage {
        QString m_msg;
        QString m_filename;
        int m_linenumber;
        MessageType m_type;
    };

    ProFileEvaluator();
    ~ProFileEvaluator();

    /* 
     * INHERITED from AbstractProItemVisitor
     */
    bool visitBeginProBlock(ProBlock * block);
    bool visitEndProBlock(ProBlock * block);
    bool visitBeginProVariable(ProVariable *variable);    
    bool visitEndProVariable(ProVariable * /*variable*/);
    bool visitBeginProFile(ProFile * value);
    bool visitEndProFile(ProFile * value);
    bool visitProValue(ProValue *value);
    bool visitProFunction(ProFunction *function);
    bool visitProOperator(ProOperator * oper);
    bool visitProCondition(ProCondition * cond);

    ProFileEvaluator::TemplateType templateType();
    bool contains(const QString &variableName) const;
    QStringList values(const QString &variableName) const;
    QStringList absFileNames(const QString &variableName);

protected:
    virtual ProFile *queryProFile(const QString &filename);
    virtual void releaseProFile(ProFile *pro);
    virtual QString propertyValue(const QString &val) const;

    virtual void logMessage(const LogMessage &msg);

private:
    void logMessage(const QString &msg, MessageType mt = MT_DebugLevel2);
    void logMessage(MessageType mt, const char *msg, ...);
    QString expandVariableReferences(const QString &value);
    QString evaluateExpandFunction(const QByteArray &func, const QString &arguments);

    bool evaluateConditionalFunction(const QByteArray &function, const QString &arguments, bool *result);
    bool evaluateFile(const QString &fileName, bool *result);
    bool evaluateFeatureFile(const QString &fileName, bool *result);

    bool isActiveConfig(const QByteArray &config, bool regex = false);

    QString currentFileName() const;
    QString getcwd() const;
    ProFile *currentProFile() const;

    QStringList qmake_feature_paths();
    QByteArray m_lastVarName;
    ProVariable::VariableOperator m_variableOperator;

    QMap<QByteArray, QStringList> m_valuemap;       // VariableName must be us-ascii, the content however can be non-us-ascii.
    QStack<ProFile*> m_profileStack;                // To handle 'include(a.pri), so we can track back to 'a.pro' when finished with 'a.pri'
    int m_lineNo;                                   // Error reporting
    QString m_oldPath;                              // To restore the current path to the path
    QString m_origfile;

    bool m_condition;
    bool m_invertNext;

}; //class ProFileEvaluator

#endif // PROFILEEVALUATOR_H

