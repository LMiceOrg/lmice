#include "candlestickchart.h"
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QChartView>



QT_CHARTS_USE_NAMESPACE

CandleStickChart::CandleStickChart(QGraphicsScene *scene)
{
    m_widget = new TwoSidedGraphicsWidget(scene);

    //! Initialize candle series
    m_series = new QCandlestickSeries();
    m_series->setName("CandleSeries");
    m_series->setIncreasingColor(QColor(Qt::green));
    m_series->setDecreasingColor(QColor(Qt::red));

    //! Initialize chart
    m_chart = new QChart();
    m_chart->addSeries(m_series);
    m_chart->setTitle("Real time candle data");
    m_chart->setAnimationOptions(QChart::SeriesAnimations);

    //! axes
    m_chart->createDefaultAxes();
    QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis *>(m_chart->axes(Qt::Horizontal).at(0));
    axisX->setCategories(m_categories);

    QValueAxis *axisY = qobject_cast<QValueAxis *>(m_chart->axes(Qt::Vertical).at(0));
    axisY->setMax(axisY->max() * 1.01);
    axisY->setMin(axisY->min() * 0.99);

    //! legend
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);

    //! chart view
    m_view1 = new QChartView(m_chart);
    m_view1->setRenderHint(QPainter::Antialiasing);

    m_widget->setWidget(0, m_view1);
    m_setting= new CandleStickSetting();
    m_widget->setWidget(1, m_setting);

    m_view1->resize(800,600);
}
