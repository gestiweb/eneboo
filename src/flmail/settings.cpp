
#include "settings.h"

Settings *Settings::instance_=0;

#ifdef _QTE_
Settings::Settings()
  : Config(AppName),
    name_ (QString("/") + AppName)
{
}
#else
Settings::Settings()
  : QSettings(),
    name_ (QString("/") + AppName)
{
#ifdef _QTWIN_
  setPath(QString::null, QString::null, QSettings::User);
#endif
}
#endif

Settings::~Settings()
{
}

Settings * Settings::instance()
{
  if( !instance_ )
    instance_ = new Settings;
  
  return instance_;
}

void Settings::forceWrite()
{
  // Deletion should be fine, as instance() will create a new one.
  delete instance_;
  instance_ = 0;
}

const QString & Settings::name()
{
  return name_;
}

bool Settings::removeGroup(const QString & key)
{
  QStringList entries = entryList( key );
  QStringList subkeys = subkeyList( key );
  
  bool bRet = true;
  for(QStringList::Iterator it=subkeys.begin(); it!=subkeys.end() && bRet; ++it)
  {
    bRet = removeGroup(key + "/" + (*it));
  }
  
  for(QStringList::Iterator it1=entries.begin(); it1!=entries.end() && bRet; ++it1)
  {
    bRet = removeEntry(key + "/" + (*it1));
  }
  
  return bRet;
}
