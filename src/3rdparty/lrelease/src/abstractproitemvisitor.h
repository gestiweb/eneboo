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

#ifndef ABSTRACTPROITEMVISITOR
#define ABSTRACTPROITEMVISITOR

#include "proitems.h"

struct AbstractProItemVisitor {
    virtual ~AbstractProItemVisitor() {}
    virtual bool visitBeginProBlock(ProBlock *block) = 0;
    virtual bool visitEndProBlock(ProBlock *block) = 0;

    virtual bool visitBeginProVariable(ProVariable *variable) = 0;
    virtual bool visitEndProVariable(ProVariable *variable) = 0;

    virtual bool visitBeginProFile(ProFile *value) = 0;
    virtual bool visitEndProFile(ProFile *value) = 0;

    virtual bool visitProValue(ProValue *value) = 0;
    virtual bool visitProFunction(ProFunction *function) = 0;
    virtual bool visitProOperator(ProOperator *function) = 0;
    virtual bool visitProCondition(ProCondition *function) = 0;

};

#endif // ABSTRACTPROITEMVISITOR

