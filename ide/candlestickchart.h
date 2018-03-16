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
class QLineSeries;
}


class CandleStickChart:public QObject
{
    Q_OBJECT
public:
    CandleStickChart(QGraphicsScene *scene);
    virtual ~CandleStickChart();
public slots:
    void setColor(const QColor&);
    void updateSerials(const Dummy_ChinaL1Msg* msg, int size);
private:
    QtCharts::QLineSeries *m_futures[2];
    QtCharts::QLineSeries *m_volumns[2];
    QtCharts::QCandlestickSeries *m_series;
    QtCharts::QChartView *m_view1;
    QtCharts::QChart *m_chart;
    TwoSidedGraphicsWidget *m_widget;
    QStringList m_categories;
    QGraphicsScene *m_scene;

    CandleStickSetting* m_setting;
};

#endif // CANDLESTICKCHART_H
