#include <qmacstyle_mac.h>
#include <qstyleplugin.h>
#include <qt_mac.h>

extern QString p2qstring(const unsigned char *c);

class MacStyle : public QStylePlugin
{
public:
    MacStyle();

    QStringList keys() const;
    QStyle *create( const QString& );
};

MacStyle::MacStyle() : QStylePlugin()
{
}

QStringList MacStyle::keys() const
{
    QString mstyle = "Macintosh";
    if(Collection c=NewCollection()) {
	GetTheme(c);
	Str255 str;
	long int s = 256;
	if(!GetCollectionItem(c, kThemeNameTag, 0, &s, &str))
	    mstyle += " (" + p2qstring(str) + ")";
    }

    QStringList list;
    list << mstyle;
    return list;
}

QStyle* MacStyle::create( const QString& s )
{
    if ( s.lower().startsWith("macintosh") )
        return new QMacStyle();

    return 0;
}

Q_EXPORT_PLUGIN( MacStyle )
