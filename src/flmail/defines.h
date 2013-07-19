#ifndef TKCBASE_DEFINES_H
#define TKCBASE_DEFINES_H

#include <qglobal.h>

// Win
#ifdef Q_WS_WIN
#define _QTWIN_
#endif

// Embedded
#ifdef Q_WS_QWS
#define _QTE_
#endif

// Linux (X11)
#ifdef Q_WS_X11
#define _QTX11_
#endif

// MacOSX
#ifdef Q_WS_MACX
#define _QTMAC_
#endif

#ifdef Q_WS_WIN
#include <windows.h>
#ifdef TKCBASE_EXPORTS
#define TKCBASE_API __declspec(dllexport)
    extern char * AppName;
    extern char * AppMIME;
    extern char * AppVersion;
    extern char * FullAppName;
#else
#define TKCBASE_API __declspec(dllimport)
    TKCBASE_API char * AppName;
    TKCBASE_API char * AppMIME;
    TKCBASE_API char * AppVersion;
    TKCBASE_API char * FullAppName;
#endif
#ifdef AETHERA_EXPORTS
        #define AETHERA_API __declspec(dllexport)
#else
        #define AETHERA_API __declspec(dllimport)
#endif
#ifdef AETHERA_PLUGIN_EXPORTS
        #define PLUGIN_API __declspec(dllexport)
#else
        #define PLUGIN_API __declspec(dllimport)
#endif
#ifdef HOME_PLUGIN_EXPORTS
        #define HOME_PLUGIN_API __declspec(dllexport)
#else
        #define HOME PLUGIN_API __declspec(dllimport)
#endif
#ifdef MAIL_PLUGIN_EXPORTS
        #define MAIL_PLUGIN_API __declspec(dllexport)
#else
        #define MAIL_PLUGIN_API __declspec(dllimport)
#endif
#ifdef MAILIMPORT_PLUGIN_EXPORTS
        #define MAILIMPORT_PLUGIN_API __declspec(dllexport)
#else
        #define MAILIMPORT_PLUGIN_API __declspec(dllimport)
#endif
#ifdef NOTE_PLUGIN_EXPORTS
        #define NOTE_PLUGIN_API __declspec(dllexport)
#else
        #define NOTE_PLUGIN_API __declspec(dllimport)
#endif
#ifdef CONTACT_PLUGIN_EXPORTS
        #define CONTACT_PLUGIN_API __declspec(dllexport)
#else
        #define CONTACT_PLUGIN_API __declspec(dllimport)
#endif
#ifdef DATEBOOK_PLUGIN_EXPORTS
        #define DATEBOOK_PLUGIN_API __declspec(dllexport)
#else
        #define DATEBOOK_PLUGIN_API __declspec(dllimport)
#endif
#ifdef JABBER_PLUGIN_EXPORTS
        #define JABBER_PLUGIN_API __declspec(dllexport)
#else
        #define JABBER_PLUGIN_API __declspec(dllimport)
#endif
#ifdef WHITEBOARD_PLUGIN_EXPORTS
        #define WHITEBOARD_PLUGIN_API __declspec(dllexport)
#else
        #define WHITEBOARD_PLUGIN_API __declspec(dllimport)
#endif
#ifdef CRM_PLUGIN_EXPORTS
        #define CRM_PLUGIN_API __declspec(dllexport)
#else
        #define CRM_PLUGIN_API __declspec(dllimport)
#endif
#ifdef TKPHONE_PLUGIN_EXPORTS
        #define TKPHONE_PLUGIN_API __declspec(dllexport)
#else
        #define TKPHONE_PLUGIN_API __declspec(dllimport)
#endif

#else
  extern char * AppName;
  extern char * AppMIME;
  extern char * AppVersion;
  extern char * FullAppName;

#ifdef GCC_NEW
#ifdef TKCBASE_EXPORTS
  #define TKCBASE_API __attribute__((visibility("default")))
#else
  #define TKCBASE_API
#endif

#ifdef AETHERA_EXPORTS
  #define AETHERA_API __attribute__((visibility("default")))
#else
  #define AETHERA_API
#endif

#ifdef AETHERA_PLUGIN_EXPORTS
  #define PLUGIN_API __attribute__((visibility("default")))
#else
  #define PLUGIN_API
#endif

#ifdef HOME_PLUGIN_EXPORTS
  #define HOME_PLUGIN_API __attribute__((visibility("default")))
#else
  #define HOME_PLUGIN_API
#endif

#ifdef MAIL_PLUGIN_EXPORTS
  #define MAIL_PLUGIN_API __attribute__((visibility("default")))
#else
  #define MAIL_PLUGIN_API
#endif

#ifdef MAILIMPORT_PLUGIN_EXPORTS
  #define MAILIMPORT_PLUGIN_API __attribute__((visibility("default")))
#else
  #define MAILIMPORT_PLUGIN_API
#endif

#ifdef NOTE_PLUGIN_EXPORTS
  #define NOTE_PLUGIN_API __attribute__((visibility("default")))
#else
  #define NOTE_PLUGIN_API
#endif

#ifdef CONTACT_PLUGIN_EXPORTS
  #define CONTACT_PLUGIN_API __attribute__((visibility("default")))
#else
  #define CONTACT_PLUGIN_API
#endif

#ifdef DATEBOOK_PLUGIN_EXPORTS
  #define DATEBOOK_PLUGIN_API __attribute__((visibility("default")))
#else
  #define DATEBOOK_PLUGIN_API
#endif

#ifdef JABBER_PLUGIN_EXPORTS
  #define JABBER_PLUGIN_API __attribute__((visibility("default")))
#else
  #define JABBER_PLUGIN_API
#endif

#ifdef WHITEBOARD_PLUGIN_EXPORTS
  #define WHITEBOARD_PLUGIN_API __attribute__((visibility("default")))
#else
  #define WHITEBOARD_PLUGIN_API
#endif

#ifdef CRM_PLUGIN_EXPORTS
  #define CRM_PLUGIN_API __attribute__((visibility("default")))
#else
  #define CRM_PLUGIN_API
#endif

#ifdef TKPHONE_PLUGIN_EXPORTS
  #define TKPHONE_PLUGIN_API __attribute__((visibility("default")))
#else
  #define TKPHONE_PLUGIN_API
#endif
#else
  #define TKCBASE_API
  #define AETHERA_API
  #define PLUGIN_API
  #define HOME_PLUGIN_API
  #define MAIL_PLUGIN_API
  #define MAILIMPORT_PLUGIN_API
  #define NOTE_PLUGIN_API
  #define CONTACT_PLUGIN_API
  #define DATEBOOK_PLUGIN_API
  #define JABBER_PLUGIN_API
  #define WHITEBOARD_PLUGIN_API
  #define CRM_PLUGIN_API
  #define TKPHONE_PLUGIN_API
#endif

#endif

#endif
