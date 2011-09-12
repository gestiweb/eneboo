/***************************************************************************
 AQSLayout_p.h
 -------------------
 begin                : 07/02/2011
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

#ifndef AQSLAYOUT_P_H_
#define AQSLAYOUT_P_H_

#include "AQSObject_p.h"
#include <qlayout.h>

class AQSLayout : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Layout, Object);

public slots:
  virtual QDomNode *toXml(bool includeComplexTypes = false) {
    AQSObject::toXml(includeComplexTypes);

    if (!o_)
      return &xml_;

    QLayoutIterator it = o_->iterator();
    QLayoutItem *child;
    while ((child = it.current()) != 0) {
      QDomElement e(xml_.createElement("item"));

      if (child->widget()) {
        QWidget *w = child->widget();
        e.setAttribute("type", "widget");
        e.setAttribute("class", w->className());
        e.setAttribute("name", w->name());
      } else if (child->layout()) {
        QLayout *l = child->layout();
        e.setAttribute("type", "layout");
        e.setAttribute("class", l->className());
        e.setAttribute("name", l->name());
      } else
        e.setAttribute("type", "spacer");

      xml_.firstChild().appendChild(e);
      ++it;
    }

    return &xml_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSLAYOUT_P_H_ */
// @AQSOBJECT@
