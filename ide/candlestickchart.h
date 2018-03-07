#ifndef CANDLESTICKCHART_H
#define CANDLESTICKCHART_H

#include "candlesticksetting.h"
#include "twosidedgraphicswidget.h"
#include <QObject>
#include <QStringList>

namespace QtCharts {
class QCandlestickSeries;
class QChart;
class QChartView;
}


class CandleStickChart:public QObject
{
    Q_OBJECT
public:
    CandleStickChart(QGraphicsScene *scene);
private:
    QtCharts::QCandlestickSeries *m_series;
    QtCharts::QChartView *m_view1;
    QtCharts::QChart *m_chart;
    TwoSidedGraphicsWidget *m_widget;
    QStringList m_categories;

    CandleStickSetting* m_setting;
};

#endif // CANDLESTICKCHART_H
