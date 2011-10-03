/***************************************************************************
 AQSVoidPtr_p.h
 -------------------
 begin                : 29/01/2011
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

#ifndef AQSVOIDPTR_P_H_
#define AQSVOIDPTR_P_H_

#include "AQS_p.h"

#define AQ_DECLARE_AQS_PREFIX_VOID(Prefix,Class,BaseClass) \
  protected: \
  void internalInit(Prefix##Class *o) { \
    o_ = o; \
    AQS##BaseClass::internalInit(o); \
  } \
  private: \
  Prefix##Class *o_; \
  void init(void *vo) { \
    o_ = reinterpret_cast<Prefix##Class *>(vo); \
    if (!o_) { \
      AQS_IF_DEBUG(printf("%s\n", \
                      AQ_QUOTEME(BaseClass: AQS##Class must be initialized with a valid Prefix##Class) \
                         )); \
    } else { \
      QObject::setName(AQ_QUOTEME(Prefix##Class)); \
      AQS##BaseClass::internalInit(o_); \
      AQS_IF_DEBUG(printf("%s init %p\n", AQ_QUOTEME(AQS##Class), o_)); \
      specializedInternalInit(); \
    } \
  } \
  void finish() { \
    if (!finished_) { \
      AQS_IF_DEBUG(printf("%s finish %p\n", AQ_QUOTEME(AQS##Class), o_)); \
      finished_ = true; \
      specializedInternalFinish(); \
    } \
  }\
  public: \
  virtual const char *RTTI() const { return #Class; } \
  operator Prefix##Class *() { return o_; } \
  operator const Prefix##Class *() const { return o_; } \
  operator Prefix##Class &() { if (o_) return *o_; } \
  operator const Prefix##Class &() const { if (o_) return *o_; } \
  AQS##Class() : AQS##BaseClass (), o_(0) {} \
  AQS##Class(void *vo) : AQS##BaseClass () {init(vo);} \
  virtual ~AQS##Class() { \
    finish(); \
  } \
  AQ_STATIC_CONSTRUCTOR(Prefix,Class)

#define AQ_DECLARE_AQS_VOID(Class,BaseClass) AQ_DECLARE_AQS_PREFIX_VOID(Q,Class,BaseClass)
#define AQ_DECLARE_AQS_VOID_AQOBJECT(Class,BaseClass) AQ_DECLARE_AQS_PREFIX_VOID(AQ,Class,BaseClass)

class AQSVoidPtr : public AQSBaseObject
{
  Q_OBJECT

  AQ_DECLARE_AQS(VoidPtr)

public:
  AQSVoidPtr(void *vo) : AQSBaseObject() {
    init(vo);
  }

public slots :
  bool isEqual(void *other) const {
    return (o_ ? o_ == other : false);
  }

  virtual QCString rtti() {
    return RTTI();
  }

protected:
  void internalInit(void *o) {
    o_ = o;
    AQSBaseObject::internalInit(o);
  }

private:
  void *o_;
};

#endif /* AQSVOIDPTR_P_H_ */

// @AQSVOIDPTR@

