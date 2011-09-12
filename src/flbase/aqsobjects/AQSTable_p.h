/***************************************************************************
 AQSTable_p.h
 -------------------
 begin                : 30/01/2011
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

#ifndef AQSTABLE_P_H_
#define AQSTABLE_P_H_

#include "AQSScrollView_p.h"
#include "AQSTableItem_p.h"
#include "AQSPixmap_p.h"
#include <qtable.h>

class AQSTable : public AQSScrollView
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Table, ScrollView);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QHeader *horizontalHeader() const;
  QHeader *verticalHeader() const;
  void setItem(int, int, QTableItem *);
  void setItem(int, int, AQSTableItem *);
  void setText(int, int, const QString &);
  void setPixmap(int, int, QPixmap *);
  void setPixmap(int, int, AQSPixmap *);
  QTableItem *item(int, int) const;
  QString text(int, int) const;
  QPixmap *pixmap(int, int) const;
  void clearCell(int, int);
  QRect cellGeometry(int, int) const;
  int columnWidth(int) const;
  int rowHeight(int) const;
  int columnPos(int) const;
  int rowPos(int) const;
  int columnAt(int) const;
  int rowAt(int) const;
  void updateCell(int, int);
  int currentRow() const;
  int currentColumn() const;
  void ensureCellVisible(int, int);
  bool isSelected(int, int) const;
  bool isRowSelected(int, bool = FALSE) const;
  bool isColumnSelected(int, bool = FALSE) const;
  void removeSelection(int);
  int currentSelection() const;
  void selectCells(int, int, int, int);
  void selectRow(int);
  void selectColumn(int);
  QWidget *cellWidget(int, int) const;
  void clearCellWidget(int, int);
  QRect cellRect(int, int) const;
  bool isRowReadOnly(int) const;
  bool isColumnReadOnly(int) const;
  void repaintSelections();
  void updateHeaderStates();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[2].append(QString::fromLatin1("int,int"));
    candidates[3].append(QString::fromLatin1("int,int,QWidget*"));
    candidates[4].append(QString::fromLatin1("int,int,QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QTable;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QTable(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QTable(argValue<QWidget *>(args[0]),
                        *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("int,int"))
      return new QTable(args[0].variant().toInt(),
                        args[1].variant().toInt());
    if (sgt == QString::fromLatin1("int,int,QWidget*"))
      return new QTable(args[0].variant().toInt(),
                        args[1].variant().toInt(),
                        argValue<QWidget *>(args[2]));
    if (sgt == QString::fromLatin1("int,int,QWidget*,QString"))
      return new QTable(args[0].variant().toInt(),
                        args[1].variant().toInt(),
                        argValue<QWidget *>(args[2]),
                        *(argValue<QString *>(args[3])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[2].append(QString::fromLatin1("int,int"));
    candidates[3].append(QString::fromLatin1("int,int,QWidget*"));
    candidates[4].append(QString::fromLatin1("int,int,QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QHeader *AQSTable::horizontalHeader() const
{
  AQ_CALL_RET(horizontalHeader());
}
inline QHeader *AQSTable::verticalHeader() const
{
  AQ_CALL_RET(verticalHeader());
}
inline void AQSTable::setItem(int arg0,  int arg1,  QTableItem *arg2)
{
  AQ_CALL_VOID(setItem(arg0, arg1, arg2));
}
inline void AQSTable::setItem(int arg0,  int arg1, AQSTableItem *arg2)
{
  AQ_CALL_VOID(setItem(arg0, arg1, *arg2));
}
inline void AQSTable::setText(int arg0,  int arg1,  const QString &arg2)
{
  AQ_CALL_VOID(setText(arg0, arg1, arg2));
}
inline void AQSTable::setPixmap(int arg0,  int arg1,  QPixmap *arg2)
{
  AQ_CALL_VOID(setPixmap(arg0, arg1, *arg2));
}
inline void AQSTable::setPixmap(int arg0,  int arg1, AQSPixmap *arg2)
{
  AQ_CALL_VOID(setPixmap(arg0, arg1, *arg2));
}
inline QTableItem *AQSTable::item(int arg0,  int arg1) const
{
  AQ_CALL_RET(item(arg0, arg1));
}
inline QString AQSTable::text(int arg0,  int arg1) const
{
  AQ_CALL_RET_V(text(arg0, arg1), QString);
}
inline QPixmap *AQSTable::pixmap(int arg0,  int arg1) const
{
  AQ_CALL_RET_PTR(pixmap(arg0, arg1), QPixmap);
}
inline void AQSTable::clearCell(int arg0,  int arg1)
{
  AQ_CALL_VOID(clearCell(arg0, arg1));
}
inline QRect AQSTable::cellGeometry(int arg0,  int arg1) const
{
  AQ_CALL_RET_V(cellGeometry(arg0, arg1), QRect);
}
inline int AQSTable::columnWidth(int arg0) const
{
  AQ_CALL_RET_V(columnWidth(arg0), int);
}
inline int AQSTable::rowHeight(int arg0) const
{
  AQ_CALL_RET_V(rowHeight(arg0), int);
}
inline int AQSTable::columnPos(int arg0) const
{
  AQ_CALL_RET_V(columnPos(arg0), int);
}
inline int AQSTable::rowPos(int arg0) const
{
  AQ_CALL_RET_V(rowPos(arg0), int);
}
inline int AQSTable::columnAt(int arg0) const
{
  AQ_CALL_RET_V(columnAt(arg0), int);
}
inline int AQSTable::rowAt(int arg0) const
{
  AQ_CALL_RET_V(rowAt(arg0), int);
}
inline void AQSTable::updateCell(int arg0,  int arg1)
{
  AQ_CALL_VOID(updateCell(arg0, arg1));
}
inline int AQSTable::currentRow() const
{
  AQ_CALL_RET_V(currentRow(), int);
}
inline int AQSTable::currentColumn() const
{
  AQ_CALL_RET_V(currentColumn(), int);
}
inline void AQSTable::ensureCellVisible(int arg0,  int arg1)
{
  AQ_CALL_VOID(ensureCellVisible(arg0, arg1));
}
inline bool AQSTable::isSelected(int arg0,  int arg1) const
{
  AQ_CALL_RET_V(isSelected(arg0, arg1), bool);
}
inline bool AQSTable::isRowSelected(int arg0,  bool arg1) const
{
  AQ_CALL_RET_V(isRowSelected(arg0, arg1), bool);
}
inline bool AQSTable::isColumnSelected(int arg0,  bool arg1) const
{
  AQ_CALL_RET_V(isColumnSelected(arg0, arg1), bool);
}
inline void AQSTable::removeSelection(int arg0)
{
  AQ_CALL_VOID(removeSelection(arg0));
}
inline int AQSTable::currentSelection() const
{
  AQ_CALL_RET_V(currentSelection(), int);
}
inline void AQSTable::selectCells(int arg0,  int arg1,  int arg2,  int arg3)
{
  AQ_CALL_VOID(selectCells(arg0, arg1, arg2, arg3));
}
inline void AQSTable::selectRow(int arg0)
{
  AQ_CALL_VOID(selectRow(arg0));
}
inline void AQSTable::selectColumn(int arg0)
{
  AQ_CALL_VOID(selectColumn(arg0));
}
inline QWidget *AQSTable::cellWidget(int arg0,  int arg1) const
{
  AQ_CALL_RET(cellWidget(arg0, arg1));
}
inline void AQSTable::clearCellWidget(int arg0,  int arg1)
{
  AQ_CALL_VOID(clearCellWidget(arg0, arg1));
}
inline QRect AQSTable::cellRect(int arg0,  int arg1) const
{
  AQ_CALL_RET_V(cellRect(arg0, arg1), QRect);
}
inline bool AQSTable::isRowReadOnly(int arg0) const
{
  AQ_CALL_RET_V(isRowReadOnly(arg0), bool);
}
inline bool AQSTable::isColumnReadOnly(int arg0) const
{
  AQ_CALL_RET_V(isColumnReadOnly(arg0), bool);
}
inline void AQSTable::repaintSelections()
{
  AQ_CALL_VOID(repaintSelections());
}
inline void AQSTable::updateHeaderStates()
{
  AQ_CALL_VOID(updateHeaderStates());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTABLE_P_H_ */
// @AQSWIDGET@
