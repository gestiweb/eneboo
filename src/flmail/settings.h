#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <defines.h>

#ifdef _QTE_
#include <qpe/config.h>
#else
#include <qsettings.h>
#endif

#ifdef _QTE_
class Settings : public Config
#else
class TKCBASE_API Settings : public QSettings
#endif
{
  public:

    static Settings * instance();

    Settings();
    virtual ~Settings();

    static void forceWrite();
    
    virtual const QString & name();
    
    virtual bool removeGroup(const QString &);

  private:

    static Settings *instance_;
    QString name_;
};

#endif
