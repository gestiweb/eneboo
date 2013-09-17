/***************************************************************************
AQSqlNodeInfo.h
-------------------
begin                : 02/09/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
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

#ifndef AQSQLNODEINFO_H_
#define AQSQLNODEINFO_H_

#include <AQSql/AQSql.h>

/**
Información de nodos del sistema de ficheros SQL
 
@author InfoSiAL S.L.
*/
class AQSqlNodeInfo
{
  friend class AQSqlFileSystem;

protected:

  AQSqlNodeInfo();

  qlonglong idNode_;
  QString dateCre_;
  QString dateMod_;
  QString dateAcc_;
  QString sha_;

public:

  AQSqlNodeInfo( const AQSqlNodeInfo & nodeInfo );

  AQSqlNodeInfo & operator=( const AQSqlNodeInfo & nodeInfo );
  bool operator==( const AQSqlNodeInfo & nodeInfo ) const;
  bool operator!=( const AQSqlNodeInfo & nodeInfo ) const
  {
    return !( operator==( nodeInfo ) );
  }

  qlonglong idNode() const;
  QString dateCre() const;
  QString dateMod() const;
  QString dateAcc() const;
  QString sha() const;
  bool isValid() const;

  qlonglong idParentNode;
  QString name;
  QString idUser;
  QString idGroup;
  QString perms;
  AQSql::SqlFlagsNode flag;
  QString specialFlag;

private:

  void copy( const AQSqlNodeInfo & nodeInfo );
};

inline AQSqlNodeInfo::AQSqlNodeInfo( const AQSqlNodeInfo & nodeInfo )
{
  copy( nodeInfo );
}

inline AQSqlNodeInfo & AQSqlNodeInfo::operator=( const AQSqlNodeInfo & nodeInfo )
{
  copy( nodeInfo );
  return *this;
}

inline bool AQSqlNodeInfo::operator==( const AQSqlNodeInfo & nodeInfo ) const
{
  return ( idParentNode == nodeInfo.idParentNode &&
           name == nodeInfo.name &&
           idUser == nodeInfo.idUser &&
           idGroup == nodeInfo.idGroup &&
           perms == nodeInfo.perms &&
           flag == nodeInfo.flag &&
           specialFlag == nodeInfo.specialFlag &&
           idNode_ == nodeInfo.idNode() &&
           dateCre_ == nodeInfo.dateCre() &&
           dateMod_ == nodeInfo.dateMod() &&
           dateAcc_ == nodeInfo.dateAcc() &&
           sha_ == nodeInfo.sha_ );
}

inline void AQSqlNodeInfo::copy( const AQSqlNodeInfo & nodeInfo )
{
  idParentNode = nodeInfo.idParentNode;
  name = nodeInfo.name;
  idUser = nodeInfo.idUser;
  idGroup = nodeInfo.idGroup;
  perms = nodeInfo.perms;
  flag = nodeInfo.flag;
  specialFlag = nodeInfo.specialFlag;
  idNode_ = nodeInfo.idNode();
  dateCre_ = nodeInfo.dateCre();
  dateMod_ = nodeInfo.dateMod();
  dateAcc_ = nodeInfo.dateAcc();
}

inline qlonglong AQSqlNodeInfo::idNode() const
{
  return idNode_;
}

inline QString AQSqlNodeInfo::dateCre() const
{
  return AQSqlNodeInfo::dateCre_;
}

inline QString AQSqlNodeInfo::dateMod() const
{
  return AQSqlNodeInfo::dateMod_;
}

inline QString AQSqlNodeInfo::dateAcc() const
{
  return dateAcc_;
}

inline QString AQSqlNodeInfo::sha() const
{
  return sha_;
}

inline bool AQSqlNodeInfo::isValid() const
{
  return ( idNode_ != -1 && !name.contains( '|' ) );
}

#endif /*AQSQLNODEINFO_H_*/
