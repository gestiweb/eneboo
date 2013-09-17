/***************************************************************************
 AQJasperPrintManager_p.h
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

#ifndef AQJASPERPRINTMANAGER_P_H_
#define AQJASPERPRINTMANAGER_P_H_

#define AQ_JPX    javax::print
#define AQ_JXPATT AQ_JPX::attribute
#define AQ_HANDLE_JAVA_EXCEPTION                    \
  {                                                 \
    jthrowable exc = javaEnv->ExceptionOccurred();  \
    if (exc) {                                      \
      javaException = exc;                          \
      javaEnv->ExceptionDescribe();                 \
      javaEnv->ExceptionClear();                    \
      throw exc;                                    \
    }                                               \
  }

static inline void aqExporterAddSetParameter(jobject exporter,
                                             jobject jarg1,
                                             jobject jarg2)
{
  jclass    cls = javaEnv->FindClass(
                    "net/sf/jasperreports/engine/JRExporter"
                  );
  AQ_HANDLE_JAVA_EXCEPTION
  jmethodID mid = javaEnv->GetMethodID(
                    cls, "setParameter",
                    "(Lnet/sf/jasperreports/engine/JRExporterParameter;Ljava/lang/Object;)V"
                  );
  AQ_HANDLE_JAVA_EXCEPTION
  javaEnv->CallVoidMethod(exporter, mid, jarg1, jarg2);
  AQ_HANDLE_JAVA_EXCEPTION
}

class AQJasperPrintManager : public AQJasperPrintManagerInterface
{
public:

  enum PageOrientation {
    Portrait,
    Landscape
  };

  enum PageSize {
    A4, B5, Letter, Legal, Executive,
    A0, A1, A2, A3, A5, A6, A7, A8, A9, B0, B1,
    B10, B2, B3, B4, B6, B7, B8, B9, C5E, Comm10E,
    DLE, Folio, Ledger, Tabloid, NPageSize
  };

  AQJasperPrintManager(AQReportsPrivate *dd) : d(dd), p_(0) {
    o_ = new JASPER::engine::JasperPrintManager(wrapperIntern);
  }
  ~AQJasperPrintManager() {
    delete o_;
    delete p_;
  }

  void loadJasperPrint(const char *src) {
    AQ_TRY(loadJasperPrint_(src));
  }

  int pageCount() const {
    return (p_ ? p_->getPages()->size() : 0);
  }

  int pageWidth() const {
    return (p_ ? p_->getPageWidth() : 0);
  }

  int pageHeight() const {
    return (p_ ? p_->getPageHeight() : 0);
  }

  int pageOrientation() {
    if (!p_)
      return 0;
    int ret = 0;
    AQ_TRY_VAL(pageOrientation_(), ret);
    return ret;
  }

  void printPageToImage(const char *dst, int pageIndex, const char *format) {
    if (!p_)
      return;
    AQ_TRY(printPageToImage_(dst, pageIndex, format));
  }

  bool printReport(bool withPrintDialog) {
    if (!p_)
      return false;
    bool ret = false;
    AQ_TRY_VAL(o_->printReport(p_, withPrintDialog), ret);
    return ret;
  }

  bool printReport(const char *printServiceName, int copies = 1) {
    if (!p_)
      return false;
    bool ret = false;
    AQ_TRY_VAL(printReport_(printServiceName, copies), ret);
    return ret;
  }

  bool exportReportToFile(const char *dst, const char *format) {
    if (!p_)
      return false;
    bool ret = false;
    AQ_TRY_VAL(exportReportToFile_(dst, format), ret);
    return ret;
  }

private:

  JASPER::engine::JasperPrintManager *o_;
  JASPER::engine::JasperPrint *p_;
  AQReportsPrivate *d;

  void loadJasperPrint_(const char *src) {
    JASPER::engine::util::JRLoader *loader = 0;
    java::lang::Object *lobj = 0;

    loader = new JASPER::engine::util::JRLoader(wrapperIntern);
    lobj = loader->loadObject(src);
    p_ = new JASPER::engine::JasperPrint(lobj->getJavaObject());

    delete lobj;
    delete loader;
  }

  int pageOrientation_() {
    JASPER::engine::JRReport *jr = new JASPER::engine::JRReport(wrapperIntern);
    int ret = (p_->getOrientation() == jr->ORIENTATION_PORTRAIT ? Portrait : Landscape);
    delete jr;
    return ret;
  }

  void printPageToImage_(const char *dst, int pageIndex, const char *format) {
    java::awt::image::ImageObserver *imgObs = 0;
    java::awt::image::BufferedImage *bufImg = 0;
    java::awt::Image *img = 0;
    java::awt::image::BufferedImage *bi = 0;
    java::awt::Graphics *grp = 0;
    java::io::File *imgFile = 0;
    javax::imageio::ImageIO *imgIO = 0;
    java::awt::image::RenderedImage *rndImg = 0;

    imgObs = new java::awt::image::ImageObserver(wrapperIntern);
    bufImg = new java::awt::image::BufferedImage(wrapperIntern);
    img = o_->printPageToImage(p_, pageIndex, 1.0);
    bi = new java::awt::image::BufferedImage(img->getWidth(imgObs), img->getHeight(imgObs),
                                             bufImg->TYPE_INT_RGB);
    grp = bi->getGraphics();
    if (grp->drawImage(img, 0, 0, imgObs)) {
      imgFile = new java::io::File(dst);
      imgIO = new javax::imageio::ImageIO(wrapperIntern);
      rndImg = new java::awt::image::RenderedImage(bi->getJavaObject());
      imgIO->write(rndImg, format, imgFile);
    }

    delete rndImg;
    delete imgIO;
    delete imgFile;
    delete grp;
    delete bi;
    delete img;
    delete bufImg;
    delete imgObs;
  }

  bool printReport_(const char *printServiceName, int copies) {
    // ***
    // PrintRequestAttributeSet printRequestAttributeSet =
    // new HashPrintRequestAttributeSet();
    AQ_JXPATT::HashPrintRequestAttributeSet *ha =
      new AQ_JXPATT::HashPrintRequestAttributeSet();
    AQ_JXPATT::PrintRequestAttributeSet *printRequestAttributeSet =
      new AQ_JXPATT::PrintRequestAttributeSet(ha->getJavaObject());
    delete ha;
    // ***

    float w = (float)pageWidth() / 72.0;
    float h = (float)pageHeight() / 72.0;

    // ***
    // printRequestAttributeSet.add(MediaSize.findMedia(w, h, MediaSize.INCH));
    {
      AQ_JXPATT::standard::MediaSize *sms =
        new AQ_JXPATT::standard::MediaSize(wrapperIntern);
      jclass cls = javaEnv->FindClass(
                     "javax/print/attribute/standard/MediaSize"
                   );
      AQ_HANDLE_JAVA_EXCEPTION
      jmethodID mid = javaEnv->GetStaticMethodID(
                        cls, "findMedia",
                        "(FFI)Ljavax/print/attribute/standard/MediaSizeName;"
                      );
      AQ_HANDLE_JAVA_EXCEPTION
      jobject jresult = javaEnv->CallStaticObjectMethod(
                          cls, mid, (jfloat)w, (jfloat)h, (jint)sms->INCH
                        );
      AQ_HANDLE_JAVA_EXCEPTION
      delete sms;
      if (jresult != NULL) {
        AQ_JXPATT::Attribute *at = new AQ_JXPATT::Attribute(jresult);
        javaEnv->DeleteLocalRef(jresult);
        printRequestAttributeSet->add(at);
        delete at;
      }
    }
    // ***

    // ***
    // printRequestAttributeSet.add(
    //      new MediaPrintableArea(0, 0, w, h, MediaPrintableArea.INCH));
    {
      AQ_JXPATT::standard::MediaPrintableArea *smpa =
        new AQ_JXPATT::standard::MediaPrintableArea(wrapperIntern);
      AQ_JXPATT::standard::MediaPrintableArea *mpa =
        new AQ_JXPATT::standard::MediaPrintableArea(0.0, 0.0, w, h, smpa->INCH);
      AQ_JXPATT::Attribute *at = new AQ_JXPATT::Attribute(mpa->getJavaObject());
      printRequestAttributeSet->add(at);
      delete smpa;
      delete mpa;
      delete at;
    }
    // ***

    // ***
    // OrientationRequested orientation = OrientationRequested.PORTRAIT;
    // if(jasperPrint.getOrientation() == JRReport.ORIENTATION_LANDSCAPE)
    //   orientation = OrientationRequested.LANDSCAPE;
    // printRequestAttributeSet.add(orientation);
    {
      jclass cls = javaEnv->FindClass(
                     "javax/print/attribute/standard/OrientationRequested"
                   );
      AQ_HANDLE_JAVA_EXCEPTION
      jfieldID fid = (
                       pageOrientation_() == Portrait ?
                       javaEnv->GetStaticFieldID(cls, "PORTRAIT",
                                                 "Ljavax/print/attribute/standard/OrientationRequested;")
                       :
                       javaEnv->GetStaticFieldID(cls, "LANDSCAPE",
                                                 "Ljavax/print/attribute/standard/OrientationRequested;")
                     );
      AQ_HANDLE_JAVA_EXCEPTION
      jobject jresult = javaEnv->GetStaticObjectField(cls, fid);
      AQ_HANDLE_JAVA_EXCEPTION
      if (jresult != NULL) {
        AQ_JXPATT::Attribute *at = new AQ_JXPATT::Attribute(jresult);
        javaEnv->DeleteLocalRef(jresult);
        printRequestAttributeSet->add(at);
        delete at;
      }
    }
    // ***

    // ***
    // printRequestAttributeSet.add(new Copies(copies));
    {
      AQ_JXPATT::standard::Copies *cps =
        new AQ_JXPATT::standard::Copies(copies);
      AQ_JXPATT::Attribute *at = new AQ_JXPATT::Attribute(cps->getJavaObject());
      printRequestAttributeSet->add(at);
      delete cps;
      delete at;
    }
    // ***

    // ***
    // JRPrintServiceExporter exporter = new JRPrintServiceExporter();
    JASPER::engine::export_::JRPrintServiceExporter *jpse =
      new JASPER::engine::export_::JRPrintServiceExporter();
    JASPER::engine::JRExporter *exporter =
      new JASPER::engine::JRExporter(jpse->getJavaObject());
    delete jpse;
    // ***

    int numPars = 3;
    java::lang::Object *pars[numPars];
    for (int i = 0; i < numPars; ++i)
      pars[i] = 0;

    // ***
    // exporter.setParameter(
    //    JRExporterParameter.JASPER_PRINT, jasperPrint);
    {
      jclass cls = javaEnv->FindClass(
                     "net/sf/jasperreports/engine/JRExporterParameter"
                   );
      AQ_HANDLE_JAVA_EXCEPTION
      jfieldID fid = javaEnv->GetStaticFieldID(
                       cls, "JASPER_PRINT",
                       "Lnet/sf/jasperreports/engine/JRExporterParameter;"
                     );
      AQ_HANDLE_JAVA_EXCEPTION
      jobject jresult = javaEnv->GetStaticObjectField(cls, fid);
      AQ_HANDLE_JAVA_EXCEPTION
      if (jresult != NULL) {
        pars[--numPars] = new java::lang::Object(jresult);
        javaEnv->DeleteLocalRef(jresult);
        aqExporterAddSetParameter(exporter->getJavaObject(),
                                  pars[numPars]->getJavaObject(),
                                  p_->getJavaObject());
      }
    }
    // ***

    // ***
    // PrintService printer = PrintServiceLookup.lookupDefaultPrintService();
    AQJavaPrintService *aqPs = new AQJavaPrintService(d);
    AQ_JPX::PrintService *printer = reinterpret_cast<AQ_JPX::PrintService *>(
                                      aqPs->printService(printServiceName)
                                    );
    delete aqPs;
    // ***

    // ***
    // exporter.setParameter(
    //    JRPrintServiceExporterParameter.PRINT_SERVICE, service);
    {
      jclass cls = javaEnv->FindClass(
                     "net/sf/jasperreports/engine/export/JRPrintServiceExporterParameter"
                   );
      AQ_HANDLE_JAVA_EXCEPTION
      jfieldID fid = javaEnv->GetStaticFieldID(
                       cls, "PRINT_SERVICE",
                       "Lnet/sf/jasperreports/engine/export/JRPrintServiceExporterParameter;"
                     );
      AQ_HANDLE_JAVA_EXCEPTION
      jobject jresult = javaEnv->GetStaticObjectField(cls, fid);
      AQ_HANDLE_JAVA_EXCEPTION
      if (jresult != NULL) {
        pars[--numPars] = new java::lang::Object(jresult);
        javaEnv->DeleteLocalRef(jresult);
        aqExporterAddSetParameter(exporter->getJavaObject(),
                                  pars[numPars]->getJavaObject(),
                                  printer->getJavaObject());
      }
    }
    // ***

    // ***
    // exporter.setParameter(
    //    JRPrintServiceExporterParameter.PRINT_REQUEST_ATTRIBUTE_SET,
    //    printRequestAttributeSet);
    {
      jclass cls = javaEnv->FindClass(
                     "net/sf/jasperreports/engine/export/JRPrintServiceExporterParameter"
                   );
      AQ_HANDLE_JAVA_EXCEPTION
      jfieldID fid = javaEnv->GetStaticFieldID(
                       cls, "PRINT_REQUEST_ATTRIBUTE_SET",
                       "Lnet/sf/jasperreports/engine/export/JRPrintServiceExporterParameter;"
                     );
      AQ_HANDLE_JAVA_EXCEPTION
      jobject jresult = javaEnv->GetStaticObjectField(cls, fid);
      AQ_HANDLE_JAVA_EXCEPTION
      if (jresult != NULL) {
        pars[--numPars] = new java::lang::Object(jresult);
        javaEnv->DeleteLocalRef(jresult);
        aqExporterAddSetParameter(exporter->getJavaObject(),
                                  pars[numPars]->getJavaObject(),
                                  printRequestAttributeSet->getJavaObject());
      }
    }
    // ***

    // Print !!
    exporter->exportReport();

    delete exporter;
    delete[] pars;
    delete printRequestAttributeSet;
    delete printer;
    return true;
  }

  bool exportReportToFile_(const char *dst, const char *format) {
    java::io::File *outFile = 0;
    outFile = new java::io::File(dst);

    if (strcmp(format, "CSV") == 0) {
      JASPER::view::save::JRCsvSaveContributor *saveC = 0;
      saveC = new JASPER::view::save::JRCsvSaveContributor(0, 0);
      saveC->save(p_, outFile);
      delete saveC;
    } else if (strcmp(format, "DOCX") == 0) {
      JASPER::view::save::JRDocxSaveContributor *saveC = 0;
      saveC = new JASPER::view::save::JRDocxSaveContributor(0, 0);
      saveC->save(p_, outFile);
      delete saveC;
    } else if (strcmp(format, "EMBEDDEDIMAGESXML") == 0) {
      JASPER::view::save::JREmbeddedImagesXmlSaveContributor *saveC = 0;
      saveC = new JASPER::view::save::JREmbeddedImagesXmlSaveContributor(0, 0);
      saveC->save(p_, outFile);
      delete saveC;
    } else if (strcmp(format, "HTML") == 0) {
      JASPER::view::save::JRHtmlSaveContributor *saveC = 0;
      saveC = new JASPER::view::save::JRHtmlSaveContributor(0, 0);
      saveC->save(p_, outFile);
      delete saveC;
    } else if (strcmp(format, "MULTIPLESHEETSXLS") == 0) {
      JASPER::view::save::JRMultipleSheetsXlsSaveContributor *saveC = 0;
      saveC = new JASPER::view::save::JRMultipleSheetsXlsSaveContributor(0, 0);
      saveC->save(p_, outFile);
      delete saveC;
    } else if (strcmp(format, "ODT") == 0) {
      JASPER::view::save::JROdtSaveContributor *saveC = 0;
      saveC = new JASPER::view::save::JROdtSaveContributor(0, 0);
      saveC->save(p_, outFile);
      delete saveC;
    } else if (strcmp(format, "PDF") == 0) {
      JASPER::view::save::JRPdfSaveContributor *saveC = 0;
      saveC = new JASPER::view::save::JRPdfSaveContributor(0, 0);
      saveC->save(p_, outFile);
      delete saveC;
    } else if (strcmp(format, "RTF") == 0) {
      JASPER::view::save::JRRtfSaveContributor *saveC = 0;
      saveC = new JASPER::view::save::JRRtfSaveContributor(0, 0);
      saveC->save(p_, outFile);
      delete saveC;
    } else if (strcmp(format, "SINGLESHEETXLS") == 0) {
      JASPER::view::save::JRSingleSheetXlsSaveContributor *saveC = 0;
      saveC = new JASPER::view::save::JRSingleSheetXlsSaveContributor(0, 0);
      saveC->save(p_, outFile);
      delete saveC;
    } else if (strcmp(format, "XML") == 0) {
      JASPER::view::save::JRXmlSaveContributor *saveC = 0;
      saveC = new JASPER::view::save::JRXmlSaveContributor(0, 0);
      saveC->save(p_, outFile);
      delete saveC;
    }

    bool ret = outFile->exists();
    delete outFile;
    return ret;
  }
};

#endif /* AQJASPERPRINTMANAGER_P_H_ */
