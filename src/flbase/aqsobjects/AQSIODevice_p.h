/***************************************************************************
 AQSIODevice_p.h
 -------------------
 begin                : 28/02/2011
 copyright            : (C) 2003-2011 by InfoSiAL S.L.
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

#ifndef AQSIODEVICE_P_H_
#define AQSIODEVICE_P_H_

#include "AQSVoidPtr_p.h"
#include <qiodevice.h>

class AQSIODevice : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(IODevice, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int flags() const;
  int mode() const;
  int state() const;
  bool isDirectAccess() const;
  bool isSequentialAccess() const;
  bool isCombinedAccess() const;
  bool isBuffered() const;
  bool isRaw() const;
  bool isSynchronous() const;
  bool isAsynchronous() const;
  bool isTranslated() const;
  bool isReadable() const;
  bool isWritable() const;
  bool isReadWrite() const;
  bool isInactive() const;
  bool isOpen() const;
  int status() const;
  void resetStatus();
  virtual bool open(int);
  virtual void close();
  virtual void flush();
  virtual uint size() const;
  virtual uint at() const;
  virtual bool at(uint);
  virtual bool atEnd() const;
  bool reset();
  virtual QByteArray *readAll();
  virtual int getch();
  virtual int putch(int);
  virtual int ungetch(int);

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSIODevice::flags() const
{
  AQ_CALL_RET_V(flags(), int);
}
inline int AQSIODevice::mode() const
{
  AQ_CALL_RET_V(mode(), int);
}
inline int AQSIODevice::state() const
{
  AQ_CALL_RET_V(state(), int);
}
inline bool AQSIODevice::isDirectAccess() const
{
  AQ_CALL_RET_V(isDirectAccess(), bool);
}
inline bool AQSIODevice::isSequentialAccess() const
{
  AQ_CALL_RET_V(isSequentialAccess(), bool);
}
inline bool AQSIODevice::isCombinedAccess() const
{
  AQ_CALL_RET_V(isCombinedAccess(), bool);
}
inline bool AQSIODevice::isBuffered() const
{
  AQ_CALL_RET_V(isBuffered(), bool);
}
inline bool AQSIODevice::isRaw() const
{
  AQ_CALL_RET_V(isRaw(), bool);
}
inline bool AQSIODevice::isSynchronous() const
{
  AQ_CALL_RET_V(isSynchronous(), bool);
}
inline bool AQSIODevice::isAsynchronous() const
{
  AQ_CALL_RET_V(isAsynchronous(), bool);
}
inline bool AQSIODevice::isTranslated() const
{
  AQ_CALL_RET_V(isTranslated(), bool);
}
inline bool AQSIODevice::isReadable() const
{
  AQ_CALL_RET_V(isReadable(), bool);
}
inline bool AQSIODevice::isWritable() const
{
  AQ_CALL_RET_V(isWritable(), bool);
}
inline bool AQSIODevice::isReadWrite() const
{
  AQ_CALL_RET_V(isReadWrite(), bool);
}
inline bool AQSIODevice::isInactive() const
{
  AQ_CALL_RET_V(isInactive(), bool);
}
inline bool AQSIODevice::isOpen() const
{
  AQ_CALL_RET_V(isOpen(), bool);
}
inline int AQSIODevice::status() const
{
  AQ_CALL_RET_V(status(), int);
}
inline void AQSIODevice::resetStatus()
{
  AQ_CALL_VOID(resetStatus());
}
inline bool AQSIODevice::open(int arg0)
{
  AQ_CALL_RET_V(open(arg0), bool);
}
inline void AQSIODevice::close()
{
  AQ_CALL_VOID(close());
}
inline void AQSIODevice::flush()
{
  AQ_CALL_VOID(flush());
}
inline uint AQSIODevice::size() const
{
  AQ_CALL_RET_V(size(), uint);
}
inline uint AQSIODevice::at() const
{
  AQ_CALL_RET_V(at(), uint);
}
inline bool AQSIODevice::at(uint arg0)
{
  AQ_CALL_RET_V(at(arg0), bool);
}
inline bool AQSIODevice::atEnd() const
{
  AQ_CALL_RET_V(atEnd(), bool);
}
inline bool AQSIODevice::reset()
{
  AQ_CALL_RET_V(reset(), bool);
}
inline QByteArray *AQSIODevice::readAll()
{
  AQ_CALL_RET_PTR(readAll(), QByteArray);
}
inline int AQSIODevice::getch()
{
  AQ_CALL_RET_V(getch(), int);
}
inline int AQSIODevice::putch(int arg0)
{
  AQ_CALL_RET_V(putch(arg0), int);
}
inline int AQSIODevice::ungetch(int arg0)
{
  AQ_CALL_RET_V(ungetch(arg0), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSIODEVICE_P_H_ */
// @AQSVOIDPTR@
