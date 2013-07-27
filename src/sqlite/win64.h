/*
** 2013 July 26
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
*/
#if defined(AQ_WIN64)
#ifndef _OFF_T_
#define _OFF_T_
     typedef long long _off_t;
#if !defined(NO_OLDNAMES) || defined(_POSIX)
  typedef long off32_t;
  typedef long long off64_t;
#endif
#endif
#ifndef _OFF64_T_DEFINED
#define _OFF64_T_DEFINED
typedef long long _off64_t;
#endif
#endif

