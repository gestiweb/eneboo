/****************************************************************************
**
** Definition of Qt/FB central server classes
**
** Created : 991025
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free Qt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef QWINDOWSYSTEM_QWS_H
#define QWINDOWSYSTEM_QWS_H

#ifndef QT_H
#include "qwssocket_qws.h"
#include "qmap.h"
#include "qdatetime.h"
#include "qptrlist.h"
#include "qimage.h"

#include "qwsproperty_qws.h"
#include "qwscommand_qws.h"
#include "qwsevent_qws.h"
#endif // QT_H

#include "qkbd_qws.h"

struct SWCursorData;
class QWSCursor;
class QWSClient;
class QWSRegionManager;
class QGfx;
class QWSServerData;

class QWSInternalWindowInfo
{

public:

    int winid;
    unsigned int clientid;
    QString name;   // Corresponds to QObject name of top-level widget

};

struct QWSWindowData;

class QWSScreenSaver
{
public:
    virtual ~QWSScreenSaver();
    virtual void restore()=0;
    virtual bool save(int level)=0;
};

class QWSWindow
{
    friend class QWSServer;
public:
    QWSWindow(int i, QWSClient* client);
    ~QWSWindow();

    int winId() const { return id; }
    const QString &name() const { return rgnName; }
    const QString &caption() const { return rgnCaption; }
    QWSClient* client() const { return c; }
    QRegion requested() const { return requested_region; }
    QRegion allocation() const { return allocated_region; }
    bool isVisible() const { return !requested_region.isEmpty(); }
    bool isPartiallyObscured() const { return requested_region!=allocated_region; }
    bool isFullyObscured() const { return allocated_region.isEmpty(); }

    void raise();
    void lower();
    void show();
    void hide();
    void setActiveWindow();

private:
    bool hidden() const { return requested_region.isEmpty(); }
    bool forClient(const QWSClient* cl) const { return cl==c; }

    void setName( const QString &n );
    void setCaption( const QString &c );

    void addAllocation( QWSRegionManager *, const QRegion & );
    void removeAllocation( QWSRegionManager *, const QRegion & );

    int  allocationIndex() const { return alloc_region_idx; }
    void setAllocationIndex( int i ) { alloc_region_idx = i; modified = TRUE; }
    void updateAllocation();

    void setNeedAck( bool n ) { needAck = n; }

    void focus(bool get);
    int focusPriority() const { return last_focus_time; }
    void operation( QWSWindowOperationEvent::Operation o );
    void shuttingDown() { last_focus_time=0; }

private:
    int id;
    QString rgnName;
    QString rgnCaption;
    int alloc_region_idx;
    bool modified;
    bool needAck;
    bool onTop;
    QWSClient* c;
    QRegion requested_region;
    QRegion allocated_region;
    QRegion exposed;
    int last_focus_time;
    QWSWindowData *d;
};

#ifndef QT_NO_SOUND
class QWSSoundServer;
#ifdef QT_USE_OLD_QWS_SOUND
class QWSSoundServerData;

class QWSSoundServer : public QObject {
    Q_OBJECT
public:
    QWSSoundServer(QObject* parent);
    ~QWSSoundServer();
    void playFile(const QString& filename);
private slots:
    void feedDevice(int fd);
private:
    QWSSoundServerData* d;
};
#endif
#endif







/*********************************************************************
 *
 * Class: QWSServer
 *
 *********************************************************************/

class QWSMouseHandler;
struct QWSCommandStruct;

#ifndef QT_NO_QWS_MULTIPROCESS
class QWSServer : public QWSServerSocket
#else
class QWSServer : public QObject
#endif
{
    friend class QCopChannel;
    friend class QWSMouseHandler;
    friend class QWSWindow;
    friend class QWSDisplay;
    friend class QWSInputMethod;
    Q_OBJECT

public:
    QWSServer( int flags = 0, QObject *parent=0, const char *name=0 );
    ~QWSServer();
    enum ServerFlags { DisableKeyboard = 0x01,
		       DisableMouse = 0x02 };


    enum GUIMode { NoGui = FALSE, NormalGUI = TRUE, Server };

    static void sendKeyEvent(int unicode, int keycode, int modifiers, bool isPress,
			     bool autoRepeat);
    static void processKeyEvent(int unicode, int keycode, int modifiers, bool isPress,
				bool autoRepeat);

#ifndef QT_NO_QWS_IM
    enum IMState { IMStart, IMCompose, IMEnd };

    void sendIMEvent( IMState state, const QString& txt, int cpos, int selLen );
#endif

#ifndef QT_NO_QWS_KEYBOARD    
    typedef struct KeyMap {
	int  key_code;
	ushort unicode;
	ushort shift_unicode;
	ushort ctrl_unicode;
    };

    typedef struct KeyOverride {
	ushort scan_code;
	KeyMap map;
    };

    static const KeyMap *keyMap();

    static void setOverrideKeys( const KeyOverride* );

    class KeyboardFilter
    {
    public:
	virtual bool filter(int unicode, int keycode, int modifiers, 
			    bool isPress, bool autoRepeat)=0;
    };

    static void setKeyboardFilter( KeyboardFilter *f );
#endif    
#ifndef QT_NO_QWS_IM
    static void setCurrentInputMethod( QWSInputMethod *im );
    static void resetInputMethod();
    static void setMicroFocus( int x, int y );
#endif

    static void setDefaultMouse( const char * );
    static void setDefaultKeyboard( const char * );
    static void setMaxWindowRect(const QRect&);
    static void sendMouseEvent(const QPoint& pos, int state);

    static void setDesktopBackground( const QImage &img );
    static void setDesktopBackground( const QColor & );
    static QWSMouseHandler *mouseHandler();
    static void setMouseHandler(QWSMouseHandler*);
#ifndef QT_NO_QWS_KEYBOARD
    static QWSKeyboardHandler* keyboardHandler();
    static void setKeyboardHandler(QWSKeyboardHandler* kh);
#endif
    QWSWindow *windowAt( const QPoint& pos );

    // For debugging only at this time
    const QPtrList<QWSWindow> &clientWindows() { return windows; }

    void openMouse();
    void closeMouse();
#ifndef QT_NO_QWS_KEYBOARD
    void openKeyboard();
    void closeKeyboard();
#endif

    static void setScreenSaver(QWSScreenSaver*);
    static void setScreenSaverIntervals(int* ms);
    static void setScreenSaverInterval(int);
    static bool screenSaverActive();
    static void screenSaverActivate(bool);

    // the following are internal.
    void refresh();
    void refresh(QRegion &);

    void enablePainting(bool);
    static void processEventQueue();
    static QPtrList<QWSInternalWindowInfo> * windowList();

    void sendPropertyNotifyEvent( int property, int state );
#ifndef QT_NO_QWS_PROPERTIES
    QWSPropertyManager *manager() {
	return &propertyManager;
    }
#endif
    
    static QPoint mousePosition;

    static void startup( int flags );
    static void closedown();

    static void beginDisplayReconfigure();
    static void endDisplayReconfigure();

#ifndef QT_NO_QWS_CURSOR
    static void setCursorVisible( bool );
    static bool isCursorVisible();
#endif

    enum WindowEvent { Create=0x0001, Destroy=0x0002, Hide=0x0004, Show=0x0008,
		       Raise=0x0010, Lower=0x0020, Geometry=0x0040, Active = 0x0080,
		       Name=0x0100 };

signals:
    void windowEvent( QWSWindow *w, QWSServer::WindowEvent e );

#ifndef QT_NO_COP
    void newChannel( const QString& channel);
    void removedChannel(const QString& channel);

#endif    
private:
#ifndef QT_NO_COP
    static void sendQCopEvent( QWSClient *c, const QCString &ch,
			       const QCString &msg, const QByteArray &data,
			       bool response = FALSE );
#endif
    void move_region( const QWSRegionMoveCommand * );
    void set_altitude( const QWSChangeAltitudeCommand * );
    void request_focus( const QWSRequestFocusCommand * );
    void request_region( int, QRegion );
    void destroy_region( const QWSRegionDestroyCommand * );
    void name_region( const QWSRegionNameCommand * );
    void set_identity( const QWSIdentifyCommand * );
#ifndef QT_NO_QWS_IM
    void set_micro_focus( const QWSSetMicroFocusCommand * );
    void reset_im( const QWSResetIMCommand * );
    static void sendKeyEventUnfiltered(int unicode, int keycode, 
				       int modifiers, bool isPress,
				       bool autoRepeat);

#endif
    static void emergency_cleanup();

    static QColor *bgColor;
    static QImage *bgImage;

    void sendMaxWindowRectEvents();
#ifndef QT_NO_QWS_MULTIPROCESS
    void newConnection( int socket );
#endif
    void invokeIdentify( const QWSIdentifyCommand *cmd, QWSClient *client );
    void invokeCreate( QWSCreateCommand *cmd, QWSClient *client );
    void invokeRegionName( const QWSRegionNameCommand *cmd, QWSClient *client );
    void invokeRegion( QWSRegionCommand *cmd, QWSClient *client );
    void invokeRegionMove( const QWSRegionMoveCommand *cmd, QWSClient *client );
    void invokeRegionDestroy( const QWSRegionDestroyCommand *cmd, QWSClient *client );
    void invokeSetAltitude( const QWSChangeAltitudeCommand *cmd, QWSClient *client );
#ifndef QT_NO_QWS_PROPERTIES
    void invokeAddProperty( QWSAddPropertyCommand *cmd );
    void invokeSetProperty( QWSSetPropertyCommand *cmd );
    void invokeRemoveProperty( QWSRemovePropertyCommand *cmd );
    void invokeGetProperty( QWSGetPropertyCommand *cmd, QWSClient *client );
#endif //QT_NO_QWS_PROPERTIES
    void invokeSetSelectionOwner( QWSSetSelectionOwnerCommand *cmd );
    void invokeConvertSelection( QWSConvertSelectionCommand *cmd );
    void invokeSetFocus( const QWSRequestFocusCommand *cmd, QWSClient *client );

    void initIO();
    void setFocus( QWSWindow*, bool gain );
#ifndef QT_NO_QWS_CURSOR
    void invokeDefineCursor( QWSDefineCursorCommand *cmd, QWSClient *client );
    void invokeSelectCursor( QWSSelectCursorCommand *cmd, QWSClient *client );
#endif
    void invokeGrabMouse( QWSGrabMouseCommand *cmd, QWSClient *client );
    void invokeGrabKeyboard( QWSGrabKeyboardCommand *cmd, QWSClient *client );
#ifndef QT_NO_SOUND
    void invokePlaySound( QWSPlaySoundCommand *cmd, QWSClient *client );
#endif
#ifndef QT_NO_COP
    void invokeRegisterChannel( QWSQCopRegisterChannelCommand *cmd,
				QWSClient *client );
    void invokeQCopSend( QWSQCopSendCommand *cmd, QWSClient *client );
#endif
    void invokeRepaintRegion( QWSRepaintRegionCommand *cmd, 
			      QWSClient *client );
#ifndef QT_NO_QWS_IM
        void invokeSetMicroFocus( const QWSSetMicroFocusCommand *cmd,
                                QWSClient *client );
        void invokeResetIM( const QWSResetIMCommand *cmd,
                                QWSClient *client );
#endif

    QWSMouseHandler* newMouseHandler(const QString& spec);
    void openDisplay();
    void closeDisplay();

    void showCursor();
    void hideCursor();
    void initializeCursor();
    void paintServerRegion();
    void paintBackground( const QRegion & );
    void clearRegion( const QRegion &r, const QColor &c );
    void refreshBackground();
    void resetGfx();

private slots:
#ifndef QT_NO_QWS_MULTIPROCESS
    void clientClosed();
    void doClient();
    void deleteWindowsLater();
#endif

    void screenSaverWake();
    void screenSaverSleep();
    void screenSaverTimeout();

private:
    void disconnectClient( QWSClient * );
    void screenSave(int level);
    void doClient( QWSClient * );
    typedef QMapIterator<int,QWSClient*> ClientIterator;
    typedef QMap<int,QWSClient*> ClientMap;
    void releaseMouse(QWSWindow* w);
    void releaseKeyboard(QWSWindow* w);
    void updateClientCursorPos();

    uchar* sharedram;
    int ramlen;

    QGfx *gfx;

    ClientMap client;
#ifndef QT_NO_QWS_PROPERTIES
    QWSPropertyManager propertyManager;
#endif
    struct SelectionOwner {
	int windowid;
	struct Time {
	    void set( int h, int m, int s, int s2 ) {
		hour = h; minute = m; sec = s; ms = s2;
	    }
	    int hour, minute, sec, ms;
	} time;
    } selectionOwner;
    QTime timer;
    QWSServerData* d;
    int* screensaverinterval;

    QWSWindow *focusw;
    QWSWindow *mouseGrabber;
    bool mouseGrabbing;
    int swidth, sheight, sdepth;
#ifndef QT_NO_QWS_CURSOR
    bool haveviscurs;
    QWSCursor *cursor;	    // cursor currently shown
    QWSCursor *nextCursor;  // cursor to show once grabbing is off
#endif
    QRegion screenRegion;   // the entire display region
    QRegion serverRegion;
    QRegion dirtyBackground;
    bool disablePainting;
    QPtrList<QWSMouseHandler> mousehandlers;
#ifndef QT_NO_QWS_KEYBOARD
    QPtrList<QWSKeyboardHandler> keyboardhandlers;
#endif

    QPtrList<QWSCommandStruct> commandQueue;
    QWSRegionManager *rgnMan;

    // Window management
    QPtrList<QWSWindow> windows; // first=topmost
    QWSWindow* newWindow(int id, QWSClient* client);
    QWSWindow* findWindow(int windowid, QWSClient* client);
    void moveWindowRegion(QWSWindow*, int dx, int dy );
    QRegion setWindowRegion(QWSWindow*, QRegion r );
    void raiseWindow( QWSWindow *, int = 0);
    void lowerWindow( QWSWindow *, int = -1);
    void exposeRegion( QRegion , int index = 0 );
    void notifyModified( QWSWindow *active = 0 );
    void syncRegions( QWSWindow *active = 0 );

    void setCursor(QWSCursor *curs);

    // multimedia
#ifndef QT_NO_SOUND
    QWSSoundServer *soundserver;
#endif
#ifndef QT_NO_COP
    QMap<QString, QPtrList<QWSClient> > channels;
#endif
#ifndef QT_NO_QWS_IM
    bool microF;
    int microX;
    int microY;
#endif
};

extern QWSServer *qwsServer; //there can be only one


#ifndef QT_NO_QWS_IM
    class QWSInputMethod
    {
    public:
	QWSInputMethod();
	virtual ~QWSInputMethod();
	virtual bool filter(int unicode, int keycode, int modifiers, 
			    bool isPress, bool autoRepeat)=0;
	virtual void reset();
	virtual void setMicroFocus( int x, int y );
	virtual void setFont( const QFont& );
    protected:
	void sendIMEvent( QWSServer::IMState, const QString& txt, int cpos, int selLen = 0 );
	//void sendKeyEvent( int unicode, int keycode, int modifiers, 
	//		    bool isPress, bool autoRepeat);
    };

inline void QWSInputMethod::sendIMEvent( QWSServer::IMState state, const QString &txt, int cpos, int selLen )
{
    qwsServer->sendIMEvent( state, txt, cpos, selLen );

}


#endif




/*********************************************************************
 *
 * Class: QWSClient
 *
 *********************************************************************/

struct QWSMouseEvent;

typedef QMap<int, QWSCursor*> QWSCursorMap;

class QWSClient : public QObject
{
    Q_OBJECT
public:
    QWSClient( QObject* parent, int socket, int id );
    ~QWSClient();

    int socket() const;

    void setIdentity(const QString&);
    QString identity() const { return id; }

    void sendEvent( QWSEvent* event );
    void sendConnectedEvent( const char *display_spec );
    void sendMaxWindowRectEvent();
    void sendRegionModifyEvent( int winid, QRegion exposed, bool ack );
    void sendFocusEvent( int winid, bool get );
    void sendPropertyNotifyEvent( int property, int state );
    void sendPropertyReplyEvent( int property, int len, char *data );
    void sendSelectionClearEvent( int windowid );
    void sendSelectionRequestEvent( QWSConvertSelectionCommand *cmd, int windowid );
    QWSCommand* readMoreCommand();

    int clientId() const { return cid; }

    QWSCursorMap cursors;	// cursors defined by this client
signals:
    void connectionClosed();
    void readyRead();
private slots:
    void closeHandler();
    void errorHandler( int );
private:
    int s; // XXX csocket->d->socket->socket() is this value
#ifndef QT_NO_QWS_MULTIPROCESS
    QWSSocket *csocket;
#endif
    QWSCommand* command;
    uint isClosed : 1;
    QString id;
    int cid;
};

#endif // QWINDOWSYSTEM_QWS_H
