#include "graphicsview.h"

#include "candlestickchart.h"
#include "loggingoutput.h"

GraphicsView::GraphicsView()
{

    setWindowTitle(tr("LMICE"));
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    //setRenderHints(QPainter::SmoothPixmapTransform);
    QMenuBar *bar= new QMenuBar(this);
    //qDebug()<<bar;
    bar->setVisible(true);


    QMenu* file = bar->addMenu("File");
    QMenu  *window = bar->addMenu(tr("Window"));

    QAction *act = new QAction(tr("New Candle chart"));
    file->addAction(act);

    QObject::connect(act, &QAction::triggered, [this, window]()
    {
        CandleStickChart* chart = new CandleStickChart(this->scene());
        QMenu *menu=window->addMenu("Chart window");

        QAction *cact = new QAction(tr("Remove candle"));
        menu->addAction(cact);
        QObject::connect(cact, &QAction::triggered, [chart, menu, window]()
        {
            delete chart;
            window->removeAction(menu->menuAction());
        });

        cact = new QAction(tr("Change Chart name"));
        menu->addAction(cact);
        QObject::connect(cact, &QAction::triggered, [menu]()
        {
            bool ok;
            QString text = QInputDialog::getText(NULL, tr("QInputDialog::getText()"),
                                                 tr("Chart Title:"), QLineEdit::Normal,
                                                 tr("New Chart"), &ok);
            if (ok && !text.isEmpty())
            {
                menu->setTitle(text);
            }
        });

        cact = new QAction(tr("Change Chart color"));
        menu->addAction(cact);
        QObject::connect(cact, &QAction::triggered, [menu, chart]()
        {
            QColor color = QColorDialog::getColor();
            chart->setColor(color);

        });
    });

    act = new QAction(tr("Open logging output"));
    file->addAction(act);

    QObject::connect(act, &QAction::triggered, [this, window]()
    {
        LoggingOutput* widget = new LoggingOutput(this->scene());
        QMenu *menu=window->addMenu("Chart window");

        QAction *cact = new QAction(tr("Remove candle"));
        menu->addAction(cact);
        QObject::connect(cact, &QAction::triggered, [widget, menu, window]()
        {
            delete widget;
            window->removeAction(menu->menuAction());
        });

        cact = new QAction(tr("Change Chart name"));
        menu->addAction(cact);
        QObject::connect(cact, &QAction::triggered, [menu]()
        {
            bool ok;
            QString text = QInputDialog::getText(NULL, tr("QInputDialog::getText()"),
                                                 tr("Chart Title:"), QLineEdit::Normal,
                                                 tr("New Chart"), &ok);
            if (ok && !text.isEmpty())
            {
                menu->setTitle(text);
            }
        });

        cact = new QAction(tr("Change Chart color"));
        menu->addAction(cact);
        QObject::connect(cact, &QAction::triggered, [menu, widget]()
        {
            QColor color = QColorDialog::getColor();
            //widget->setColor(color);
            foreach(auto child, widget->graphicsWidgets())
            {
                QTreeView* tv = qobject_cast<QTreeView*>(child->widget());
                if(tv)
                {
//                    int h,s,l,a;
//                    color.getHsl(&h,&s,&l,&a);
//                    h=180+h;
//                    if(h>360) h-=360;
//                    if(h<0) h+= 360;
//                    QColor tcolor = QColor::fromHsl(h,s,l,a);


//                    QPalette pal = tv->palette();
//                    pal.setColor(QPalette::Window, color);
//                    pal.setColor(QPalette::Base, color);
//                    //pal.setColor(QPalette::Background, color);
////                    pal.setColor(QPalette::AlternateBase, color);

//                    pal.setColor(QPalette::WindowText, tcolor);
//                    pal.setColor(QPalette::Text, tcolor);
//                    pal.setColor(QPalette::Foreground, tcolor);

////                    tv->setAutoFillBackground(true);
////                    tv->setPalette(pal);
////                    tv->viewport()->setAutoFillBackground(true);
////                    tv->viewport()->setPalette(pal);
//                    child->setAutoFillBackground(true);
//                    child->setPalette(pal);
                }
                else
                {
                    child->setPalette(color);
                }

                //qDebug()<<child;
            }

        });
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

GraphicsView::~GraphicsView()
{

}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
    if (scene())
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
    QGraphicsView::resizeEvent(event);
}
