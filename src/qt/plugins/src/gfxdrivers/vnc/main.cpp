#include <qgfxdriverplugin_qws.h>
#include <qgfxvnc_qws.h>

class GfxVncDriver : public QGfxDriverPlugin
{
public:
    GfxVncDriver();

    QStringList keys() const;
    QScreen *create( const QString&, int displayId );
};

GfxVncDriver::GfxVncDriver()
: QGfxDriverPlugin()
{
}

QStringList GfxVncDriver::keys() const
{
    QStringList list;
    list << "VNC";
    return list;
}

QScreen* GfxVncDriver::create( const QString& driver, int displayId )
{
    if ( driver.lower() == "vnc" )
        return new QVNCScreen( displayId );

    return 0;
}

Q_EXPORT_PLUGIN( GfxVncDriver )
