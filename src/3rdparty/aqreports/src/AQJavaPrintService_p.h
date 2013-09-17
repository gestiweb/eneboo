/***************************************************************************
 AQJavaPrintService_p.h
 -------------------
 begin                : 12/01/2011
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

#ifndef AQJAVAPRINTSERVICE_P_H_
#define AQJAVAPRINTSERVICE_P_H_

class AQJavaPrintService : public AQJavaPrintServiceInterface
{
public:

  AQJavaPrintService(AQReportsPrivate *dd) : d(dd) {}

  AQStdStringList printServiceNames() {
    AQStdStringList ret;
    AQ_TRY_VAL(printServiceNames_(), ret);
    return ret;
  }

  void *printService(const char *printServiceName) {
    void *ret = 0;
    AQ_TRY_VAL(printService_(printServiceName), ret);
    return ret;
  }

private:

  AQReportsPrivate *d;

  AQStdStringList printServiceNames_() {
    AQStdStringList ret;
    javax::print::PrintServiceLookup *pl = 0;
    javax::print::PrintService *ps = 0;
    JavaObjectArray *joa = 0;
    java::lang::Object **cos = 0;

    pl = new javax::print::PrintServiceLookup(wrapperIntern);
    joa = pl->lookupPrintServices(0, 0);
    cos = reinterpret_cast<java::lang::Object **>(joa->getArrayData());
    for (int i = 0; i < joa->getArrayLength(); ++i) {
      ps = new javax::print::PrintService(cos[i]->getJavaObject());
      ret.push_back(string(ps->getName()));
      delete ps;
    }

    delete pl;
    delete joa;
    delete[] cos;
    return ret;
  }

  void *printService_(const char *printServiceName) {
    javax::print::PrintServiceLookup *pl = 0;
    javax::print::PrintService *ps = 0;
    JavaObjectArray *joa = 0;
    java::lang::Object **cos = 0;

    pl = new javax::print::PrintServiceLookup(wrapperIntern);
    joa = pl->lookupPrintServices(0, 0);
    cos = reinterpret_cast<java::lang::Object **>(joa->getArrayData());
    for (int i = 0; i < joa->getArrayLength(); ++i) {
      ps = new javax::print::PrintService(cos[i]->getJavaObject());
      if (strcmp(printServiceName, ps->getName()) == 0)
        break;
      delete ps;
    }

    delete pl;
    delete joa;
    delete[] cos;
    return ps;
  }
};

#endif /* AQJAVAPRINTSERVICE_P_H_ */
