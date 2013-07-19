#include <qgfxdriverplugin_qws.h>
#include <qgfxvga16_qws.h>

class GfxVga16Driver : public QGfxDriverPlugin
{
public:
    GfxVga16Driver();

    QStringList keys() const;
    QScreen *create( const QString&, int displayId );
};

GfxVga16Driver::GfxVga16Driver()
: QGfxDriverPlugin()
{
}

QStringList GfxVga16Driver::keys() const
{
    QStringList list;
    list << "VGA16";
    return list;
}

QScreen* GfxVga16Driver::create( const QString& driver, int displayId )
{
    if ( driver.lower() == "vga16" )
        return new QVga16Screen( displayId );

    return 0;
}

Q_EXPORT_PLUGIN( GfxVga16Driver )
