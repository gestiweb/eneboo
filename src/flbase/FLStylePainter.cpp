/***************************************************************************
 FLStylePainter.h
 -------------------
 begin                : 24/12/2008
 copyright            : (C) 2003-2008 by InfoSiAL S.L.
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
/****************************************************************************
 ** $Id: qt/qsvgdevice.cpp   3.3.8   edited Jan 11 14:46 $
 **
 ** Implementation of the QSvgDevice class
 **
 ** Copyright (C) 2000-2007 Trolltech ASA.  All rights reserved.
 **
 ** This file is part of the xml module of the Qt GUI Toolkit.
 **
 ** This file may be distributed under the terms of the Q Public License
 ** as defined by Trolltech ASA of Norway and appearing in the file
 ** LICENSE.QPL included in the packaging of this file.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid Qt Enterprise Edition licenses may use this
 ** file in accordance with the Qt Commercial License Agreement provided
 ** with the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
 **   information about Qt Commercial License Agreements.
 ** See http://www.trolltech.com/qpl/ for QPL licensing information.
 ** See http://www.trolltech.com/gpl/ for GPL licensing information.
 **
 ** Contact info@trolltech.com if any conditions of this licensing are
 ** not clear to you.
 **
 *****************************************************************************/

#include <qdom.h>
#include <qapplication.h>
#include <qregexp.h>
#include <qpaintdevicemetrics.h>
#include <qdict.h>
#include <qfile.h>
#include <qpixmap.h>
#include <qvaluestack.h>

#include <math.h>

#include "FLStylePainter.h"
#include "FLManagerModules.h"
#include "FLSqlDatabase.h"
#include "FLSqlConnections.h"

#include "qwt_painter.h"

#define Q_PI 3.14159265358979323846f

const double deg2rad = 0.017453292519943295769;

struct QSvgDeviceState {
  int textx, texty;
  int textalign;
  Q_DUMMY_COMPARISON_OPERATOR(QSvgDeviceState)
};

typedef QValueList<QSvgDeviceState> StateList;

enum ElementType {
  InvalidElement = 0,
  AnchorElement,
  CircleElement,
  ClipElement,
  CommentElement,
  DescElement,
  EllipseElement,
  GroupElement,
  ImageElement,
  LineElement,
  PolylineElement,
  PolygonElement,
  PathElement,
  RectElement,
  SvgElement,
  TextElement,
  TitleElement,
  TSpanElement,
  SectionElement,
  SetStyleElement
};

class FLStylePainterPrivate
{
public:

  FLStylePainterPrivate() {
  }

  void setObjNodesMap(const QDomElement &elem);
  static QStringList paramsTransform(const QString &tr);
  static void normalizeTranslates(QDomNode &node, bool isRoot);
  void parseDomDoc();

  void saveAttributes();
  void restoreAttributes();
  QColor parseColor(const QString &col);
  double parseLen(const QString &str, bool *ok = 0, bool horiz = true) const;
  int lenToInt(const QDomNamedNodeMap &map, const QString &attr, int def = 0) const;
  double lenToDouble(const QDomNamedNodeMap &map, const QString &attr, int def = 0) const;
  void setStyleProperty(const QString &prop, const QString &val, QPen *pen, QFont *font, int *talign);
  void setStyle(const QString &s);
  void setTransform(const QString &tr);
  void applyTransforms();
  bool play(const QString &objName, ElementType elm);
  bool play(const QDomNode &node, ElementType elm);
  int pathArcSegment(QPointArray &path, int pcount, double xc, double yc, double th0, double th1, double rx, double ry,
                     double xAxisRotation);
  int pathArc(QPointArray &path, int pcount, double rx, double ry,
              double x_axis_rotation, int large_arc_flag,
              int sweep_flag, double x, double y, double curx, double cury);
  void drawPath(const QString &data);

  QPainter *painter_;
  QString styleName_;

  QRect lastLabelRect_;
  QString text_;
  int tf_;
  QPixmap pix_;
  int sx_, sy_, sw_, sh_;
  int saves_;
  QStringList transStack_;
  QMap<QString, QPair<double, double> > objBasesMap_;

  QDomDocument doc_;
  QMap<QString, QDomElement> objNodesMap_;

  StateList stack_;
  QSvgDeviceState *curr_;
  QMap<QString, ElementType> svgTypeMap_;
  QMap<QString, QString> svgColMap_;
  QMap<QString, QRegion> clipPathTable_;

  FLStylePainter::ErrCode errCode_;

  static bool svgMode_;
  float relDpi_;
};

bool FLStylePainterPrivate::svgMode_ = false;

void FLStylePainterPrivate::setObjNodesMap(const QDomElement &elem)
{
  QDomNode n(elem.firstChild());
  while (!n.isNull()) {
    QDomElement e(n.toElement());
    setObjNodesMap(e);
    if (e.attribute("id").startsWith("_##"))
      objNodesMap_.insert(e.attribute("id"), e);
    n = n.nextSibling();
  }
}

QStringList FLStylePainterPrivate::paramsTransform(const QString &tr)
{
  QString t = tr.simplifyWhiteSpace();
  QRegExp reg(QString::fromLatin1("\\s*([\\w]+)\\s*\\(([^\\(]*)\\)"));
  int index = 0;
  QStringList res;
  while ((index = reg.search(t, index)) >= 0) {
    QString command = reg.cap(1);
    QString params = reg.cap(2);
    QStringList plist = QStringList::split(QRegExp(QString::fromLatin1("[,\\s]")), params);
    res += command;
    res += plist;
    index += reg.matchedLength();
  }
  return res;
}

void FLStylePainterPrivate::normalizeTranslates(QDomNode &node, bool isRoot)
{
  QDomNodeList nl = node.childNodes();
  QDomNode itNod;
  for (uint i = 0; i < nl.count(); ++i) {
    itNod = nl.item(i);
    if (!itNod.attributes().contains("id")) {
      QDomElement e = itNod.toElement();
      e.setAttribute("id", node.attributes().namedItem("id").nodeValue() + "-" + QString::number(i));
    }
    normalizeTranslates(itNod, false);
  }

  QDomElement elem = node.toElement();
  if (!isRoot && elem.hasAttribute("transform")) {
    QDomElement dadNode(elem.parentNode().toElement());
    if (dadNode.tagName() != "g")
      return;

    if (!dadNode.hasAttribute("transform")) {
      dadNode.setAttribute("transform", elem.attribute("transform"));
      dadNode.setAttribute("aqnorm", "true");
      elem.removeAttribute("transform");
    } else if (dadNode.attribute("aqnorm") == "true") {
      if (dadNode.attribute("transform") != elem.attribute("transform")) {
        double tx1 = 0, ty1 = 0;
        double tx2 = 0, ty2 = 0;

        QStringList params = paramsTransform(dadNode.attribute("transform"));
        if (params[0] == "translate") {
          tx1 = params[1].toDouble();
          ty1 = params[2].toDouble();
        }

        params = paramsTransform(elem.attribute("transform"));
        if (params[0] == "translate") {
          tx2 = params[1].toDouble();
          ty2 = params[2].toDouble();
          QString s("translate(%1,%2)");
          elem.setAttribute("transform", s.arg(tx2 - tx1).arg(ty2 - ty1));
        } else if (params[0] == "matrix") {
          double m[6];
          for (int i = 0; i < 6; ++i)
            m[i] = params[i + 1].toDouble();
          QString s("matrix(%1,%2,%3,%4,%5,%6)");
          elem.setAttribute("transform", s.arg(m[0]).arg(m[1]).arg(m[2]).arg(m[3]).arg(m[4] - tx1).arg(m[5] - ty1));
        }
      } else
        elem.removeAttribute("transform");
    }
  }

  elem.removeAttribute("aqnorm");
}

void FLStylePainterPrivate::parseDomDoc()
{
  if (!styleName_.isEmpty() && styleName_ != "_mark" && styleName_ != "_simple") {
    QString errMsg;
    int errLine, errColumn;

    if (styleName_.lower().startsWith(QString::fromLatin1("abanq:"))) {
      QString content(FLSqlConnections::database()->managerModules()->contentCached(styleName_.mid(6)));
      if (!doc_.setContent(content, &errMsg, &errLine, &errColumn)) {
#ifdef FL_DEBUG
        qWarning("FLStylePainterPrivate : " +
                 QApplication::tr("Error en fichero XML.\nError : %1\nLinea : %2\nColumna : %3").
                 arg(errMsg, QString::number(errLine), QString::number(errColumn)));
#endif
        return;
      }
    } else if (styleName_.lower().startsWith(QString::fromLatin1("file:"))) {
      QFile file(styleName_.mid(5));
      if (!file.open(IO_ReadOnly))
        return;
      if (!doc_.setContent(&file, &errMsg, &errLine, &errColumn)) {
#ifdef FL_DEBUG
        qWarning("FLStylePainterPrivate : " +
                 QApplication::tr("Error en fichero XML.\nError : %1\nLinea : %2\nColumna : %3").
                 arg(errMsg, QString::number(errLine), QString::number(errColumn)));
#endif
        return;
      }
    }

    setObjNodesMap(doc_.documentElement());

    const struct ElementTable {
      const char *name;
      ElementType type;
    } etab[] = {
      { "a", AnchorElement },
      { "#comment", CommentElement },
      { "circle", CircleElement },
      { "clipPath", ClipElement },
      { "desc", DescElement },
      { "ellipse", EllipseElement },
      { "g", GroupElement },
      { "image", ImageElement },
      { "line", LineElement },
      { "polyline", PolylineElement },
      { "polygon", PolygonElement },
      { "path", PathElement },
      { "rect", RectElement },
      { "svg", SvgElement },
      { "text", TextElement },
      { "tspan", TSpanElement },
      { "title", TitleElement },
      { 0, InvalidElement }
    };

    if (svgTypeMap_.isEmpty()) {
      const ElementTable *t = etab;
      while (t->name) {
        svgTypeMap_.insert(t->name, t->type);
        t++;
      }
    }

    QSvgDeviceState st;
    st.textx = st.texty = 0;
    st.textalign = Qt::AlignLeft;
    stack_.append(st);
    curr_ = &stack_.last();
  }
}

void FLStylePainterPrivate::saveAttributes()
{
  painter_->save();
  QSvgDeviceState st(*curr_);
  stack_.append(st);
  curr_ = &stack_.last();
}

void FLStylePainterPrivate::restoreAttributes()
{
  painter_->restore();
  stack_.remove(stack_.fromLast());
  curr_ = &stack_.last();
}

QColor FLStylePainterPrivate::parseColor(const QString &col)
{
  static const struct ColorTable {
    const char *name;
    const char *rgb;
  }
  coltab[] = {
    { "black", "#000000" },
    { "silver", "#c0c0c0" },
    { "gray", "#808080" },
    { "white", "#ffffff" },
    { "maroon", "#800000" },
    { "red", "#ff0000" },
    { "purple", "#800080" },
    { "fuchsia", "#ff00ff" },
    { "green", "#008000" },
    { "lime", "#00ff00" },
    { "olive", "#808000" },
    { "yellow", "#ffff00" },
    { "navy", "#000080" },
    { "blue", "#0000ff" },
    { "teal", "#008080" },
    { "aqua", "#00ffff" },
    { 0, 0 }
  };

  if (svgColMap_.isEmpty()) {
    const struct ColorTable *t = coltab;
    while (t->name) {
      svgColMap_.insert(t->name, t->rgb);
      t++;
    }
  }

  if (svgColMap_.contains(col))
    return QColor(svgColMap_[col]);

  QString c = col;
  c.replace(QRegExp(QString::fromLatin1("\\s*")), "");
  QRegExp reg(QString::fromLatin1("^rgb\\((\\d+)(%?),(\\d+)(%?),(\\d+)(%?)\\)$"));
  if (reg.search(c) >= 0) {
    int comp[3];
    for (int i = 0; i < 3; i++) {
      comp[i] = reg.cap(2 * i + 1).toInt();
      if (!reg.cap(2 * i + 2).isEmpty())
        comp[i] = int((double(255 * comp[i]) / 100.0));
    }
    return QColor(comp[0], comp[1], comp[2]);
  }

  return QColor(col);
}

void FLStylePainterPrivate::setStyleProperty(const QString &prop, const QString &val, QPen *pen, QFont *font,
                                             int *talign)
{
  if (prop == "stroke") {
    if (val == "none") {
      pen->setStyle(Qt::NoPen);
    } else {
      pen->setColor(parseColor(val));
      if (pen->style() == Qt::NoPen)
        pen->setStyle(Qt::SolidLine);
      if (pen->width() == 0)
        pen->setWidth(1);
    }
  } else if (prop == "stroke-width") {
    double w = parseLen(val);
    if (w > 0.0001)
      pen->setWidth(int(w));
    else
      pen->setStyle(Qt::NoPen);
  } else if (prop == "stroke-linecap") {
    if (val == "butt")
      pen->setCapStyle(Qt::FlatCap);
    else if (val == "round")
      pen->setCapStyle(Qt::RoundCap);
    else if (val == "square")
      pen->setCapStyle(Qt::SquareCap);
  } else if (prop == "stroke-linejoin") {
    if (val == "miter")
      pen->setJoinStyle(Qt::MiterJoin);
    else if (val == "round")
      pen->setJoinStyle(Qt::RoundJoin);
    else if (val == "bevel")
      pen->setJoinStyle(Qt::BevelJoin);
  } else if (prop == "stroke-dasharray") {
    if (val == "18,6")
      pen->setStyle(Qt::DashLine);
    else if (val == "3")
      pen->setStyle(Qt::DotLine);
    else if (val == "9,6,3,6" || val == "4,2,1,2" ||
             val == "8,2,1,2" || val == "2,1,0.5,1")
      pen->setStyle(Qt::DashDotLine);
    else if (val == "9,3,3")
      pen->setStyle(Qt::DashDotDotLine);
    else if (val != "none")
      pen->setStyle(Qt::DotLine);
  } else if (prop == "fill") {
    if (val == "none")
      painter_->setBrush(Qt::NoBrush);
    else
      painter_->setBrush(parseColor(val));
  } else if (prop == "font-size") {
    font->setPointSizeFloat(float(parseLen(val)));
  } else if (prop == "font-family") {
    font->setFamily(val);
  } else if (prop == "font-style") {
    if (val == "normal")
      font->setItalic(false);
    else if (val == "italic")
      font->setItalic(true);
    else
      qWarning("FLStylePainterPrivate::setStyleProperty: unhandled "
               "font-style: %s", val.latin1());
  } else if (prop == "font-weight") {
    int w = font->weight();
    if (val == "100" || val == "200")
      w = QFont::Light;
    if (val == "300" || val == "400" || val == "normal")
      w = QFont::Normal;
    else if (val == "500" || val == "600")
      w = QFont::DemiBold;
    else if (val == "700" || val == "bold" || val == "800")
      w = QFont::Bold;
    else if (val == "900")
      w = QFont::Black;
    font->setWeight(w);
  } else if (prop == "text-anchor") {
    if (val == "middle")
      *talign = Qt::AlignHCenter;
    else if (val == "end")
      *talign = Qt::AlignRight;
    else
      *talign = Qt::AlignLeft;
  } else if (prop == "clip-path") {
    if (val.startsWith("url(#")) {
      QString clipName = val.mid(5, val.length() - 6);
      if (!clipName.isEmpty()) {
        QRegion clipRegion = clipPathTable_[clipName];
        if (!clipRegion.isEmpty())
          painter_->setClipRegion(painter_->clipRegion() & clipRegion, QPainter::CoordPainter);
      }
    }
  }
}

double FLStylePainterPrivate::parseLen(const QString &str, bool *ok, bool horiz) const
{
  QRegExp reg(QString::fromLatin1("([+-]?\\d*\\.*\\d*[Ee]?[+-]?\\d*)(em|ex|px|%|pt|pc|cm|mm|in|)$"));
  if (reg.search(str) == -1) {
    qWarning("FLStylePainterPrivate::parseLen: couldn't parse " + str);
    if (ok)
      *ok = false;
    return 0.0;
  }

  double dbl = reg.cap(1).toDouble();
  QString u = reg.cap(2);
  if (!u.isEmpty() && u != "px") {
    QPaintDeviceMetrics m(painter_->device());
    if (u == "em") {
      QFontInfo fi(painter_->font());
      dbl *= fi.pixelSize();
    } else if (u == "ex") {
      QFontInfo fi(painter_->font());
      dbl *= 0.5 * fi.pixelSize();
    } else if (u == "%")
      dbl *= (horiz ? painter_->window().width() : painter_->window().height()) / 100.0;
    else if (u == "cm")
      dbl *= m.logicalDpiX() / 2.54;
    else if (u == "mm")
      dbl *= m.logicalDpiX() / 25.4;
    else if (u == "in")
      dbl *= m.logicalDpiX();
    else if (u == "pt")
      dbl *= m.logicalDpiX() / 72.0;
    else if (u == "pc")
      dbl *= m.logicalDpiX() / 6.0;
    else
      qWarning("FLStylePainterPrivate::parseLen: Unknown unit " + u);
  }
  if (ok)
    *ok = true;
  return dbl;
}

int FLStylePainterPrivate::lenToInt(const QDomNamedNodeMap &map, const QString &attr, int def) const
{
  if (map.contains(attr)) {
    bool ok;
    double dbl = parseLen(map.namedItem(attr).nodeValue(), &ok);
    if (ok)
      return qRound(dbl);
  }
  return def;
}

double FLStylePainterPrivate::lenToDouble(const QDomNamedNodeMap &map, const QString &attr, int def) const
{
  if (map.contains(attr)) {
    bool ok;
    double d = parseLen(map.namedItem(attr).nodeValue(), &ok);
    if (ok)
      return d;
  }
  return def;
}

void FLStylePainterPrivate::setStyle(const QString &s)
{
  QStringList rules = QStringList::split(QChar(';'), s);

  QPen pen = painter_->pen();
  QFont font = painter_->font();

  QStringList::ConstIterator it = rules.begin();
  for (; it != rules.end(); it++) {
    int col = (*it).find(':');
    if (col > 0) {
      QString prop = (*it).left(col).simplifyWhiteSpace();
      QString val = (*it).right((*it).length() - col - 1);
      val = val.lower().stripWhiteSpace();
      setStyleProperty(prop, val, &pen, &font, &curr_->textalign);
    }
  }

  painter_->setPen(pen);
  painter_->setFont(font);
}

void FLStylePainterPrivate::setTransform(const QString &tr)
{
  QString t = tr.simplifyWhiteSpace();

  QRegExp reg(QString::fromLatin1("\\s*([\\w]+)\\s*\\(([^\\(]*)\\)"));
  int index = 0;
  while ((index = reg.search(t, index)) >= 0) {
    QString command = reg.cap(1);
    QString params = reg.cap(2);
    QStringList plist = QStringList::split(QRegExp(QString::fromLatin1("[,\\s]")), params);
    if (command == "translate") {
      double tx = 0, ty = 0;
      tx = plist[0].toDouble();
      if (plist.count() >= 2)
        ty = plist[1].toDouble();
      painter_->translate(tx, ty);
    } else if (command == "rotate") {
      painter_->rotate(plist[0].toDouble());
    } else if (command == "scale") {
      double sx, sy;
      sx = sy = plist[0].toDouble();
      if (plist.count() >= 2)
        sy = plist[1].toDouble();
      painter_->scale(sx, sy);
    } else if (command == "matrix" && plist.count() >= 6) {
      double m[6];
      for (int i = 0; i < 6; i++)
        m[i] = plist[i].toDouble();
      QWMatrix wm(m[0], m[1], m[2], m[3], m[4], m[5]);
      painter_->setWorldMatrix(wm, TRUE);
    } else if (command == "skewX") {
      painter_->shear(0.0, tan(plist[0].toDouble() * deg2rad));
    } else if (command == "skewY") {
      painter_->shear(tan(plist[0].toDouble() * deg2rad), 0.0);
    }

    index += reg.matchedLength();
  }
}

void FLStylePainterPrivate::applyTransforms()
{
  QStringList::const_iterator it;
  for (it = transStack_.begin(); it != transStack_.end(); ++it) {
    if (*it != "void")
      setTransform((*it).section(':', 1, 1));
  }
}

bool FLStylePainterPrivate::play(const QString &objName, ElementType elm)
{
  QDomNode node;
  if (objNodesMap_.contains(objName))
    node = objNodesMap_[objName];
  if (node.isNull()) {
    errCode_ = FLStylePainter::IdNotFound;
    qWarning("FLStylePainter::play : Object id " + objName + " not found.");
    return false;
  }

  return play(node, elm);
}

bool FLStylePainterPrivate::play(const QDomNode &node, ElementType elm)
{
  QDomElement pre = node.previousSibling().toElement();
  if (!pre.isNull() && !pre.attribute("id").startsWith("_##"))
    play(pre, SectionElement);

  if (elm != SetStyleElement)
    saveAttributes();

  ElementType t = svgTypeMap_[node.nodeName()];

  if (t == LineElement && painter_->pen().style() == Qt::NoPen) {
    QPen p = painter_->pen();
    p.setStyle(Qt::SolidLine);
    painter_->setPen(p);
  }

  QDomNamedNodeMap attr = node.attributes();
  if (attr.contains("style"))
    setStyle(attr.namedItem("style").nodeValue());
  if (elm != SetStyleElement && t != SvgElement) {
    applyTransforms();
    if (attr.contains("transform"))
      setTransform(attr.namedItem("transform").nodeValue());
  }

  uint i = attr.length();
  if (i > 0) {
    QPen pen = painter_->pen();
    QFont font = painter_->font();
    while (i--) {
      QDomNode n = attr.item(i);
      QString a = n.nodeName();
      QString val = n.nodeValue().lower().stripWhiteSpace();
      setStyleProperty(a, val, &pen, &font, &curr_->textalign);
    }
    painter_->setPen(pen);
    painter_->setFont(font);
  }

  if (elm == SetStyleElement)
    return true;

  QString idObj(attr.namedItem("id").nodeValue());
  bool isSectionDraw = false;
  if (elm == SectionElement)
    isSectionDraw = (!idObj.startsWith("_##"));

  int x1, y1, x2, y2, rx, ry, w, h;
  double cx1, cy1, crx, cry;
  switch (t) {
    case CommentElement:
      break;
    case RectElement:
      if (elm == RectElement || isSectionDraw) {
        rx = ry = 0;
        x1 = lenToInt(attr, "x");
        y1 = lenToInt(attr, "y");
        w = lenToInt(attr, "width");
        h = lenToInt(attr, "height");
        if (w == 0 || h == 0)
          break;
        x2 = (int) attr.contains("rx");
        y2 = (int) attr.contains("ry");
        if (x2)
          rx = lenToInt(attr, "rx");
        if (y2)
          ry = lenToInt(attr, "ry");
        if (x2 && !y2)
          ry = rx;
        else if (!x2 && y2)
          rx = ry;
        rx = int(200.0 * double(rx) / double(w));
        ry = int(200.0 * double(ry) / double(h));
        if (rx == 0 && ry == 0)
          QwtPainter::drawRect(painter_, x1, y1, w, h);
        else
          painter_->drawRoundRect(x1, y1, w, h, rx, ry);
        if (!isSectionDraw)
          lastLabelRect_ = painter_->xForm(QRect(x1, y1, w, h));
      }
      break;
    case CircleElement:
      cx1 = lenToDouble(attr, "cx") + 0.5;
      cy1 = lenToDouble(attr, "cy") + 0.5;
      crx = lenToDouble(attr, "r");
      QwtPainter::drawEllipse(painter_, QRect((int)(cx1 - crx), (int)(cy1 - crx), (int)(2 * crx), (int)(2 * crx)));
      break;
    case EllipseElement:
      cx1 = lenToDouble(attr, "cx") + 0.5;
      cy1 = lenToDouble(attr, "cy") + 0.5;
      crx = lenToDouble(attr, "rx");
      cry = lenToDouble(attr, "ry");
      QwtPainter::drawEllipse(painter_, QRect((int)(cx1 - crx), (int)(cy1 - cry), (int)(2 * crx), (int)(2 * cry)));
      break;
    case LineElement:
      if (elm == LineElement || isSectionDraw) {
        x1 = lenToInt(attr, "x1");
        x2 = lenToInt(attr, "x2");
        y1 = lenToInt(attr, "y1");
        y2 = lenToInt(attr, "y2");
        QPen p = painter_->pen();
        w = p.width();
        p.setWidth((unsigned int)(w * (QABS(painter_->worldMatrix().m11()) + QABS(painter_->worldMatrix().m22())) / 2));
        painter_->setPen(p);
        QwtPainter::drawLine(painter_, x1, y1, x2, y2);
        p.setWidth(w);
        painter_->setPen(p);
      }
      break;
    case PolylineElement:
    case PolygonElement: {
      QString pts = attr.namedItem("points").nodeValue();
      pts = pts.simplifyWhiteSpace();
      QStringList sl = QStringList::split(QRegExp(QString::fromLatin1("[ ,]")), pts);
      QPointArray ptarr((uint) sl.count() / 2);
      for (int i = 0; i < (int) sl.count() / 2; i++) {
        double dx = sl[2 * i].toDouble();
        double dy = sl[2 * i + 1].toDouble();
        ptarr.setPoint(i, int(dx), int(dy));
      }
      if (t == PolylineElement) {
        if (painter_->brush().style() != Qt::NoBrush) {
          QPen pn = painter_->pen();
          painter_->setPen(Qt::NoPen);
          painter_->drawPolygon(ptarr);
          painter_->setPen(pn);
        }
        QwtPainter::drawPolyline(painter_, ptarr);
      } else {
        QwtPainter::drawPolygon(painter_, ptarr);
      }
    }
    break;
    case SvgElement:
    case GroupElement:
    case AnchorElement: {
      QStringList backStack;
      if (t != SvgElement) {
        backStack = transStack_;
        transStack_.clear();
      }
      QDomNode child = node.firstChild();
      while (!child.isNull()) {
        play(child, elm);
        child = child.nextSibling();
      }
      if (t != SvgElement)
        transStack_ = backStack;
    }
    break;
    case PathElement:
      drawPath(attr.namedItem("d").nodeValue());
      break;
    case TSpanElement:
    case TextElement:
      if (elm == TextElement || isSectionDraw) {
        if (relDpi_ != 1.0) {
          QFont fnt = painter_->font();
          fnt.setPointSizeFloat(fnt.pointSizeFloat() * relDpi_);
          painter_->setFont(fnt);
        }

        if (!isSectionDraw && !lastLabelRect_.isNull() &&
            (tf_ & QPainter::WordBreak) != 0) {
          QPen pn = painter_->pen();
          QColor pcolor = pn.color();
          QColor bcolor = painter_->brush().color();
          pn.setColor(bcolor);
          painter_->setPen(pn);
          QwtPainter::drawText(painter_, painter_->xFormDev(lastLabelRect_),
                               curr_->textalign | tf_, text_);
          pn.setColor(pcolor);
          painter_->setPen(pn);
          lastLabelRect_.setSize(QSize(0, 0));
          break;
        }

        if (attr.contains("x"))
          curr_->textx = lenToInt(attr, "x");
        if (attr.contains("y"))
          curr_->texty = lenToInt(attr, "y");
        if (t == TSpanElement) {
          curr_->textx += lenToInt(attr, "dx");
          curr_->texty += lenToInt(attr, "dy");
        }

        QPen pn = painter_->pen();
        QColor pcolor = pn.color();
        QColor bcolor = painter_->brush().color();
        QDomNode c = node.firstChild();
        while (!c.isNull()) {
          if (c.isText()) {
            pn.setColor(bcolor);
            painter_->setPen(pn);
            QString text(isSectionDraw ? c.toText().nodeValue() : text_);
            w = painter_->fontMetrics().width(text);
            if (curr_->textalign == Qt::AlignHCenter)
              curr_->textx -= w / 2;
            else if (curr_->textalign == Qt::AlignRight)
              curr_->textx -= w;
            QwtPainter::drawText(painter_, curr_->textx, curr_->texty, text);
            pn.setColor(pcolor);
            painter_->setPen(pn);
            curr_->textx += w;
          } else if (c.isElement() && c.toElement().tagName() == "tspan") {
            play(c, elm);
          }
          c = c.nextSibling();
        }
        if (t == TSpanElement) {
          StateList::Iterator it = --stack_.fromLast();
          (*it).textx = curr_->textx;
          (*it).texty = curr_->texty;
        }
      }
      break;
    case ImageElement:
      if (elm == ImageElement || isSectionDraw) {
        x1 = lenToInt(attr, "x");
        y1 = lenToInt(attr, "y");
        if (isSectionDraw) {
          w = lenToInt(attr, "width");
          h = lenToInt(attr, "height");
          QString href = attr.namedItem("xlink:href").nodeValue();
          if (href.isEmpty())
            break;
          QPixmap pix;
          if (!pix.load(href)) {
            qWarning("FLStylePainterPrivate::play: Couldn't load image " + href);
            break;
          }
          painter_->drawPixmap(QRect(x1, y1, w, h), pix);
        } else
          painter_->drawPixmap(x1, y1, pix_, sx_, sy_, sw_, sh_);
      }
      break;
    case DescElement:
    case TitleElement:
      break;
    case ClipElement: {
      QDomNode child = node.firstChild();
      QRegion region;
      while (!child.isNull()) {
        QDomNamedNodeMap childAttr = child.attributes();
        if (child.nodeName() == "rect") {
          QRect r;
          r.setX(lenToInt(childAttr, "x"));
          r.setY(lenToInt(childAttr, "y"));
          r.setWidth(lenToInt(childAttr, "width"));
          r.setHeight(lenToInt(childAttr, "height"));
          region |= r;
        } else if (child.nodeName() == "ellipse") {
          QRect r;
          int x = lenToInt(childAttr, "cx");
          int y = lenToInt(childAttr, "cy");
          int width = lenToInt(childAttr, "rx");
          int height = lenToInt(childAttr, "ry");
          r.setX(x - width);
          r.setY(y - height);
          r.setWidth(width * 2);
          r.setHeight(height * 2);
          QRegion rgn(r, QRegion::Ellipse);
          region |= rgn;
        }
        child = child.nextSibling();
      }
      if (!idObj.isEmpty())
        clipPathTable_[idObj] = region;
      break;
    }
    case InvalidElement:
      qWarning("FLStylePainterPrivate::play: unknown element type " + node.nodeName());
      break;
  };

  restoreAttributes();

  return true;
}

int FLStylePainterPrivate::pathArcSegment(QPointArray &path, int pcount, double xc,
                                          double yc, double th0, double th1,
                                          double rx, double ry, double xAxisRotation)
{
  double sinTh, cosTh;
  double a00, a01, a10, a11;
  double x1, y1, x2, y2, x3, y3;
  double t;
  double thHalf;

  sinTh = sin(xAxisRotation * (Q_PI / 180.0));
  cosTh = cos(xAxisRotation * (Q_PI / 180.0));

  a00 = cosTh * rx;
  a01 = -sinTh * ry;
  a10 = sinTh * rx;
  a11 = cosTh * ry;

  thHalf = 0.5 * (th1 - th0);
  t = (8.0 / 3.0) * sin(thHalf * 0.5) * sin(thHalf * 0.5) / sin(thHalf);
  x1 = xc + cos(th0) - t * sin(th0);
  y1 = yc + sin(th0) + t * cos(th0);
  x3 = xc + cos(th1);
  y3 = yc + sin(th1);
  x2 = x3 + t * sin(th1);
  y2 = y3 - t * cos(th1);

  QPointArray ptarr(4), bezier;
  ptarr.setPoint(0, path.point(pcount - 1));
  ptarr.setPoint(1, a00 * x1 + a01 * y1, a10 * x1 + a11 * y1);
  ptarr.setPoint(2, a00 * x2 + a01 * y2, a10 * x2 + a11 * y2);
  ptarr.setPoint(3, a00 * x3 + a01 * y3, a10 * x3 + a11 * y3);

  bezier = ptarr.cubicBezier();
  if (bezier.size() > path.size() - pcount)
    path.resize(path.size() - pcount + bezier.size());
  for (int k = 0; k < (int) bezier.size(); ++k)
    path.setPoint(pcount++, bezier[k]);

  return pcount;
}

// the arc handling code underneath is from XSVG (BSD license)
/*
 * Copyright  2002 USC/Information Sciences Institute
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Information Sciences Institute not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission.  Information Sciences Institute
 * makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * INFORMATION SCIENCES INSTITUTE DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL INFORMATION SCIENCES
 * INSTITUTE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
 * OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
int FLStylePainterPrivate::pathArc(QPointArray &path, int pcount,
                                   double rx, double ry,
                                   double x_axis_rotation, int large_arc_flag,
                                   int sweep_flag, double x, double y,
                                   double curx, double cury)
{
  double sin_th, cos_th;
  double a00, a01, a10, a11;
  double x0, y0, x1, y1, xc, yc;
  double d, sfactor, sfactor_sq;
  double th0, th1, th_arc;
  int i, n_segs;
  double dx, dy, dx1, dy1, Pr1, Pr2, Px, Py, check;

  rx = QABS(rx);
  ry = QABS(ry);

  sin_th = sin(x_axis_rotation * (Q_PI / 180.0));
  cos_th = cos(x_axis_rotation * (Q_PI / 180.0));

  dx = (curx - x) / 2.0;
  dy = (cury - y) / 2.0;
  dx1 = cos_th * dx + sin_th * dy;
  dy1 = -sin_th * dx + cos_th * dy;
  Pr1 = rx * rx;
  Pr2 = ry * ry;
  Px = dx1 * dx1;
  Py = dy1 * dy1;

  check = Px / Pr1 + Py / Pr2;
  if (check > 1) {
    rx = rx * sqrt(check);
    ry = ry * sqrt(check);
  }

  a00 = cos_th / rx;
  a01 = sin_th / rx;
  a10 = -sin_th / ry;
  a11 = cos_th / ry;
  x0 = a00 * curx + a01 * cury;
  y0 = a10 * curx + a11 * cury;
  x1 = a00 * x + a01 * y;
  y1 = a10 * x + a11 * y;

  d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
  sfactor_sq = 1.0 / d - 0.25;
  if (sfactor_sq < 0)
    sfactor_sq = 0;
  sfactor = sqrt(sfactor_sq);
  if (sweep_flag == large_arc_flag)
    sfactor = -sfactor;
  xc = 0.5 * (x0 + x1) - sfactor * (y1 - y0);
  yc = 0.5 * (y0 + y1) + sfactor * (x1 - x0);

  th0 = atan2(y0 - yc, x0 - xc);
  th1 = atan2(y1 - yc, x1 - xc);

  th_arc = th1 - th0;
  if (th_arc < 0 && sweep_flag)
    th_arc += 2 * Q_PI;
  else if (th_arc > 0 && !sweep_flag)
    th_arc -= 2 * Q_PI;

  n_segs = int(ceil(QABS(th_arc / (Q_PI * 0.5 + 0.001))));

  for (i = 0; i < n_segs; i++) {
    pcount = pathArcSegment(path, pcount, xc, yc,
                            th0 + i * th_arc / n_segs,
                            th0 + (i + 1) * th_arc / n_segs,
                            rx, ry, x_axis_rotation);
  }

  return pcount;
}

void FLStylePainterPrivate::drawPath(const QString &data)
{
  double x0 = 0, y0 = 0;
  double x = 0, y = 0;
  double controlX = 0, controlY = 0;
  QPointArray path(500);
  QValueList<int> subIndex;
  QPointArray quad(4), bezier;
  int pcount = 0;
  uint idx = 0;
  int mode = 0, lastMode = 0;
  bool relative = false;
  QString commands("MZLHVCSQTA");
  int cmdArgs[] = { 2, 0, 2, 1, 1, 6, 4, 4, 2, 7 };
  QRegExp reg(QString::fromLatin1("\\s*,?\\s*([+-]?\\d*\\.?\\d*)"));

  subIndex.append(0);

  while (idx < data.length()) {
    QChar ch = data[(int) idx++];
    if (ch.isSpace())
      continue;
    QChar chUp = ch.upper();
    int cmd = commands.find(chUp);
    if (cmd >= 0) {
      mode = cmd;
      relative = (ch != chUp);
    } else {
      if (mode && !ch.isLetter()) {
        cmd = mode;
        idx--;
      } else {
        qWarning("FLStylePainterPrivate::drawPath: Unknown command");
        return;
      }
    }

    const int maxArgs = 7;
    double arg[maxArgs];
    int numArgs = cmdArgs[cmd];
    for (int i = 0; i < numArgs; i++) {
      int pos = reg.search(data, idx);
      if (pos == -1) {
        qWarning("FLStylePainterPrivate::drawPath: Error parsing arguments");
        return;
      }
      arg[i] = reg.cap(1).toDouble();
      idx = pos + reg.matchedLength();
    };

    double offsetX = relative ? x : 0;
    double offsetY = relative ? y : 0;
    switch (mode) {
      case 0:
        if (x != x0 || y != y0)
          path.setPoint(pcount++, int(x0), int(y0));
        x = x0 = arg[0] + offsetX;
        y = y0 = arg[1] + offsetY;
        subIndex.append(pcount);
        path.setPoint(pcount++, int(x0), int(y0));
        mode = 2;
        break;
      case 1:
        path.setPoint(pcount++, int(x0), int(y0));
        x = x0;
        y = y0;
        mode = 0;
        break;
      case 2:
        x = arg[0] + offsetX;
        y = arg[1] + offsetY;
        path.setPoint(pcount++, int(x), int(y));
        break;
      case 3:
        x = arg[0] + offsetX;
        path.setPoint(pcount++, int(x), int(y));
        break;
      case 4:
        y = arg[0] + offsetY;
        path.setPoint(pcount++, int(x), int(y));
        break;
      case 5:
      case 6:
      case 7:
      case 8: {
        quad.setPoint(0, int(x), int(y));
        if (mode == 6 || mode == 8) {
          bool cont = mode == lastMode || mode == 6 && lastMode == 5 || mode == 8 && lastMode == 7;
          x = cont ? 2 * x - controlX : x;
          y = cont ? 2 * y - controlY : y;
          quad.setPoint(1, int(x), int(y));
          quad.setPoint(2, int(x), int(y));
        }
        for (int j = 0; j < numArgs / 2; j++) {
          x = arg[2 * j] + offsetX;
          y = arg[2 * j + 1] + offsetY;
          quad.setPoint(j + 4 - numArgs / 2, int(x), int(y));
        }

        controlX = quad[2].x();
        controlY = quad[2].y();

        if (mode == 7 || mode == 8) {
          int x31 = quad[0].x() + int(2.0 * (quad[2].x() - quad[0].x()) / 3.0);
          int y31 = quad[0].y() + int(2.0 * (quad[2].y() - quad[0].y()) / 3.0);
          int x32 = quad[2].x() + int(2.0 * (quad[3].x() - quad[2].x()) / 3.0);
          int y32 = quad[2].y() + int(2.0 * (quad[3].y() - quad[2].y()) / 3.0);
          quad.setPoint(1, x31, y31);
          quad.setPoint(2, x32, y32);
        }

        bezier = quad.cubicBezier();
        if (bezier.size() > path.size() - pcount)
          path.resize(path.size() - pcount + bezier.size());
        for (int k = 0; k < (int) bezier.size(); k++)
          path.setPoint(pcount++, bezier[k]);
        break;
      }
      case 9: {
        double rx = arg[0];
        double ry = arg[1];
        double xAxisRotation = arg[2];
        double largeArcFlag = arg[3];
        double sweepFlag = arg[4];
        double ex = arg[5] + offsetX;
        double ey = arg[6] + offsetY;
        double curx = x;
        double cury = y;
        pcount = pathArc(path, pcount++, rx, ry,
                         xAxisRotation, int(largeArcFlag), int(sweepFlag),
                         ex, ey, curx, cury);
        x = ex;
        y = ey;
      }
      break;
    };
    lastMode = mode;
    if (pcount >= (int) path.size() - 4)
      path.resize(2 * path.size());
  }

  subIndex.append(pcount);
  if (painter_->brush().style() != Qt::NoBrush) {
    if (x != x0 || y != y0)
      path.setPoint(pcount++, int(x0), int(y0));
    QPen pen = painter_->pen();
    painter_->setPen(Qt::NoPen);
    QwtPolygon poly(pcount);
    for (int i = 0; i < pcount; ++i)
      poly.setPoint(i, path.point(i));
    QwtPainter::drawPolygon(painter_, poly);
    painter_->setPen(pen);
  }

  QValueListConstIterator<int> it = subIndex.begin();
  int start = 0;
  while (it != subIndex.fromLast()) {
    int next = *++it;
    int polcount = next - start;
    QwtPolygon poly(polcount);
    for (int i = 0; i < polcount; ++i)
      poly.setPoint(i, path.point(start + i));
    QwtPainter::drawPolyline(painter_, poly);
    start = next;
  }
}

FLStylePainter::FLStylePainter()
{
  d = new FLStylePainterPrivate;
  d->painter_ = new QPainter;
  d->relDpi_ = 1.0;
#ifndef FL_NO_STYLEPAINTER
  d->painter_->setWorldXForm(true);
  d->saves_ = 0;
  d->lastLabelRect_.setSize(QSize(0, 0));
  d->errCode_ = NoError;
#endif
}

FLStylePainter::~FLStylePainter()
{
  delete d->painter_;
  delete d;
}

QPainter *FLStylePainter::painter() const
{
  return d->painter_;
}

QString FLStylePainter::styleName() const
{
#ifndef FL_NO_STYLEPAINTER
  return d->styleName_;
#else
  return QString::null;
#endif
}

void FLStylePainter::setStyleName(const QString &style)
{
#ifndef FL_NO_STYLEPAINTER
  d->styleName_ = style;
  d->parseDomDoc();
#endif
}

float FLStylePainter::relDpi() const
{
  return d->relDpi_;
}

void FLStylePainter::setRelDpi(float relDpi)
{
  d->relDpi_ = relDpi;
}

void FLStylePainter::beginMark(int x, int y, QObject *obj)
{
#ifndef FL_NO_STYLEPAINTER
  if (d->styleName_ == "_mark" || d->styleName_ == "_simple" ||
      d->styleName_.isEmpty()) {
    d->saves_++;
    d->painter_->save(obj->QObject::name());
    d->painter_->translate(x, y);
  }
  if (d->styleName_ == "_mark") {
    d->painter_->setBrush(Qt::NoBrush);
    d->painter_->setPen(Qt::blue);
    d->painter_->setFont(QFont("Arial", 6));
    d->painter_->drawText(0, 0, obj->QObject::name());
  }
#else
  d->painter_->save();
  d->painter_->translate(x, y);
#endif
}

void FLStylePainter::endMark()
{
#ifndef FL_NO_STYLEPAINTER
  if (d->saves_ > 0) {
    d->painter_->restore();
    d->saves_--;
  }
#else
  d->painter_->restore();
#endif
}

void FLStylePainter::beginSection(int x, int y, int w, int h, QObject *obj)
{
#ifndef FL_NO_STYLEPAINTER
  if (d->styleName_ == "_mark" || d->styleName_ == "_simple" ||
      d->styleName_.isEmpty()) {
    d->saves_++;
    d->painter_->save(obj->QObject::name());
    d->painter_->translate(x, y);
  }
  if (d->styleName_ == "_mark") {
    d->painter_->setBrush(Qt::NoBrush);
    d->painter_->setPen(Qt::red);
    d->painter_->setFont(QFont("Arial", 6));
    d->painter_->drawText(0, 0, obj->QObject::name());
    d->painter_->setPen(QPen(Qt::red, 0, Qt::DotLine));
    d->painter_->drawRect(0, 0, w, h);
  } else if (obj && !d->styleName_.isEmpty() && d->doc_.hasChildNodes()) {
    QString objName(obj->QObject::name());
    QDomElement node = d->objNodesMap_[objName];

    if (node.attributes().contains("transform")) {
      double xx = x, yy = y;
      double tx = 0, ty = 0;

      if (d->objBasesMap_.contains(objName)) {
        QPair<double, double> pa = d->objBasesMap_[objName];
        xx = pa.first;
        yy = pa.second;
      } else
        d->objBasesMap_.insert(objName, qMakePair(xx, yy));

      QStringList params = d->paramsTransform(node.attribute("transform"));
      if (params[0] == "translate") {
        tx = params[1].toDouble();
        ty = params[2].toDouble();
        QString s("translate(%1,%2)");
        d->transStack_.push_back(objName + ":" + s.arg(tx + x - xx).arg(ty + y - yy));
      } else if (params[0] == "matrix") {
        double m[6];
        for (int i = 0; i < 6; i++)
          m[i] = params[i + 1].toDouble();
        QString s("matrix(%1,%2,%3,%4,%5,%6)");
        d->transStack_.push_back(objName + ":" +
                                 s.arg(m[0]).arg(m[1])
                                 .arg(m[2]).arg(m[3])
                                 .arg(m[4] + x - xx).arg(m[5] + y - yy));
      }
    } else
      d->transStack_.push_back("void");
  }
#else
  d->painter_->save();
  d->painter_->translate(x, y);
#endif
}

void FLStylePainter::endSection()
{
#ifndef FL_NO_STYLEPAINTER
  if (d->saves_ > 0) {
    d->painter_->restore();
    d->saves_--;
  }
  if (!d->transStack_.isEmpty()) {
    if (d->transStack_.back() != "void") {
      QString objName(d->transStack_.back().section(':', 0, 0));
      QDomElement last = d->objNodesMap_[objName].lastChild().toElement();
      if (!last.attribute("id").startsWith("_##"))
        d->play(last, SectionElement);
    }
    d->transStack_.pop_back();
  }
#else
  d->painter_->restore();
#endif
}

bool FLStylePainter::drawPixmap(const QPixmap &pixmap, int sx, int sy,
                                int sw, int sh, QObject *obj)
{
#ifndef FL_NO_STYLEPAINTER
  d->errCode_ = NoError;

  if (d->styleName_.isEmpty())
    return false;

  if (d->styleName_ == "_simple" &&
      QString(obj->QObject::name()).startsWith("_##Label"))
    return true;

  if (d->styleName_ == "_mark" || !d->doc_.hasChildNodes())
    return false;

  d->pix_ = pixmap;
  d->sx_ = sx;
  d->sy_ = sy;
  d->sw_ = sw;
  d->sh_ = sh;

  return d->play(obj->QObject::name(), ImageElement);
#else
  return false;
#endif
}

bool FLStylePainter::drawText(const QString &text, int tf, QObject *obj)
{
#ifndef FL_NO_STYLEPAINTER
  d->errCode_ = NoError;

  if (FLStylePainterPrivate::svgMode_ && d->relDpi_ != 1.0) {
    QFont fnt(d->painter_->font());
    fnt.setPointSizeFloat(fnt.pointSizeFloat() / d->relDpi_);
    int oldAscent = d->painter_->fontMetrics().ascent();
    d->painter_->setFont(fnt);
    d->painter_->translate(0, oldAscent - d->painter_->fontMetrics().ascent());
  }

  if (d->styleName_.isEmpty())
    return false;

  if (d->styleName_ == "_simple" &&
      QString(obj->QObject::name()).startsWith("_##Label"))
    return true;

  if (d->styleName_ == "_mark" || !d->doc_.hasChildNodes())
    return false;

  d->text_ = text;
  d->tf_ = tf;

  return d->play(obj->QObject::name(), TextElement);
#else
  return false;
#endif
}

bool FLStylePainter::drawLine(QObject *obj)
{
#ifndef FL_NO_STYLEPAINTER
  d->errCode_ = NoError;

  if (d->styleName_.isEmpty())
    return false;

  if (d->styleName_ == "_simple" &&
      QString(obj->QObject::name()).startsWith("_##Line"))
    return true;

  return (d->styleName_ != "_mark" && d->doc_.hasChildNodes() &&
          d->play(obj->QObject::name(), LineElement));
#else
  return false;
#endif
}

bool FLStylePainter::drawRect(QObject *obj)
{
#ifndef FL_NO_STYLEPAINTER
  d->errCode_ = NoError;

  if (d->styleName_.isEmpty())
    return false;

  if (d->styleName_ == "_simple" &&
      QString(obj->QObject::name()).startsWith("_##Label"))
    return true;

  return (d->styleName_ != "_mark" && d->doc_.hasChildNodes() &&
          d->play(obj->QObject::name(), RectElement));
#else
  return false;
#endif
}

bool FLStylePainter::setStyle(QObject *obj)
{
#ifndef FL_NO_STYLEPAINTER
  d->errCode_ = NoError;

  if (d->styleName_.isEmpty())
    return false;

  if (d->styleName_ == "_simple" &&
      (QString(obj->QObject::name()).startsWith("_##Label")
       || QString(obj->QObject::name()).startsWith("_##Line")))
    return true;

  return (d->styleName_ != "_mark" && d->doc_.hasChildNodes() && d->play(obj->QObject::name(), SetStyleElement));
#else
  return false;
#endif
}

void FLStylePainter::applyTransforms()
{
#ifndef FL_NO_STYLEPAINTER
  d->applyTransforms();
#endif
}

FLStylePainter::ErrCode FLStylePainter::errCode() const
{
  return d->errCode_;
}

void FLStylePainter::normalizeSVGFile(const QString &fileName,
                                      const QStringList &fileNames)
{
  QDomDocument doc("svgdoc");
  QDomDocument docRes("svg");
  QDomElement docElemRes;
  QFile file;
  QString errMsg;
  int errLine, errColumn;
  QStringList idList;

  for (QStringList::const_iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
    file.setName(*it);
    if (!file.open(IO_ReadOnly))
      continue;

    if (!doc.setContent(&file, &errMsg, &errLine, &errColumn)) {
#ifdef FL_DEBUG
      qWarning("FLStylePainter : " +
               QApplication::tr("Error en fichero XML %1.\nError : %2\nLinea : %3\nColumna : %4").
               arg(fileName, errMsg, QString::number(errLine), QString::number(errColumn)));
#endif
      file.close();
      return;
    }

    file.close();

    if (it == fileNames.begin()) {
      docRes = doc.cloneNode(false).toDocument();
      docElemRes = doc.documentElement().cloneNode(false).toElement();
    }

    QDomNode n(doc.documentElement().firstChild());
    while (!n.isNull()) {
      QDomElement e(n.toElement());
      if (!e.isNull()) {
        QString eid(e.attribute("id"));
        if (!idList.contains(eid)) {
          idList.append(eid);
          FLStylePainterPrivate::normalizeTranslates(e, true);
          docElemRes.appendChild(e.cloneNode());
        }
      }
      n = n.nextSibling();
    }

  }

  file.setName(fileName);
  if (!file.open(IO_WriteOnly))
    return;

  docElemRes.setAttributeNS("http://www.w3.org/2000/svg",
                            "xmlns:xlink",
                            "http://www.w3.org/1999/xlink");
  docRes.appendChild(docElemRes);
  QTextStream s(&file);
  s.setEncoding(QTextStream::UnicodeUTF8);
  s << docRes;
}

void FLStylePainter::setSVGMode(bool mode)
{
  FLStylePainterPrivate::svgMode_ = mode;
  QwtPainter::setSVGMode(mode);
}
