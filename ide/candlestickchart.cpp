#include "candlestickchart.h"
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QCandlestickSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtWidgets>

QT_CHARTS_USE_NAMESPACE

CandleStickChart::CandleStickChart(QGraphicsScene *scene)
{
    m_scene = scene;
    m_widget = new TwoSidedGraphicsWidget(scene);

    //! Initialize candle series
    m_series = new QCandlestickSeries();
    m_series->setName("CandleSeries");
    m_series->setIncreasingColor(QColor(Qt::green));
    m_series->setDecreasingColor(QColor(Qt::red));

    m_futures[0] = new QLineSeries();
    m_futures[0]->setName("Ask");
    m_futures[0]->setColor(QColor(Qt::red));
    m_futures[1] = new QLineSeries();
    m_futures[1]->setName("Bid");
    m_futures[1]->setColor(QColor(Qt::green));

    m_volumns[0] = new QLineSeries();
    m_volumns[0]->setName("Ask");
    m_volumns[0]->setColor(QColor(Qt::red));
    m_volumns[1] = new QLineSeries();
    m_volumns[1]->setName("Bid");
    m_volumns[1]->setColor(QColor(Qt::green));

    //! Initialize chart
    m_chart = new QChart();
    //m_chart->addSeries(m_series);
    m_chart->addSeries(m_futures[0]);
    m_chart->addSeries(m_futures[1]);
    m_chart->setTitle("Real time candle data");
    m_chart->setAnimationOptions(QChart::SeriesAnimations);  //在缩放窗口大小时可以用动画进行美化

    //! axes
    m_chart->createDefaultAxes();
//    QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis *>(m_chart->axes(Qt::Horizontal).at(0));
//    axisX->setCategories(m_categories);

    QValueAxis *axisY = qobject_cast<QValueAxis *>(m_chart->axes(Qt::Vertical).at(0));
    axisY->setMax(axisY->max() * 1.01);
    axisY->setMin(axisY->min() * 0.99);

    //! legend
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);

    m_setting= new CandleStickSetting();


    //! chart view
    m_view1 = new QChartView(m_chart);
    m_view1->setRenderHint(QPainter::Antialiasing);

    //m_view1->viewport()->installEventFilter(m_widget->filter());

    //! line edit
    QLineEdit *m_edit = new QLineEdit();
    QVBoxLayout *box = new QVBoxLayout();
    box->setSpacing(0);
    box->setMargin(0);
    box->setGeometry(QRect(0,0,0,0));
    box->addWidget(m_view1, 1);
    box->addWidget(m_edit);
    m_edit->setTextMargins(0, 0, 0, 0);
    m_edit->setContextMenuPolicy(Qt::NoContextMenu);

    QWidget* window = new QWidget;
    window->setLayout(box);
    window->setGeometry(0,-20,0,-20);
    m_widget->setWidget(0, window);
    m_widget->setWidget(1, m_setting);

//    m_widget->setWidget(0, m_view1);
//    m_widget->setWidget(1, m_setting);

    m_chart->layout()->setContentsMargins(1,1,1,1);
    m_setting->setContentsMargins(1,1,1,1);


    QSize size = qApp->primaryScreen()->availableSize()/4;
    m_view1->resize(size);

    connect(m_setting, SIGNAL(updateChinaLevel1Msg(const Dummy_ChinaL1Msg*, int)),
            this, SLOT(updateSerials(const Dummy_ChinaL1Msg*, int)) );

    QObject::connect(m_setting, &CandleStickSetting::changeColor, [this](const QColor& c)
    {
        setColor(c);
    });

    QObject::connect(m_setting, &CandleStickSetting::removeAllSeries, [this]()
    {
        QList<QAbstractAxis*> axes=m_chart->axes();
        foreach(auto ax, axes)
            m_chart->removeAxis( ax);
        m_chart->removeAllSeries();
    });

    QObject::connect(m_edit, &QLineEdit::returnPressed, [this, m_edit]()
    {
        m_setting->exec( m_edit->text() +"\n" );
        m_chart->update();
    });


}

CandleStickChart::~CandleStickChart()
{
    m_widget->clear();
}

void CandleStickChart::setColor(const QColor & color)
{
    m_view1->setBackgroundBrush(color);
    m_setting->setPalette(color);
}

void CandleStickChart::updateSerials(const Dummy_ChinaL1Msg *msg, int size)
{;
    QCandlestickSet* set;
    QList<QAbstractSeries *> series = m_chart->series();
    qDebug()<<"series "<<series.length();


    qDebug()<<"clear futures";

        m_futures[0] = new QLineSeries();
        m_futures[0]->setName("Ask");
        m_futures[0]->setColor(QColor(Qt::red));



        m_futures[1] = new QLineSeries();
        m_futures[1]->setName("Bid");
        m_futures[1]->setColor(QColor(Qt::green));

//    m_futures[1]->clear();
//    m_volumns[0]->clear();
//    m_volumns[1]->clear();
    //m_series->clear();
    //m_categories.clear();
    double ma=0,mi=0;

    if(size <= 0 || !msg)
        return;

    qDebug()<<"call for";

    m_chart->setTitle(tr("%1 Candle chart").arg(msg[0].m_inst) );

    int64_t begin = msg[0].m_time;
    for(int i=0; i<size; ++i)
    {

//        set = new QCandlestickSet(
//                    msg[i].m_offer, msg[i].m_offer_quantity,
//                    msg[i].m_bid, msg[i].m_bid_quantity,
//                    msg[i].m_time);
//        m_series->append(set);
        m_futures[0]->append( (msg[i].m_time - begin) / 500000LL, msg[i].m_bid);
        m_futures[1]->append( (msg[i].m_time - begin) / 500000LL, msg[i].m_offer);
        if(ma < msg[i].m_bid)
            ma = msg[i].m_bid;
        if(ma < msg[i].m_offer)
            ma= msg[i].m_offer;
        if(mi > msg[i].m_bid)
            mi = msg[i].m_bid;
        if(mi > msg[i].m_offer)
            mi = msg[i].m_offer;
        //m_categories<<tr("%1").arg(msg[i].m_time);
//        double mid = (msg[i].m_bid+msg[i].m_offer) /2;
//        m_volumns[0]->append(i, mid - msg[i].m_bid_quantity);
//        m_volumns[1]->append(i, mid + msg[i].m_offer_quantity);

//        qDebug()<<msg[i].m_offer<<msg[i].m_offer_quantity
//                <<msg[i].m_bid  <<msg[i].m_bid_quantity;
        //if(i>8) break;
    }
    //qDebug()<<"size set: "<<m_categories;QDateTimeAxis
    QValueAxis *axisX = NULL;
    if(axisX== 0)
    {
    axisX = new QValueAxis;
    axisX->setTickCount(10);
    //axisX->setRange(0, size);
    axisX->setLabelFormat("%u");
    m_chart->addSeries(m_futures[0]);
    m_chart->setAxisX(axisX, m_futures[0]);
    }



        //m_chart->addSeries(m_futures[1]);

//    QDateTimeAxis *axisX = NULL;
//    if(m_chart->axes(Qt::Horizontal).length() > 0)
//        axisX = qobject_cast<QDateTimeAxis *>(m_chart->axes(Qt::Horizontal).at(0));
////    axisX->setCategories(m_categories);
//    if(axisX== 0)
//    {
//       //m_chart->createDefaultAxes();

//        axisX = new QDateTimeAxis;

//    }

//    axisX->setTickCount(15);
//    //axisX->setRange(0, size);
//    axisX->setFormat("HH:mm:ss.z");
//    //axisX->setLabelFormat("%u");


//    m_chart->addSeries(m_futures[0]);
//    //m_chart->addSeries(m_futures[1]);
//    m_chart->setAxisX(axisX, m_futures[0]);


//    m_chart->addSeries(m_volumns[0]);
//    m_chart->addSeries(m_volumns[1]);
    QValueAxis *axisY = NULL;
    if( m_chart->axes(Qt::Vertical).length() > 0)
    {
        axisY = qobject_cast<QValueAxis *>(m_chart->axes(Qt::Vertical).at(0));
    }
    if(axisY == 0)
    {
        axisY = new QValueAxis;
        m_chart->setAxisY(axisY);
    }
    axisY->setMax(ma * 1.2);
    axisY->setMin(0);//mi * 0.8);
    qDebug()<<axisY->max()<<axisY->min();

    int i=size -1;
            qDebug()<<msg[i].m_offer<<msg[i].m_offer_quantity
                    <<msg[i].m_bid  <<msg[i].m_bid_quantity;

    //m_chart->scroll(100, 0);


}
