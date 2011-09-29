/***************************************************************************
 FLJasperEngine.h
 -------------------
 begin                : 02/01/2011
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

#ifndef FLJASPERENGINE_H_
#define FLJASPERENGINE_H_

#include <qobject.h>
#include <qdom.h>

class FLDomNodeInterface;
class FLJasperEnginePrivate;
class FLReportPages;

/**
Motor de generación informes para JasperReports

Necesita el plugin AQReports para poder utilizar como backend
el motor JasperReports

@author InfoSiAL S.L.
*/
class FLJasperEngine : public QObject
{
  Q_OBJECT
  Q_ENUMS(AQReportsMsgType)

public:

  enum AQReportsMsgType { AQErrorMsg, AQLogMsg, AQSysMsg };

  FLJasperEngine(QObject *parent = 0);
  ~FLJasperEngine();

public slots:

  /**
  Establece los datos del informe a partir de un documento XML

  El documento contiene el conjunto de registros en formato XML
  que pueden ser seleccionados mediante una expresion XPath, para
  mas detalles ver setDataSourceParameters().

  Nota: Se puede obtener un conjunto de registros en el formato que procesa
        Kugar mediante FLReportEngine, estableciendo una consulta y obteniendo
        el resultado, para mas detalles ver la documentacion de FLReportEngine.

  @param d Objeto QDomNode con los datos
  @return TRUE si ha ido bien
  */
  bool setReportData(QDomNode n);
  bool setReportData(FLDomNodeInterface *n);

  /**
  Establece la plantilla para el informe.

  El nombre de la plantilla corresponde con el nombre del fichero con
  extesión ".jrxml".

  @param t Nombre dela plantilla
  @return TRUE si ha ido bien
  */
  bool setReportTemplate(const QString &t);

  /**
  @return Los datos de origen del informe en un documento XML.
          Se debe llamar despues de establecer los datos para el informe con
          set*ReportData, de lo contrario el resultado será indefinido.
  */
  FLDomNodeInterface *reportData() const;

  /**
  @return Plantilla del informe en un documento XML con el formato usado por Jasper.
          Se debe llamar despues de establecer los datos para el informe con
          set*ReportTemplate, de lo contrario el resultado será indefinido.
  */
  FLDomNodeInterface *reportTemplate() const;

  /**
  Establece los parámetros a utilizar para acceder a los datos XML

  Los parámetros que acepta determinan como se seleccionan los registros desde
  el documento XML mediante una expresión XPath, y como se formatean los campos
  de fecha y numéricos. Para una descripción mas detallada se puede consultar la
  documentación del API de JasperReports, concretamente esto dos enlaces:

  \b http://jasperreports.sourceforge.net/api/net/sf/jasperreports/engine/JasperFillManager.html

  \b http://jasperreports.sourceforge.net/api/net/sf/jasperreports/engine/data/JRXmlDataSource.html

  @param  selectExpression  Expresión XPath para seleccionar los registros de los datos XML
  @param  datePattern       Patrón para formatear los campos tipo fecha
  @param  numberPattern     Patrón para formatear los campos numéricos
  */
  void setDataSourceParameters(const QString &selectExpression,
                               const QString &datePattern,
                               const QString &numberPattern);

  /**
  Compila el informe generando el fichero ".jasper" correspondiente

  Importante: El fichero creado es automáticamente borrado cuando se destruye el
              objeto del tipo FLJasperEngine que lo creo y cada vez que se
              llama a compileReport() para crear uno nuevo.

  @return El nombre y ruta del fichero creado, o vacio si no se pudo crear
  */
  QString compileReport();

  /**
  Rellena el informe con los datos, generando el fichero ".jrprint" correspondiente

  Si existe, se utiliza el ultimo fichero ".jasper" creado con compileReport(), si no existe,
  intenta crear uno llamando a compileReport().

  Utiliza los parámetros establecidos con setDataSourceParameters() para seleccionar y
  formatear los datos XML. Si no se han establecido parámetros utiliza por defecto los
  necesarios para trabajar con datos XML en el formato que procesa Kugar.

  Importante: El fichero creado es automáticamente borrado cuando se destruye el
              objeto del tipo FLJasperEngine que lo creo y cada vez que se
              llama a fillReport() para crear uno nuevo.

  @return El nombre y ruta del fichero creado, o vacio si no se pudo crear
  */
  QString fillReport();

  /**
  Imprime el informe a imagenes que devuelve como una colección de paginas

  Si existe, se utiliza el ultimo fichero ".jrprint" creado con fillReport(), si no existe,
  intenta crear uno llamando a fillReport().

  Nota: La coleccion de paginas se puede visualizarla en un FLReportViewer
        mediante FLReportViewer::setReportPages()

  @return La colección de paginas generada. La coleccion será vacia (pageCount() == -1)
          si no se pudo crear. FLJasperEngine no toma propiedad de la coleccion devuelta,
          debe ser borrada explicitamente con deleteLater para eliminarla de memoria.
  */
  FLReportPages *printReportToPages();

  /**
  Actualizar el contenido de la ultima coleccion de paginas generada con printReportToPages.

  Siempre compila y rellena el informe llamando a compileReport() y fillReport()

  Si no se ha invocado aun a printReportToPages esta funcion no hace nada.
  */
  void updateReportPages();

  /**
  Imprime el informe a la impresora predeterminada o permitiendo elegir una mediante
  un cuadro de dialogo.

  Si existe, se utiliza el ultimo fichero ".jrprint" creado con fillReport(), si no existe,
  intenta crear uno llamando a fillReport().

  @param  withPrintDialog Si TRUE (por defecto) se muestra el cuadro de dialogo de
                          impresion nativo de Jasper
  @return                 TRUE si no hubo error, FALSE en caso contrario
  */
  bool printReport(bool withPrintDialog = true);

  /**
  Imprime el informe directamente al servicio de impresión especificado.

  Si existe, se utiliza el ultimo fichero ".jrprint" creado con fillReport(), si no existe,
  intenta crear uno llamando a fillReport().

  @param  printServiceName  El nombre de un servicio de impresión en el que se desea imprimir,
                            generalmente una impresora. El nombre deberá ser reconocido por
                            Jasper, es decir, debe ser alguno de los nombres que devuelve el
                            método printServiceNames()
  @param  copies            Número de copias a imprimir, por defecto 1
  @return                   TRUE si no hubo error, FALSE en caso contrario
  */
  bool printReport(const QString &printServiceName, int copies = 1);

  /**
  @return La lista de nombres de los servicios de impresion (impresoras) reconocidos y
          utilizables por Jasper
  */
  QStringList printServiceNames();

  /**
  Métodos para exportar a varios formatos

  Si existe, se utiliza el ultimo fichero ".jrprint" creado con fillReport(), si no existe,
  intenta crear uno llamando a fillReport().

  @param outFileName  Nombre del fichero destino donde exportar
  @param format       Indica el formato de exportacion, los posibles valores son:
                      - Csv
                      - Docx
                      - EmbeddedImagesXml
                      - Html
                      - MultipleSheetsXls
                      - Odt
                      - Pdf
                      - Rtf
                      - SingleSheetXls
                      - Xml
  */
  bool exportReportToFile(const QString &outFileName, const QString &format);

  /**
  Abre un dialogo del tipo "Guardar como" para exportar el informe indicando
  el nombre del fichero y el formato de exportacion

  @param defaultFileName  Nombre de fichero a utilizar por defecto
  @param defaultFormat    Nombre del formato a utilizar por defecto. Los posibles valores son
                          los utilizados por exportReportToFile.
  */
  void execExportFileDialog(const QString &defaultFileName = QString::null,
                            const QString &defaultFormat = QString::null);

  /**
  Proporcionada por ergonomia.

  Exporta el informe a PDF con la posibilidad de mostrar antes el dialogo llamando a
  execExportFileDialog.
  */
  void exportReportToPdf(const QString &outFileName = QString::null,
                         bool withFileDialog = true);

  /**
  Proporcionada por ergonomia.

  Exporta el informe a CSV con la posibilidad de mostrar antes el dialogo llamando a
  execExportFileDialog.
  */
  void exportReportToCsv(const QString &outFileName = QString::null,
                         bool withFileDialog = true);

  /**
  @return Nombre del último fichero ".jasper" creado en la ultima llamada a compileReport().
          Vacio si todavia no se ha creado ninguno.
  */
  QString lastJasperFile() const;

  /**
  @return Nombre del último fichero ".jrprint" creado en la ultima llamada a fillReport().
          Vacio si todavia no se ha creado ninguno.
  */
  QString lastJRPrintFile() const;

  /**
  @return La última colección de paginas generada con printReportToPages.
          Será cero si todavia no se ha creado ninguna.
  */
  FLReportPages *lastReportPages() const;

  /**
  Para establecer si se debe mostrar el dialogo con el mensaje de error cuando se produce uno
  */
  void setShowErrorDialog(bool b = true);

signals:

  /**
  Propaga los mensajes recibidos por el plugin de AQReports,
  indicando el tipo (AQReportsMsgType) y el texto del mensaje
  */
  void msgReceived(int, const QString &);

private slots:

  /**
  Uso interno
  */
  void setDefaultExportFormat(const QString &format);

private:

  FLJasperEnginePrivate *d;

  /**
  Uso interno
  */
  void msgHandler(AQReportsMsgType type, const QString &msg);
  friend void JasperEngineMsgHandler(AQReportsMsgType, const char *, void *);
};

#endif /* FLJASPERENGINE_H_ */
