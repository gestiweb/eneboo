/* Bluecurve style class definition
 * (c) 2002 Red Hat, Inc.
 *
 * Authors: Bernhard Rosenkraenzer <bero@redhat.com>
 *          Preston Brown <pbrown@redhat.com>
 *
 * Released under the GNU General Public License (GPL) version 2.
 */
#ifndef _BLUECURVE_H_
#define _BLUECURVE_H_ 1

#ifdef QT_PLUGIN
#  define Q_EXPORT_STYLE_BLUECURVE
#else
#  define Q_EXPORT_STYLE_BLUECURVE Q_EXPORT
#endif // QT_PLUGIN

#include <qcommonstyle.h>
#include <qcolor.h>
#include <qpalette.h>
#include <qintcache.h>

class Q_EXPORT_STYLE_BLUECURVE BluecurveStyle : public QCommonStyle
{
    Q_OBJECT
    QStyle *basestyle;

public:
    BluecurveStyle();
    virtual ~BluecurveStyle();

    void polish(QWidget *widget);
    void unPolish(QWidget *widget);

    void polishPopupMenu( QPopupMenu * );

    void drawPrimitive(PrimitiveElement, QPainter *, const QRect &,
                       const QColorGroup &, SFlags = Style_Default,
                       const QStyleOption & = QStyleOption::Default) const;

    void drawControl(ControlElement, QPainter *, const QWidget *,
                     const QRect &, const QColorGroup &,
                     SFlags = Style_Default,
                     const QStyleOption & = QStyleOption::Default) const;

    void drawControlMask(ControlElement, QPainter *, const QWidget *,
                         const QRect &,
                         const QStyleOption & = QStyleOption::Default) const;

    QRect subRect(SubRect, const QWidget *) const;

    void drawComplexControl(ComplexControl, QPainter *, const QWidget *,
                            const QRect &, const QColorGroup &,
                            SFlags = Style_Default,
                            SCFlags = SC_All, SCFlags = SC_None,
                            const QStyleOption & = QStyleOption::Default) const;

    QRect querySubControlMetrics(ComplexControl, const QWidget *,
                                 SubControl, const QStyleOption & = QStyleOption::Default) const;

    SubControl querySubControl(ComplexControl, const QWidget *, const QPoint &,
                               const QStyleOption &data = QStyleOption::Default) const;

    int pixelMetric(PixelMetric, const QWidget * = 0) const;

    QSize sizeFromContents(ContentsType, const QWidget *, const QSize &,
                           const QStyleOption & = QStyleOption::Default) const;

    int styleHint(StyleHint, const QWidget * = 0,
                  const QStyleOption & = QStyleOption::Default,
                  QStyleHintReturn * = 0 ) const;

    QPixmap stylePixmap(StylePixmap stylepixmap,
                        const QWidget* widget = 0,
                        const QStyleOption& = QStyleOption::Default) const;
protected:
    struct BluecurveColorData {
        QRgb buttonColor;
        QRgb spotColor;

        QColor shades[8];
        QColor spots[3];

        QPixmap *radioPix[8];
        QPixmap *radioMask;

        QPixmap *checkPix[6];

        ~BluecurveColorData();

        bool isGroup (const QColorGroup &cg) {
            return cg.button().rgb() == buttonColor && cg.highlight().rgb() == spotColor;
        }
    };

    bool eventFilter(QObject *object, QEvent *event);
    void drawGradientBox(QPainter *p, QRect const &r,
                         const QColorGroup &cg,
                         const BluecurveColorData *cdata,
                         bool horiz,
                         double shade1, double shade2) const;
    void drawGradient(QPainter *p, QRect const &r,
                      const QColorGroup &cg,
                      double shade1, double shade2,
                      bool horiz) const;

    void getShade (const QColorGroup &cg, int shadenr, QColor &res) const;
    void drawTextRect (QPainter *p, const QRect &r,
                       const QColorGroup &cg, QStyle::SFlags flags,
                       const QBrush *fill = 0) const;
    void drawLightBevel(QPainter *p, const QRect &r,
                        const QColorGroup &cg,
                        QStyle::SFlags flags,
                        const QBrush *fill = 0) const;

    BluecurveColorData *realizeData (const QColorGroup &cg) const;
    const BluecurveColorData *lookupData (const QColorGroup &cg) const;


    QIntCache<BluecurveColorData> m_dataCache;
    static const double shadeFactors[8];
};
#endif
