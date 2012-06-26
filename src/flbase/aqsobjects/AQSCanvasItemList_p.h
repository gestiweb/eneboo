/***************************************************************************
 AQSCanvasItemList_p.h
 -------------------
 begin                : 07/10/2011
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

#ifndef AQSCANVASITEMLIST_P_H_
#define AQSCANVASITEMLIST_P_H_

#include "AQSCanvasItem_p.h"

// @AQ_PTR_INDIRECTION@

class AQSCanvasItemList : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(CanvasItemList, VoidPtr);

public slots:
  int createIterator() {
    int itRet = itCount_;
    QCanvasItemList::const_iterator it;
    itMap_[itRet] = it;
    ++itCount_;
    return itRet;
  }

  void destroyIterator(int it) {
    itMap_.remove(it);
  }

  void iteratorBegin(int it) {
    if (!o_)
      return;
    itMap_[it] = o_->begin();
  }

  bool iteratorIsBegin(int it) {
    if (!o_)
      return false;
    return (itMap_.contains(it) && itMap_[it] == o_->begin());
  }

  void iteratorEnd(int it) {
    if (!o_)
      return;
    itMap_[it] = o_->end();
  }

  bool iteratorIsEnd(int it) {
    if (!o_)
      return false;
    return (itMap_.contains(it) && itMap_[it] == o_->end());
  }

  void iteratorNext(int it) {
    if (!itMap_.contains(it))
      return;
    itMap_[it]++;
  }

  void iteratorPrev(int it) {
    if (!itMap_.contains(it))
      return;
    itMap_[it]--;
  }

  QCanvasItem *iteratorValue(int it) {
    if (!itMap_.contains(it))
      return 0;
    return *itMap_[it];
  }

private:
  static int itCount_;
  QMap<int, QCanvasItemList::const_iterator> itMap_;

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  uint remove(QCanvasItem *);
  uint remove(AQSCanvasItem *);
  void clear();
  int size() const;
  bool empty() const;
  bool isEmpty() const;
  QCanvasItem *first();
  QCanvasItem *last();
  int findIndex(QCanvasItem *) const;
  int findIndex(AQSCanvasItem *) const;
  int contains(QCanvasItem *) const;
  int contains(AQSCanvasItem *) const;
  int count() const;

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline uint AQSCanvasItemList::remove(QCanvasItem *arg0)
{
  AQ_CALL_RET_V(remove(arg0), uint);
}
inline uint AQSCanvasItemList::remove(AQSCanvasItem *arg0)
{
  AQ_CALL_RET_V(remove(*arg0), uint);
}
inline void AQSCanvasItemList::clear()
{
  AQ_CALL_VOID(clear());
}
inline int AQSCanvasItemList::size() const
{
  AQ_CALL_RET_V(size(), int);
}
inline bool AQSCanvasItemList::empty() const
{
  AQ_CALL_RET_V(empty(), bool);
}
inline bool AQSCanvasItemList::isEmpty() const
{
  AQ_CALL_RET_V(isEmpty(), bool);
}
inline QCanvasItem *AQSCanvasItemList::first()
{
  AQ_CALL_RET(first());
}
inline QCanvasItem *AQSCanvasItemList::last()
{
  AQ_CALL_RET(last());
}
inline int AQSCanvasItemList::findIndex(QCanvasItem *arg0) const
{
  AQ_CALL_RET_V(findIndex(arg0), int);
}
inline int AQSCanvasItemList::findIndex(AQSCanvasItem *arg0) const
{
  AQ_CALL_RET_V(findIndex(*arg0), int);
}
inline int AQSCanvasItemList::contains(QCanvasItem *arg0) const
{
  AQ_CALL_RET_V(contains(arg0), int);
}
inline int AQSCanvasItemList::contains(AQSCanvasItem *arg0) const
{
  AQ_CALL_RET_V(contains(*arg0), int);
}
inline int AQSCanvasItemList::count() const
{
  AQ_CALL_RET_V(count(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCANVASITEMLIST_P_H_ */
// @AQSVOIDPTR@
