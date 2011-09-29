/***************************************************************************
           mutil.cpp  -  Kugar utlity class
           -------------------
 begin     : Tue Aug 17 1999
 copyright : (C) 1999 by Mutiny Bay Software
 email     : info@mutinybaysoftware.com
 copyright : (C) 2002 Alexander Dymo
 email     : cloudtemple@mksat.net
***************************************************************************/

#include <math.h>

#include "mutil.h"


/** Formats a date using one of the pre-defined formats */
QString MUtil::formatDate( const QDate & value, int format ) {
  QString string;
  QString month, day, year;

  // Reformat date parts - not necessarily required
  month.setNum( value.month() );
  day.setNum( value.day() );
  year.setNum( value.year() );

  // Add leading zeros to month and day
  if ( value.month() < 10 )
    month = "0" + month;
  if ( value.day() < 10 )
    day = "0" + day;

  // Create a two digit year
  year.setNum( value.year() );
  year = year.right( 2 );

  // Create the formatted date string
  switch ( format ) {
    case MUtil::MDY_SLASH:
      string =
        QString( "%1/%2/%3" ).arg( value.month() ).arg( value.day() ).arg( year );
      break;
    case MUtil::MDY_DASH:
      string =
        QString( "%1-%2-%3" ).arg( value.month() ).arg( value.day() ).arg( year );
      break;
    case MUtil::MMDDY_SLASH:
      string = QString( "%1/%2/%3" ).arg( month ).arg( day ).arg( year );
      break;
    case MUtil::MMDDY_DASH:
      string = QString( "%1-%2-%3" ).arg( month ).arg( day ).arg( year );
      break;
    case MUtil::MDYYYY_SLASH:
      string =
        QString( "%1/%2/%3" ).arg( value.month() ).arg( value.day() ).arg( value.year() );
      break;
    case MUtil::MDYYYY_DASH:
      string =
        QString( "%1-%2-%3" ).arg( value.month() ).arg( value.day() ).arg( value.year() );
      break;
    case MUtil::MMDDYYYY_SLASH:
      string = QString( "%1/%2/%3" ).arg( month ).arg( day ).arg( value.year() );
      break;
    case MUtil::MMDDYYYY_DASH:
      string = QString( "%1-%2-%3" ).arg( month ).arg( day ).arg( value.year() );
      break;
    case MUtil::YYYYMD_SLASH:
      string =
        QString( "%1/%2/%3" ).arg( value.year() ).arg( value.month() ).
        arg( value.day() );
      break;
    case MUtil::YYYYMD_DASH:
      string =
        QString( "%1-%2-%3" ).arg( value.year() ).arg( value.month() ).
        arg( value.day() );
      break;
    case MUtil::DDMMYY_PERIOD:
      string = QString( "%1.%2.%3" ).arg( day ).arg( month ).arg( year );
      break;
    case MUtil::DDMMYYYY_PERIOD:
      string = QString( "%1.%2.%3" ).arg( day ).arg( month ).arg( value.year() );
      break;
    case MUtil::DMY_SLASH:
      string =
        QString( "%1/%2/%3" ).arg( value.day() ).arg( value.month() ).arg( year );
      break;
    case MUtil::DMY_DASH:
      string =
        QString( "%1-%2-%3" ).arg( value.day() ).arg( value.month() ).arg( year );
      break;
    case MUtil::DDMMY_SLASH:
      string = QString( "%1/%2/%3" ).arg( day ).arg( month ).arg( year );
      break;
    case MUtil::DDMMY_DASH:
      string = QString( "%1-%2-%3" ).arg( day ).arg( month ).arg( year );
      break;
    case MUtil::DMYYYY_SLASH:
      string =
        QString( "%1/%2/%3" ).arg( value.day() ).arg( value.month() ).arg( value.year() );
      break;
    case MUtil::DMYYYY_DASH:
      string =
        QString( "%1-%2-%3" ).arg( value.day() ).arg( value.month() ).arg( value.year() );
      break;
    case MUtil::DDMMYYYY_SLASH:
      string = QString( "%1/%2/%3" ).arg( day ).arg( month ).arg( value.year() );
      break;
    case MUtil::DDMMYYYY_DASH:
      string = QString( "%1-%2-%3" ).arg( day ).arg( month ).arg( value.year() );
      break;
    case MUtil::DD:
      string = QString( "%1" ).arg( day );
      break;
    case MUtil::MM:
      string = QString( "%1" ).arg( month );
      break;
    case MUtil::Month:
      string = QDate::longMonthName( value.month() ) ;
      break;
    case MUtil::YYYY:
      string = QString( "%1" ).arg( value.year() );
      break;
    case MUtil::YY:
      string = year.right( 2 );
      break;
    case MUtil::Y:
      string = year.right( 1 );
      break;
    default:
      string = value.toString();
  }
  return string;
}

/** Returns the count for an array of doubles */
int MUtil::count( QMemArray < double >*values ) {
  return values->size();
}

/** Returns the sum for an array of doubles */
double MUtil::sum( QMemArray < double >*values ) {
  double tmpSum = 0.0;
  int size = count( values );

  for ( int i = 0; i < size; i++ )
    tmpSum += values->at( i );
  return tmpSum;
}

/** Returns the average value for an array of doubles */
double MUtil::average( QMemArray < double >*values ) {
  return sum( values ) / count( values );
}

/** Returns the variance for an array of doubles */
double MUtil::variance( QMemArray < double >*values ) {
  double tmpVar = 0.0;
  double tmpAvg = average( values );
  int size = count( values );

  for ( int i = 0; i < size; i++ )
    tmpVar += pow(( values->at( i ) - tmpAvg ), 2 ) / size;

  return tmpVar;
}

/** Returns the standard deviation for an array of doubles */
double MUtil::stdDeviation( QMemArray < double >*values ) {
  return sqrt( variance( values ) );
}
