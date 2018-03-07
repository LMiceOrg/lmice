#include "graphicsview.h"

#include "candlestickchart.h"
GraphicsView::GraphicsView()
{

    setWindowTitle(tr("LMICE"));
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    //setRenderHints(QPainter::SmoothPixmapTransform);
    QMenuBar *bar= new QMenuBar(this);
    qDebug()<<bar;
    bar->setVisible(true);

    QMenu* file = bar->addMenu("File");
    QAction *act = new QAction(tr("New Candle chart"));
    file->addAction(act);
    QObject::connect(act, &QAction::triggered, [=]()
    {
        new CandleStickChart(scene());
    });

    QAction* about = new QAction("about");
    file->addAction(about);
    QAction* config = new QAction("config");
    file->addAction(config);
    QObject::connect(config, &QAction::triggered, [&]()
    {
        QMessageBox box;
        box.setText("config its");
        box.exec();
    });
    QAction* exit = new QAction("exit");
    file->addAction(exit);
    QObject::connect(exit, &QAction::triggered, [&]()
    {
        auto b = QMessageBox::question(NULL, tr("Quit"), tr("Do you quit LMICE?"));
        if(b == QMessageBox::Yes)
            qApp->exit(0);
    });

    QAction* open = new QAction("Open");
    file->addAction(open);
    QObject::connect(open, &QAction::triggered, [&]()
    {
        QMessageBox box;
        box.setText("open it");
        box.exec();
    });
}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
    if (scene())
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
    QGraphicsView::resizeEvent(event);
}
