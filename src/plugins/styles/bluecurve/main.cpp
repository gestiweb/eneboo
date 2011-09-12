/* Qt Plugin interface for the Bluecurve style
 * (c) 2002 Red Hat, Inc.
 * Programmed by Bernhard Rosenkraenzer <bero@redhat.com>
 * Released under the GNU General Public License (GPL) version 2.
 */
#include <qstyleplugin.h>
#include "bluecurve.h"

class BluecurveStylePlugin : public QStylePlugin
{
public:
    BluecurveStylePlugin();

    QStringList keys() const;
    QStyle *create(const QString &);
};

BluecurveStylePlugin::BluecurveStylePlugin()
        : QStylePlugin()
{
}

QStringList BluecurveStylePlugin::keys() const
{
    QStringList list;
    list << "Bluecurve";
    return list;
}

QStyle *BluecurveStylePlugin::create(const QString &s)
{
    if (s.lower() == "bluecurve")
        return new BluecurveStyle;
    return 0;
}

Q_EXPORT_PLUGIN( BluecurveStylePlugin )
