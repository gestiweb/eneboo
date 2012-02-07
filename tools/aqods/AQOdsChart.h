/***************************************************************************
 AQOdsChart.h
 -------------------
 begin                : 13/12/2011
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

#ifndef AQODSCHART_H_
#define AQODSCHART_H_

#include "odf-gen/chart.h"

#include "AQOdsCellAddress.h"
#include "AQOdsColor.h"
#include "AQOdsLength.h"

class AQOdsSeries : public Series
{
public:
  AQOdsSeries(const AQOdsCellAddress &name,
              const AQOdsCellRange &domain,
              const AQOdsCellRange &values,
              const AQOdsColor &color)
    : Series(name, domain, values, color) {}
};

class AQOdsChart : public Chart
{
public:
  AQOdsChart(const QString &name,
             const AQOdsLength &width,
             const AQOdsLength &height)
    : Chart(name, width, height) {}

  void addRange(const AQOdsCellRange &range) {
    add_range(range);
  }

  void addSeries(const AQOdsSeries &series) {
    add_series(series);
  }

  void setTitle(const QString &title) {
    set_title(title);
  }

  void setSubtitle(const QString &subtitle) {
    set_subtitle(subtitle);
  }

  void setXAxisLabel(const QString &x_axis_label) {
    set_x_axis_label(x_axis_label);
  }

  void setYAxisLabel(const QString &y_axis_label) {
    set_y_axis_label(y_axis_label);
  }
};

MAP_ODS_TYPE(AQOdsChart, object);

class AQOdsAutoChart : public AQOdsChart
{
public:
  AQOdsAutoChart(const QString &name,
                 const AQOdsLength &width,
                 const AQOdsLength &height,
                 const AQOdsSheet &sheet)
    : AQOdsChart(name, width, height) {
    ColorGenerator generator;

    add_range(CellRange(CellAddress(sheet.get_name(), 1, 1),
                        CellAddress(sheet.get_name(),
                                    sheet.get_columns(),
                                    sheet.get_rows())));

    for (unsigned int i = 2; i <= sheet.get_columns(); i++) {
      CellAddress name(sheet.get_name(), i, 1),
                  domain_start(sheet.get_name(), 1, 2),
                  domain_end(sheet.get_name(), 1, sheet.get_rows()),
                  values_start(sheet.get_name(), i, 2),
                  values_end(sheet.get_name(), i, sheet.get_rows());
      CellRange domain(domain_start, domain_end),
                values(values_start, values_end);
      Color color(generator.next());

      add_series(Series(name, domain, values, color));
    }
  }
};

MAP_ODS_TYPE(AQOdsAutoChart, object);

#endif /* AQODSCHART_H_ */
