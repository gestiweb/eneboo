/***************************************************************************
                       FLReportViewer.h  -  description
                          -------------------
 begin                : vie jun 28 2002
 copyright            : (C) 2002-2004 by InfoSiAL S.L.
 email                : mail@infosial.com
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FLREPORTVIEWER_H
#define FLREPORTVIEWER_H

#include <qdom.h>

#include "FLWidgetReportViewer.h"
#include "mreportviewer.h"

#include "AQGlobal.h"

class MReportViewer;
class MPageCollection;
class FLSqlQuery;
class FLSqlCursor;
class FLReportEngine;
class FLSmtpClient;
class FLPicture;
class FLReportPages;

/**
Visor para previsualizar informes o colecciones de paginas

El visor tiene tiene dos modos de funcionamiento;

- Visor con capacidades de renderizado utilizando un motor de informes Kugar.
  Cuando se establecen los datos y plantilla, o se le asigna un motor de
  renderizado. En este caso esta clase funciona como una simplificacion de
  alto nivel para FLReportEngine.
- Sólo visualizador de páginas. Cuando se le indica sólo el conjunto de
  páginas a visualizar.

@author InfoSiAL S.L.
*/
class AQ_EXPORT FLReportViewer: public FLWidgetReportViewer
{

  Q_OBJECT

public:


  /**
  constructor

  @param  embedInParent Si es TRUE y se ha indicado un widget padre, el visor intenta
                        incrustarse en la capa principal del padre. Si es FALSE el visor
                        será una ventana separada emergente.
  @param  rptEngine Opcionalmente aqui se puede establecer el motor de informes a usar por el visor,
                    de lo contrario el visor creará su propio motor internamente.
  */
  FLReportViewer(QWidget *parent = 0, const char *name = 0,
                 bool embedInParent = false, FLReportEngine *rptEngine = 0);

  /**
  destructor
  */
  ~FLReportViewer();

  /**
  Muestra el formulario y entra en un nuevo bucle de eventos.
  */
  void exec();

  /**
  Renderiza el informe en el visor.

  Sólo tiene efecto si se han indicado datos y plantilla para el visor con
  setReportData y setReportTemplate, o se le ha asignado un motor de informes
  explícitamente con setReportEngine o en el constructor.

  @return TRUE si todo ha ido bien
  */
  bool renderReport(const int initRow = 0, const int initCol = 0,
                    const bool append = false, const bool displayReport = false);

  bool renderReport(const int initRow = 0, const int initCol = 0, const uint flags = MReportViewer::Display);

  /**
  Establece los datos del informe a partir de una consulta

  @param q Objeto FLSqlQuery con la consulta de la que se toman los datos
  @return TRUE si todo ha ido bien
  */
  bool setReportData(FLSqlQuery *q);

  /**
  Establece los datos del informe a partir de una tabla

  @param t Objeto FLSqlCursor con  la tabla de la que se toman los datos
  @return TRUE si todo ha ido bien
  */
  bool setReportData(FLSqlCursor *t);

  /**
  Establece los datos del informe a partir de un documento xml

  @param n Objeto FLDomDocument con  la tabla de la que se toman los datos
  @return TRUE si todo ha ido bien
  */
  bool setReportData(QDomNode n);

  /**
  Establece la plantilla para el informe.

  El nombre de la plantilla corresponde con el nombre del fichero con extesión ".kut".

  @param  t     Nombre de la plantilla
  @param  style Nombre de la hoja de estilo a aplicar
  @return TRUE si todo ha ido bien
  */
  bool setReportTemplate(const QString &t, const QString &style = QString::null);

  /**
  Establece la plantilla para el informe.

  La plantilla viene dada por un nodo XML KugarTemplate

  @param  d     Nodo XML que contiene la plantilla
  @param  style Nombre de la hoja de estilo a aplicar
  @return TRUE si todo ha ido bien
  */
  bool setReportTemplate(QDomNode d, const QString &style = QString::null);

  /**
  Reimplementación de QWidget::sizeHint()
  */
  QSize sizeHint() const;

  /**
  Establece el número de copias por defecto a imprimir
  */
  void setNumCopies(const int numCopies);

  /**
  Establece si el informe se debe imprimir en una impresora ESC/POS
  */
  void setPrintToPos(bool ptp);

  /**
  Establece el nombre de la impresora a la que imprimir.

  Si se establece el nombre de la impresora no se mostrará el cuadro de dialogo de impresión, y se
  usará esa impresora para imprimir directamente. Para que el cuadro de diálogo de impresión se muestre bastará
  con establecer un nombre vacío; setPrinterName( QString::null ).
  */
  void setPrinterName(const QString &pName);

  /**
  Devuelve si el último informe ha sido imprimido en impresora o archivo.
  */
  bool reportPrinted();

  /**
  Establece el nombre del estilo
  */
  void setStyleName(const QString &style);

  /**
  Visor básico de Kugar
  */
  MReportViewer *rptViewer() const {
    return rptViewer_;
  }

  /**
  Motor de informes de Kugar que actualmente está usando el informe
  */
  FLReportEngine *rptEngine() const {
    return rptEngine_;
  }

  /**
  Incrusta el visor básico de Kugar en la capa principal de un objeto widget padre.

  @param parentFrame: Nuevo padre. Debe ser QFrame con al menos una capa VBox
  */
  void rptViewerEmbedInParent(QWidget *parentFrame);

  /**
  Cambia el widget padre del visor incrustandolo en la capa principal

  @param parentFrame: Nuevo padre. Debe ser QFrame con al menos una capa VBox
  */
  void rptViewerReparent(QWidget *parentFrame);

  /**
  Obtiene una versión csv de los datos del informe (una vez ejecutado)

  Solo tiene efecto si el visor tiene un motor de informes activo
  */
  QString csvData();

  /**
  Establece directamente la coleccion de paginas a visualizar.
  No seran visibles hasta que no se ejecute updateDisplay.

  Al estableder la colección de paginas la clase pasa a ser un mero
  visualizador de esas páginas, es decir, no tiene un motor de informes
  asignado y los métodos específicos que llaman al motor de informes no
  tendran efecto (renderReport, csvData, etc..)
  */
  void setReportPages(FLReportPages *pgs);

  /**
  Establece el modo de color de la impresión (PrintColor, PrintGrayScale)
  */
  void setColorMode(uint c);

  /**
  Obtiene el modo de color de la impresión establecido
  */
  uint colorMode() const;

public slots:

  /**
  Establece el motor de informes a usar por el visor

  @param r Motor de informes si es cero (por defecto), se quita el motor actual del visor,
           destruyendolo si ese motor es hijo del visor (fue construido por el)
  */
  void setReportEngine(FLReportEngine *r = 0);

  /**
  Imprime directamente el informe sin mostrarlo
  */
  void slotPrintReport();

  /**
  Imprime el informe en un fichero PS
  */
  void slotPrintReportToPS(const QString &outPsFile);

  /**
  Imprime el informe en un fichero PDF
  */
  void slotPrintReportToPDF(const QString &outPdfFile);

  /**
  Muestra la primera página del informe
  */
  void slotFirstPage();

  /**
  Muestra la útlima página del informe
  */
  void slotLastPage();

  /**
  Muestra la siguiente página del informe
  */
  void slotNextPage();

  /**
  Muestra la anterior página del informe
  */
  void slotPrevPage();

  /**
  Cierra el visor
  */
  void slotExit();

  /**
  Aumenta zoom de la página actual
  */
  void slotZoomUp();

  /**
  Disminuye zoom de la página actual
  */
  void slotZoomDown();

  /**
  Exporta a un fichero de disco la version CSV del informe
  */
  void exportFileCSVData();

  /**
  Exporta el informe a un fichero en formato PDF
  */
  void exportToPDF();

  /**
  Exporta el informe a un fichero en formato PDF y lo envia por correo eléctronico
  */
  void sendEMailPDF();

  /**
  Muestra u oculta la ventana principal inicial

  @param show TRUE la muestra, FALSE la oculta
  */
  void showInitCentralWidget(bool show);

  /**
  Guarda como plantilla de estilo SVG
  */
  void saveSVGStyle();

  /**
  Guarda la página actual como plantilla de estilo SVG simplificada ( sólo los campos de datos )
  */
  void saveSimpleSVGStyle();

  /**
  Carga y aplica una plantilla de estilo SVG
  */
  void loadSVGStyle();

  /**
  Establece si el visor debe cerrarse automáticamente tras imprimir el informe

  @param b TRUE para cierre automático, FALSE para desactivar cierre automático
  */
  void setAutoClose(bool b);

  /**
  Establece la resolucion de la impresora

  @param dpi Resolucion en puntos por pulgada
  */
  void setResolution(int dpi);
  void setPixel(int relDpi);
  void setDefaults();

  /**
  Actualizar el informe.

  Emite la señal requestUpdateReport y llama a updateDisplay.
  Si el visor tiene un motor de informes ejecuta de nuevo la consulta y el renderizado.
  */
  void updateReport();

  /**
  Actualiza el visor, redibujando la coleccion de paginas en pantalla
  */
  void  updateDisplay();

  /**
  Para desactivar las acciones por defecto para imprimir y exportar

  Esto es util cuando se quieren capturar la señales que disparan
  los botones del formulario, desactivar lo que hacen por defecto,
  y susituirlo por funcionalidad especifica. Por ejemplo para mostrar un
  dialogo de impresion personalizado.
  */
  void disableSlotsPrintExports(bool disablePrints = true, bool disableExports = true);

  /**
  Exporta el informe a una hoja de cálculo ODS y la visualiza

  Solo tiene efecto si el visor tiene un motor de informes activo
  */
  void exportToOds();

signals:

  /**
  Señal emitida cuando se va a actualizar el informe
  */
  void requestUpdateReport();

protected:

  /**
  Captura evento cerrar
  */
  void closeEvent(QCloseEvent *e);

  /**
  Captura evento mostrar
  */
  void showEvent(QShowEvent *e);

private:
 
  /**
  Almacena si se ha abierto el formulario con el método FLReportViewer::exec()
  */
  bool loop;

  /**
  Indica si el último informe fue impreso, es decir, enviado a impresora o archivo
  */
  bool reportPrinted_;

  /**
  Visor básico de Kugar
  */
  MReportViewer *rptViewer_;

  /**
  Motor de informes de AbanQ
  */
  FLReportEngine *rptEngine_;

  /**
  Ventana principal inicial
  */
  QWidget *initCentralWidget_;

  /**
  Cliente SMTP para enviar el informe por correo electrónico
  */
  FLSmtpClient *smtpClient_;

  /**
  Indica si el visor debe cerrarse automáticamente despues de imprimir
  */
  bool autoClose_;

  /**
  Colección de paginas del informe ( lista de QPictures )
  */
  MPageCollection *report;

  /**
  Indica si el visor es un objeto incrustado en el padre o una venta emergente
  */
  bool embedInParent_;

  /**
  Nombre de la plantilla del informe
  */
  QString template_;

  /**
  Nodo plantilla de informe
  */
  QDomNode xmlTemplate_;

  /**
  Nodo datos de informe
  */
  QDomNode xmlData_;

  /**
  Guarda la consulta origen
  */
  FLSqlQuery *qry_;

  /**
  Nombre del estilo del informe
  */
  QString styleName_;

  /**
  Para desactivar las acciones por defecto para imprimir y exportar

  Esto es util cuando se quieren capturar la señales que disparan
  los botones del formulario, desactivar lo que hacen por defecto,
  y susituirlo por funcionalidad especifica. Por ejemplo para mostrar un
  dialogo de impresion personalizado.
  */
  bool slotsPrintDisabled_;
  bool slotsExportDisabled_;
  
  /** Uso interno */
  bool printing_;

public:

  /**
  Metodos proporcionados por ergonomia. Son un enlace a los
  mismos métodos que proporciona FLReportPages, para manejar
  la coleccion de paginas del visor
  */
  FLPicture *getCurrentPage();
  FLPicture *getFirstPage();
  FLPicture *getPreviousPage();
  FLPicture *getNextPage();
  FLPicture *getLastPage();
  FLPicture *getPageAt(uint i);
  void  clearPages();
  void  appendPage();
  int   getCurrentIndex();
  void  setCurrentPage(int idx);
  void  setPageSize(int s);
  void  setPageOrientation(int o);
  void  setPageDimensions(QSize dim);
  int   pageSize();
  int   pageOrientation();
  QSize pageDimensions();
  int   pageCount();
};

#endif
